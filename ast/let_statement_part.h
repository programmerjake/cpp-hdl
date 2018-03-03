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
#include "comment.h"
#include "let_statement_name.h"
#include <vector>
#include "type.h"
#include "../parse/source.h"

namespace ast
{
class LetStatementPart final : public Node
{
public:
    struct Part final
    {
        ConsecutiveComments beforeCommaComments;
        LetStatementName *name;
        constexpr Part(ConsecutiveComments beforeCommaComments, LetStatementName *name) noexcept
            : beforeCommaComments(beforeCommaComments),
              name(name)
        {
        }
    };
    LetStatementName *firstName;
    std::vector<Part> parts;
    ConsecutiveComments beforeColonComments;
    Type *type;
    explicit LetStatementPart(parse::LocationRange locationRange,
                              LetStatementName *firstName,
                              std::vector<Part> parts,
                              ConsecutiveComments beforeColonComments,
                              Type *type) noexcept : Node(locationRange),
                                                     firstName(firstName),
                                                     parts(std::move(parts)),
                                                     beforeColonComments(beforeColonComments),
                                                     type(type)
    {
    }
};
}
