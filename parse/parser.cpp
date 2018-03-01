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
#include "token.h"
#include "tokenizer.h"
#include "../ast/ast.h"
#include <cassert>
#include <utility>

namespace parse
{
struct Parser
{
    ast::Context &context;
    CommentGroupingTokenizer tokenizer;
    std::function<void(LocationRange locationRange, std::string message)> errorHandler;
    [[noreturn]] void reportError(LocationRange locationRange, std::string message)
    {
        errorHandler(locationRange, std::move(message));
#ifdef __GNUC__
        __builtin_unreachable(); // shut up warning about reachable code
#endif
    }
    template <typename T, typename... Args>
    decltype(new T(std::declval<Args>()...)) create(Args &&... args)
    {
        return context.arena.create<T>(std::forward<Args>(args)...);
    }
    explicit Parser(
        ast::Context &context,
        const Source *source,
        std::function<void(LocationRange locationRange, std::string message)> errorHandler)
        : context(context), tokenizer(source), errorHandler(std::move(errorHandler))
    {
    }
    CommentGroupingTokenizer::CommentsAndToken peek()
    {
        return tokenizer.peek();
    }
    CommentGroupingTokenizer::CommentsAndToken get()
    {
        return tokenizer.get();
    }
    CommentGroupingTokenizer::CommentsAndToken matchAndGet(TokenType type)
    {
        if(peek().token.type != type)
        {
            std::string errorMessage = "expected: ";
            errorMessage += Token::getTypeString(type);
            reportError(peek().token.locationRange.begin(), std::move(errorMessage));
        }
        return get();
    }
    CommentGroupingTokenizer::CommentsAndToken matchAndGet(TokenType type,
                                                           util::string_view errorMessage)
    {
        if(peek().token.type != type)
            reportError(peek().token.locationRange.begin(), std::string(errorMessage));
        return get();
    }
    ast::TopLevelModule *parseTopLevelModule()
    {
        Location startLocation = peek().token.locationRange.begin();
        std::vector<ast::Import *> imports;
        while(peek().token.type == TokenType::Import)
            imports.push_back(parseImport());
        auto *mainModule = parseModule();
        if(peek().token.type != TokenType::EndOfFile)
            reportError(peek().token.locationRange.begin(), "extra tokens before end-of-file");
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        ast::ConsecutiveComments beforeEndOfFileComments = get().comments;
        return create<ast::TopLevelModule>(
            locationRange, std::move(imports), mainModule, beforeEndOfFileComments);
    }
    ast::Import *parseImport()
    {
        Location startLocation = peek().token.locationRange.begin();
        auto importKeyword = matchAndGet(TokenType::Import);
        auto importName = matchAndGet(TokenType::Identifier, "expected: import name");
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        auto finalSemicolon = matchAndGet(TokenType::Semicolon);
        return create<ast::Import>(locationRange,
                                   importKeyword.comments,
                                   importName.comments,
                                   importName.token.locationRange,
                                   context.stringPool.intern(importName.token.getText()),
                                   finalSemicolon.comments);
    }
    ast::Module *parseModule()
    {
        Location startLocation = peek().token.locationRange.begin();
        auto moduleKeyword = matchAndGet(TokenType::Module);
        auto moduleName = matchAndGet(TokenType::Identifier, "expected: module name");
        ast::TemplateParameters *templateParameters = nullptr;
        if(peek().token.type == TokenType::LAngle)
            templateParameters = parseTemplateParameters();
        auto implementsKeyword = peek();
        ast::Type *parentType = nullptr;
        if(implementsKeyword.token.type == TokenType::Implements)
        {
            get();
            parentType = parseType();
        }
        else
        {
            implementsKeyword = {};
        }
        auto openingLBrace = matchAndGet(TokenType::LBrace);
        std::vector<ast::Statement *> statements;
        while(peek().token.type != TokenType::RBrace)
            statements.push_back(parseStatement());
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        auto closingLBrace = matchAndGet(TokenType::RBrace);
        return create<ast::Module>(locationRange,
                                   moduleKeyword.comments,
                                   moduleName.comments,
                                   moduleName.token.locationRange,
                                   context.stringPool.intern(moduleName.token.getText()),
                                   templateParameters,
                                   implementsKeyword.comments,
                                   parentType,
                                   openingLBrace.comments,
                                   std::move(statements),
                                   closingLBrace.comments);
    }
};

ast::TopLevelModule *parseTopLevelModule(
    ast::Context &context,
    const Source *source,
    std::function<void(LocationRange locationRange, std::string message)> errorHandler)
{
    struct FatalError
    {
    };
    try
    {
        return Parser(context,
                      source,
                      [errorHandler](LocationRange locationRange, std::string message)
                      {
                          errorHandler(locationRange, std::move(message));
                          throw FatalError();
                      })
            .parseTopLevelModule();
    }
    catch(FatalError &)
    {
        return nullptr;
    }
}
}
