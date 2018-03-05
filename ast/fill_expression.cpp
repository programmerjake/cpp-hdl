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

#include "fill_expression.h"

namespace ast
{
void FillExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Expression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::FillExpression";
    state.setSimple(dumpNode, "beforeFillComments", beforeFillComments);
    state.setSimple(dumpNode, "beforeLParenComments", beforeLParenComments);
    state.setPointer(dumpNode, "countExpression", countExpression);
    state.setSimple(dumpNode, "beforeCommaComments", beforeCommaComments);
    state.setPointer(dumpNode, "valueExpression", valueExpression);
    state.setSimple(dumpNode, "beforeRParenComments", beforeRParenComments);
}
}
