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
#include "match_pattern.h"
#include "comment.h"
#include <vector>
#include "statement.h"
#include "../parse/source.h"

namespace ast
{
class MatchStatementPart final : public Node
{
public:
    struct Part final
    {
        ConsecutiveComments beforeCommaComments;
        MatchPattern *matchPattern;
        constexpr Part(ConsecutiveComments beforeCommaComments, MatchPattern *matchPattern) noexcept
            : beforeCommaComments(beforeCommaComments),
              matchPattern(matchPattern)
        {
        }
    };
    MatchPattern *firstMatchPattern;
    std::vector<Part> parts;
    ConsecutiveComments beforeEqualRAngleComments;
    Statement *statement;
    explicit MatchStatementPart(parse::LocationRange locationRange,
                                MatchPattern *firstMatchPattern,
                                std::vector<Part> parts,
                                ConsecutiveComments beforeEqualRAngleComments,
                                Statement *statement) noexcept
        : Node(locationRange),
          firstMatchPattern(firstMatchPattern),
          parts(std::move(parts)),
          beforeEqualRAngleComments(beforeEqualRAngleComments),
          statement(statement)
    {
    }
};
}
