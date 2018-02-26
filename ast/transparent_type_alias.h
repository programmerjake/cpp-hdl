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
#include "symbol.h"
#include "../source.h"
#include "../util/string_pool.h"
#include "ast_macros.h"

namespace ast
{
class TransparentTypeAlias final : public Type, public Symbol
{
public:
    const Type *aliasedType;
    TransparentTypeAlias(LocationRange locationRange,
                         LocationRange nameLocation,
                         util::StringPool::Entry name,
                         const Type *aliasedType)
        : Type(locationRange, aliasedType->canonicalType),
          Symbol(nameLocation, name),
          aliasedType(aliasedType)
    {
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return aliasedType->getFlippedType();
    }
    AST_NODE_DECLARE_VISITOR()
};
}
