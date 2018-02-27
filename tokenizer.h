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

#include <cstdint>
#include "util/string_view.h"
#include "source.h"
#include "math/bit_vector.h"

enum class TokenType : std::uint_fast8_t
{
    EndOfFile = 0,

    UnprefixedDecimalLiteralInteger,
    DecimalLiteralInteger,
    HexadecimalLiteralInteger,
    OctalLiteralInteger,
    BinaryLiteralInteger,

    Bit,
    Bundle,
    Input,
    Module,
    Output,
    Reg,
    S16,
    S32,
    S64,
    S8,
    SInt,
    U16,
    U32,
    U64,
    U8,
    UInt,

    FirstKeyword = Bit,
    AfterLastKeyword = UInt + 1ULL,

    Identifier = AfterLastKeyword,
    BlockComment,
    LineComment,

    FSlash,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Comma,
    Colon,
    Semicolon,
    Tilde,
    EMark,
    Percent,
    Caret,
    Amp,
    Star,
    Minus,
    Equal,
    Plus,
    VBar,
    Dot,
    LAngle,
    RAngle,
    QMark,

    ColonColon,
    DotDotDot,
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
        case Type::Bit:
            return "bit"_sv;
        case Type::Bundle:
            return "bundle"_sv;
        case Type::Input:
            return "input"_sv;
        case Type::Module:
            return "module"_sv;
        case Type::Output:
            return "output"_sv;
        case Type::Reg:
            return "reg"_sv;
        case Type::S16:
            return "s16"_sv;
        case Type::S32:
            return "s32"_sv;
        case Type::S64:
            return "s64"_sv;
        case Type::S8:
            return "s8"_sv;
        case Type::SInt:
            return "sint"_sv;
        case Type::U16:
            return "u16"_sv;
        case Type::U32:
            return "u32"_sv;
        case Type::U64:
            return "u64"_sv;
        case Type::U8:
            return "u8"_sv;
        case Type::UInt:
            return "uint"_sv;
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
        case Type::LBracket:
            return "["_sv;
        case Type::RBracket:
            return "]"_sv;
        case Type::LParen:
            return "("_sv;
        case Type::RParen:
            return ")"_sv;
        case Type::Comma:
            return ","_sv;
        case Type::Colon:
            return ":"_sv;
        case Type::Semicolon:
            return ";"_sv;
        case Type::Tilde:
            return "~"_sv;
        case Type::EMark:
            return "!"_sv;
        case Type::Percent:
            return "%"_sv;
        case Type::Caret:
            return "^"_sv;
        case Type::Amp:
            return "&"_sv;
        case Type::Star:
            return "*"_sv;
        case Type::Minus:
            return "-"_sv;
        case Type::Equal:
            return "="_sv;
        case Type::Plus:
            return "+"_sv;
        case Type::VBar:
            return "|"_sv;
        case Type::Dot:
            return "."_sv;
        case Type::LAngle:
            return "<"_sv;
        case Type::RAngle:
            return ">"_sv;
        case Type::QMark:
            return "?"_sv;
        case Type::ColonColon:
            return "::"_sv;
        case Type::DotDotDot:
            return "..."_sv;
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
    math::GMPInteger getValue() const;
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
