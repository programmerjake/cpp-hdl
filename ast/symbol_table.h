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

#include "../util/string_pool.h"
#include "symbol.h"
#include <unordered_map>
#include <vector>
#include "context.h"

namespace ast
{
class SymbolTable
{
public:
    std::unordered_map<util::StringPool::Entry, Symbol *> localSymbolsMap = {};
    std::vector<Symbol *> localSymbolsList = {};
    SymbolTable()
    {
    }
    Symbol *find(util::StringPool::Entry name) const
    {
        auto iter = localSymbolsMap.find(name);
        if(iter != localSymbolsMap.end())
            return std::get<1>(*iter);
        return nullptr;
    }
    bool insert(Symbol *symbol)
    {
        assert(symbol);
        assert(symbol->containingSymbolTable == nullptr);
        if(std::get<1>(localSymbolsMap.emplace(symbol->name, symbol)))
        {
            symbol->containingSymbolTable = this;
            localSymbolsList.push_back(symbol);
            return true;
        }
        return false;
    }
    static SymbolTable *createGlobalSymbolTable(Context &context);
};
}
