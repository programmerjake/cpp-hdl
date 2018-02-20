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
#include <type_traits>
#include "parse_error.h"

struct Tokenizer::TokenParser
{
    Location currentLocation;
    util::string_view sourceText;
    TokenParser(Location currentLocation) noexcept
        : currentLocation(currentLocation),
          sourceText(currentLocation.getNonnullSource()->text())
    {
    }
    static constexpr int eof = -1;
    bool atEnd() const noexcept
    {
        return currentLocation.offset >= sourceText.size();
    }
    int peek() const noexcept
    {
        if(atEnd())
            return eof;
        return static_cast<unsigned char>(sourceText[currentLocation.offset]);
    }
    int get() noexcept
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
            bool hasDigits = true;
            int base = 10;
            if(peek() == '0')
            {
                get();
                if(peek() == 'b' || peek() == 'B')
                {
                    hasDigits = false;
                    tokenType = TokenType::BinaryLiteralInteger;
                    get();
                    base = 2;
                }
                else if(peek() == 'h' || peek() == 'H' || peek() == 'x' || peek() == 'X')
                {
                    hasDigits = false;
                    tokenType = TokenType::HexadecimalLiteralInteger;
                    get();
                    base = 16;
                }
                else if(peek() == 'o' || peek() == 'O')
                {
                    hasDigits = false;
                    tokenType = TokenType::OctalLiteralInteger;
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
            while(getDigitValue(peek(), base) >= 0)
            {
                get();
                hasDigits = true;
            }
            if(getDigitValue(peek()) >= base)
                throw ParseError(currentLocation, "digit too big for number");
            if(!hasDigits)
                throw ParseError(currentLocation, "number is missing digits after base indicator");
            return Token(tokenType, LocationRange(startLocation, currentLocation));
        }
        throw ParseError(currentLocation, "illegal character");
    }
};

GMPInteger Token::getValue() const
{
    util::string_view text = getText();
    int base = 10;
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
    default:
        assert(false);
        return {};
    }
    GMPInteger value(0UL);
    for(unsigned char ch : text)
    {
        int digitValue = Tokenizer::TokenParser::getDigitValue(ch, base);
        mpz_mul_ui(value, value, base);
        if(digitValue > 0)
            mpz_add_ui(value, value, digitValue);
    }
    return value;
}

Token Tokenizer::parseToken(Location &currentLocation)
{
    TokenParser tokenParser(currentLocation);
    auto retval = tokenParser.parseToken();
    currentLocation = tokenParser.currentLocation;
    return retval;
}
