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

#include "visitor_forward.h"
#include "ast_macros.h"

namespace ast
{
// clang-format off
#define AST_VISITOR_NODE_LIST(f) \
    f(Module) \
    f(TransparentTypeAlias) \
    f(Variable) \
    f(FlippedBundle) \
    f(Bundle) \
    f(BitVectorType) \
    f(ModuleTemplate) \
    f(ModuleTemplateArgument) \
    f(ModuleTemplateParameterKind) \
    f(TemplateParameter) \
    f(ValueTemplateArgument) \
    f(ValueTemplateParameterKind)
// clang-format on

#define AST_VISITOR_DECLARE_NODE_TYPE(T) class T;

AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_NODE_TYPE)

#undef AST_VISITOR_DECLARE_NODE_TYPE

#define AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION(T) virtual VisitStatus visit(const T *node) = 0;

class ConstVisitor
{
public:
    virtual ~ConstVisitor() = default;
    AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION)
};

#undef AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION
#define AST_VISITOR_DECLARE_VISIT_FUNCTION(T)       \
    virtual VisitStatus visit(T *node)              \
    {                                               \
        return visit(static_cast<const T *>(node)); \
    }

class Visitor : public ConstVisitor
{
public:
    using ConstVisitor::visit;
    AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_VISIT_FUNCTION)
};

#undef AST_VISITOR_DECLARE_VISIT_FUNCTION
}
