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

#include "parser.h"

ast::Module *Parser::parseModule()
{
    auto moduleToken = matchToken(TokenType::Module, "expected module definition");
    auto *retval = arena.create<ast::Module>(moduleToken.locationRange);
    matchToken(TokenType::LBrace, "expected '{' after module keyword in module definition");
    auto rBraceToken = matchToken(TokenType::RBrace, "expected '}'");
    retval->locationRange.setEnd(rBraceToken.locationRange.end());
    return retval;
}

ast::Module *Parser::parseTopLevelModule()
{
    auto module = parseModule();
    if(peek().type != TokenType::EndOfFile)
        throw ParseError(peek().locationRange.begin(), "extra tokens before end of file");
    return module;
}
