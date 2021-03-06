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

#include "symbol_table.h"
#include "node.h"

namespace ast
{
SymbolTable *SymbolTable::getGlobalSymbolTable(Context &context)
{
    if(!context.globalSymbolTable)
    {
        context.globalSymbolTable = context.arena.create<SymbolTable>();
    }
    return context.globalSymbolTable;
}

void SymbolTable::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    dumpNode->nodeName = "ast::SymbolTable";
    for(std::size_t i = 0; i < localSymbolsList.size(); i++)
    {
        auto *symbolAsNode = dynamic_cast<Node *>(localSymbolsList[i]);
        state.setPointerIndexed(dumpNode, "localSymbolsList", i, symbolAsNode);
    }
}
}
