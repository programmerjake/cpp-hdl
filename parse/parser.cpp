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
        auto closingRBrace = matchAndGet(TokenType::RBrace);
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
                                   closingRBrace.comments);
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
        auto closingRBrace = matchAndGet(TokenType::RBrace);
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
                                      closingRBrace.comments);
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
        auto functionKeyword = matchAndGet(TokenType::Function);
        auto functionName = matchAndGet(TokenType::Identifier, "expected: function name");
        ast::TemplateParameters *templateParameters = nullptr;
        if(peek().token.type == TokenType::LAngle)
            templateParameters = parseTemplateParameters();
        auto openingLParen = matchAndGet(TokenType::LParen);
        ast::FunctionParameter *firstParameter = nullptr;
        std::vector<ast::Function::Parameter> parameters;
        if(peek().token.type != TokenType::RParen)
        {
            firstParameter = parseFunctionParameter();
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parameters.push_back({commaToken.comments, parseFunctionParameter()});
            }
        }
        auto closingRParen = matchAndGet(TokenType::RParen);
        CommentsAndToken colonToken = {};
        ast::Type *returnType = nullptr;
        if(peek().token.type == TokenType::Colon)
        {
            colonToken = get();
            returnType = parseType();
        }
        auto openingLBrace = matchAndGet(TokenType::LBrace);
        std::vector<ast::Statement *> statements;
        while(peek().token.type != TokenType::RBrace && peek().token.type != TokenType::EndOfFile)
            statements.push_back(parseStatement());
        auto closingRBrace = matchAndGet(TokenType::RBrace);
        LocationRange locationRange(functionKeyword.token.locationRange.begin(),
                                    closingRBrace.token.locationRange.end());
        return create<ast::Function>(locationRange,
                                     functionKeyword.comments,
                                     functionName.comments,
                                     functionName.token.locationRange,
                                     context.stringPool.intern(functionName.token.getText()),
                                     templateParameters,
                                     openingLParen.comments,
                                     firstParameter,
                                     std::move(parameters),
                                     closingRParen.comments,
                                     colonToken.comments,
                                     returnType,
                                     openingLBrace.comments,
                                     std::move(statements),
                                     closingRBrace.comments);
    }
    ast::FunctionParameter *parseFunctionParameter()
    {
        auto parameterName =
            matchAndGet(TokenType::Identifier, "expected: function parameter name");
        auto colonToken = matchAndGet(TokenType::Colon);
        auto *type = parseType();
        LocationRange locationRange(parameterName.token.locationRange.begin(),
                                    type->locationRange.end());
        return create<ast::FunctionParameter>(
            locationRange,
            parameterName.comments,
            parameterName.token.locationRange,
            context.stringPool.intern(parameterName.token.getText()),
            colonToken.comments,
            type);
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
            auto typeKeyword = get();
            auto typeName = matchAndGet(TokenType::Identifier, "expected: type name");
            auto equalToken = matchAndGet(TokenType::Equal);
            auto *type = parseType();
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            return create<ast::TypeStatement>(
                LocationRange(typeKeyword.token.locationRange.begin(),
                              finalSemicolon.token.locationRange.end()),
                typeKeyword.comments,
                typeName.comments,
                typeName.token.locationRange,
                context.stringPool.intern(typeName.token.getText()),
                equalToken.comments,
                type,
                finalSemicolon.comments);
        }
        case TokenType::Const:
        {
            auto constKeyword = get();
            auto *firstPart = parseConstStatementPart();
            std::vector<ast::ConstStatement::Part> parts;
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseConstStatementPart()});
            }
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            return create<ast::ConstStatement>(
                LocationRange(constKeyword.token.locationRange.begin(),
                              finalSemicolon.token.locationRange.end()),
                constKeyword.comments,
                firstPart,
                std::move(parts),
                finalSemicolon.comments);
        }
        case TokenType::Let:
        {
            auto letKeyword = get();
            auto *firstPart = parseLetStatementPart();
            std::vector<ast::LetStatement::Part> parts;
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseLetStatementPart()});
            }
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            return create<ast::LetStatement>(
                LocationRange(letKeyword.token.locationRange.begin(),
                              finalSemicolon.token.locationRange.end()),
                letKeyword.comments,
                firstPart,
                std::move(parts),
                finalSemicolon.comments);
        }
        case TokenType::Input:
        case TokenType::Output:
        {
            auto inputOutputToken = get();
            bool isInput = inputOutputToken.token.type == TokenType::Input;
            auto *firstPart = parseInputOutputStatementPart(isInput);
            std::vector<ast::InputOutputStatement::Part> parts;
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseInputOutputStatementPart(isInput)});
            }
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            auto *retval = create<ast::InputOutputStatement>(
                LocationRange(inputOutputToken.token.locationRange.begin(),
                              finalSemicolon.token.locationRange.end()),
                inputOutputToken.comments,
                isInput,
                firstPart,
                std::move(parts),
                finalSemicolon.comments);
            firstPart->parentStatement = retval;
            for(auto &part : retval->parts)
                part.part->parentStatement = retval;
            return retval;
        }
        case TokenType::Reg:
        {
            auto regKeyword = get();
            auto *firstPart = parseRegStatementPart();
            std::vector<ast::RegStatement::Part> parts;
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseRegStatementPart()});
            }
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            return create<ast::RegStatement>(
                LocationRange(regKeyword.token.locationRange.begin(),
                              finalSemicolon.token.locationRange.end()),
                regKeyword.comments,
                firstPart,
                std::move(parts),
                finalSemicolon.comments);
        }
        case TokenType::If:
        {
            auto ifToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *condition = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            auto *thenStatement = parseStatement();
            LocationRange locationRange(ifToken.token.locationRange.begin(),
                                        thenStatement->locationRange.end());
            CommentsAndToken elseToken = {};
            ast::Statement *elseStatement = nullptr;
            if(peek().token.type == TokenType::Else)
            {
                elseToken = get();
                elseStatement = parseStatement();
                locationRange.setEnd(elseStatement->locationRange.end());
            }
            return create<ast::IfStatement>(locationRange,
                                            ifToken.comments,
                                            openingLParen.comments,
                                            condition,
                                            closingRParen.comments,
                                            thenStatement,
                                            elseToken.comments,
                                            elseStatement);
        }
        case TokenType::For:
        {
            auto forKeyword = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            if(peek().token.type == TokenType::Type)
            {
                auto typeKeyword = get();
                auto typeName = matchAndGet(TokenType::Identifier, "expected: for type name");
                auto inKeyword = matchAndGet(TokenType::In);
                auto *type = parseType();
                auto closingRParen = matchAndGet(TokenType::RParen);
                auto *statement = parseStatement();
                LocationRange locationRange(forKeyword.token.locationRange.begin(),
                                            statement->locationRange.end());
                return create<ast::ForTypeStatement>(
                    locationRange,
                    forKeyword.comments,
                    openingLParen.comments,
                    typeKeyword.comments,
                    typeName.comments,
                    typeName.token.locationRange,
                    context.stringPool.intern(typeName.token.getText()),
                    inKeyword.comments,
                    type,
                    closingRParen.comments,
                    statement);
            }
            auto variableName = matchAndGet(TokenType::Identifier, "expected: for variable name");
            auto inKeyword = matchAndGet(TokenType::In);
            auto *firstExpression = parseExpression();
            CommentsAndToken toKeyword = {};
            ast::Expression *secondExpression = nullptr;
            if(peek().token.type == TokenType::To)
            {
                toKeyword = get();
                secondExpression = parseExpression();
            }
            auto closingRParen = matchAndGet(TokenType::RParen);
            auto *statement = parseStatement();
            LocationRange locationRange(forKeyword.token.locationRange.begin(),
                                        statement->locationRange.end());
            return create<ast::ForStatement>(
                locationRange,
                forKeyword.comments,
                openingLParen.comments,
                variableName.comments,
                variableName.token.locationRange,
                context.stringPool.intern(variableName.token.getText()),
                inKeyword.comments,
                firstExpression,
                toKeyword.comments,
                secondExpression,
                closingRParen.comments,
                statement);
        }
        case TokenType::Match:
        {
            auto matchToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *matchee = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            auto openingLBrace = matchAndGet(TokenType::LBrace);
            std::vector<ast::MatchStatementPart *> parts;
            while(peek().token.type != TokenType::RBrace
                  && peek().token.type != TokenType::EndOfFile)
                parts.push_back(parseMatchStatementPart());
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            LocationRange locationRange(matchToken.token.locationRange.begin(),
                                        closingRBrace.token.locationRange.end());
            return create<ast::MatchStatement>(locationRange,
                                               matchToken.comments,
                                               openingLParen.comments,
                                               matchee,
                                               closingRParen.comments,
                                               openingLBrace.comments,
                                               std::move(parts),
                                               closingRBrace.comments);
        }
        case TokenType::LBrace:
        {
            auto openingLBrace = get();
            std::vector<ast::Statement *> statements;
            while(peek().token.type != TokenType::RBrace
                  && peek().token.type != TokenType::EndOfFile)
                statements.push_back(parseStatement());
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            LocationRange locationRange(openingLBrace.token.locationRange.begin(),
                                        closingRBrace.token.locationRange.end());
            return create<ast::BlockStatement>(locationRange,
                                               openingLBrace.comments,
                                               std::move(statements),
                                               closingRBrace.comments);
        }
        case TokenType::Return:
        {
            auto returnKeyword = get();
            ast::Expression *expression = nullptr;
            if(peek().token.type != TokenType::Semicolon)
                expression = parseExpression();
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            LocationRange locationRange(returnKeyword.token.locationRange.begin(),
                                        finalSemicolon.token.locationRange.end());
            return create<ast::ReturnStatement>(
                locationRange, returnKeyword.comments, expression, finalSemicolon.comments);
        }
        case TokenType::Break:
        {
            auto breakKeyword = get();
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            LocationRange locationRange(breakKeyword.token.locationRange.begin(),
                                        finalSemicolon.token.locationRange.end());
            return create<ast::BreakStatement>(
                locationRange, breakKeyword.comments, finalSemicolon.comments);
        }
        case TokenType::Continue:
        {
            auto continueKeyword = get();
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            LocationRange locationRange(continueKeyword.token.locationRange.begin(),
                                        finalSemicolon.token.locationRange.end());
            return create<ast::ContinueStatement>(
                locationRange, continueKeyword.comments, finalSemicolon.comments);
        }
        case TokenType::Semicolon:
        {
            auto finalSemicolon = get();
            return create<ast::EmptyStatement>(finalSemicolon.token.locationRange,
                                               finalSemicolon.comments);
        }
        default:
        {
            auto *expression = parseExpression();
            auto finalSemicolon = matchAndGet(TokenType::Semicolon);
            LocationRange locationRange(expression->locationRange.begin(),
                                        finalSemicolon.token.locationRange.end());
            return create<ast::ExpressionStatement>(
                expression->locationRange, expression, finalSemicolon.comments);
        }
        }
    }
    ast::ConstStatementPart *parseConstStatementPart()
    {
        auto constName = matchAndGet(TokenType::Identifier, "expected: const name");
        auto equalToken = matchAndGet(TokenType::Equal);
        auto *expression = parseExpression();
        LocationRange locationRange(constName.token.locationRange.begin(),
                                    expression->locationRange.end());
        return create<ast::ConstStatementPart>(locationRange,
                                               constName.comments,
                                               constName.token.locationRange,
                                               context.stringPool.intern(constName.token.getText()),
                                               equalToken.comments,
                                               expression);
    }
    ast::MatchStatementPart *parseMatchStatementPart()
    {
        auto *firstMatchPattern = parseMatchPattern();
        std::vector<ast::MatchStatementPart::Part> parts;
        while(peek().token.type == TokenType::Comma)
        {
            auto commaToken = get();
            parts.push_back({commaToken.comments, parseMatchPattern()});
        }
        auto equalRAngleToken = matchAndGet(TokenType::EqualRAngle);
        auto *statement = parseStatement();
        LocationRange locationRange(firstMatchPattern->locationRange.begin(),
                                    statement->locationRange.end());
        return create<ast::MatchStatementPart>(locationRange,
                                               firstMatchPattern,
                                               std::move(parts),
                                               equalRAngleToken.comments,
                                               statement);
    }
    ast::MatchPattern *parseMatchPattern()
    {
        if(peek().token.isIntegerPattern())
        {
            auto patternToken = get();
            return create<ast::NumberPatternMatchPattern>(patternToken.token.locationRange,
                                                          patternToken.comments,
                                                          patternToken.token.getIntegerValue());
        }
        auto *firstExpression = parseExpression();
        auto locationRange = firstExpression->locationRange;
        CommentsAndToken toToken = {};
        ast::Expression *secondExpression = nullptr;
        if(peek().token.type == TokenType::To)
        {
            toToken = get();
            secondExpression = parseExpression();
            locationRange.setEnd(secondExpression->locationRange.end());
        }
        return create<ast::RangeMatchPattern>(
            locationRange, firstExpression, toToken.comments, secondExpression);
    }
    ast::LetStatementPart *parseLetStatementPart()
    {
        auto *firstName = parseLetStatementName();
        std::vector<ast::LetStatementPart::Part> parts;
        while(peek().token.type == TokenType::Comma)
        {
            auto commaToken = get();
            parts.push_back({commaToken.comments, parseLetStatementName()});
        }
        auto colonToken = matchAndGet(TokenType::Colon);
        auto *type = parseType();
        LocationRange locationRange(firstName->locationRange.begin(), type->locationRange.end());
        auto *retval = create<ast::LetStatementPart>(
            locationRange, firstName, std::move(parts), colonToken.comments, type);
        firstName->parentPart = retval;
        for(auto &part : retval->parts)
            part.name->parentPart = retval;
        return retval;
    }
    ast::LetStatementName *parseLetStatementName()
    {
        auto idToken = matchAndGet(TokenType::Identifier, "expected: name");
        return create<ast::LetStatementName>(idToken.token.locationRange,
                                             idToken.comments,
                                             idToken.token.locationRange,
                                             context.stringPool.intern(idToken.token.getText()));
    }
    ast::InputOutputStatementPart *parseInputOutputStatementPart(bool isInput)
    {
        auto *firstName = parseInputOutputStatementName(isInput);
        std::vector<ast::InputOutputStatementPart::Part> parts;
        while(peek().token.type == TokenType::Comma)
        {
            auto commaToken = get();
            parts.push_back({commaToken.comments, parseInputOutputStatementName(isInput)});
        }
        auto colonToken = matchAndGet(TokenType::Colon);
        auto *type = parseType();
        LocationRange locationRange(firstName->locationRange.begin(), type->locationRange.end());
        auto *retval = create<ast::InputOutputStatementPart>(
            locationRange, firstName, std::move(parts), colonToken.comments, type);
        firstName->parentPart = retval;
        for(auto &part : retval->parts)
            part.name->parentPart = retval;
        return retval;
    }
    ast::InputOutputStatementName *parseInputOutputStatementName(bool isInput)
    {
        auto idToken = matchAndGet(TokenType::Identifier,
                                   isInput ? "expected: input name" : "expected: output name");
        return create<ast::InputOutputStatementName>(
            idToken.token.locationRange,
            idToken.comments,
            idToken.token.locationRange,
            context.stringPool.intern(idToken.token.getText()));
    }
    ast::RegStatementPart *parseRegStatementPart()
    {
        auto *firstName = parseRegStatementNameAndInitializer();
        std::vector<ast::RegStatementPart::Part> parts;
        while(peek().token.type == TokenType::Comma)
        {
            auto commaToken = get();
            parts.push_back({commaToken.comments, parseRegStatementNameAndInitializer()});
        }
        auto colonToken = matchAndGet(TokenType::Colon);
        auto *type = parseType();
        LocationRange locationRange(firstName->locationRange.begin(), type->locationRange.end());
        auto *retval = create<ast::RegStatementPart>(
            locationRange, firstName, std::move(parts), colonToken.comments, type);
        firstName->parentPart = retval;
        for(auto &part : retval->parts)
            part.name->parentPart = retval;
        return retval;
    }
    ast::RegStatementNameAndInitializer *parseRegStatementNameAndInitializer()
    {
        auto regName = matchAndGet(TokenType::Identifier, "expected: reg name");
        CommentsAndToken equalToken = {};
        ast::Expression *initializer = nullptr;
        auto locationRange = regName.token.locationRange;
        if(peek().token.type == TokenType::Equal)
        {
            equalToken = get();
            initializer = parseExpression();
            locationRange.setEnd(initializer->locationRange.end());
        }
        return create<ast::RegStatementNameAndInitializer>(
            locationRange,
            regName.comments,
            regName.token.locationRange,
            context.stringPool.intern(regName.token.getText()),
            equalToken.comments,
            initializer);
    }
    ast::Expression *parsePrimaryExpression()
    {
        switch(peek().token.type)
        {
        case TokenType::ColonColon:
        case TokenType::Identifier:
        {
            auto *scopedId = parseScopedId();
            return create<ast::ScopedIdExpression>(scopedId->locationRange, scopedId);
        }
        case TokenType::UnprefixedDecimalLiteralInteger:
        case TokenType::DecimalLiteralInteger:
        case TokenType::HexadecimalLiteralInteger:
        case TokenType::OctalLiteralInteger:
        case TokenType::BinaryLiteralInteger:
        {
            auto numberToken = get();
            return create<ast::NumberExpression>(numberToken.token.locationRange,
                                                 numberToken.comments,
                                                 numberToken.token.getIntegerValue().value);
        }
        case TokenType::HexadecimalLiteralIntegerPattern:
        case TokenType::OctalLiteralIntegerPattern:
        case TokenType::BinaryLiteralIntegerPattern:
            reportError(peek().token.locationRange, "number pattern not allowed here");
            return nullptr;
        case TokenType::LParen:
        {
            auto openingLParen = get();
            auto *expression = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::ParenExpression>(
                LocationRange(openingLParen.token.locationRange.begin(),
                              closingRParen.token.locationRange.end()),
                openingLParen.comments,
                expression,
                closingRParen.comments);
        }
        case TokenType::LBrace:
        {
            auto openingLBrace = get();
            bool hasTrailingComma = false;
            std::vector<ast::ListExpression::Part> parts;
            while(peek().token.type != TokenType::RBrace
                  || peek().token.type != TokenType::EndOfFile)
            {
                auto *expression = parseExpression();
                if(peek().token.type == TokenType::Comma)
                {
                    hasTrailingComma = true;
                    parts.push_back({expression, get().comments});
                    continue;
                }
                hasTrailingComma = false;
                parts.push_back({expression, {}});
                break;
            }
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            return create<ast::ListExpression>(
                LocationRange(openingLBrace.token.locationRange.begin(),
                              closingRBrace.token.locationRange.end()),
                openingLBrace.comments,
                std::move(parts),
                hasTrailingComma,
                closingRBrace.comments);
        }
        case TokenType::Cast:
        {
            auto castToken = get();
            auto openingLBrace = matchAndGet(TokenType::LBrace);
            auto *type = parseType();
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *expression = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::CastExpression>(
                LocationRange(castToken.token.locationRange.begin(),
                              closingRParen.token.locationRange.end()),
                castToken.comments,
                openingLBrace.comments,
                type,
                closingRBrace.comments,
                openingLParen.comments,
                expression,
                closingRParen.comments);
        }
        case TokenType::Fill:
        {
            auto fillToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *countExpression = parseExpression();
            auto commaToken = matchAndGet(TokenType::Comma);
            auto *valueExpression = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::FillExpression>(
                LocationRange(fillToken.token.locationRange.begin(),
                              closingRParen.token.locationRange.end()),
                fillToken.comments,
                openingLParen.comments,
                countExpression,
                commaToken.comments,
                valueExpression,
                closingRParen.comments);
        }
        case TokenType::Cat:
        {
            auto catToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *firstExpression = parseExpression();
            std::vector<ast::CatExpression::Part> parts;
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseExpression()});
            }
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::CatExpression>(
                LocationRange(catToken.token.locationRange.begin(),
                              closingRParen.token.locationRange.end()),
                catToken.comments,
                openingLParen.comments,
                firstExpression,
                std::move(parts),
                closingRParen.comments);
        }
        case TokenType::PopCount:
        {
            auto popCountToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *expression = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::PopCountExpression>(
                LocationRange(popCountToken.token.locationRange.begin(),
                              closingRParen.token.locationRange.end()),
                popCountToken.comments,
                openingLParen.comments,
                expression,
                closingRParen.comments);
        }
        case TokenType::EndOfFile:
        case TokenType::Bit:
        case TokenType::Break:
        case TokenType::Const:
        case TokenType::Continue:
        case TokenType::Else:
        case TokenType::Enum:
        case TokenType::Flip:
        case TokenType::For:
        case TokenType::Function:
        case TokenType::If:
        case TokenType::Implements:
        case TokenType::Import:
        case TokenType::In:
        case TokenType::Input:
        case TokenType::Interface:
        case TokenType::Let:
        case TokenType::Match:
        case TokenType::Memory:
        case TokenType::Module:
        case TokenType::Output:
        case TokenType::Reg:
        case TokenType::Return:
        case TokenType::S16:
        case TokenType::S32:
        case TokenType::S64:
        case TokenType::S8:
        case TokenType::SInt:
        case TokenType::To:
        case TokenType::Type:
        case TokenType::TypeOf:
        case TokenType::U16:
        case TokenType::U32:
        case TokenType::U64:
        case TokenType::U8:
        case TokenType::UInt:
        case TokenType::BlockComment:
        case TokenType::LineComment:
        case TokenType::FSlash:
        case TokenType::RBrace:
        case TokenType::LBracket:
        case TokenType::RBracket:
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
        case TokenType::DotDotDot:
        case TokenType::EqualRAngle:
        case TokenType::LAngleLAngle:
        case TokenType::RAngleRAngle:
        case TokenType::LAngleEqual:
        case TokenType::RAngleEqual:
        case TokenType::EqualEqual:
        case TokenType::EMarkEqual:
        case TokenType::AmpAmp:
        case TokenType::VBarVBar:
        case TokenType::LAngleMinusRAngle:
            reportError(peek().token.locationRange, "expected: expression");
            return nullptr;
        }
    }
    ast::Expression *parsePostfixExpression()
    {
        auto *retval = parseExpression();
        while(true)
        {
            if(peek().token.type == TokenType::LParen)
            {
                auto openingLParen = get();
                ast::Expression *firstExpression = nullptr;
                std::vector<ast::FunctionCallExpression::Part> parts;
                if(peek().token.type != TokenType::RParen
                   && peek().token.type != TokenType::EndOfFile)
                {
                    firstExpression = parseExpression();
                    while(peek().token.type == TokenType::Comma)
                    {
                        auto commaToken = get();
                        parts.push_back({commaToken.comments, parseExpression()});
                    }
                }
                auto closingRParen = matchAndGet(TokenType::RParen);
                LocationRange locationRange(retval->locationRange.begin(),
                                            closingRParen.token.locationRange.end());
                retval = create<ast::FunctionCallExpression>(locationRange,
                                                             retval,
                                                             openingLParen.comments,
                                                             firstExpression,
                                                             std::move(parts),
                                                             closingRParen.comments);
            }
            else if(peek().token.type == TokenType::LBracket)
            {
                auto openingLBracket = get();
                auto *startIndex = parseExpression();
                CommentsAndToken toToken = {};
                ast::Expression *endIndex = nullptr;
                if(peek().token.type == TokenType::To)
                {
                    toToken = get();
                    endIndex = parseExpression();
                }
                auto closingRBracket = matchAndGet(TokenType::RBracket);
                LocationRange locationRange(retval->locationRange.begin(),
                                            closingRBracket.token.locationRange.end());
                retval = create<ast::SliceExpression>(locationRange,
                                                      retval,
                                                      openingLBracket.comments,
                                                      startIndex,
                                                      toToken.comments,
                                                      endIndex,
                                                      closingRBracket.comments);
            }
            else if(peek().token.type == TokenType::Dot)
            {
                auto dotToken = get();
                auto memberName = matchAndGet(TokenType::Identifier, "expected: member name");
                LocationRange locationRange(retval->locationRange.begin(),
                                            memberName.token.locationRange.end());
                retval = create<ast::MemberExpression>(
                    locationRange,
                    retval,
                    dotToken.comments,
                    memberName.comments,
                    memberName.token.locationRange,
                    context.stringPool.intern(memberName.token.getText()));
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseUnaryExpression()
    {
        switch(peek().token.type)
        {
        case TokenType::EMark:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::LogicalNotExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::Tilde:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::BitwiseNotExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::Plus:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::UnaryPlusExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::Minus:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::UnaryMinusExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::Amp:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::AndReduceExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::VBar:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::OrReduceExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        case TokenType::Caret:
        {
            auto operatorToken = get();
            auto *argument = parseUnaryExpression();
            return create<ast::XorReduceExpression>(
                LocationRange(operatorToken.token.locationRange.begin(),
                              argument->locationRange.end()),
                operatorToken.comments,
                argument);
        }
        default:
            return parsePostfixExpression();
        }
    }
    ast::Expression *parseMultiplicativeExpression()
    {
        ast::Expression *retval = parseUnaryExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::Star:
            {
                auto operatorToken = get();
                auto *rhs = parseUnaryExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval =
                    create<ast::MulExpression>(locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::FSlash:
            {
                auto operatorToken = get();
                auto *rhs = parseUnaryExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval =
                    create<ast::DivExpression>(locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::Percent:
            {
                auto operatorToken = get();
                auto *rhs = parseUnaryExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval =
                    create<ast::RemExpression>(locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseAdditiveExpression()
    {
        ast::Expression *retval = parseMultiplicativeExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::Plus:
            {
                auto operatorToken = get();
                auto *rhs = parseMultiplicativeExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval =
                    create<ast::AddExpression>(locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::Minus:
            {
                auto operatorToken = get();
                auto *rhs = parseMultiplicativeExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval =
                    create<ast::SubExpression>(locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseShiftExpression()
    {
        ast::Expression *retval = parseAdditiveExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::LAngleLAngle:
            {
                auto operatorToken = get();
                auto *rhs = parseAdditiveExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::LeftShiftExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::RAngleRAngle:
            {
                auto operatorToken = get();
                auto *rhs = parseAdditiveExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::RightShiftExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseRelationalExpression()
    {
        ast::Expression *retval = parseShiftExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::LAngle:
            {
                auto operatorToken = get();
                auto *rhs = parseShiftExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareLTExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::LAngleEqual:
            {
                auto operatorToken = get();
                auto *rhs = parseShiftExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareLEExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::RAngle:
            {
                auto operatorToken = get();
                auto *rhs = parseShiftExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareGTExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::RAngleEqual:
            {
                auto operatorToken = get();
                auto *rhs = parseShiftExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareGEExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseEqualityExpression()
    {
        ast::Expression *retval = parseRelationalExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::EqualEqual:
            {
                auto operatorToken = get();
                auto *rhs = parseRelationalExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareEqExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::EMarkEqual:
            {
                auto operatorToken = get();
                auto *rhs = parseRelationalExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::CompareNEExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseBitwiseAndExpression()
    {
        ast::Expression *retval = parseEqualityExpression();
        while(true)
        {
            if(peek().token.type == TokenType::Amp)
            {
                auto operatorToken = get();
                auto *rhs = parseEqualityExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::BitwiseAndExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseBitwiseXorExpression()
    {
        ast::Expression *retval = parseBitwiseAndExpression();
        while(true)
        {
            if(peek().token.type == TokenType::Caret)
            {
                auto operatorToken = get();
                auto *rhs = parseBitwiseAndExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::BitwiseXorExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseBitwiseOrExpression()
    {
        ast::Expression *retval = parseBitwiseXorExpression();
        while(true)
        {
            if(peek().token.type == TokenType::VBar)
            {
                auto operatorToken = get();
                auto *rhs = parseBitwiseXorExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::BitwiseOrExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseLogicalAndExpression()
    {
        ast::Expression *retval = parseBitwiseOrExpression();
        while(true)
        {
            if(peek().token.type == TokenType::AmpAmp)
            {
                auto operatorToken = get();
                auto *rhs = parseBitwiseOrExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::LogicalAndExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseLogicalOrExpression()
    {
        ast::Expression *retval = parseLogicalAndExpression();
        while(true)
        {
            if(peek().token.type == TokenType::VBarVBar)
            {
                auto operatorToken = get();
                auto *rhs = parseLogicalAndExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::LogicalOrExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
            }
            else
            {
                break;
            }
        }
        return retval;
    }
    ast::Expression *parseConditionalExpression()
    {
        ast::Expression *retval = parseLogicalOrExpression();
        if(peek().token.type == TokenType::QMark)
        {
            auto qMarkToken = get();
            auto *thenExpression = parseExpression();
            auto colonToken = matchAndGet(TokenType::Colon);
            auto *elseExpression = parseAssignmentExpression();
            LocationRange locationRange(retval->locationRange.begin(),
                                        elseExpression->locationRange.end());
            return create<ast::ConditionalExpression>(locationRange,
                                                      retval,
                                                      qMarkToken.comments,
                                                      thenExpression,
                                                      colonToken.comments,
                                                      elseExpression);
        }
        return retval;
    }
    ast::Expression *parseAssignmentExpression()
    {
        ast::Expression *retval = parseConditionalExpression();
        while(true)
        {
            switch(peek().token.type)
            {
            case TokenType::Equal:
            {
                auto operatorToken = get();
                auto *rhs = parseConditionalExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::AssignmentExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            case TokenType::LAngleMinusRAngle:
            {
                auto operatorToken = get();
                auto *rhs = parseConditionalExpression();
                LocationRange locationRange(retval->locationRange.begin(),
                                            rhs->locationRange.end());
                retval = create<ast::ConnectExpression>(
                    locationRange, retval, operatorToken.comments, rhs);
                continue;
            }
            default:
                break;
            }
            break;
        }
        return retval;
    }
    ast::Expression *parseExpression()
    {
        return parseAssignmentExpression();
    }
    ast::ScopedId *parseScopedId()
    {
        LocationRange locationRange = peek().token.locationRange;
        CommentsAndToken initialColonColon = {};
        bool hasInitialColonColon = false;
        if(peek().token.type == TokenType::ColonColon)
        {
            initialColonColon = get();
            hasInitialColonColon = true;
        }
        auto initialName = matchAndGet(TokenType::Identifier, "expected: symbol name");
        locationRange.setEnd(initialName.token.locationRange.end());
        ast::TemplateArguments *initialTemplateArguments = nullptr;
        if(peek().token.type == TokenType::LBrace)
        {
            initialTemplateArguments = parseTemplateArguments();
            locationRange.setEnd(initialTemplateArguments->locationRange.end());
        }
        auto *retval = create<ast::ScopedId>(locationRange,
                                             nullptr,
                                             initialColonColon.comments,
                                             hasInitialColonColon,
                                             initialName.comments,
                                             initialName.token.locationRange,
                                             context.stringPool.intern(initialName.token.getText()),
                                             initialTemplateArguments);
        while(peek().token.type == TokenType::ColonColon)
        {
            auto colonColonToken = get();
            auto name = matchAndGet(TokenType::Identifier, "expected: symbol name");
            locationRange.setEnd(name.token.locationRange.end());
            ast::TemplateArguments *templateArguments = nullptr;
            if(peek().token.type == TokenType::LBrace)
            {
                templateArguments = parseTemplateArguments();
                locationRange.setEnd(templateArguments->locationRange.end());
            }
            retval = create<ast::ScopedId>(locationRange,
                                           retval,
                                           colonColonToken.comments,
                                           true,
                                           name.comments,
                                           name.token.locationRange,
                                           context.stringPool.intern(name.token.getText()),
                                           templateArguments);
        }
        return retval;
    }
    ast::TemplateArguments *parseTemplateArguments()
    {
        auto openingLBrace = matchAndGet(TokenType::LBrace);
        ast::TemplateArgument *firstArgument = nullptr;
        std::vector<ast::TemplateArguments::Part> parts;
        if(peek().token.type != TokenType::RBrace && peek().token.type != TokenType::EndOfFile)
        {
            firstArgument = parseTemplateArgument();
            while(peek().token.type == TokenType::Comma)
            {
                auto commaToken = get();
                parts.push_back({commaToken.comments, parseTemplateArgument()});
            }
        }
        auto closingRBrace = matchAndGet(TokenType::RBrace);
        LocationRange locationRange(openingLBrace.token.locationRange.begin(),
                                    closingRBrace.token.locationRange.end());
        return create<ast::TemplateArguments>(locationRange,
                                              openingLBrace.comments,
                                              firstArgument,
                                              std::move(parts),
                                              closingRBrace.comments);
    }
    ast::TemplateArgument *parseTemplateArgument()
    {
        if(peek().token.type == TokenType::Type)
        {
            auto typeToken = get();
            auto *type = parseType();
            LocationRange locationRange(typeToken.token.locationRange.begin(),
                                        type->locationRange.end());
            return create<ast::TypeTemplateArgument>(locationRange, typeToken.comments, type);
        }
        auto *expression = parseExpression();
        return create<ast::ValueTemplateArgument>(expression->locationRange, expression);
    }
    ast::Type *parseType()
    {
        switch(peek().token.type)
        {
        case TokenType::ColonColon:
        case TokenType::Identifier:
        {
            auto *scopedId = parseScopedId();
            return create<ast::ScopedIdType>(scopedId->locationRange, scopedId);
        }
        case TokenType::Flip:
        {
            auto flipToken = get();
            auto *type = parseType();
            LocationRange locationRange(flipToken.token.locationRange.begin(),
                                        type->locationRange.end());
            return create<ast::FlipType>(locationRange, flipToken.comments, type);
        }
        case TokenType::TypeOf:
        {
            auto typeOfToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto *expression = parseExpression();
            auto closingRParen = matchAndGet(TokenType::RParen);
            return create<ast::TypeOfType>(LocationRange(typeOfToken.token.locationRange.begin(),
                                                         closingRParen.token.locationRange.end()),
                                           typeOfToken.comments,
                                           openingLParen.comments,
                                           expression,
                                           closingRParen.comments);
        }
        case TokenType::UInt:
        {
            auto uintToken = get();
            auto openingLBrace = matchAndGet(TokenType::LBrace);
            auto *bitCount = parseExpression();
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            return create<ast::UIntType>(LocationRange(uintToken.token.locationRange.begin(),
                                                       closingRBrace.token.locationRange.end()),
                                         uintToken.comments,
                                         openingLBrace.comments,
                                         bitCount,
                                         closingRBrace.comments);
        }
        case TokenType::SInt:
        {
            auto sintToken = get();
            auto openingLBrace = matchAndGet(TokenType::LBrace);
            auto *bitCount = parseExpression();
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            return create<ast::SIntType>(LocationRange(sintToken.token.locationRange.begin(),
                                                       closingRBrace.token.locationRange.end()),
                                         sintToken.comments,
                                         openingLBrace.comments,
                                         bitCount,
                                         closingRBrace.comments);
        }
        case TokenType::U8:
        {
            auto u8Token = get();
            return create<ast::U8Type>(u8Token.token.locationRange, u8Token.comments);
        }
        case TokenType::U16:
        {
            auto u16Token = get();
            return create<ast::U16Type>(u16Token.token.locationRange, u16Token.comments);
        }
        case TokenType::U32:
        {
            auto u32Token = get();
            return create<ast::U32Type>(u32Token.token.locationRange, u32Token.comments);
        }
        case TokenType::U64:
        {
            auto u64Token = get();
            return create<ast::U64Type>(u64Token.token.locationRange, u64Token.comments);
        }
        case TokenType::S8:
        {
            auto s8Token = get();
            return create<ast::S8Type>(s8Token.token.locationRange, s8Token.comments);
        }
        case TokenType::S16:
        {
            auto s16Token = get();
            return create<ast::S16Type>(s16Token.token.locationRange, s16Token.comments);
        }
        case TokenType::S32:
        {
            auto s32Token = get();
            return create<ast::S32Type>(s32Token.token.locationRange, s32Token.comments);
        }
        case TokenType::S64:
        {
            auto s64Token = get();
            return create<ast::S64Type>(s64Token.token.locationRange, s64Token.comments);
        }
        case TokenType::Bit:
        {
            auto bitToken = get();
            return create<ast::BitType>(bitToken.token.locationRange, bitToken.comments);
        }
        case TokenType::Memory:
        {
            auto memoryToken = get();
            auto openingLBracket = matchAndGet(TokenType::LBracket);
            auto *size = parseExpression();
            auto closingRBracket = matchAndGet(TokenType::RBracket);
            auto colonToken = matchAndGet(TokenType::Colon);
            auto *elementType = parseType();
            LocationRange locationRange(memoryToken.token.locationRange.begin(),
                                        elementType->locationRange.end());
            return create<ast::MemoryType>(locationRange,
                                           memoryToken.comments,
                                           openingLBracket.comments,
                                           size,
                                           closingRBracket.comments,
                                           colonToken.comments,
                                           elementType);
        }
        case TokenType::LBrace:
        {
            auto openingLBrace = get();
            bool hasTrailingComma = false;
            std::vector<ast::TupleType::Part> parts;
            while(peek().token.type != TokenType::RBrace
                  || peek().token.type != TokenType::EndOfFile)
            {
                auto *type = parseType();
                if(peek().token.type == TokenType::Comma)
                {
                    hasTrailingComma = true;
                    parts.push_back({type, get().comments});
                    continue;
                }
                hasTrailingComma = false;
                parts.push_back({type, {}});
                break;
            }
            auto closingRBrace = matchAndGet(TokenType::RBrace);
            return create<ast::TupleType>(LocationRange(openingLBrace.token.locationRange.begin(),
                                                        closingRBrace.token.locationRange.end()),
                                          openingLBrace.comments,
                                          std::move(parts),
                                          hasTrailingComma,
                                          closingRBrace.comments);
        }
        case TokenType::Function:
        {
            auto functionToken = get();
            auto openingLParen = matchAndGet(TokenType::LParen);
            auto parseParameter = [&]() -> ast::FunctionType::Parameter
            {
                if(peek().token.type == TokenType::Identifier)
                {
                    auto tempTokenizer = tokenizer;
                    auto name = tempTokenizer.get();
                    if(tempTokenizer.peek().token.type == TokenType::Colon)
                    {
                        tokenizer = tempTokenizer;
                        auto colonToken = get();
                        auto *type = parseType();
                        return {name.comments,
                                name.token.locationRange,
                                context.stringPool.intern(name.token.getText()),
                                colonToken.comments,
                                type};
                    }
                }
                return {{}, {}, {}, {}, parseType()};
            };
            ast::FunctionType::Parameter firstParameter{};
            std::vector<ast::FunctionType::Part> parts;
            if(peek().token.type != TokenType::RParen)
            {
                firstParameter = parseParameter();
                while(peek().token.type == TokenType::Comma)
                {
                    auto commaToken = get();
                    parts.push_back({commaToken.comments, parseParameter()});
                }
            }
            auto closingRParen = matchAndGet(TokenType::RParen);
            LocationRange locationRange(functionToken.token.locationRange.begin(),
                                        closingRParen.token.locationRange.end());
            CommentsAndToken colonToken = {};
            ast::Type *returnType = nullptr;
            if(peek().token.type == TokenType::Colon)
            {
                colonToken = get();
                returnType = parseType();
                locationRange.setEnd(returnType->locationRange.end());
            }
            return create<ast::FunctionType>(locationRange,
                                             functionToken.comments,
                                             openingLParen.comments,
                                             std::move(firstParameter),
                                             std::move(parts),
                                             closingRParen.comments,
                                             colonToken.comments,
                                             returnType);
        }
        case TokenType::EndOfFile:
        case TokenType::UnprefixedDecimalLiteralInteger:
        case TokenType::DecimalLiteralInteger:
        case TokenType::HexadecimalLiteralInteger:
        case TokenType::OctalLiteralInteger:
        case TokenType::BinaryLiteralInteger:
        case TokenType::HexadecimalLiteralIntegerPattern:
        case TokenType::OctalLiteralIntegerPattern:
        case TokenType::BinaryLiteralIntegerPattern:
        case TokenType::Break:
        case TokenType::Cast:
        case TokenType::Cat:
        case TokenType::Const:
        case TokenType::Continue:
        case TokenType::Else:
        case TokenType::Enum:
        case TokenType::Fill:
        case TokenType::For:
        case TokenType::If:
        case TokenType::Implements:
        case TokenType::Import:
        case TokenType::In:
        case TokenType::Input:
        case TokenType::Interface:
        case TokenType::Let:
        case TokenType::Match:
        case TokenType::Module:
        case TokenType::Output:
        case TokenType::PopCount:
        case TokenType::Reg:
        case TokenType::Return:
        case TokenType::To:
        case TokenType::Type:
        case TokenType::BlockComment:
        case TokenType::LineComment:
        case TokenType::FSlash:
        case TokenType::RBrace:
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
        case TokenType::DotDotDot:
        case TokenType::EqualRAngle:
        case TokenType::LAngleLAngle:
        case TokenType::RAngleRAngle:
        case TokenType::LAngleEqual:
        case TokenType::RAngleEqual:
        case TokenType::EqualEqual:
        case TokenType::EMarkEqual:
        case TokenType::AmpAmp:
        case TokenType::VBarVBar:
        case TokenType::LAngleMinusRAngle:
            break;
        }
        reportError(peek().token.locationRange, "expected: type");
        return nullptr;
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
