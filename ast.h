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
#include "string_view.h"

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

struct Context final
{
    StringPool stringPool;
    Arena arena;
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

#define AST_NODE_DECLARE_VISITOR()                              \
public:                                                         \
    virtual VisitStatus visit(Visitor &visitor) override final; \
    virtual VisitStatus visit(ConstVisitor &visitor) const override final;

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
    std::unordered_map<StringPool::Entry, Symbol *> localSymbols = {};
    SymbolTable()
    {
    }
    Symbol *find(StringPool::Entry name) const
    {
        auto iter = localSymbols.find(name);
        if(iter != localSymbols.end())
            return std::get<1>(*iter);
        return nullptr;
    }
    bool insert(Symbol *symbol)
    {
        assert(symbol);
        assert(symbol->containingSymbolTable == nullptr);
        if(std::get<1>(localSymbols.emplace(symbol->name, symbol)))
        {
            symbol->containingSymbolTable = this;
            return true;
        }
        return false;
    }
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

struct Type : public Node
{
    explicit Type(LocationRange locationRange) : Node(locationRange)
    {
    }
};

struct Bundle final : public Type, public Symbol, public SymbolScope
{
    std::vector<Node *> members;
    Bundle(LocationRange locationRange,
           LocationRange nameLocation,
           StringPool::Entry name,
           SymbolLookupChain symbolLookupChain,
           SymbolTable *symbolTable)
        : Type(locationRange),
          Symbol(nameLocation, name),
          SymbolScope(symbolLookupChain, symbolTable),
          members()
    {
    }
    AST_NODE_DECLARE_VISITOR()
};

#undef AST_NODE_DECLARE_VISITOR
#define AST_NODE_DECLARE_VISIT_FUNCTION(T) virtual VisitStatus visit(const T *node) = 0;

struct ConstVisitor
{
    virtual ~ConstVisitor() = default;
    AST_NODE_DECLARE_VISIT_FUNCTION(Module)
    AST_NODE_DECLARE_VISIT_FUNCTION(Bundle)
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
    AST_NODE_DECLARE_VISIT_FUNCTION(Bundle)
};

#undef AST_NODE_DECLARE_VISIT_FUNCTION

struct DumpVisitor;

std::unique_ptr<ConstVisitor> makeDumpVisitor(std::ostream &os = std::cerr);

inline void Node::dump(std::ostream &os) const
{
    visit(*makeDumpVisitor(os));
}
}
