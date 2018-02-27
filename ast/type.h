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

namespace ast
{
class Type : public Node
{
public:
    bool isStateless;
    /** type after removing all the transparent type aliases; the definition of type equality */
    const Type *const canonicalType;
    explicit Type(LocationRange locationRange, bool isStateless, const Type *canonicalType)
        : Node(locationRange), isStateless(isStateless), canonicalType(canonicalType)
    {
    }
    explicit Type(LocationRange locationRange, bool isStateless)
        : Node(locationRange), isStateless(isStateless), canonicalType(this)
    {
    }
    friend bool operator==(const Type &a, const Type &b) noexcept
    {
        return a.canonicalType == b.canonicalType;
    }
    friend bool operator!=(const Type &a, const Type &b) noexcept
    {
        return a.canonicalType != b.canonicalType;
    }
    virtual const Type *getFlippedType() const noexcept = 0;
};
}
