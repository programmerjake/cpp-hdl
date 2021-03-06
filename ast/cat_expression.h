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

#include "expression.h"
#include "comment.h"
#include "../parse/source.h"
#include <vector>
#include "../util/dump_tree.h"

namespace ast
{
class CatExpression final : public Expression
{
public:
    struct Part
    {
        ConsecutiveComments beforeCommaComments;
        Expression *expression;
        constexpr Part(ConsecutiveComments beforeCommaComments, Expression *expression) noexcept
            : beforeCommaComments(beforeCommaComments),
              expression(expression)
        {
        }
    };
    ConsecutiveComments beforeCatComments;
    ConsecutiveComments beforeLParenComments;
    Expression *firstExpression;
    std::vector<Part> parts;
    ConsecutiveComments beforeRParenComments;
    explicit CatExpression(parse::LocationRange locationRange,
                           ConsecutiveComments beforeCatComments,
                           ConsecutiveComments beforeLParenComments,
                           Expression *firstExpression,
                           std::vector<Part> parts,
                           ConsecutiveComments beforeRParenComments) noexcept
        : Expression(locationRange),
          beforeCatComments(beforeCatComments),
          beforeLParenComments(beforeLParenComments),
          firstExpression(firstExpression),
          parts(std::move(parts)),
          beforeRParenComments(beforeRParenComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
