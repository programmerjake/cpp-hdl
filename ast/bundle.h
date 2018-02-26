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

#include "type.h"
#include "../source.h"
#include "ast_macros.h"
#include "symbol.h"
#include "symbol_scope.h"
#include <vector>
#include "variable.h"
#include "../util/string_pool.h"
#include "symbol_lookup_chain.h"
#include "symbol_table.h"
#include "context.h"

namespace ast
{
class Bundle;

class FlippedBundle final : public Type
{
    friend class Bundle;

private:
    Bundle *const flippedType;

private:
    struct PrivateAccessTag
    {
    private:
        PrivateAccessTag() = default;
    };

public:
    FlippedBundle(PrivateAccessTag, LocationRange locationRange, Bundle *flippedType)
        : Type(locationRange, true), flippedType(flippedType)
    {
    }
    virtual const Type *getFlippedType() const noexcept override;
    AST_NODE_DECLARE_VISITOR()
};

class Bundle final : public Type, public Symbol, public SymbolScope
{
    friend class FlippedBundle;

private:
    std::vector<Variable *> members{};
    FlippedBundle *flippedType;
    bool defined = false;

public:
    Bundle(LocationRange locationRange,
           LocationRange nameLocation,
           util::StringPool::Entry name,
           SymbolLookupChain symbolLookupChain,
           SymbolTable *symbolTable,
           Context &context)
        : Type(locationRange, true),
          Symbol(nameLocation, name),
          SymbolScope(symbolLookupChain, symbolTable),
          flippedType(context.arena.create<FlippedBundle>(
              FlippedBundle::PrivateAccessTag{}, locationRange, this))
    {
    }
    bool isDefined() const noexcept
    {
        return defined;
    }
    const std::vector<Variable *> &getMembers() const noexcept
    {
        assert(defined);
        return members;
    }
    void define(std::vector<Variable *> newMembers) noexcept
    {
        assert(!defined);
        defined = true;
        members.swap(newMembers);
        isStateless = true;
        for(auto *member : members)
        {
            if(!member->type->isStateless)
            {
                isStateless = false;
                break;
            }
        }
        flippedType->isStateless = isStateless;
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return flippedType;
    }
    AST_NODE_DECLARE_VISITOR()
};

inline const Type *FlippedBundle::getFlippedType() const noexcept
{
    return flippedType;
}
}


