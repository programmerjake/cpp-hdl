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

#include "node.h"
#include "../parse/source.h"
#include "comment.h"
#include "../parse/token.h"
#include "expression.h"

namespace ast
{
class MatchPattern : public Node
{
public:
    explicit MatchPattern(parse::LocationRange locationRange) noexcept : Node(locationRange)
    {
    }
};

class NumberPatternMatchPattern final : public MatchPattern
{
public:
    ConsecutiveComments beforeNumberComments;
    parse::Token::IntegerValue pattern;
    explicit NumberPatternMatchPattern(parse::LocationRange locationRange,
                                       ConsecutiveComments beforeNumberComments,
                                       parse::Token::IntegerValue pattern) noexcept
        : MatchPattern(locationRange),
          beforeNumberComments(beforeNumberComments),
          pattern(std::move(pattern))
    {
    }
};

class RangeMatchPattern final : public MatchPattern
{
public:
    Expression *firstExpression;
    ConsecutiveComments beforeToComments;
    Expression *secondExpression;
    explicit RangeMatchPattern(parse::LocationRange locationRange,
                               Expression *firstExpression,
                               ConsecutiveComments beforeToComments,
                               Expression *secondExpression) noexcept
        : MatchPattern(locationRange),
          firstExpression(firstExpression),
          beforeToComments(beforeToComments),
          secondExpression(secondExpression)
    {
    }
};
}
