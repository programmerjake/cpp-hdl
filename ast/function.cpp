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

#include "function.h"
#include <sstream>

namespace ast
{
void Function::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Node::dump(dumpNode, state);
    Symbol::dump(dumpNode, state);
    SymbolScope::dump(dumpNode, state);
    dumpNode->nodeName = "ast::Function";
    state.setSimple(dumpNode, "beforeFunctionComments", beforeFunctionComments);
    state.setSimple(dumpNode, "beforeNameComments", beforeNameComments);
    state.setPointer(dumpNode, "templateParameters", templateParameters);
    state.setSimple(dumpNode, "beforeLParenComments", beforeLParenComments);
    state.setPointer(dumpNode, "firstFunctionParameter", firstFunctionParameter);
    for(std::size_t i = 0; i < parameters.size(); i++)
    {
        auto &part = parameters[i];
        std::ostringstream ss;
        ss << "parameters[" << i << "].";
        state.setSimple(dumpNode, ss.str() + "beforeCommaComments", part.beforeCommaComments);
        state.setPointer(dumpNode, ss.str() + "functionParameter", part.functionParameter);
    }
    state.setSimple(dumpNode, "beforeRParenComments", beforeRParenComments);
    state.setSimple(dumpNode, "beforeColonComments", beforeColonComments);
    state.setPointer(dumpNode, "returnType", returnType);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
    state.setPointerArray(dumpNode, "statements", statements);
    state.setSimple(dumpNode, "beforeRBraceComments", beforeRBraceComments);
}
}
