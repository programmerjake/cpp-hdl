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
#define AST_VISITOR_NODE_LIST(function_macro)                                              \
    function_macro(Module) function_macro(TransparentTypeAlias) function_macro(Variable)   \
        function_macro(FlippedBundle) function_macro(Bundle) function_macro(BitVectorType) \
            function_macro(NamedInterface) function_macro(AnonymousInterface)

#define AST_VISITOR_DECLARE_NODE_TYPE(T) class T;

AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_NODE_TYPE)

#undef AST_VISITOR_DECLARE_NODE_TYPE

#define AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION(T) virtual VisitStatus visit(const T *node) = 0;

struct ConstVisitor
{
    virtual ~ConstVisitor() = default;
    AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION)
};

#undef AST_VISITOR_DECLARE_CONST_VISIT_FUNCTION
#define AST_VISITOR_DECLARE_VISIT_FUNCTION(T)       \
    virtual VisitStatus visit(T *node)              \
    {                                               \
        return visit(static_cast<const T *>(node)); \
    }

struct Visitor : public ConstVisitor
{
    using ConstVisitor::visit;
    AST_VISITOR_NODE_LIST(AST_VISITOR_DECLARE_VISIT_FUNCTION)
};

#undef AST_VISITOR_DECLARE_VISIT_FUNCTION
}
