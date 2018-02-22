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

#include "ast.h"
#include <deque>

namespace ast
{
struct DumpVisitor final : public ConstVisitor
{
    std::size_t indentLevel = 0;
    std::ostream &os;
    struct Indent
    {
        std::size_t level = 0;
        friend std::ostream &operator<<(std::ostream &os, const Indent &indent)
        {
            for(std::size_t i = 0; i < indent.level; i++)
                os << "    ";
            return os;
        }
    };
    Indent indent;
    struct PushIndent
    {
        std::size_t &level;
        std::size_t previousLevel;
        explicit PushIndent(DumpVisitor *dumpVisitor, int amount = 1) noexcept
            : level(dumpVisitor->indent.level),
              previousLevel(level)
        {
            level += amount;
        }
        PushIndent(const PushIndent &) = delete;
        PushIndent &operator=(const PushIndent &) = delete;
        ~PushIndent()
        {
            level = previousLevel;
        }
    };
    std::unordered_map<const void *, std::size_t> visitedObjectNumbers;
    bool writeObjectNumberAndCheckIfFirst(const void *object)
    {
        auto results = visitedObjectNumbers.emplace(object, visitedObjectNumbers.size());
        os << (std::get<1>(results) ? '=' : '*') << std::get<1>(*std::get<0>(results));
        return std::get<1>(results);
    }
    explicit DumpVisitor(std::ostream &os) : os(os)
    {
    }
    void visit(const SymbolTable *symbolTable)
    {
        os << indent << "SymbolTable ";
        bool isFirst = writeObjectNumberAndCheckIfFirst(symbolTable);
        os << "\n";
        if(!isFirst)
            return;
        PushIndent pushIndent(this);
        for(auto &symbol : symbolTable->localSymbolsList)
        {
            auto *node = dynamic_cast<const Node *>(symbol);
            if(node)
            {
                node->visit(*this);
            }
            else
            {
                os << indent << *symbol->name << "<not a Node>\n";
            }
        }
    }
    void visit(SymbolLookupChain symbolLookupChain)
    {
        os << indent << "SymbolLookupChain\n";
        PushIndent pushIndent(this);
        std::deque<SymbolTable *> symbolTables;
        for(auto node = symbolLookupChain.head; node; node = node->parent)
            symbolTables.push_front(node->symbolTable);
        for(auto *symbolTable : symbolTables)
            visit(symbolTable);
    }
    virtual VisitStatus visit(const Module *node) override;
    virtual VisitStatus visit(const TransparentTypeAlias *node) override;
    virtual VisitStatus visit(const Bundle *node) override;
    virtual VisitStatus visit(const BitVectorType *node) override;
};

#define AST_NODE_IMPLEMENT_VISITOR(T)                 \
    VisitStatus T::visit(Visitor &visitor)            \
    {                                                 \
        return visitor.visit(this);                   \
    }                                                 \
    VisitStatus T::visit(ConstVisitor &visitor) const \
    {                                                 \
        return visitor.visit(this);                   \
    }

const BitVectorType *TypePool::getBitVectorType(BitVector::Kind kind, std::size_t bitWidth)
{
    auto &retval =
        bitVectorTypes[static_cast<std::underlying_type_t<BitVector::Kind>>(kind)][bitWidth];
    if(!retval)
        retval =
            typeArena.create<BitVectorType>(kind, bitWidth, BitVectorType::PrivateConstructTag{});
    return retval;
}

SymbolTable *SymbolTable::createGlobalSymbolTable(ast::Context &context)
{
    auto *retval = context.arena.create<SymbolTable>();
    for(auto &builtinAlias : BitVectorType::getBuiltinAliases())
    {
        bool succeeded = retval->insert(context.arena.create<TransparentTypeAlias>(
            LocationRange{},
            LocationRange{},
            context.stringPool.intern(builtinAlias.name),
            context.typePool.getBitVectorType(builtinAlias.kind, builtinAlias.bitWidth)));
        static_cast<void>(succeeded);
        assert(succeeded);
    }
    return retval;
}

AST_NODE_IMPLEMENT_VISITOR(Module)

VisitStatus DumpVisitor::visit(const Module *node)
{
    os << indent << "module " << *node->name << " ";
    bool isFirst = writeObjectNumberAndCheckIfFirst(node);
    os << '\n';
    if(!isFirst)
        return VisitStatus::Continue;
    PushIndent pushIndent(this);
    visit(node->symbolLookupChain);
    for(auto *member : node->members)
        member->visit(*this);
    return VisitStatus::Continue;
}

AST_NODE_IMPLEMENT_VISITOR(TransparentTypeAlias)

VisitStatus DumpVisitor::visit(const TransparentTypeAlias *node)
{
    os << indent << "TransparentTypeAlias " << *node->name << " ";
    bool isFirst = writeObjectNumberAndCheckIfFirst(node);
    os << '\n';
    if(!isFirst)
        return VisitStatus::Continue;
    PushIndent pushIndent(this);
    node->aliasedType->visit(*this);
    return VisitStatus::Continue;
}

AST_NODE_IMPLEMENT_VISITOR(Bundle)

VisitStatus DumpVisitor::visit(const Bundle *node)
{
    os << indent << "bundle " << *node->name << " ";
    bool isFirst = writeObjectNumberAndCheckIfFirst(node);
    os << '\n';
    if(!isFirst)
        return VisitStatus::Continue;
    PushIndent pushIndent(this);
    visit(node->symbolLookupChain);
    for(auto *member : node->members)
        member->visit(*this);
    return VisitStatus::Continue;
}

AST_NODE_IMPLEMENT_VISITOR(BitVectorType)

VisitStatus DumpVisitor::visit(const BitVectorType *node)
{
    os << indent;
    util::string_view name = {};
    for(auto &builtinAlias : BitVectorType::getBuiltinAliases())
    {
        if(builtinAlias.kind == node->kind && builtinAlias.bitWidth == node->bitWidth)
        {
            name = builtinAlias.name;
            break;
        }
    }
    if(!name.empty())
        os << name << ": ";
    switch(node->kind)
    {
    case BitVector::Kind::Unsigned:
        os << "uint";
        break;
    case BitVector::Kind::Signed:
        os << "sint";
        break;
    }
    os << "(" << node->bitWidth << ")\n";
    return VisitStatus::Continue;
}

std::unique_ptr<ConstVisitor> makeDumpVisitor(std::ostream &os)
{
    return std::unique_ptr<ConstVisitor>(new DumpVisitor(os));
}
}
