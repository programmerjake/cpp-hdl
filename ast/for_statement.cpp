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

#include "for_statement.h"

namespace ast
{
void GenericForStatement::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Statement::dump(dumpNode, state);
    Symbol::dump(dumpNode, state);
    dumpNode->nodeName = "ast::GenericForStatement";
    state.setSimple(dumpNode, "beforeForComments", beforeForComments);
    state.setSimple(dumpNode, "beforeLParenComments", beforeLParenComments);
    state.setSimple(dumpNode, "beforeNameComments", beforeNameComments);
    state.setSimple(dumpNode, "beforeInComments", beforeInComments);
    state.setSimple(dumpNode, "beforeRParenComments", beforeRParenComments);
    state.setPointer(dumpNode, "statement", statement);
}

void ForTypeStatement::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericForStatement::dump(dumpNode, state);
    dumpNode->nodeName = "ast::ForTypeStatement";
    state.setSimple(dumpNode, "beforeTypeKeywordComments", beforeTypeKeywordComments);
    state.setPointer(dumpNode, "type", type);
}

void ForStatement::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericForStatement::dump(dumpNode, state);
    dumpNode->nodeName = "ast::ForStatement";
    Expression *firstExpression;
    ConsecutiveComments beforeToComments;
    Expression *secondExpression;
    state.setPointer(dumpNode, "firstExpression", firstExpression);
    state.setSimple(dumpNode, "beforeToComments", beforeToComments);
    state.setPointer(dumpNode, "secondExpression", secondExpression);
}
}
