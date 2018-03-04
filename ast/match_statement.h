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
#include "comment.h"
#include "expression.h"
#include <vector>
#include "match_statement_part.h"

namespace ast
{
class MatchStatement final : public Statement
{
public:
    ConsecutiveComments beforeMatchComments;
    ConsecutiveComments beforeLParenComments;
    Expression *matchee;
    ConsecutiveComments beforeRParenComments;
    ConsecutiveComments beforeLBraceComments;
    std::vector<MatchStatementPart *> parts;
    ConsecutiveComments beforeRBraceComments;
    explicit MatchStatement(parse::LocationRange locationRange,
                            ConsecutiveComments beforeMatchComments,
                            ConsecutiveComments beforeLParenComments,
                            Expression *matchee,
                            ConsecutiveComments beforeRParenComments,
                            ConsecutiveComments beforeLBraceComments,
                            std::vector<MatchStatementPart *> parts,
                            ConsecutiveComments beforeRBraceComments) noexcept
        : Statement(locationRange),
          beforeMatchComments(beforeMatchComments),
          beforeLParenComments(beforeLParenComments),
          matchee(matchee),
          beforeRParenComments(beforeRParenComments),
          beforeLBraceComments(beforeLBraceComments),
          parts(parts),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
};
}
