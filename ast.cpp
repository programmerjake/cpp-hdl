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

#include "ast.h"

namespace ast
{
struct DumpVisitor final : public ConstVisitor
{
    std::size_t indentLevel = 0;
    std::ostream &os;
    struct Indent
    {
        std::size_t level = 0;
        friend std::ostream &operator<<(std::ostream &os, const Indent &indent)
        {
            for(std::size_t i = 0; i < indent.level; i++)
                os << "    ";
            return os;
        }
    };
    Indent indent;
    struct PushIndent
    {
        std::size_t &level;
        std::size_t previousLevel;
        explicit PushIndent(DumpVisitor *dumpVisitor, int amount = 1) noexcept
            : level(dumpVisitor->indent.level),
              previousLevel(level)
        {
            level += amount;
        }
        PushIndent(const PushIndent &) = delete;
        PushIndent &operator=(const PushIndent &) = delete;
        ~PushIndent()
        {
            level = previousLevel;
        }
    };
    explicit DumpVisitor(std::ostream &os) : os(os)
    {
    }
    virtual VisitStatus visit(const Module *node) override;
};

#define AST_NODE_IMPLEMENT_VISITOR(T)                 \
    VisitStatus T::visit(Visitor &visitor)            \
    {                                                 \
        return visitor.visit(this);                   \
    }                                                 \
    VisitStatus T::visit(ConstVisitor &visitor) const \
    {                                                 \
        return visitor.visit(this);                   \
    }

AST_NODE_IMPLEMENT_VISITOR(Module)

VisitStatus DumpVisitor::visit(const Module *node)
{
    os << indent << "module\n";
    PushIndent pushIndent(this);
    return VisitStatus::Continue;
}

std::unique_ptr<ConstVisitor> makeDumpVisitor(std::ostream &os)
{
    return std::unique_ptr<ConstVisitor>(new DumpVisitor(os));
}
}
