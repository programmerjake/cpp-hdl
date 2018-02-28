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

#include <string>

namespace parse
{
template <typename T>
struct CharProperties
{
    typedef T CharType;
    typedef typename std::char_traits<T>::int_type IntType;
    static constexpr IntType eof = std::char_traits<T>::eof();
    static constexpr bool isWhitespace(IntType ch) noexcept
    {
        switch(ch)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
        }
    }
    static constexpr bool isLineCommentTerminator(IntType ch) noexcept
    {
        switch(ch)
        {
        case eof:
        case '\r':
        case '\n':
            return true;
        default:
            return false;
        }
    }
    static constexpr bool isDigit(IntType ch) noexcept
    {
        if(ch >= '0' && ch <= '9')
            return true;
        return false;
    }
    static constexpr bool isUpper(IntType ch) noexcept
    {
        if(ch >= 'A' && ch <= 'Z')
            return true;
        return false;
    }
    static constexpr bool isLower(IntType ch) noexcept
    {
        if(ch >= 'a' && ch <= 'z')
            return true;
        return false;
    }
    static constexpr bool isIdentifierStart(IntType ch) noexcept
    {
        if(ch >= 0x80) // all multi-byte UTF-8 characters
            return true;
        if(isLower(ch))
            return true;
        if(isUpper(ch))
            return true;
        if(ch == '_')
            return true;
        return false;
    }
    static constexpr bool isIdentifierContinue(IntType ch) noexcept
    {
        if(isIdentifierStart(ch))
            return true;
        if(isDigit(ch))
            return true;
        return false;
    }
    static constexpr int getDigitValue(IntType ch) noexcept
    {
        if(ch >= '0' && ch <= '9')
            return ch - '0';
        if(ch >= 'a' && ch <= 'z')
            return ch - 'a' + 0xA;
        if(ch >= 'A' && ch <= 'Z')
            return ch - 'A' + 0xA;
        return -1;
    }
    static constexpr int getDigitValue(IntType ch, int base) noexcept
    {
        int retval = getDigitValue(ch);
        if(retval >= base)
            return -1;
        return retval;
    }
    static constexpr bool isDigitSeparator(IntType ch) noexcept
    {
        if(ch == '_' || ch == '\'')
            return true;
        return false;
    }
};

template <typename T>
constexpr typename CharProperties<T>::IntType CharProperties<T>::eof;
}
