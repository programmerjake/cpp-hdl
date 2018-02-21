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
    auto nameToken = matchToken(TokenType::Identifier, "expected module name");
    auto moduleSymbolTable = context.arena.create<ast::SymbolTable>();
    auto *retval = context.arena.create<ast::Module>(moduleToken.locationRange,
                                                     nameToken.locationRange,
                                                     context.stringPool.intern(nameToken.getText()),
                                                     ast::SymbolLookupChain(),
                                                     moduleSymbolTable);
    if(!currentSymbolTable->insert(retval))
        throw ParseError(nameToken.locationRange.begin(), "name redefinition");
    PushSymbolScope pushSymbolScope(this, moduleSymbolTable);
    retval->symbolLookupChain = currentSymbolLookupChain;
    matchToken(TokenType::LBrace, "expected '{' after module keyword in module definition");
    while(true)
    {
        switch(peek().type)
        {
        case TokenType::EndOfFile:
        case TokenType::RBrace:
            break;
        case TokenType::Module:
            retval->members.push_back(parseModule());
            continue;
        case TokenType::Bundle:
            retval->members.push_back(parseBundle());
            continue;
        case TokenType::UnprefixedDecimalLiteralInteger:
        case TokenType::DecimalLiteralInteger:
        case TokenType::HexadecimalLiteralInteger:
        case TokenType::OctalLiteralInteger:
        case TokenType::BinaryLiteralInteger:
        case TokenType::Identifier:
        case TokenType::BlockComment:
        case TokenType::LineComment:
        case TokenType::FSlash:
        case TokenType::LBrace:
#warning finish
            break;
        }
        break;
    }
    auto rBraceToken = matchToken(TokenType::RBrace, "expected '}'");
    retval->locationRange.setEnd(rBraceToken.locationRange.end());
    return retval;
}

ast::Bundle *Parser::parseBundle()
{
    auto bundleToken = matchToken(TokenType::Bundle, "expected bundle definition");
    auto nameToken = matchToken(TokenType::Identifier, "expected bundle name");
    auto bundleSymbolTable = context.arena.create<ast::SymbolTable>();
    auto *retval = context.arena.create<ast::Bundle>(bundleToken.locationRange,
                                                     nameToken.locationRange,
                                                     context.stringPool.intern(nameToken.getText()),
                                                     ast::SymbolLookupChain(),
                                                     bundleSymbolTable);
    if(!currentSymbolTable->insert(retval))
        throw ParseError(nameToken.locationRange.begin(), "name redefinition");
    PushSymbolScope pushSymbolScope(this, bundleSymbolTable);
    retval->symbolLookupChain = currentSymbolLookupChain;
    matchToken(TokenType::LBrace, "expected '{' after bundle keyword in bundle definition");
#warning finish
    auto rBraceToken = matchToken(TokenType::RBrace, "expected '}'");
    retval->locationRange.setEnd(rBraceToken.locationRange.end());
    return retval;
}

ast::Module *Parser::parseTopLevelModule()
{
    auto globalSymbolTable = context.arena.create<ast::SymbolTable>();
    PushSymbolScope pushSymbolScope(this, globalSymbolTable);
    auto module = parseModule();
    if(peek().type != TokenType::EndOfFile)
        throw ParseError(peek().locationRange.begin(), "extra tokens before end of file");
    return module;
}
