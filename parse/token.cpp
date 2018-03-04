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

#include "token.h"
#include <type_traits>
#include "parse_error.h"
#include "character_properties.h"

namespace parse
{
Token::IntegerValue Token::getIntegerValue() const
{
    util::string_view text = getText();
    int base = 10;
    bool isPattern = false;
    switch(type)
    {
    case Type::UnprefixedDecimalLiteralInteger:
        base = 10;
        break;
    case Type::DecimalLiteralInteger:
        text.remove_prefix(2);
        base = 10;
        break;
    case Type::HexadecimalLiteralInteger:
        text.remove_prefix(2);
        base = 16;
        break;
    case Type::OctalLiteralInteger:
        text.remove_prefix(2);
        base = 8;
        break;
    case Type::BinaryLiteralInteger:
        text.remove_prefix(2);
        base = 2;
        break;
    case Type::HexadecimalLiteralIntegerPattern:
        text.remove_prefix(2);
        base = 16;
        isPattern = true;
        break;
    case Type::OctalLiteralIntegerPattern:
        text.remove_prefix(2);
        base = 8;
        isPattern = true;
        break;
    case Type::BinaryLiteralIntegerPattern:
        text.remove_prefix(2);
        base = 2;
        isPattern = true;
        break;
    default:
        assert(false);
        return {};
    }
    math::GMPInteger value(0UL);
    math::GMPInteger mask(-1L);
    for(unsigned char ch : text)
    {
        int digitValue = CharProperties<char>::getDigitValue(ch, base);
        bool isWildcard = digitValue < 0;
        mpz_mul_ui(value, value, base);
        if(!isWildcard && digitValue != 0)
            mpz_add_ui(value, value, digitValue);
        if(isPattern)
        {
            mpz_mul_ui(value, value, base);
            if(!isWildcard)
                mpz_add_ui(value, value, base - 1);
        }
    }
    return {std::move(value), std::move(mask)};
}
}
