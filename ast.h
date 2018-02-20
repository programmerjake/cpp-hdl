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

#include "arena.h"
#include "bit_vector.h"
#include "source.h"
#include <vector>
#include <memory>
#include <iostream>

namespace ast
{
enum class VisitStatus : bool
{
    Stop = false,
    Continue = true
};

struct Visitor;
struct ConstVisitor;

#define AST_NODE_DECLARE_VISITOR()                              \
public:                                                         \
    virtual VisitStatus visit(Visitor &visitor) override final; \
    virtual VisitStatus visit(ConstVisitor &visitor) const override final;

struct Node
{
    LocationRange locationRange;
    explicit Node(LocationRange locationRange) : locationRange(locationRange)
    {
    }
    virtual ~Node() = default;
    virtual VisitStatus visit(Visitor &visitor) = 0;
    virtual VisitStatus visit(ConstVisitor &visitor) const = 0;
    void dump(std::ostream &os = std::cerr) const;
};

struct Module final : public Node
{
    explicit Module(LocationRange locationRange) : Node(locationRange)
    {
    }
    AST_NODE_DECLARE_VISITOR()
};

#undef AST_NODE_DECLARE_VISITOR
#define AST_NODE_DECLARE_VISIT_FUNCTION(T) virtual VisitStatus visit(const T *node) = 0;

struct ConstVisitor
{
    virtual ~ConstVisitor() = default;
    AST_NODE_DECLARE_VISIT_FUNCTION(Module)
};

#undef AST_NODE_DECLARE_VISIT_FUNCTION
#define AST_NODE_DECLARE_VISIT_FUNCTION(T)                     \
    virtual VisitStatus visit(T *node)                         \
    {                                                          \
        return static_cast<ConstVisitor *>(this)->visit(node); \
    }

struct Visitor : public ConstVisitor
{
    virtual ~Visitor() = default;
    AST_NODE_DECLARE_VISIT_FUNCTION(Module)
};

#undef AST_NODE_DECLARE_VISIT_FUNCTION

struct DumpVisitor;

std::unique_ptr<ConstVisitor> makeDumpVisitor(std::ostream &os = std::cerr);

inline void Node::dump(std::ostream &os) const
{
    visit(*makeDumpVisitor(os));
}
}
