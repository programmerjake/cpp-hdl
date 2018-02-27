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
#include "../source.h"
#include "../util/arena.h"

namespace ast
{
class TemplateParameterKind : public Node
{
public:
    bool isList;
    enum class Kind
    {
        Value,
        Module,
    };
    const Kind kind;
    TemplateParameterKind(LocationRange locationRange, bool isList, Kind kind)
        : Node(locationRange), isList(isList), kind(kind)
    {
    }
    virtual std::size_t getHash() const noexcept = 0;
    virtual bool isSame(const TemplateParameterKind &other) const noexcept = 0;
    virtual TemplateParameterKind *duplicate(util::Arena &arena) const = 0;
};

inline std::size_t TemplateParameterKind::getHash() const noexcept
{
    auto retval = static_cast<std::size_t>(kind);
    retval *= 2;
    retval += isList;
    return retval;
}

inline bool TemplateParameterKind::isSame(const TemplateParameterKind &other) const noexcept
{
    return isList == other.isList && kind == other.kind;
}
}


