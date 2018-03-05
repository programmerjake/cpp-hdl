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
#include "const_statement_part.h"
#include <vector>
#include "../parse/source.h"
#include "../util/dump_tree.h"

namespace ast
{
class ConstStatement final : public Statement
{
public:
    struct Part final
    {
        ConsecutiveComments beforeCommaComments;
        ConstStatementPart *part;
        constexpr Part(ConsecutiveComments beforeCommaComments, ConstStatementPart *part) noexcept
            : beforeCommaComments(beforeCommaComments),
              part(part)
        {
        }
    };
    ConsecutiveComments beforeConstComments;
    ConstStatementPart *firstPart;
    std::vector<Part> parts;
    ConsecutiveComments beforeSemicolonComments;
    explicit ConstStatement(parse::LocationRange locationRange,
                            ConsecutiveComments beforeConstComments,
                            ConstStatementPart *firstPart,
                            std::vector<Part> parts,
                            ConsecutiveComments beforeSemicolonComments) noexcept
        : Statement(locationRange),
          beforeConstComments(beforeConstComments),
          firstPart(firstPart),
          parts(std::move(parts)),
          beforeSemicolonComments(beforeSemicolonComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
