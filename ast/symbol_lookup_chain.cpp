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

#include "symbol_lookup_chain.h"

namespace ast
{
void SymbolLookupChainNode::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    dumpNode->nodeName = "ast::SymbolLookupChainNode";
    state.setPointer(dumpNode, "parent", parent);
    state.setPointer(dumpNode, "symbolTable", symbolTable);
}

void SymbolLookupChain::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    dumpNode->nodeName = "ast::SymbolLookupChain";
    state.setPointer(dumpNode, "head", head);
}
}
