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

#include "../parse/source.h"
#include <iterator>
#include "../parse/token.h"
#include <ostream>

namespace ast
{
struct ConsecutiveComments
{
    parse::LocationRange locationRange;
    class iterator
    {
        friend struct ConsecutiveComments;

    private:
        class TokenWrapper final
        {
            friend class iterator;

        private:
            parse::Token value;

        private:
            constexpr explicit TokenWrapper(parse::Token value) noexcept : value(value)
            {
            }

        public:
            const parse::Token *operator->() const noexcept
            {
                return &value;
            }
            const parse::Token &operator*() const noexcept
            {
                return value;
            }
        };

    private:
        parse::Location location;
        mutable parse::Token value;
        mutable parse::Location nextLocation;

    private:
        constexpr explicit iterator(parse::Location location) noexcept : location(location), value()
        {
        }
        void parseValue() const noexcept;

    public:
        constexpr iterator() noexcept : location(), value()
        {
        }
        typedef std::input_iterator_tag iterator_category;
        typedef parse::Token value_type;
        typedef std::ptrdiff_t difference_type;
        typedef const parse::Token *pointer;
        typedef const parse::Token &reference;
        friend bool operator==(const iterator &l, const iterator &r) noexcept
        {
            return l.location.offset == r.location.offset;
        }
        friend bool operator!=(const iterator &l, const iterator &r) noexcept
        {
            return !operator==(l, r);
        }
        const parse::Token *operator->() const noexcept
        {
            if(!value.locationRange)
                parseValue();
            return &value;
        }
        const parse::Token &operator*() const noexcept
        {
            return *operator->();
        }
        iterator &operator++() noexcept
        {
            if(!value.locationRange)
                parseValue();
            value = parse::Token();
            location = nextLocation;
            return *this;
        }
        TokenWrapper operator++(int) noexcept
        {
            if(!value.locationRange)
                parseValue();
            auto retval = value;
            value = parse::Token();
            location = nextLocation;
            return TokenWrapper(retval);
        }
    };
    typedef iterator const_iterator;
    constexpr iterator begin() const noexcept
    {
        return iterator(locationRange.begin());
    }
    constexpr iterator end() const noexcept
    {
        return iterator(locationRange.end());
    }
    constexpr ConsecutiveComments() noexcept : locationRange()
    {
    }
    constexpr explicit ConsecutiveComments(parse::LocationRange locationRange) noexcept
        : locationRange(locationRange)
    {
    }
    friend std::ostream &operator<<(std::ostream &os, const ConsecutiveComments &value)
    {
        if(!value.locationRange)
        {
            os << "{<nullptr>}";
            return os;
        }
        os << "{";
        auto separator = "";
        for(auto &token : value)
        {
            os << separator;
            os << token.locationRange;
            separator = ", ";
        }
        os << "}";
        return os;
    }
};
}
