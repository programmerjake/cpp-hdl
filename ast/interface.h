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

#include "node.h"
#include "symbol.h"
#include "symbol_scope.h"
#include "comment.h"
#include "template_parameters.h"
#include "type.h"
#include "statement.h"
#include <vector>
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "../util/dump_tree.h"

namespace ast
{
class Interface final : public Node, public Symbol, public SymbolScope
{
public:
    ConsecutiveComments beforeInterfaceComments;
    ConsecutiveComments beforeNameComments;
    TemplateParameters *templateParameters;
    ConsecutiveComments beforeImplementsComments;
    Type *parentType;
    ConsecutiveComments beforeLBraceComments;
    std::vector<Statement *> statements;
    ConsecutiveComments beforeRBraceComments;
    explicit Interface(parse::LocationRange locationRange,
                       SymbolLookupChain symbolLookupChain,
                       SymbolTable *symbolTable,
                       ConsecutiveComments beforeInterfaceComments,
                       ConsecutiveComments beforeNameComments,
                       parse::LocationRange symbolLocationRange,
                       util::StringPool::Entry name,
                       TemplateParameters *templateParameters,
                       ConsecutiveComments beforeImplementsComments,
                       Type *parentType,
                       ConsecutiveComments beforeLBraceComments,
                       std::vector<Statement *> statements,
                       ConsecutiveComments beforeRBraceComments) noexcept
        : Node(locationRange),
          Symbol(symbolLocationRange, name),
          SymbolScope(symbolLookupChain, symbolTable),
          beforeInterfaceComments(beforeInterfaceComments),
          beforeNameComments(beforeNameComments),
          templateParameters(templateParameters),
          beforeImplementsComments(beforeImplementsComments),
          parentType(parentType),
          beforeLBraceComments(beforeLBraceComments),
          statements(std::move(statements)),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
