/**
 * Copyright 2018 Jacob Lifshay
 *
 * This file is part of Cpp-HDL.
 *
 * Cpp-HDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cpp-HDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cpp-HDL.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "arena.h"
#include "bit_vector.h"
#include "source.h"
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <unordered_map>
#include <initializer_list>
#include "string_view.h"
#include "tokenizer.h"

namespace ast
{
enum class VisitStatus : bool
{
    Stop = false,
    Continue = true
};

struct Visitor;
struct ConstVisitor;

struct Symbol;

class StringPool final
{
private:
    std::unordered_multimap<std::size_t, std::string> strings;

public:
    typedef const std::string *Entry;
    Entry intern(util::string_view str)
    {
        auto hash = std::hash<util::string_view>()(str);
        auto range = strings.equal_range(hash);
        for(auto i = std::get<0>(range); i != std::get<1>(range); ++i)
            if(std::get<1>(*i) == str)
                return &std::get<1>(*i);
        auto iter = strings.emplace(hash, static_cast<std::string>(str));
        return &std::get<1>(*iter);
    }
};

struct Node
{
    LocationRange locationRange;
    explicit Node(LocationRange locationRange) : locationRange(locationRange)
    {
    }
    virtual ~Node() = default;
    virtual VisitStatus visit(Visitor &visitor) = 0;
    virtual VisitStatus visit(ConstVisitor &visitor) const = 0;
    void dump(std::ostream &os = std::cerr) const;
};

struct Type : public Node
{
    bool isStateless;
    /** type after removing all the transparent type aliases; the definition of type equality */
    const Type *const canonicalType;
    explicit Type(LocationRange locationRange, bool isStateless, const Type *canonicalType)
        : Node(locationRange), isStateless(isStateless), canonicalType(canonicalType)
    {
    }
    explicit Type(LocationRange locationRange, bool isStateless)
        : Node(locationRange), isStateless(isStateless), canonicalType(this)
    {
    }
    friend bool operator==(const Type &a, const Type &b) noexcept
    {
        return a.canonicalType == b.canonicalType;
    }
    friend bool operator!=(const Type &a, const Type &b) noexcept
    {
        return a.canonicalType != b.canonicalType;
    }
    virtual const Type *getFlippedType() const noexcept = 0;
};

enum class BitVectorTypeDirection
{
    Input,
    Output,
    Reg
};

class BitVectorType;

class TypePool final
{
private:
    Arena typeArena;
    std::unordered_map<std::underlying_type_t<BitVectorTypeDirection>,
                       std::unordered_map<std::underlying_type_t<BitVector::Kind>,
                                          std::unordered_map<std::size_t, BitVectorType *>>>
        bitVectorTypes;

public:
    const BitVectorType *getBitVectorType(BitVectorTypeDirection direction,
                                          BitVector::Kind kind,
                                          std::size_t bitWidth);
};

struct Context final
{
    StringPool stringPool;
    Arena arena;
    TypePool typePool;
};

#define AST_NODE_DECLARE_VISITOR()                        \
public:                                                   \
    virtual VisitStatus visit(Visitor &visitor) override; \
    virtual VisitStatus visit(ConstVisitor &visitor) const override;

struct SymbolTable;

struct Symbol
{
    virtual ~Symbol() = default;
    LocationRange nameLocation;
    const StringPool::Entry name;
    SymbolTable *containingSymbolTable;
    explicit Symbol(LocationRange nameLocation, StringPool::Entry name)
        : nameLocation(nameLocation), name(name), containingSymbolTable()
    {
    }
};

struct SymbolLookupChainNode;

struct SymbolTable
{
    std::unordered_map<StringPool::Entry, Symbol *> localSymbolsMap = {};
    std::vector<Symbol *> localSymbolsList = {};
    SymbolTable()
    {
    }
    Symbol *find(StringPool::Entry name) const
    {
        auto iter = localSymbolsMap.find(name);
        if(iter != localSymbolsMap.end())
            return std::get<1>(*iter);
        return nullptr;
    }
    bool insert(Symbol *symbol)
    {
        assert(symbol);
        assert(symbol->containingSymbolTable == nullptr);
        if(std::get<1>(localSymbolsMap.emplace(symbol->name, symbol)))
        {
            symbol->containingSymbolTable = this;
            localSymbolsList.push_back(symbol);
            return true;
        }
        return false;
    }
    static SymbolTable *createGlobalSymbolTable(Context &context);
};

struct SymbolLookupChainNode
{
    const SymbolLookupChainNode *parent;
    SymbolTable *symbolTable;
    constexpr SymbolLookupChainNode(const SymbolLookupChainNode *parent, SymbolTable *symbolTable)
        : parent(parent), symbolTable(symbolTable)
    {
    }
};

struct SymbolLookupChain
{
    const SymbolLookupChainNode *head;
    constexpr SymbolLookupChain() noexcept : head(nullptr)
    {
    }
    constexpr explicit SymbolLookupChain(const SymbolLookupChainNode *head) noexcept : head(head)
    {
    }
    Symbol *find(StringPool::Entry name) const
    {
        for(auto *node = head; node; node = node->parent)
        {
            auto retval = node->symbolTable->find(name);
            if(retval)
                return retval;
        }
        return nullptr;
    }
};

struct SymbolScope
{
    virtual ~SymbolScope() = default;
    SymbolLookupChain symbolLookupChain; // includes symbolTable
    SymbolTable *symbolTable;
    SymbolScope(SymbolLookupChain symbolLookupChain, SymbolTable *symbolTable) noexcept
        : symbolLookupChain(symbolLookupChain),
          symbolTable(symbolTable)
    {
    }
};

struct Interface : public Node, public SymbolScope
{
};

struct NamedInterface final: public Interface, public Symbol
{
    AST_NODE_DECLARE_VISITOR()
};

struct AnonymousInterface final: public Interface, public Symbol
{
    AST_NODE_DECLARE_VISITOR()
};

struct Module final : public Node, public Symbol, public SymbolScope
{
    std::vector<Node *> members;
    Module(LocationRange locationRange,
           LocationRange nameLocation,
           StringPool::Entry name,
           SymbolLookupChain symbolLookupChain,
           SymbolTable *symbolTable)
        : Node(locationRange),
          Symbol(nameLocation, name),
          SymbolScope(symbolLookupChain, symbolTable),
          members()
    {
    }
    AST_NODE_DECLARE_VISITOR()
};

struct BitVectorTypeBuiltinAlias
{
    TokenType name;
    BitVector::Kind kind;
    std::size_t bitWidth;
    constexpr BitVectorTypeBuiltinAlias(TokenType name,
                                        BitVector::Kind kind,
                                        std::size_t bitWidth) noexcept : name(name),
                                                                         kind(kind),
                                                                         bitWidth(bitWidth)
    {
    }
};

class BitVectorType final : public Type
{
    friend class TypePool;

private:
    struct PrivateAccessTag
    {
        friend class TypePool;

    private:
        constexpr PrivateAccessTag() = default;
    };

public:
    using Direction = BitVectorTypeDirection;
    static constexpr Direction flipDirection(Direction v) noexcept
    {
        switch(v)
        {
        case Direction::Input:
            return Direction::Output;
        case Direction::Output:
            return Direction::Input;
        default:
            return v;
        }
    }
    static constexpr util::string_view getDirectionName(Direction v) noexcept
    {
        using namespace util::string_view_literals;
        switch(v)
        {
        case Direction::Input:
            return "input"_sv;
        case Direction::Output:
            return "output"_sv;
        case Direction::Reg:
            return "reg"_sv;
        }
        assert(v == Direction::Input);
        return ""_sv;
    }
    static constexpr bool isStatelessDirection(Direction v) noexcept
    {
        switch(v)
        {
        case Direction::Input:
        case Direction::Output:
            return false;
        case Direction::Reg:
            return true;
        }
        assert(v == Direction::Input);
        return true;
    }

public:
    Direction direction;
    BitVector::Kind kind;
    std::size_t bitWidth;

private:
    BitVectorType *flippedType;

public:
    BitVectorType(Direction direction,
                  BitVector::Kind kind,
                  std::size_t bitWidth,
                  BitVectorType *flippedType,
                  PrivateAccessTag)
        : Type({}, isStatelessDirection(direction)),
          direction(),
          kind(kind),
          bitWidth(bitWidth),
          flippedType(flippedType)
    {
        if(flippedType)
            flippedType->flippedType = this;
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return flippedType;
    }

public:
    using BuiltinAlias = BitVectorTypeBuiltinAlias;
    class BuiltinAliases
    {
    public:
        typedef const BuiltinAlias *iterator;
        typedef const BuiltinAlias *const_iterator;

    private:
        const BuiltinAlias *array;
        std::size_t count;

    public:
        template <std::size_t N>
        explicit constexpr BuiltinAliases(const BuiltinAlias(&array)[N]) noexcept : array(array),
                                                                                    count(N)
        {
        }
        constexpr const_iterator begin() const noexcept
        {
            return array;
        }
        constexpr const_iterator end() const noexcept
        {
            return array + count;
        }
        constexpr std::size_t size() const noexcept
        {
            return count;
        }
    };
    static BuiltinAliases getBuiltinAliases() noexcept
    {
        using namespace util::string_view_literals;
        static constexpr BuiltinAlias builtinAliases[] = {
            BuiltinAlias(TokenType::Bit, BitVector::Kind::Unsigned, 1),
            BuiltinAlias(TokenType::U8, BitVector::Kind::Unsigned, 8),
            BuiltinAlias(TokenType::U16, BitVector::Kind::Unsigned, 16),
            BuiltinAlias(TokenType::U32, BitVector::Kind::Unsigned, 32),
            BuiltinAlias(TokenType::U64, BitVector::Kind::Unsigned, 64),
            BuiltinAlias(TokenType::S8, BitVector::Kind::Signed, 8),
            BuiltinAlias(TokenType::S16, BitVector::Kind::Signed, 16),
            BuiltinAlias(TokenType::S32, BitVector::Kind::Signed, 32),
            BuiltinAlias(TokenType::S64, BitVector::Kind::Signed, 64),
        };
        return BuiltinAliases(builtinAliases);
    }
    AST_NODE_DECLARE_VISITOR()
};

struct TransparentTypeAlias final : public Type, public Symbol
{
    const Type *aliasedType;
    TransparentTypeAlias(LocationRange locationRange,
                         LocationRange nameLocation,
                         StringPool::Entry name,
                         const Type *aliasedType)
        : Type(locationRange, aliasedType->canonicalType),
          Symbol(nameLocation, name),
          aliasedType(aliasedType)
    {
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return aliasedType->getFlippedType();
    }
    AST_NODE_DECLARE_VISITOR()
};

struct Variable final : public Node, public Symbol
{
    const Type *type;
    Variable(LocationRange locationRange,
             LocationRange nameLocation,
             StringPool::Entry name,
             const Type *type)
        : Node(locationRange), Symbol(nameLocation, name), type(type)
    {
    }
    AST_NODE_DECLARE_VISITOR()
};

class Bundle;

class FlippedBundle final : public Type
{
    friend class Bundle;

private:
    Bundle *const flippedType;

private:
    struct PrivateAccessTag
    {
    private:
        PrivateAccessTag() = default;
    };

public:
    FlippedBundle(PrivateAccessTag, LocationRange locationRange, Bundle *flippedType)
        : Type(locationRange, true), flippedType(flippedType)
    {
    }
    virtual const Type *getFlippedType() const noexcept override;
    AST_NODE_DECLARE_VISITOR()
};

class Bundle final : public Type, public Symbol, public SymbolScope
{
    friend class FlippedBundle;

private:
    std::vector<Variable *> members{};
    FlippedBundle *flippedType;
    bool defined = false;

public:
    Bundle(LocationRange locationRange,
           LocationRange nameLocation,
           StringPool::Entry name,
           SymbolLookupChain symbolLookupChain,
           SymbolTable *symbolTable,
           Context &context)
        : Type(locationRange, true),
          Symbol(nameLocation, name),
          SymbolScope(symbolLookupChain, symbolTable),
          flippedType(context.arena.create<FlippedBundle>(
              FlippedBundle::PrivateAccessTag{}, locationRange, this))
    {
    }
    bool isDefined() const noexcept
    {
        return defined;
    }
    const std::vector<Variable *> &getMembers() const noexcept
    {
        assert(defined);
        return members;
    }
    void define(std::vector<Variable *> newMembers) noexcept
    {
        assert(!defined);
        defined = true;
        members.swap(newMembers);
        isStateless = true;
        for(auto *member : members)
        {
            if(!member->type->isStateless)
            {
                isStateless = false;
                break;
            }
        }
        flippedType->isStateless = isStateless;
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return flippedType;
    }
    AST_NODE_DECLARE_VISITOR()
};

inline const Type *FlippedBundle::getFlippedType() const noexcept
{
    return flippedType;
}

#undef AST_NODE_DECLARE_VISITOR
#define AST_NODE_DECLARE_VISIT_FUNCTION(T) virtual VisitStatus visit(const T *node) = 0;

struct ConstVisitor
{
    virtual ~ConstVisitor() = default;
    AST_NODE_DECLARE_VISIT_FUNCTION(Module)
    AST_NODE_DECLARE_VISIT_FUNCTION(TransparentTypeAlias)
    AST_NODE_DECLARE_VISIT_FUNCTION(Variable)
    AST_NODE_DECLARE_VISIT_FUNCTION(FlippedBundle)
    AST_NODE_DECLARE_VISIT_FUNCTION(Bundle)
    AST_NODE_DECLARE_VISIT_FUNCTION(BitVectorType)
};

#undef AST_NODE_DECLARE_VISIT_FUNCTION
#define AST_NODE_DECLARE_VISIT_FUNCTION(T)          \
    virtual VisitStatus visit(T *node)              \
    {                                               \
        return visit(static_cast<const T *>(node)); \
    }

struct Visitor : public ConstVisitor
{
    using ConstVisitor::visit;
    AST_NODE_DECLARE_VISIT_FUNCTION(Module)
    AST_NODE_DECLARE_VISIT_FUNCTION(TransparentTypeAlias)
    AST_NODE_DECLARE_VISIT_FUNCTION(Variable)
    AST_NODE_DECLARE_VISIT_FUNCTION(FlippedBundle)
    AST_NODE_DECLARE_VISIT_FUNCTION(Bundle)
    AST_NODE_DECLARE_VISIT_FUNCTION(BitVectorType)
};

#undef AST_NODE_DECLARE_VISIT_FUNCTION

struct DumpVisitor;

std::unique_ptr<ConstVisitor> makeDumpVisitor(std::ostream &os = std::cerr);

inline void Node::dump(std::ostream &os) const
{
    visit(*makeDumpVisitor(os));
}
}
