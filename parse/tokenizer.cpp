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

#include "tokenizer.h"
#include "character_properties.h"
#include <type_traits>
#include "parse_error.h"

namespace parse
{
struct Tokenizer::TokenParser
{
    typedef char CharType;
    typedef typename CharProperties<CharType>::IntType IntType;
    Location currentLocation;
    util::string_view sourceText;
    static constexpr auto eof = CharProperties<CharType>::eof;
    TokenParser(Location currentLocation) noexcept
        : currentLocation(currentLocation),
          sourceText(currentLocation.getNonnullSource()->text())
    {
    }
    bool atEnd() const noexcept
    {
        return currentLocation.offset >= sourceText.size();
    }
    IntType peek() const noexcept
    {
        if(atEnd())
            return eof;
        return static_cast<unsigned char>(sourceText[currentLocation.offset]);
    }
    IntType get() noexcept
    {
        if(atEnd())
            return eof;
        return static_cast<unsigned char>(sourceText[currentLocation.offset++]);
    }
    static constexpr bool isWhitespace(int ch) noexcept
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
    static constexpr bool isLineCommentTerminator(int ch) noexcept
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
    static constexpr bool isDigit(int ch) noexcept
    {
        if(ch >= '0' && ch <= '9')
            return true;
        return false;
    }
    static constexpr bool isUpper(int ch) noexcept
    {
        if(ch >= 'A' && ch <= 'Z')
            return true;
        return false;
    }
    static constexpr bool isLower(int ch) noexcept
    {
        if(ch >= 'a' && ch <= 'z')
            return true;
        return false;
    }
    static constexpr bool isIdentifierStart(int ch) noexcept
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
    static constexpr bool isIdentifierContinue(int ch) noexcept
    {
        if(isIdentifierStart(ch))
            return true;
        if(isDigit(ch))
            return true;
        return false;
    }
    static constexpr int getDigitValue(int ch) noexcept
    {
        if(ch >= '0' && ch <= '9')
            return ch - '0';
        if(ch >= 'a' && ch <= 'z')
            return ch - 'a' + 0xA;
        if(ch >= 'A' && ch <= 'Z')
            return ch - 'A' + 0xA;
        return -1;
    }
    static constexpr int getDigitValue(int ch, int base) noexcept
    {
        int retval = getDigitValue(ch);
        if(retval >= base)
            return -1;
        return retval;
    }
    static constexpr bool isDigitSeparator(int ch) noexcept
    {
        if(ch == '_' || ch == '\'')
            return true;
        return false;
    }
    Token parseToken()
    {
        while(isWhitespace(peek()) || peek() == '/')
        {
            while(isWhitespace(peek()))
                get();
            if(peek() == '/')
            {
                Location startLocation = currentLocation;
                get();
                if(peek() == '/')
                {
                    while(!isLineCommentTerminator(peek()))
                        get();
                    return Token(TokenType::LineComment,
                                 LocationRange(startLocation, currentLocation));
                }
                if(peek() == '*')
                {
                    get();
                    while(peek() != eof)
                    {
                        if(get() == '*')
                        {
                            if(peek() == '/')
                            {
                                get();
                                return Token(TokenType::BlockComment,
                                             LocationRange(startLocation, currentLocation));
                            }
                        }
                    }
                    throw ParseError(startLocation, "block comment is missing closing */");
                }
                return Token(TokenType::FSlash, LocationRange(startLocation, currentLocation));
            }
        }
        if(atEnd())
        {
            auto retval = Token(TokenType::EndOfFile, LocationRange(currentLocation, 0));
            currentLocation = {};
            return retval;
        }
        Location startLocation = currentLocation;
        if(isIdentifierStart(peek()))
        {
            do
            {
                get();
            } while(isIdentifierContinue(peek()));
            LocationRange locationRange(startLocation, currentLocation);
            auto tokenText = locationRange.getText();
            typedef std::underlying_type_t<TokenType> TokenUnderlyingType;
            auto tokenType = TokenType::Identifier;
            for(auto i = static_cast<TokenUnderlyingType>(TokenType::FirstKeyword);
                i < static_cast<TokenUnderlyingType>(TokenType::AfterLastKeyword);
                i++)
            {
                if(tokenText == Token::getTypeString(static_cast<TokenType>(i)))
                {
                    tokenType = static_cast<TokenType>(i);
                    break;
                }
            }
            return Token(tokenType, locationRange);
        }
        if(isDigit(peek()))
        {
            auto tokenType = TokenType::UnprefixedDecimalLiteralInteger;
            TokenType patternTokenType{};
            bool hasDigits = true;
            int base = 10;
            bool patternAllowed = false;
            bool isPattern = false;
            if(peek() == '0')
            {
                get();
                if(peek() == 'b' || peek() == 'B')
                {
                    hasDigits = false;
                    tokenType = TokenType::BinaryLiteralInteger;
                    patternAllowed = true;
                    patternTokenType = TokenType::BinaryLiteralIntegerPattern;
                    get();
                    base = 2;
                }
                else if(peek() == 'h' || peek() == 'H' || peek() == 'x' || peek() == 'X')
                {
                    hasDigits = false;
                    tokenType = TokenType::HexadecimalLiteralInteger;
                    patternAllowed = true;
                    patternTokenType = TokenType::HexadecimalLiteralIntegerPattern;
                    get();
                    base = 16;
                }
                else if(peek() == 'o' || peek() == 'O')
                {
                    hasDigits = false;
                    tokenType = TokenType::OctalLiteralInteger;
                    patternAllowed = true;
                    patternTokenType = TokenType::OctalLiteralIntegerPattern;
                    get();
                    base = 8;
                }
                else if(peek() == 'd' || peek() == 'D')
                {
                    hasDigits = false;
                    tokenType = TokenType::DecimalLiteralInteger;
                    get();
                    base = 10;
                }
                else if(isDigit(peek()))
                {
                    throw ParseError(currentLocation,
                                     "number must not have leading zeros (for octal, use '0o377')");
                }
                else
                {
                    return Token(tokenType, LocationRange(startLocation, currentLocation));
                }
            }
            constexpr IntType wildcardChar = '?';
            while(getDigitValue(peek(), base) >= 0 || peek() == wildcardChar)
            {
                if(get() == wildcardChar)
                {
                    if(!patternAllowed)
                        throw ParseError(currentLocation,
                                         "wildcard is not legal in decimal integer");
                    isPattern = true;
                }
                hasDigits = true;
            }
            if(getDigitValue(peek()) >= base)
                throw ParseError(currentLocation, "digit too big for number");
            if(!hasDigits)
                throw ParseError(currentLocation, "number is missing digits after base indicator");
            if(isPattern)
                tokenType = patternTokenType;
            return Token(tokenType, LocationRange(startLocation, currentLocation));
        }
        switch(peek())
        {
        case '{':
            get();
            return Token(TokenType::LBrace, LocationRange(startLocation, currentLocation));
        case '}':
            get();
            return Token(TokenType::RBrace, LocationRange(startLocation, currentLocation));
        case '[':
            get();
            return Token(TokenType::LBracket, LocationRange(startLocation, currentLocation));
        case ']':
            get();
            return Token(TokenType::RBracket, LocationRange(startLocation, currentLocation));
        case '(':
            get();
            return Token(TokenType::LParen, LocationRange(startLocation, currentLocation));
        case ')':
            get();
            return Token(TokenType::RParen, LocationRange(startLocation, currentLocation));
        case ',':
            get();
            return Token(TokenType::Comma, LocationRange(startLocation, currentLocation));
        case ':':
            get();
            if(peek() == ':')
            {
                get();
                return Token(TokenType::ColonColon, LocationRange(startLocation, currentLocation));
            }
            return Token(TokenType::Colon, LocationRange(startLocation, currentLocation));
        case ';':
            get();
            return Token(TokenType::Semicolon, LocationRange(startLocation, currentLocation));
        case '~':
            get();
            return Token(TokenType::Tilde, LocationRange(startLocation, currentLocation));
        case '!':
            get();
            return Token(TokenType::EMark, LocationRange(startLocation, currentLocation));
        case '%':
            get();
            return Token(TokenType::Percent, LocationRange(startLocation, currentLocation));
        case '^':
            get();
            return Token(TokenType::Caret, LocationRange(startLocation, currentLocation));
        case '&':
            get();
            return Token(TokenType::Amp, LocationRange(startLocation, currentLocation));
        case '*':
            get();
            return Token(TokenType::Star, LocationRange(startLocation, currentLocation));
        case '-':
            get();
            return Token(TokenType::Minus, LocationRange(startLocation, currentLocation));
        case '=':
            get();
            if(peek() == '>')
            {
                get();
                return Token(TokenType::EqualRAngle, LocationRange(startLocation, currentLocation));
            }
            return Token(TokenType::Equal, LocationRange(startLocation, currentLocation));
        case '+':
            get();
            return Token(TokenType::Plus, LocationRange(startLocation, currentLocation));
        case '|':
            get();
            return Token(TokenType::VBar, LocationRange(startLocation, currentLocation));
        case '.':
            get();
            if(peek() == '.')
            {
                auto afterFirstDot = currentLocation;
                get();
                if(peek() == '.')
                {
                    get();
                    return Token(TokenType::DotDotDot,
                                 LocationRange(startLocation, currentLocation));
                }
                currentLocation = afterFirstDot;
            }
            return Token(TokenType::Dot, LocationRange(startLocation, currentLocation));
        case '<':
            get();
            return Token(TokenType::LAngle, LocationRange(startLocation, currentLocation));
        case '>':
            get();
            return Token(TokenType::RAngle, LocationRange(startLocation, currentLocation));
        case '?':
            get();
            return Token(TokenType::QMark, LocationRange(startLocation, currentLocation));
        }
        throw ParseError(currentLocation, "illegal character");
    }
};

Token Tokenizer::parseToken(Location &currentLocation)
{
    TokenParser tokenParser(currentLocation);
    auto retval = tokenParser.parseToken();
    currentLocation = tokenParser.currentLocation;
    return retval;
}
}
