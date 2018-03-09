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

#pragma once

#include "statement.h"
#include "symbol_scope.h"
#include "comment.h"
#include "../parse/source.h"
#include <vector>
#include "../util/dump_tree.h"

namespace ast
{
class BlockStatement final : public Statement, public SymbolScope
{
public:
    ConsecutiveComments beforeLBraceComments;
    std::vector<Statement *> statements;
    ConsecutiveComments beforeRBraceComments;
    explicit BlockStatement(parse::LocationRange locationRange,
                            SymbolLookupChain symbolLookupChain,
                            SymbolTable *symbolTable,
                            ConsecutiveComments beforeLBraceComments,
                            std::vector<Statement *> statements,
                            ConsecutiveComments beforeRBraceComments) noexcept
        : Statement(locationRange),
          SymbolScope(symbolLookupChain, symbolTable),
          beforeLBraceComments(beforeLBraceComments),
          statements(std::move(statements)),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
