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
    using CommentsAndToken = CommentGroupingTokenizer::CommentsAndToken;
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
    CommentsAndToken peek()
    {
        return tokenizer.peek();
    }
    CommentsAndToken get()
    {
        return tokenizer.get();
    }
    CommentsAndToken matchAndGet(TokenType type)
    {
        if(peek().token.type != type)
        {
            std::string errorMessage = "expected: ";
            errorMessage += Token::getTypeString(type);
            reportError(peek().token.locationRange.begin(), std::move(errorMessage));
        }
        return get();
    }
    CommentsAndToken matchAndGet(TokenType type, util::string_view errorMessage)
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
        CommentsAndToken implementsKeyword = {};
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
        while(peek().token.type != TokenType::RBrace && peek().token.type != TokenType::EndOfFile)
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
    ast::TemplateParameters *parseTemplateParameters()
    {
        Location startLocation = peek().token.locationRange.begin();
        auto openingLBrace = matchAndGet(TokenType::LBrace);
        ast::TemplateParameter *firstTemplateParameter = nullptr;
        std::vector<ast::TemplateParameters::Part> parts;
        if(peek().token.type != TokenType::RBrace)
        {
            firstTemplateParameter = parseTemplateParameter();
            while(peek().token.type == TokenType::Comma)
            {
                auto comma = get();
                parts.push_back(
                    ast::TemplateParameters::Part(comma.comments, parseTemplateParameter()));
            }
        }
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        auto closingRBrace = matchAndGet(TokenType::RBrace);
        return create<ast::TemplateParameters>(locationRange,
                                               openingLBrace.comments,
                                               firstTemplateParameter,
                                               std::move(parts),
                                               closingRBrace.comments);
    }
    ast::TemplateParameter *parseTemplateParameter()
    {
        if(peek().token.type == TokenType::Type)
        {
            auto typeToken = get();
            LocationRange locationRange = typeToken.token.locationRange;
            auto nameToken = matchAndGet(TokenType::Identifier, "template parameter name");
            locationRange.setEnd(nameToken.token.locationRange.end());
            CommentsAndToken implementsKeyword = {};
            ast::Type *parentType = nullptr;
            if(peek().token.type == TokenType::Implements)
            {
                implementsKeyword = get();
                parentType = parseType();
                locationRange.setEnd(parentType->locationRange.end());
            }
            CommentsAndToken dotDotDotToken = {};
            bool hasDotDotDot = peek().token.type == TokenType::DotDotDot;
            if(hasDotDotDot)
            {
                dotDotDotToken = get();
                locationRange.setEnd(dotDotDotToken.token.locationRange.end());
            }
            return create<ast::TypeTemplateParameter>(
                locationRange,
                typeToken.comments,
                nameToken.comments,
                nameToken.token.locationRange,
                context.stringPool.intern(nameToken.token.getText()),
                implementsKeyword.comments,
                parentType,
                dotDotDotToken.comments,
                hasDotDotDot);
        }
        if(peek().token.type == TokenType::Identifier)
        {
            auto nameToken = get();
            LocationRange locationRange = nameToken.token.locationRange;
            CommentsAndToken colonToken = matchAndGet(TokenType::Colon);
            ast::Type *valueType = parseType();
            locationRange.setEnd(valueType->locationRange.end());
            CommentsAndToken dotDotDotToken = {};
            bool hasDotDotDot = peek().token.type == TokenType::DotDotDot;
            if(hasDotDotDot)
            {
                dotDotDotToken = get();
                locationRange.setEnd(dotDotDotToken.token.locationRange.end());
            }
            return create<ast::ValueTemplateParameter>(
                locationRange,
                nameToken.comments,
                nameToken.token.locationRange,
                context.stringPool.intern(nameToken.token.getText()),
                colonToken.comments,
                valueType,
                dotDotDotToken.comments,
                hasDotDotDot);
        }
        reportError(peek().token.locationRange, "expected: 'type' or template parameter name");
    }
    ast::Interface *parseInterface()
    {
        Location startLocation = peek().token.locationRange.begin();
        auto interfaceKeyword = matchAndGet(TokenType::Interface);
        auto interfaceName = matchAndGet(TokenType::Identifier, "expected: interface name");
        ast::TemplateParameters *templateParameters = nullptr;
        if(peek().token.type == TokenType::LAngle)
            templateParameters = parseTemplateParameters();
        CommentsAndToken implementsKeyword = {};
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
        while(peek().token.type != TokenType::RBrace && peek().token.type != TokenType::EndOfFile)
            statements.push_back(parseStatement());
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        auto closingLBrace = matchAndGet(TokenType::RBrace);
        return create<ast::Interface>(locationRange,
                                      interfaceKeyword.comments,
                                      interfaceName.comments,
                                      interfaceName.token.locationRange,
                                      context.stringPool.intern(interfaceName.token.getText()),
                                      templateParameters,
                                      implementsKeyword.comments,
                                      parentType,
                                      openingLBrace.comments,
                                      std::move(statements),
                                      closingLBrace.comments);
    }
    ast::Enum *parseEnum()
    {
        Location startLocation = peek().token.locationRange.begin();
        auto enumKeyword = matchAndGet(TokenType::Enum);
        auto enumName = matchAndGet(TokenType::Identifier, "expected: enum name");
        auto colonToken = matchAndGet(TokenType::Colon);
        auto *underlyingType = parseType();
        auto openingLBrace = matchAndGet(TokenType::LBrace);
        std::vector<ast::Enum::Part> parts;
        while(peek().token.type == TokenType::Identifier)
        {
            auto enumValueName = get();
            auto equalToken = matchAndGet(TokenType::Equal);
            auto *value = parseExpression();
            auto *enumPart =
                create<ast::EnumPart>(LocationRange(enumValueName.token.locationRange.begin(),
                                                    value->locationRange.end()),
                                      enumValueName.comments,
                                      enumValueName.token.locationRange,
                                      context.stringPool.intern(enumValueName.token.getText()),
                                      equalToken.comments,
                                      value);
            if(peek().token.type == TokenType::Comma)
            {
                parts.push_back({enumPart, get().comments});
            }
            else
            {
                parts.push_back({enumPart, {}});
                break;
            }
        }
        LocationRange locationRange(startLocation, peek().token.locationRange.end());
        auto closingRBrace = matchAndGet(TokenType::RBrace);
        auto *retval = create<ast::Enum>(locationRange,
                                         enumKeyword.comments,
                                         enumName.comments,
                                         enumName.token.locationRange,
                                         context.stringPool.intern(enumName.token.getText()),
                                         colonToken.comments,
                                         underlyingType,
                                         openingLBrace.comments,
                                         std::move(parts),
                                         closingRBrace.comments);
        for(auto &part : retval->parts)
            part.enumPart->parentEnum = retval;
        return retval;
    }
    ast::Function *parseFunction()
    {
#error finish
    }
    ast::FunctionParameter *parseFunctionParameter()
    {
#error finish
    }
    ast::Statement *parseStatement()
    {
        switch(peek().token.type)
        {
        case TokenType::Module:
        {
            auto *parsedModule = parseModule();
            return create<ast::ModuleStatement>(parsedModule->locationRange, parsedModule);
        }
        case TokenType::Interface:
        {
            auto *parsedInterface = parseInterface();
            return create<ast::InterfaceStatement>(parsedInterface->locationRange, parsedInterface);
        }
        case TokenType::Enum:
        {
            auto *parsedEnum = parseEnum();
            return create<ast::EnumStatement>(parsedEnum->locationRange, parsedEnum);
        }
        case TokenType::Function:
        {
            auto *function = parseFunction();
            return create<ast::FunctionStatement>(function->locationRange, function);
        }
        case TokenType::Type:
        {
#error finish
            break;
        }
        case TokenType::Const:
        {
#error finish
            break;
        }
        case TokenType::Let:
        {
#error finish
            break;
        }
        case TokenType::Input:
        case TokenType::Output:
        {
#error finish
            break;
        }
        case TokenType::Reg:
        {
#error finish
            break;
        }
        case TokenType::If:
        {
#error finish
            break;
        }
        case TokenType::For:
        {
#error finish
            break;
        }
        case TokenType::Match:
        {
#error finish
            break;
        }
        case TokenType::LBrace:
        {
#error finish
            break;
        }
        case TokenType::Return:
        {
#error finish
            break;
        }
        case TokenType::Break:
        {
#error finish
            break;
        }
        case TokenType::Continue:
        {
#error finish
            break;
        }
        case TokenType::Semicolon:
        {
#error finish
            break;
        }
        default:
        {
            auto *expression = parseExpression();
#error finish
            break;
        }
        }
    }
    ast::ConstStatementPart *parseConstStatementPart()
    {
#error finish
    }
    ast::MatchStatementPart *parseMatchStatementPart()
    {
#error finish
    }
    ast::MatchPattern *parseMatchPattern()
    {
#error finish
    }
    ast::LetStatementPart *parseLetStatementPart()
    {
#error finish
    }
    ast::LetStatementName *parseLetStatementName()
    {
#error finish
    }
    ast::InputOutputStatementPart *parseInputOutputStatementPart()
    {
#error finish
    }
    ast::InputOutputStatementName *parseInputOutputStatementName()
    {
#error finish
    }
    ast::RegStatementPart *parseRegStatementPart()
    {
#error finish
    }
    ast::RegStatementName *parseRegStatementName()
    {
#error finish
    }
    ast::Expression *parseExpression()
    {
#error finish
    }
    ast::ScopedId *parseScopedId()
    {
#error finish
    }
    ast::TemplateArguments *parseTemplateArguments()
    {
#error finish
    }
    ast::TemplateArgument *parseTemplateArgument()
    {
#error finish
    }
    ast::Type *parseType()
    {
#error finish
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
