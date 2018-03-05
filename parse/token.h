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
#include "../util/string_view.h"
#include "source.h"
#include "../math/bit_vector.h"

namespace parse
{
enum class TokenType : std::uint_fast8_t
{
    EndOfFile = 0,

    UnprefixedDecimalLiteralInteger,
    DecimalLiteralInteger,
    HexadecimalLiteralInteger,
    OctalLiteralInteger,
    BinaryLiteralInteger,
    HexadecimalLiteralIntegerPattern,
    OctalLiteralIntegerPattern,
    BinaryLiteralIntegerPattern,

    Bit,
    Break,
    Cast,
    Cat,
    Const,
    Continue,
    Else,
    Enum,
    Fill,
    Flip,
    For,
    Function,
    If,
    Implements,
    Import,
    In,
    Input,
    Interface,
    Let,
    Match,
    Memory,
    Module,
    Output,
    PopCount,
    Reg,
    Return,
    S16,
    S32,
    S64,
    S8,
    SInt,
    To,
    Type,
    TypeOf,
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
    EqualRAngle,
    LAngleLAngle,
    RAngleRAngle,
    LAngleEqual,
    RAngleEqual,
    EqualEqual,
    EMarkEqual,
    AmpAmp,
    VBarVBar,
    LAngleMinusRAngle,
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
        case Type::HexadecimalLiteralIntegerPattern:
            return "<HexadecimalLiteralIntegerPattern>"_sv;
        case Type::OctalLiteralIntegerPattern:
            return "<OctalLiteralIntegerPattern>"_sv;
        case Type::BinaryLiteralIntegerPattern:
            return "<BinaryLiteralIntegerPattern>"_sv;
        case Type::Bit:
            return "bit"_sv;
        case Type::Break:
            return "break"_sv;
        case Type::Cast:
            return "cast"_sv;
        case Type::Cat:
            return "cat"_sv;
        case Type::Const:
            return "const"_sv;
        case Type::Continue:
            return "continue"_sv;
        case Type::Else:
            return "else"_sv;
        case Type::Enum:
            return "enum"_sv;
        case Type::Fill:
            return "fill"_sv;
        case Type::Flip:
            return "flip"_sv;
        case Type::For:
            return "for"_sv;
        case Type::Function:
            return "function"_sv;
        case Type::If:
            return "if"_sv;
        case Type::Implements:
            return "implements"_sv;
        case Type::Import:
            return "import"_sv;
        case Type::In:
            return "in"_sv;
        case Type::Input:
            return "input"_sv;
        case Type::Interface:
            return "interface"_sv;
        case Type::Let:
            return "let"_sv;
        case Type::Match:
            return "match"_sv;
        case Type::Memory:
            return "memory"_sv;
        case Type::Module:
            return "module"_sv;
        case Type::Output:
            return "output"_sv;
        case Type::PopCount:
            return "popCount"_sv;
        case Type::Reg:
            return "reg"_sv;
        case Type::Return:
            return "return"_sv;
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
        case Type::To:
            return "to"_sv;
        case Type::Type:
            return "type"_sv;
        case Type::TypeOf:
            return "typeOf"_sv;
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
        case Type::EqualRAngle:
            return "=>"_sv;
        case Type::LAngleLAngle:
            return "<<"_sv;
        case Type::RAngleRAngle:
            return ">>"_sv;
        case Type::LAngleEqual:
            return "<="_sv;
        case Type::RAngleEqual:
            return ">="_sv;
        case Type::EqualEqual:
            return "=="_sv;
        case Type::EMarkEqual:
            return "!="_sv;
        case Type::AmpAmp:
            return "&&"_sv;
        case Type::VBarVBar:
            return "||"_sv;
        case Type::LAngleMinusRAngle:
            return "<->"_sv;
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
    struct IntegerValue
    {
        math::GMPInteger value;
        math::GMPInteger mask;
        IntegerValue() noexcept : value(), mask()
        {
        }
        IntegerValue(math::GMPInteger value, math::GMPInteger mask) noexcept
            : value(std::move(value)),
              mask(std::move(mask))
        {
        }
        IntegerValue(math::GMPInteger value) noexcept : value(std::move(value)), mask(-1L)
        {
        }
    };
    IntegerValue getIntegerValue() const;
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
    static constexpr bool isInteger(Type type) noexcept
    {
        switch(type)
        {
        case Type::UnprefixedDecimalLiteralInteger:
        case Type::DecimalLiteralInteger:
        case Type::HexadecimalLiteralInteger:
        case Type::OctalLiteralInteger:
        case Type::BinaryLiteralInteger:
            return true;
        default:
            return false;
        }
    }
    constexpr bool isInteger() const noexcept
    {
        return isInteger(type);
    }
    static constexpr bool isIntegerPattern(Type type) noexcept
    {
        switch(type)
        {
        case Type::HexadecimalLiteralIntegerPattern:
        case Type::OctalLiteralIntegerPattern:
        case Type::BinaryLiteralIntegerPattern:
            return true;
        default:
            return false;
        }
    }
    constexpr bool isIntegerPattern() const noexcept
    {
        return isIntegerPattern(type);
    }
};
}
