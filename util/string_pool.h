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

#include <unordered_map>
#include <string>
#include "string_view.h"
#include <cassert>
#include <utility>

namespace util
{
class StringPoolEntry;
}

namespace std
{
template <>
struct hash<util::StringPoolEntry>
{
    std::size_t operator()(const util::StringPoolEntry &entry) const noexcept;
};
}

namespace util
{
class StringPool;

class StringPoolEntry final
{
    friend class StringPool;
    friend std::size_t std::hash<util::StringPoolEntry>::operator()(
        const util::StringPoolEntry &entry) const noexcept;

private:
    const std::string *value;

private:
    constexpr explicit StringPoolEntry(const std::string *value) noexcept : value(value)
    {
    }

public:
    constexpr StringPoolEntry() noexcept : value(nullptr)
    {
    }
    constexpr explicit operator bool() const noexcept
    {
        return value != nullptr;
    }
    const std::string &operator*() const noexcept
    {
        assert(operator bool());
        return *value;
    }
    const std::string *operator->() const noexcept
    {
        return value;
    }
    constexpr friend bool operator==(StringPoolEntry a, StringPoolEntry b) noexcept
    {
        return a.value == b.value;
    }
    constexpr friend bool operator!=(StringPoolEntry a, StringPoolEntry b) noexcept
    {
        return a.value != b.value;
    }
};

class StringPool final
{
private:
    std::unordered_multimap<std::size_t, std::string> strings;

public:
    using Entry = StringPoolEntry;
    Entry intern(string_view str)
    {
        auto hash = std::hash<string_view>()(str);
        auto range = strings.equal_range(hash);
        for(auto i = std::get<0>(range); i != std::get<1>(range); ++i)
            if(std::get<1>(*i) == str)
                return Entry(&std::get<1>(*i));
        auto iter = strings.emplace(hash, static_cast<std::string>(str));
        return Entry(&std::get<1>(*iter));
    }
};
}

namespace std
{
inline std::size_t hash<util::StringPoolEntry>::operator()(const util::StringPoolEntry &entry) const
    noexcept
{
    return hash<const std::string *>()(entry.value);
}
}
