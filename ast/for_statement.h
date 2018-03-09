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

#include "statement.h"
#include "symbol_scope.h"
#include "comment.h"
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "node.h"
#include "symbol.h"
#include "type.h"
#include "expression.h"
#include "../util/dump_tree.h"

namespace ast
{
class ForStatementVariable;

class GenericForStatement : public Statement, public SymbolScope
{
public:
    ConsecutiveComments beforeForComments;
    ConsecutiveComments beforeLParenComments;
    ForStatementVariable *variable;
    ConsecutiveComments beforeInComments;
    ConsecutiveComments beforeRParenComments;
    Statement *statement;
    explicit GenericForStatement(parse::LocationRange locationRange,
                                 SymbolLookupChain symbolLookupChain,
                                 SymbolTable *symbolTable,
                                 ConsecutiveComments beforeForComments,
                                 ConsecutiveComments beforeLParenComments,
                                 ForStatementVariable *variable,
                                 ConsecutiveComments beforeInComments,
                                 ConsecutiveComments beforeRParenComments,
                                 Statement *statement) noexcept
        : Statement(locationRange),
          SymbolScope(symbolLookupChain, symbolTable),
          beforeForComments(beforeForComments),
          beforeLParenComments(beforeLParenComments),
          variable(variable),
          beforeInComments(beforeInComments),
          beforeRParenComments(beforeRParenComments),
          statement(statement)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override = 0;
};

class ForStatementVariable final : public Node, public Symbol
{
public:
    ConsecutiveComments beforeNameComments;
    GenericForStatement *forStatement;
    explicit ForStatementVariable(parse::LocationRange locationRange,
                                  ConsecutiveComments beforeNameComments,
                                  parse::LocationRange symbolLocationRange,
                                  util::StringPool::Entry name,
                                  GenericForStatement *forStatement) noexcept
        : Node(locationRange),
          Symbol(symbolLocationRange, name),
          beforeNameComments(beforeNameComments),
          forStatement(forStatement)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class ForTypeStatement final : public GenericForStatement
{
public:
    ConsecutiveComments beforeTypeKeywordComments;
    Type *type;
    explicit ForTypeStatement(parse::LocationRange locationRange,
                              SymbolLookupChain symbolLookupChain,
                              SymbolTable *symbolTable,
                              ConsecutiveComments beforeForComments,
                              ConsecutiveComments beforeLParenComments,
                              ConsecutiveComments beforeTypeKeywordComments,
                              ForStatementVariable *variable,
                              ConsecutiveComments beforeInComments,
                              Type *type,
                              ConsecutiveComments beforeRParenComments,
                              Statement *statement) noexcept
        : GenericForStatement(locationRange,
                              symbolLookupChain,
                              symbolTable,
                              beforeForComments,
                              beforeLParenComments,
                              variable,
                              beforeInComments,
                              beforeRParenComments,
                              statement),
          beforeTypeKeywordComments(beforeTypeKeywordComments),
          type(type)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class ForStatement final : public GenericForStatement
{
public:
    Expression *firstExpression;
    ConsecutiveComments beforeToComments;
    Expression *secondExpression;
    explicit ForStatement(parse::LocationRange locationRange,
                          SymbolLookupChain symbolLookupChain,
                          SymbolTable *symbolTable,
                          ConsecutiveComments beforeForComments,
                          ConsecutiveComments beforeLParenComments,
                          ForStatementVariable *variable,
                          ConsecutiveComments beforeInComments,
                          Expression *firstExpression,
                          ConsecutiveComments beforeToComments,
                          Expression *secondExpression,
                          ConsecutiveComments beforeRParenComments,
                          Statement *statement) noexcept : GenericForStatement(locationRange,
                                                                               symbolLookupChain,
                                                                               symbolTable,
                                                                               beforeForComments,
                                                                               beforeLParenComments,
                                                                               variable,
                                                                               beforeInComments,
                                                                               beforeRParenComments,
                                                                               statement),
                                                           firstExpression(firstExpression),
                                                           beforeToComments(beforeToComments),
                                                           secondExpression(secondExpression)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
