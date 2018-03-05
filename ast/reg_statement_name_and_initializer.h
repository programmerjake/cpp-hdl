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
#include "symbol.h"
#include "comment.h"
#include "expression.h"
#include "../parse/source.h"
#include "../util/string_pool.h"

namespace ast
{
class RegStatementPart;

class RegStatementNameAndInitializer final : public Node, public Symbol
{
public:
    ConsecutiveComments beforeNameComments;
    ConsecutiveComments beforeEqualComments;
    Expression *initializer;
    RegStatementPart *parentPart;
    explicit RegStatementNameAndInitializer(parse::LocationRange locationRange,
                                            ConsecutiveComments beforeNameComments,
                                            parse::LocationRange nameLocationRange,
                                            util::StringPool::Entry name,
                                            ConsecutiveComments beforeEqualComments,
                                            Expression *initializer,
                                            RegStatementPart *parentPart = nullptr) noexcept
        : Node(locationRange),
          Symbol(nameLocationRange, name),
          beforeNameComments(beforeNameComments),
          beforeEqualComments(beforeEqualComments),
          initializer(initializer),
          parentPart(parentPart)
    {
    }
};
}
