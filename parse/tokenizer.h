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
#include "../ast/comment.h"

namespace parse
{
class Tokenizer final
{
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
    explicit operator bool()
    {
        return peek().locationRange ? true : false;
    }
};

class CommentGroupingTokenizer final
{
public:
    struct CommentsAndToken final
    {
        ast::ConsecutiveComments comments;
        Token token;
        constexpr CommentsAndToken() noexcept : comments(), token()
        {
        }
        constexpr CommentsAndToken(ast::ConsecutiveComments comments, Token token) noexcept
            : comments(comments),
              token(token)
        {
        }
    };

private:
    Location currentLocation;
    CommentsAndToken currentToken;

public:
    constexpr explicit CommentGroupingTokenizer(const Source *source) noexcept
        : currentLocation(source, 0),
          currentToken()
    {
    }
    static CommentsAndToken parseToken(Location &currentLocation)
    {
        auto token = Tokenizer::parseToken(currentLocation);
        ast::ConsecutiveComments comments(LocationRange(token.locationRange.begin()));
        while(token.isComment())
        {
            comments.locationRange.setEnd(token.locationRange.end());
            token = Tokenizer::parseToken(currentLocation);
        }
        return {comments, token};
    }
    CommentsAndToken peek()
    {
        if(!currentToken.token.locationRange)
            currentToken = parseToken(currentLocation);
        return currentToken;
    }
    CommentsAndToken get()
    {
        auto retval = peek();
        currentToken = {};
        return retval;
    }
    explicit operator bool()
    {
        return peek().token.locationRange ? true : false;
    }
};
}
