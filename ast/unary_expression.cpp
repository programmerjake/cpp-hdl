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

#include "unary_expression.h"

namespace ast
{
void UnaryExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Expression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::UnaryExpression";
    state.setSimple(dumpNode, "beforeOperatorComments", beforeOperatorComments);
    state.setPointer(dumpNode, "argument", argument);
}

void LogicalNotExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::LogicalNotExpression";
}

void BitwiseNotExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BitwiseNotExpression";
}

void UnaryPlusExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::UnaryPlusExpression";
}

void UnaryMinusExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::UnaryMinusExpression";
}

void AndReduceExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::AndReduceExpression";
}

void OrReduceExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::OrReduceExpression";
}

void XorReduceExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    UnaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::XorReduceExpression";
}
}
