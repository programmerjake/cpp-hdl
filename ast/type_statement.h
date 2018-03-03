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
#include "type.h"
#include "../parse/source.h"
#include "../util/string_pool.h"

namespace ast
{
class TypeStatement final : public Statement, public Symbol
{
public:
    ConsecutiveComments beforeTypeComments;
    ConsecutiveComments beforeNameComments;
    ConsecutiveComments beforeEqualComments;
    Type *type;
    ConsecutiveComments beforeSemicolonComments;
    explicit TypeStatement(parse::LocationRange locationRange,
                           ConsecutiveComments beforeTypeComments,
                           ConsecutiveComments beforeNameComments,
                           parse::LocationRange nameLocationRange,
                           util::StringPool::Entry name,
                           ConsecutiveComments beforeEqualComments,
                           Type *type,
                           ConsecutiveComments beforeSemicolonComments) noexcept
        : Statement(locationRange),
          Symbol(nameLocationRange, name),
          beforeTypeComments(beforeTypeComments),
          beforeNameComments(beforeNameComments),
          beforeEqualComments(beforeEqualComments),
          type(type),
          beforeSemicolonComments(beforeSemicolonComments)
    {
    }
};
}
