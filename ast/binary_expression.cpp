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

#include "binary_expression.h"

namespace ast
{
void BinaryExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Expression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BinaryExpression";
    state.setPointer(dumpNode, "lhs", lhs);
    state.setSimple(dumpNode, "beforeOperatorComments", beforeOperatorComments);
    state.setPointer(dumpNode, "rhs", rhs);
}

void LogicalAndExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::LogicalAndExpression";
}

void LogicalOrExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::LogicalOrExpression";
}

void BitwiseAndExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BitwiseAndExpression";
}

void BitwiseOrExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BitwiseOrExpression";
}

void BitwiseXorExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BitwiseXorExpression";
}

void LeftShiftExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::LeftShiftExpression";
}

void RightShiftExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::RightShiftExpression";
}

void CompareEqExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareEqExpression";
}

void CompareNEExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareNEExpression";
}

void CompareLEExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareLEExpression";
}

void CompareGEExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareGEExpression";
}

void CompareLTExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareLTExpression";
}

void CompareGTExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::CompareGTExpression";
}

void AddExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::AddExpression";
}

void SubExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::SubExpression";
}

void MulExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::MulExpression";
}

void DivExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::DivExpression";
}

void RemExpression::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    BinaryExpression::dump(dumpNode, state);
    dumpNode->nodeName = "ast::RemExpression";
}
}
