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

#include <memory>
#include <vector>
#include <utility>

class Arena
{
private:
    struct ObjectBase
    {
        virtual ~ObjectBase() = default;
    };
    struct ConstructObjectTag
    {
        explicit ConstructObjectTag() = default;
    };
    template <typename T>
    struct Object final : public ObjectBase
    {
        T value;
        template <typename... Args>
        Object(ConstructObjectTag, Args &&... args)
            : value(std::forward<Args>(args)...)
        {
        }
    };
    std::vector<std::unique_ptr<ObjectBase>> objects;

public:
    template <typename T, typename... Args>
    T *create(Args &&... args)
    {
        objects.emplace_back();
        std::unique_ptr<Object<T>> object(
            new Object<T>(ConstructObjectTag{}, std::forward<Args>(args)...));
        T *retval = std::addressof(object->value);
        objects.back() = std::move(object);
        return retval;
    }
};
