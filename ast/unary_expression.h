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
class UnaryExpression : public Expression
{
public:
    ConsecutiveComments beforeOperatorComments;
    Expression *argument;
    explicit UnaryExpression(parse::LocationRange locationRange,
                             ConsecutiveComments beforeOperatorComments,
                             Expression *argument) noexcept
        : Expression(locationRange),
          beforeOperatorComments(beforeOperatorComments),
          argument(argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override = 0;
};

class LogicalNotExpression final : public UnaryExpression
{
public:
    explicit LogicalNotExpression(parse::LocationRange locationRange,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class BitwiseNotExpression final : public UnaryExpression
{
public:
    explicit BitwiseNotExpression(parse::LocationRange locationRange,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class UnaryPlusExpression final : public UnaryExpression
{
public:
    explicit UnaryPlusExpression(parse::LocationRange locationRange,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class UnaryMinusExpression final : public UnaryExpression
{
public:
    explicit UnaryMinusExpression(parse::LocationRange locationRange,
                                  ConsecutiveComments beforeOperatorComments,
                                  Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class AndReduceExpression final : public UnaryExpression
{
public:
    explicit AndReduceExpression(parse::LocationRange locationRange,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class OrReduceExpression final : public UnaryExpression
{
public:
    explicit OrReduceExpression(parse::LocationRange locationRange,
                                ConsecutiveComments beforeOperatorComments,
                                Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class XorReduceExpression final : public UnaryExpression
{
public:
    explicit XorReduceExpression(parse::LocationRange locationRange,
                                 ConsecutiveComments beforeOperatorComments,
                                 Expression *argument) noexcept
        : UnaryExpression(locationRange, beforeOperatorComments, argument)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
