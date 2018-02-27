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

#include "template_parameter_kind.h"
#include "type.h"
#include "../source.h"
#include "ast_macros.h"

namespace ast
{
class ModuleTemplateParameterKind final : public TemplateParameterKind
{
public:
    const Type *interface;
    ModuleTemplateParameterKind(LocationRange locationRange, bool isList, const Type *interface)
        : TemplateParameterKind(locationRange, isList, Kind::Value), interface(interface)
    {
    }
    virtual std::size_t getHash() const noexcept override
    {
        return TemplateParameterKind::getHash() + std::hash<const Type *>()(interface);
    }
    virtual bool isSame(const TemplateParameterKind &other) const noexcept override
    {
        if(auto casted_other = dynamic_cast<const ModuleTemplateParameterKind *>(&other))
            return TemplateParameterKind::isSame(other) && interface == casted_other->interface;
        return false;
    }
    virtual TemplateParameterKind *duplicate(util::Arena &arena) const override
    {
        return arena.create<ModuleTemplateParameterKind>(*this);
    }
    AST_NODE_DECLARE_VISITOR()
};
}
