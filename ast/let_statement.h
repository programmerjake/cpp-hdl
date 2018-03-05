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
#include "let_statement_part.h"
#include <vector>
#include "../parse/source.h"
#include "../util/dump_tree.h"

namespace ast
{
class LetStatement final : public Statement
{
public:
    struct Part final
    {
        ConsecutiveComments beforeCommaComments;
        LetStatementPart *part;
        constexpr Part(ConsecutiveComments beforeCommaComments, LetStatementPart *part) noexcept
            : beforeCommaComments(beforeCommaComments),
              part(part)
        {
        }
    };
    ConsecutiveComments beforeLetComments;
    LetStatementPart *firstPart;
    std::vector<Part> parts;
    ConsecutiveComments beforeSemicolonComments;
    explicit LetStatement(parse::LocationRange locationRange,
                          ConsecutiveComments beforeLetComments,
                          LetStatementPart *firstPart,
                          std::vector<Part> parts,
                          ConsecutiveComments beforeSemicolonComments) noexcept
        : Statement(locationRange),
          beforeLetComments(beforeLetComments),
          firstPart(firstPart),
          parts(std::move(parts)),
          beforeSemicolonComments(beforeSemicolonComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
