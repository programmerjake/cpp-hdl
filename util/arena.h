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
#include <deque>
#include <utility>

namespace util
{
class Arena
{
private:
    std::deque<std::unique_ptr<void, void (*)(void *object)>> objects;

public:
    template <typename T, typename... Args>
    decltype(new T(std::declval<Args>()...)) create(Args &&... args)
    {
        objects.emplace_back(nullptr,
                             [](void *object) noexcept
                             {
                                 delete static_cast<T *>(object);
                             });
        auto &objectSlot = objects.back();
        std::unique_ptr<T> object(new T(std::forward<Args>(args)...));
        T *retval = object.get();
        objectSlot.reset(const_cast<void *>(static_cast<const void *>(object.release())));
        return retval;
    }
};
}
