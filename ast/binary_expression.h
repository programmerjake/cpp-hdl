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

#include "expression.h"
#include "comment.h"
#include "../parse/source.h"
#include "../util/dump_tree.h"

namespace ast
{
class BinaryExpression : public Expression
{
public:
    Expression *lhs;
    ConsecutiveComments beforeOperatorComments;
    Expression *rhs;
    explicit BinaryExpression(parse::LocationRange locationRange,
                              Expression *lhs,
                              ConsecutiveComments beforeOperatorComments,
                              Expression *rhs) noexcept
        : Expression(locationRange),
          lhs(lhs),
          beforeOperatorComments(beforeOperatorComments),
          rhs(rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override = 0;
};

class LogicalAndExpression final : public BinaryExpression
{
public:
    explicit LogicalAndExpression(parse::LocationRange locationRange,
                                  Expression *lhs,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class LogicalOrExpression final : public BinaryExpression
{
public:
    explicit LogicalOrExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class BitwiseAndExpression final : public BinaryExpression
{
public:
    explicit BitwiseAndExpression(parse::LocationRange locationRange,
                                  Expression *lhs,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class BitwiseOrExpression final : public BinaryExpression
{
public:
    explicit BitwiseOrExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class BitwiseXorExpression final : public BinaryExpression
{
public:
    explicit BitwiseXorExpression(parse::LocationRange locationRange,
                                  Expression *lhs,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class LeftShiftExpression final : public BinaryExpression
{
public:
    explicit LeftShiftExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class RightShiftExpression final : public BinaryExpression
{
public:
    explicit RightShiftExpression(parse::LocationRange locationRange,
                                  Expression *lhs,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareEqExpression final : public BinaryExpression
{
public:
    explicit CompareEqExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareNEExpression final : public BinaryExpression
{
public:
    explicit CompareNEExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareLEExpression final : public BinaryExpression
{
public:
    explicit CompareLEExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareGEExpression final : public BinaryExpression
{
public:
    explicit CompareGEExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareLTExpression final : public BinaryExpression
{
public:
    explicit CompareLTExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class CompareGTExpression final : public BinaryExpression
{
public:
    explicit CompareGTExpression(parse::LocationRange locationRange,
                                 Expression *lhs,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class AddExpression final : public BinaryExpression
{
public:
    explicit AddExpression(parse::LocationRange locationRange,
                           Expression *lhs,
                           ConsecutiveComments beforeOperatorComments,
                           Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class SubExpression final : public BinaryExpression
{
public:
    explicit SubExpression(parse::LocationRange locationRange,
                           Expression *lhs,
                           ConsecutiveComments beforeOperatorComments,
                           Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class MulExpression final : public BinaryExpression
{
public:
    explicit MulExpression(parse::LocationRange locationRange,
                           Expression *lhs,
                           ConsecutiveComments beforeOperatorComments,
                           Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class DivExpression final : public BinaryExpression
{
public:
    explicit DivExpression(parse::LocationRange locationRange,
                           Expression *lhs,
                           ConsecutiveComments beforeOperatorComments,
                           Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class RemExpression final : public BinaryExpression
{
public:
    explicit RemExpression(parse::LocationRange locationRange,
                           Expression *lhs,
                           ConsecutiveComments beforeOperatorComments,
                           Expression *rhs) noexcept
        : BinaryExpression(locationRange, lhs, beforeOperatorComments, rhs)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
