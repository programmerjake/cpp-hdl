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
#include "comment.h"
#include "expression.h"
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "symbol_scope.h"
#include "type.h"
#include <vector>
#include "../util/dump_tree.h"

namespace ast
{
class Enum;

class EnumPart final : public Node, public Symbol
{
public:
    ConsecutiveComments beforeNameComments;
    ConsecutiveComments beforeEqualComments;
    Expression *value;
    Enum *parentEnum;
    explicit EnumPart(parse::LocationRange locationRange,
                      ConsecutiveComments beforeNameComments,
                      parse::LocationRange symbolLocationRange,
                      util::StringPool::Entry name,
                      ConsecutiveComments beforeEqualComments,
                      Expression *value) noexcept : Node(locationRange),
                                                    Symbol(symbolLocationRange, name),
                                                    beforeNameComments(beforeNameComments),
                                                    beforeEqualComments(beforeEqualComments),
                                                    value(value),
                                                    parentEnum(nullptr)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class Enum final : public Node, public Symbol, public SymbolScope
{
public:
    struct Part final
    {
        EnumPart *enumPart;
        ConsecutiveComments beforeCommaComments;
        constexpr Part(EnumPart *enumPart, ConsecutiveComments beforeCommaComments) noexcept
            : enumPart(enumPart),
              beforeCommaComments(beforeCommaComments)
        {
        }
    };
    ConsecutiveComments beforeEnumComments;
    ConsecutiveComments beforeNameComments;
    ConsecutiveComments beforeColonComments;
    Type *underlyingType;
    ConsecutiveComments beforeLBraceComments;
    std::vector<Part> parts;
    ConsecutiveComments beforeRBraceComments;
    explicit Enum(parse::LocationRange locationRange,
                  SymbolLookupChain symbolLookupChain,
                  SymbolTable *symbolTable,
                  ConsecutiveComments beforeEnumComments,
                  ConsecutiveComments beforeNameComments,
                  parse::LocationRange symbolLocationRange,
                  util::StringPool::Entry name,
                  ConsecutiveComments beforeColonComments,
                  Type *underlyingType,
                  ConsecutiveComments beforeLBraceComments,
                  std::vector<Part> parts,
                  ConsecutiveComments beforeRBraceComments) noexcept
        : Node(locationRange),
          Symbol(symbolLocationRange, name),
          SymbolScope(symbolLookupChain, symbolTable),
          beforeEnumComments(beforeEnumComments),
          beforeNameComments(beforeNameComments),
          beforeColonComments(beforeColonComments),
          underlyingType(underlyingType),
          beforeLBraceComments(beforeLBraceComments),
          parts(std::move(parts)),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
