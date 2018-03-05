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

#include "type.h"
#include "comment.h"
#include <vector>
#include "../parse/source.h"
#include "../util/dump_tree.h"

namespace ast
{
class TupleType final : public Type
{
public:
    struct Part final
    {
        Type *part;
        ConsecutiveComments beforeCommaComments;
        constexpr Part(Type *part, ConsecutiveComments beforeCommaComments) noexcept
            : part(part),
              beforeCommaComments(beforeCommaComments)
        {
        }
    };
    ConsecutiveComments beforeLBraceComments;
    std::vector<Part> parts;
    bool hasTrailingComma;
    ConsecutiveComments beforeRBraceComments;
    explicit TupleType(parse::LocationRange locationRange,
                       ConsecutiveComments beforeLBraceComments,
                       std::vector<Part> parts,
                       bool hasTrailingComma,
                       ConsecutiveComments beforeRBraceComments) noexcept
        : Type(locationRange),
          beforeLBraceComments(beforeLBraceComments),
          parts(std::move(parts)),
          hasTrailingComma(hasTrailingComma),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
