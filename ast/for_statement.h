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
#include "symbol.h"
#include "comment.h"
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "type.h"
#include "expression.h"

namespace ast
{
class GenericForStatement : public Statement, public Symbol
{
public:
    ConsecutiveComments beforeForComments;
    ConsecutiveComments beforeLParenComments;
    ConsecutiveComments beforeNameComments;
    ConsecutiveComments beforeInComments;
    ConsecutiveComments beforeRParenComments;
    Statement *statement;
    explicit GenericForStatement(parse::LocationRange locationRange,
                                 ConsecutiveComments beforeForComments,
                                 ConsecutiveComments beforeLParenComments,
                                 ConsecutiveComments beforeNameComments,
                                 parse::LocationRange symbolLocationRange,
                                 util::StringPool::Entry name,
                                 ConsecutiveComments beforeInComments,
                                 ConsecutiveComments beforeRParenComments,
                                 Statement *statement) noexcept
        : Statement(locationRange),
          Symbol(symbolLocationRange, name),
          beforeForComments(beforeForComments),
          beforeLParenComments(beforeLParenComments),
          beforeNameComments(beforeNameComments),
          beforeInComments(beforeInComments),
          beforeRParenComments(beforeRParenComments),
          statement(statement)
    {
    }
};

class ForTypeStatement final : public GenericForStatement
{
public:
    ConsecutiveComments beforeTypeKeywordComments;
    Type *type;
    explicit ForTypeStatement(parse::LocationRange locationRange,
                              ConsecutiveComments beforeForComments,
                              ConsecutiveComments beforeLParenComments,
                              ConsecutiveComments beforeTypeKeywordComments,
                              ConsecutiveComments beforeNameComments,
                              parse::LocationRange symbolLocationRange,
                              util::StringPool::Entry name,
                              ConsecutiveComments beforeInComments,
                              Type *type,
                              ConsecutiveComments beforeRParenComments,
                              Statement *statement) noexcept
        : GenericForStatement(locationRange,
                              beforeForComments,
                              beforeLParenComments,
                              beforeNameComments,
                              symbolLocationRange,
                              name,
                              beforeInComments,
                              beforeRParenComments,
                              statement),
          beforeTypeKeywordComments(beforeTypeKeywordComments),
          type(type)
    {
    }
};

class ForStatement final : public GenericForStatement
{
public:
    Expression *firstExpression;
    ConsecutiveComments beforeToComments;
    Expression *secondExpression;
    explicit ForStatement(parse::LocationRange locationRange,
                          ConsecutiveComments beforeForComments,
                          ConsecutiveComments beforeLParenComments,
                          ConsecutiveComments beforeNameComments,
                          parse::LocationRange symbolLocationRange,
                          util::StringPool::Entry name,
                          ConsecutiveComments beforeInComments,
                          Expression *firstExpression,
                          ConsecutiveComments beforeToComments,
                          Expression *secondExpression,
                          ConsecutiveComments beforeRParenComments,
                          Statement *statement) noexcept : GenericForStatement(locationRange,
                                                                               beforeForComments,
                                                                               beforeLParenComments,
                                                                               beforeNameComments,
                                                                               symbolLocationRange,
                                                                               name,
                                                                               beforeInComments,
                                                                               beforeRParenComments,
                                                                               statement),
                                                           firstExpression(firstExpression),
                                                           beforeToComments(beforeToComments),
                                                           secondExpression(secondExpression)
    {
    }
};
}
