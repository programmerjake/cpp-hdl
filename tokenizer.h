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

#include "source.h"
#include <cassert>
#include "string_view.h"
#include <cstdint>
#include "bit_vector.h"

enum class TokenType : std::uint_fast8_t
{
    EndOfFile = 0,

    UnprefixedDecimalLiteralInteger,
    DecimalLiteralInteger,
    HexadecimalLiteralInteger,
    OctalLiteralInteger,
    BinaryLiteralInteger,

    Module,

    FirstKeyword = Module,
    AfterLastKeyword = Module + 1ULL,

    Identifier = AfterLastKeyword,
    BlockComment,
    LineComment,

    FSlash,
    LBrace,
    RBrace,
};

struct Token final
{
    using Type = TokenType;
    static constexpr util::string_view getTypeString(Type type) noexcept
    {
        using namespace util::string_view_literals;
        switch(type)
        {
        case Type::EndOfFile:
            return "<EndOfFile>"_sv;
        case Type::UnprefixedDecimalLiteralInteger:
            return "<UnprefixedDecimalLiteralInteger>"_sv;
        case Type::DecimalLiteralInteger:
            return "<DecimalLiteralInteger>"_sv;
        case Type::HexadecimalLiteralInteger:
            return "<HexadecimalLiteralInteger>"_sv;
        case Type::OctalLiteralInteger:
            return "<OctalLiteralInteger>"_sv;
        case Type::BinaryLiteralInteger:
            return "<BinaryLiteralInteger>"_sv;
        case Type::Module:
            return "module"_sv;
        case Type::Identifier:
            return "<Identifier>"_sv;
        case Type::BlockComment:
            return "<BlockComment>"_sv;
        case Type::LineComment:
            return "<LineComment>"_sv;
        case Type::FSlash:
            return "/"_sv;
        case Type::LBrace:
            return "{"_sv;
        case Type::RBrace:
            return "}"_sv;
        }
        return {};
    }
    constexpr util::string_view getTypeString() const noexcept
    {
        return getTypeString(type);
    }
    Type type;
    LocationRange locationRange;
    util::string_view getText() const noexcept
    {
        return locationRange.getText();
    }
    constexpr Token() noexcept : type(Type::EndOfFile), locationRange()
    {
    }
    constexpr Token(Type type, LocationRange locationRange) noexcept : type(type),
                                                                       locationRange(locationRange)
    {
    }
    GMPInteger getValue() const;
    static constexpr bool isComment(Type type) noexcept
    {
        switch(type)
        {
        case Type::BlockComment:
        case Type::LineComment:
            return true;
        default:
            return false;
        }
    }
    constexpr bool isComment() const noexcept
    {
        return isComment(type);
    }
};

class Tokenizer final
{
    friend GMPInteger Token::getValue() const;

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
