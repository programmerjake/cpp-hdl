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

#include "match_pattern.h"

namespace ast
{
void MatchPattern::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Node::dump(dumpNode, state);
    dumpNode->nodeName = "ast::MatchPattern";
}

void NumberPatternMatchPattern::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    MatchPattern::dump(dumpNode, state);
    dumpNode->nodeName = "ast::NumberPatternMatchPattern";
    state.setSimple(dumpNode, "beforeNumberComments", beforeNumberComments);
    state.setSimple(dumpNode, "pattern", pattern);
}

void RangeMatchPattern::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    MatchPattern::dump(dumpNode, state);
    dumpNode->nodeName = "ast::RangeMatchPattern";
    state.setPointer(dumpNode, "firstExpression", firstExpression);
    state.setSimple(dumpNode, "beforeToComments", beforeToComments);
    state.setPointer(dumpNode, "secondExpression", secondExpression);
}
}
