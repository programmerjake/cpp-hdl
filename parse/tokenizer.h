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

#include "../math/bit_vector.h"
#include "token.h"
#include "source.h"

namespace parse
{
class Tokenizer final
{
    friend math::GMPInteger Token::getValue() const;

private:
    struct TokenParser;

private:
    Location currentLocation;
    Token currentToken;

public:
    constexpr explicit Tokenizer(const Source *source) noexcept : currentLocation(source, 0),
                                                                  currentToken()
    {
    }
    static Token parseToken(Location &currentLocation);
    Token peek()
    {
        if(!currentToken.locationRange)
            currentToken = parseToken(currentLocation);
        return currentToken;
    }
    Token get()
    {
        auto retval = peek();
        currentToken = {};
        return retval;
    }
    operator bool()
    {
        return peek().locationRange ? true : false;
    }
};
}
