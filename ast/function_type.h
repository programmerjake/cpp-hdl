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
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "comment.h"
#include <vector>

namespace ast
{
class FunctionType final : public Type
{
public:
    struct Parameter
    {
        ConsecutiveComments beforeNameComments;
        parse::LocationRange nameLocationRange;
        util::StringPool::Entry name;
        ConsecutiveComments beforeColonComments;
        Type *type;
        constexpr Parameter() noexcept : beforeNameComments(),
                                         nameLocationRange(),
                                         name(),
                                         beforeColonComments(),
                                         type()
        {
        }
        constexpr Parameter(ConsecutiveComments beforeNameComments,
                            parse::LocationRange nameLocationRange,
                            util::StringPool::Entry name,
                            ConsecutiveComments beforeColonComments,
                            Type *type) noexcept : beforeNameComments(beforeNameComments),
                                                   nameLocationRange(nameLocationRange),
                                                   name(name),
                                                   beforeColonComments(beforeColonComments),
                                                   type(type)
        {
        }
    };
    struct Part : public Parameter
    {
        ConsecutiveComments beforeCommaComments;
        constexpr Part(ConsecutiveComments beforeCommaComments, Parameter parameter) noexcept
            : Parameter(parameter),
              beforeCommaComments(beforeCommaComments)
        {
        }
    };
    ConsecutiveComments beforeFunctionComments;
    ConsecutiveComments beforeLParenComments;
    Parameter firstParameter;
    std::vector<Part> parts;
    ConsecutiveComments beforeRParenComments;
    ConsecutiveComments beforeColonComments;
    Type *returnType;
    explicit FunctionType(parse::LocationRange locationRange,
                          ConsecutiveComments beforeFunctionComments,
                          ConsecutiveComments beforeLParenComments,
                          Parameter firstParameter,
                          std::vector<Part> parts,
                          ConsecutiveComments beforeRParenComments,
                          ConsecutiveComments beforeColonComments,
                          Type *returnType) noexcept
        : Type(locationRange),
          beforeFunctionComments(beforeFunctionComments),
          beforeLParenComments(beforeLParenComments),
          firstParameter(firstParameter),
          parts(parts),
          beforeRParenComments(beforeRParenComments),
          beforeColonComments(beforeColonComments),
          returnType(returnType)
    {
    }
};
}
