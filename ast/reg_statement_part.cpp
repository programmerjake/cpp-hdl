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

#include "reg_statement_part.h"
#include <sstream>

namespace ast
{
void RegStatementPart::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Node::dump(dumpNode, state);
    dumpNode->nodeName = "ast::RegStatementPart";
    state.setPointer(dumpNode, "firstName", firstName);
    for(std::size_t i = 0; i < parts.size(); i++)
    {
        auto &part = parts[i];
        std::ostringstream ss;
        ss << "parts[" << i << "].";
        state.setSimple(dumpNode, ss.str() + "beforeCommaComments", part.beforeCommaComments);
        state.setPointer(dumpNode, ss.str() + "name", part.name);
    }
    state.setSimple(dumpNode, "beforeColonComments", beforeColonComments);
    state.setPointer(dumpNode, "type", type);
}
}
