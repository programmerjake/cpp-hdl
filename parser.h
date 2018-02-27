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

#include "ast/context.h"
#include "ast/symbol_lookup_chain.h"
#include "ast/symbol_table.h"
#include "ast/module.h"
#include "ast/bundle.h"
#include "ast/variable.h"
#include "util/arena.h"
#include "tokenizer.h"
#include "parse_error.h"
#include "util/string_view.h"
#include "ast/template_parameter.h"

class Parser final
{
private:
    Tokenizer tokenizer;
    ast::Context &context;
    ast::SymbolLookupChain currentSymbolLookupChain{};
    ast::SymbolTable *currentSymbolTable = nullptr;
    ast::SymbolTable *globalSymbolTable;

private:
    class PushSymbolScope
    {
        PushSymbolScope(const PushSymbolScope &) = delete;
        PushSymbolScope &operator=(const PushSymbolScope &) = delete;

    private:
        ast::SymbolLookupChain &currentSymbolLookupChain;
        ast::SymbolTable *&currentSymbolTable;
        ast::SymbolLookupChain previousSymbolLookupChain;
        ast::SymbolTable *previousSymbolTable;

    public:
        PushSymbolScope(Parser *parser,
                        ast::SymbolLookupChain newSymbolLookupChain,
                        ast::SymbolTable *newSymbolTable) noexcept
            : currentSymbolLookupChain(parser->currentSymbolLookupChain),
              currentSymbolTable(parser->currentSymbolTable),
              previousSymbolLookupChain(currentSymbolLookupChain),
              previousSymbolTable(currentSymbolTable)
        {
            currentSymbolLookupChain = newSymbolLookupChain;
            currentSymbolTable = newSymbolTable;
        }
        PushSymbolScope(Parser *parser, ast::SymbolTable *newSymbolTable)
            : PushSymbolScope(
                  parser,
                  ast::SymbolLookupChain(parser->context.arena.create<ast::SymbolLookupChainNode>(
                      parser->currentSymbolLookupChain.head, newSymbolTable)),
                  newSymbolTable)
        {
        }
        ~PushSymbolScope()
        {
            currentSymbolLookupChain = previousSymbolLookupChain;
            currentSymbolTable = previousSymbolTable;
        }
    };

private:
    Parser(Tokenizer tokenizer, ast::Context &context)
        : tokenizer(tokenizer),
          context(context),
          globalSymbolTable(ast::SymbolTable::createGlobalSymbolTable(context))
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
    ast::GenericModule *parseModule(bool isTopLevel = false);
    ast::Bundle *parseBundle();
    std::vector<ast::Variable *> parseVariables();
    std::pair<const ast::Type *, LocationRange> parseType();
    std::pair<ast::Symbol *, Token> parseScopedName();
    std::vector<const ast::TemplateParameter *> parseTemplateParameters();

private:
    ast::Module *parseTopLevelModule();

public:
    static ast::Module *parseTopLevelModule(Tokenizer tokenizer, ast::Context &context)
    {
        return Parser(tokenizer, context).parseTopLevelModule();
    }
};
