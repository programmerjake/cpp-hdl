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

#include "function_type.h"

namespace ast
{
void FunctionType::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Type::dump(dumpNode, state);
    dumpNode->nodeName = "ast::FunctionType";
    state.setSimple(dumpNode, "beforeFunctionComments", beforeFunctionComments);
    state.setSimple(dumpNode, "beforeLParenComments", beforeLParenComments);
    state.setSimple(
        dumpNode, "firstParameter.beforeNameComments", firstParameter.beforeNameComments);
    state.setSimple(dumpNode, "firstParameter.nameLocationRange", firstParameter.nameLocationRange);
    state.setSimple(dumpNode, "firstParameter.name", firstParameter.name);
    state.setSimple(
        dumpNode, "firstParameter.beforeColonComments", firstParameter.beforeColonComments);
    state.setPointer(dumpNode, "firstParameter.type", firstParameter.type);
    for(std::size_t i = 0; i < parts.size(); i++)
    {
        auto &part = parts[i];
        std::ostringstream ss;
        ss << "parts[" << i << "].";
        state.setSimple(dumpNode, ss.str() + "beforeNameComments", part.beforeNameComments);
        state.setSimple(dumpNode, ss.str() + "nameLocationRange", part.nameLocationRange);
        state.setSimple(dumpNode, ss.str() + "name", part.name);
        state.setSimple(dumpNode, ss.str() + "beforeColonComments", part.beforeColonComments);
        state.setPointer(dumpNode, ss.str() + "type", part.type);
        state.setSimple(dumpNode, ss.str() + "beforeCommaComments", part.beforeCommaComments);
    }
    state.setSimple(dumpNode, "beforeRParenComments", beforeRParenComments);
    state.setSimple(dumpNode, "beforeColonComments", beforeColonComments);
    state.setPointer(dumpNode, "returnType", returnType);
}
}
