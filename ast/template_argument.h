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
#include "type.h"
#include "expression.h"

namespace ast
{
class TemplateArgument : public Node
{
public:
    explicit TemplateArgument(parse::LocationRange locationRange) noexcept : Node(locationRange)
    {
    }
};

class TypeTemplateArgument final : public TemplateArgument
{
public:
    ConsecutiveComments beforeTypeComments;
    Type *type;
    explicit TypeTemplateArgument(parse::LocationRange locationRange,
                                  ConsecutiveComments beforeTypeComments,
                                  Type *type) noexcept : TemplateArgument(locationRange),
                                                         beforeTypeComments(beforeTypeComments),
                                                         type(type)
    {
    }
};

class ValueTemplateArgument final : public TemplateArgument
{
public:
    Expression *value;
    explicit ValueTemplateArgument(parse::LocationRange locationRange, Expression *value) noexcept
        : TemplateArgument(locationRange),
          value(value)
    {
    }
};
}
