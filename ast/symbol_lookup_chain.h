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

#include "symbol_table.h"
#include "../util/string_pool.h"

namespace ast
{
struct SymbolLookupChainNode
{
    const SymbolLookupChainNode *parent;
    SymbolTable *symbolTable;
    constexpr SymbolLookupChainNode(const SymbolLookupChainNode *parent, SymbolTable *symbolTable)
        : parent(parent), symbolTable(symbolTable)
    {
    }
};

struct SymbolLookupChain
{
    const SymbolLookupChainNode *head;
    constexpr SymbolLookupChain() noexcept : head(nullptr)
    {
    }
    constexpr explicit SymbolLookupChain(const SymbolLookupChainNode *head) noexcept : head(head)
    {
    }
    Symbol *find(util::StringPool::Entry name) const
    {
        for(auto *node = head; node; node = node->parent)
        {
            auto retval = node->symbolTable->find(name);
            if(retval)
                return retval;
        }
        return nullptr;
    }
};
}

