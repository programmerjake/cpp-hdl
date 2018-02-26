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
#include "ast/bit_vector_type.h"

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
        case TokenType::Bit:
        case TokenType::Input:
        case TokenType::Output:
        case TokenType::Reg:
        case TokenType::S16:
        case TokenType::S32:
        case TokenType::S64:
        case TokenType::S8:
        case TokenType::SInt:
        case TokenType::U16:
        case TokenType::U32:
        case TokenType::U64:
        case TokenType::U8:
        case TokenType::UInt:
        case TokenType::Identifier:
        case TokenType::BlockComment:
        case TokenType::LineComment:
        case TokenType::FSlash:
        case TokenType::LBrace:
        case TokenType::LBracket:
        case TokenType::RBracket:
        case TokenType::LParen:
        case TokenType::RParen:
        case TokenType::Comma:
        case TokenType::Colon:
        case TokenType::Semicolon:
        case TokenType::Tilde:
        case TokenType::EMark:
        case TokenType::Percent:
        case TokenType::Caret:
        case TokenType::Amp:
        case TokenType::Star:
        case TokenType::Minus:
        case TokenType::Equal:
        case TokenType::Plus:
        case TokenType::VBar:
        case TokenType::Dot:
        case TokenType::LAngle:
        case TokenType::RAngle:
        case TokenType::QMark:
        case TokenType::ColonColon:
            // TODO: finish
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
                                                     bundleSymbolTable,
                                                     context);
    if(!currentSymbolTable->insert(retval))
        throw ParseError(nameToken.locationRange.begin(), "name redefinition");
    PushSymbolScope pushSymbolScope(this, bundleSymbolTable);
    retval->symbolLookupChain = currentSymbolLookupChain;
    matchToken(TokenType::LBrace, "expected '{' after bundle keyword in bundle definition");
    std::vector<ast::Variable *> members;
    // TODO: finish
    while(peek().type == TokenType::Identifier)
    {
        auto newVariables = parseVariables();
        members.insert(members.end(), newVariables.begin(), newVariables.end());
    }
    auto rBraceToken = matchToken(TokenType::RBrace, "expected '}'");
    retval->locationRange.setEnd(rBraceToken.locationRange.end());
    retval->define(std::move(members));
    return retval;
}

std::vector<ast::Variable *> Parser::parseVariables()
{
    std::vector<ast::Variable *> retval;
    while(true)
    {
        auto variableName = matchToken(TokenType::Identifier, "expected variable name");
        auto *variable =
            context.arena.create<ast::Variable>(variableName.locationRange,
                                                variableName.locationRange,
                                                context.stringPool.intern(variableName.getText()),
                                                static_cast<const ast::Type *>(nullptr));
        if(!currentSymbolTable->insert(variable))
            throw ParseError(variableName.locationRange.begin(), "name redefinition");
        retval.push_back(variable);
        if(peek().type == TokenType::Comma)
        {
            get();
            continue;
        }
        break;
    }
    matchToken(TokenType::Colon, "expected ':'");
    auto *type = parseType();
    for(auto *variable : retval)
        variable->type = type;
    matchToken(TokenType::Semicolon, "expected ';'");
    return retval;
}

const ast::Type *Parser::parseType()
{
    if(peek().type == TokenType::Input || peek().type == TokenType::Output
       || peek().type == TokenType::Reg)
    {
        using Direction = ast::BitVectorType::Direction;
        using Kind = math::BitVector::Kind;
        auto directionToken = get();
        auto direction = Direction::Reg;
        if(directionToken.type == TokenType::Input)
            direction = Direction::Input;
        else if(directionToken.type == TokenType::Output)
            direction = Direction::Output;
        else
            assert(directionToken.type == TokenType::Reg);
        auto kind = Kind::Unsigned;
        bool foundBuiltinAlias = false;
        std::size_t bitWidth = 0;
        switch(peek().type)
        {
        case TokenType::UInt:
            kind = Kind::Unsigned;
            get();
            break;
        case TokenType::SInt:
            kind = Kind::Signed;
            get();
            break;
        default:
            for(auto &builtinAlias : ast::BitVectorType::getBuiltinAliases())
            {
                if(builtinAlias.name == peek().type)
                {
                    bitWidth = builtinAlias.bitWidth;
                    kind = builtinAlias.kind;
                    foundBuiltinAlias = true;
                    get();
                    break;
                }
            }
            if(!foundBuiltinAlias)
                throw ParseError(peek().locationRange.begin(), "expected bit vector type");
            break;
        }
        if(!foundBuiltinAlias)
        {
            matchToken(TokenType::LAngle, "expected '<'");
            auto bitWidthToken =
                matchToken(TokenType::UnprefixedDecimalLiteralInteger, "expected bit vector width");
            auto bitWidthGMPInteger = bitWidthToken.getValue();
            if(mpz_cmp_ui(bitWidthGMPInteger.value, 1) < 0)
                throw ParseError(bitWidthToken.locationRange.begin(),
                                 "bit vector must be non-zero width");
            if(mpz_cmp_ui(bitWidthGMPInteger.value, math::BitVector::maxBitCount()) > 0)
                throw ParseError(bitWidthToken.locationRange.begin(), "bit vector is too wide");
            bitWidth = mpz_get_ui(bitWidthGMPInteger);
            matchToken(TokenType::RAngle, "expected '>'");
        }
        return context.typePool.getBitVectorType(direction, kind, bitWidth);
    }
    bool flipped = false;
    if(peek().type == TokenType::EMark)
    {
        get();
        flipped = true;
    }
    auto scopedName = parseScopedName();
    if(auto type = dynamic_cast<const ast::Type *>(std::get<0>(scopedName)))
        return flipped ? type->getFlippedType() : type;
    throw ParseError(std::get<1>(scopedName).locationRange.begin(), "expected type name");
}

std::pair<ast::Symbol *, Token> Parser::parseScopedName()
{
    Token lastName;
    ast::Symbol *symbol;
    if(peek().type == TokenType::ColonColon)
    {
        get();
        lastName = matchToken(TokenType::Identifier, "expected name");
        symbol = globalSymbolTable->find(context.stringPool.intern(lastName.getText()));
    }
    else
    {
        lastName = matchToken(TokenType::Identifier, "expected name");
        symbol = currentSymbolLookupChain.find(context.stringPool.intern(lastName.getText()));
    }
    while(symbol && peek().type == TokenType::ColonColon)
    {
        if(ast::SymbolScope *scope = dynamic_cast<ast::SymbolScope *>(symbol))
        {
            get();
            lastName = matchToken(TokenType::Identifier, "expected name");
            symbol = scope->symbolTable->find(context.stringPool.intern(lastName.getText()));
        }
        else
        {
            throw ParseError(lastName.locationRange.begin(), "name is not a scope");
        }
    }
    if(!symbol)
        throw ParseError(lastName.locationRange.begin(), "name not found");
    return {symbol, lastName};
}

ast::Module *Parser::parseTopLevelModule()
{
    PushSymbolScope pushSymbolScope(this, globalSymbolTable);
    auto module = parseModule();
    if(peek().type != TokenType::EndOfFile)
        throw ParseError(peek().locationRange.begin(), "extra tokens before end of file");
    return module;
}
