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

#include "match_statement.h"

namespace ast
{
void MatchStatement::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Statement::dump(dumpNode, state);
    dumpNode->nodeName = "ast::MatchStatement";
    state.setSimple(dumpNode, "beforeMatchComments", beforeMatchComments);
    state.setSimple(dumpNode, "beforeLParenComments", beforeLParenComments);
    state.setPointer(dumpNode, "matchee", matchee);
    state.setSimple(dumpNode, "beforeRParenComments", beforeRParenComments);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
    state.setPointerArray(dumpNode, "parts", parts);
    state.setSimple(dumpNode, "beforeRBraceComments", beforeRBraceComments);
}
}
