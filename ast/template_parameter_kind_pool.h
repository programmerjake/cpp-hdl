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

#include "template_parameter_kind.h"
#include "../util/arena.h"
#include <unordered_set>
#include <type_traits>

namespace ast
{
class TemplateParameterKindPool final
{
private:
    struct TemplateParameterKindEquals
    {
        bool operator()(const TemplateParameterKind *a, const TemplateParameterKind *b) const
            noexcept
        {
            if(!a)
                return !b;
            return b && a->isSame(*b);
        }
    };
    struct TemplateParameterKindHash
    {
        std::size_t operator()(const TemplateParameterKind *a) const noexcept
        {
            if(!a)
                return -1;
            return a->getHash();
        }
    };

private:
    util::Arena arena;
    std::unordered_set<const TemplateParameterKind *,
                       TemplateParameterKindHash,
                       TemplateParameterKindEquals> values;

private:
    const TemplateParameterKind *internImplementation(const TemplateParameterKind &value)
    {
        auto iter = values.find(&value);
        if(iter == values.end())
        {
            auto result = values.insert(value.duplicate(arena));
            assert(std::get<1>(result));
            iter = std::get<0>(result);
        }
        return *iter;
    }

public:
    template <typename T>
    const T *intern(const T &value)
    {
        static_assert(std::is_base_of<TemplateParameterKind, T>::value, "");
        auto *retval = internImplementation(value);
        assert(dynamic_cast<const T *>(retval));
        return static_cast<const T *>(retval);
    }
};
}


