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

#include "reg_statement_name_and_initializer.h"
#include "reg_statement.h"

namespace ast
{
void RegStatementNameAndInitializer::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Node::dump(dumpNode, state);
    Symbol::dump(dumpNode, state);
    dumpNode->nodeName = "ast::RegStatementNameAndInitializer";
    state.setSimple(dumpNode, "beforeRegComments", beforeNameComments);
    state.setSimple(dumpNode, "beforeEqualComments", beforeEqualComments);
    state.setPointer(dumpNode, "initializer", initializer);
    state.setPointer(dumpNode, "parentPart", parentPart);
}
}
