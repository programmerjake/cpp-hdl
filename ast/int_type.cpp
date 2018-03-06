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

#include "int_type.h"

namespace ast
{
void IntegerType::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    Type::dump(dumpNode, state);
    dumpNode->nodeName = "ast::IntegerType";
    state.setSimple(dumpNode, "isSigned", isSigned);
}

void UIntType::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    IntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::UIntType";
    state.setSimple(dumpNode, "beforeUIntComments", beforeUIntComments);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
    state.setPointer(dumpNode, "bitCount", bitCount);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
}

void SIntType::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    IntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::SIntType";
    state.setSimple(dumpNode, "beforeSIntComments", beforeSIntComments);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
    state.setPointer(dumpNode, "bitCount", bitCount);
    state.setSimple(dumpNode, "beforeLBraceComments", beforeLBraceComments);
}

void U8Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::U8Type";
}

void U16Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::U16Type";
}

void U32Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::U32Type";
}

void U64Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::U64Type";
}

void S8Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::S8Type";
}

void S16Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::S16Type";
}

void S32Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::S32Type";
}

void S64Type::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::S64Type";
}

void BitType::dump(util::DumpTree *dumpNode, util::DumpState &state) const
{
    GenericBuiltInIntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::BitType";
}
}
