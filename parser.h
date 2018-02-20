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

#include "ast.h"
#include "arena.h"
#include "tokenizer.h"
#include "parse_error.h"
#include "string_view.h"

class Parser final
{
private:
    Tokenizer tokenizer;
    Arena &arena;

private:
    Parser(Tokenizer tokenizer, Arena &arena) : tokenizer(tokenizer), arena(arena)
    {
    }

private:
    void skipComments()
    {
        while(tokenizer.peek().isComment())
            tokenizer.get();
    }
    Token peek()
    {
        skipComments();
        return tokenizer.peek();
    }
    Token get()
    {
        skipComments();
        return tokenizer.get();
    }
    Token matchTokenNoGet(TokenType type, util::string_view errorMessage)
    {
        auto retval = peek();
        if(retval.type != type)
            throw ParseError(retval.locationRange.begin(), errorMessage);
        return retval;
    }
    Token matchToken(TokenType type, util::string_view errorMessage)
    {
        matchTokenNoGet(type, errorMessage);
        return get();
    }

private:
    ast::Module *parseModule();

private:
    ast::Module *parseTopLevelModule();

public:
    static ast::Module *parseTopLevelModule(Tokenizer tokenizer, Arena &arena)
    {
        return Parser(tokenizer, arena).parseTopLevelModule();
    }
};
