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
#include "../parse/source.h"
#include "comment.h"
#include "expression.h"
#include "../util/dump_tree.h"

namespace ast
{
class IntegerType : public Type
{
public:
    const bool isSigned;
    explicit IntegerType(parse::LocationRange locationRange, bool isSigned) noexcept
        : Type(locationRange),
          isSigned(isSigned)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override = 0;
};

class UIntType final : public IntegerType
{
public:
    ConsecutiveComments beforeUIntComments;
    ConsecutiveComments beforeLBraceComments;
    Expression *bitCount;
    ConsecutiveComments beforeRBraceComments;
    explicit UIntType(parse::LocationRange locationRange,
                      ConsecutiveComments beforeUIntComments,
                      ConsecutiveComments beforeLBraceComments,
                      Expression *bitCount,
                      ConsecutiveComments beforeRBraceComments) noexcept
        : IntegerType(locationRange, false),
          beforeUIntComments(beforeUIntComments),
          beforeLBraceComments(beforeLBraceComments),
          bitCount(bitCount),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class SIntType final : public IntegerType
{
public:
    ConsecutiveComments beforeSIntComments;
    ConsecutiveComments beforeLBraceComments;
    Expression *bitCount;
    ConsecutiveComments beforeRBraceComments;
    explicit SIntType(parse::LocationRange locationRange,
                      ConsecutiveComments beforeSIntComments,
                      ConsecutiveComments beforeLBraceComments,
                      Expression *bitCount,
                      ConsecutiveComments beforeRBraceComments) noexcept
        : IntegerType(locationRange, true),
          beforeSIntComments(beforeSIntComments),
          beforeLBraceComments(beforeLBraceComments),
          bitCount(bitCount),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

template <bool Signed, std::size_t BitCount>
class GenericBuiltInIntegerType : public IntegerType
{
public:
    static constexpr bool isSigned = Signed;
    static constexpr std::size_t bitCount = BitCount;
    ConsecutiveComments beforeNameComments;
    explicit GenericBuiltInIntegerType(parse::LocationRange locationRange,
                                       ConsecutiveComments beforeNameComments) noexcept
        : IntegerType(locationRange, isSigned),
          beforeNameComments(beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override = 0;
};

template <bool Signed, std::size_t BitCount>
constexpr bool GenericBuiltInIntegerType<Signed, BitCount>::isSigned;

template <bool Signed, std::size_t BitCount>
constexpr std::size_t GenericBuiltInIntegerType<Signed, BitCount>::bitCount;

template <bool Signed, std::size_t BitCount>
void GenericBuiltInIntegerType<Signed, BitCount>::dump(util::DumpTree *dumpNode,
                                                       util::DumpState &state) const
{
    IntegerType::dump(dumpNode, state);
    dumpNode->nodeName = "ast::GenericBuiltInIntegerType";
    // state.setSimple(dumpNode, "isSigned", isSigned); // aliases parent class
    state.setSimple(dumpNode, "bitCount", bitCount);
    state.setSimple(dumpNode, "beforeNameComments", beforeNameComments);
}

class U8Type final : public GenericBuiltInIntegerType<false, 8>
{
public:
    explicit U8Type(parse::LocationRange locationRange,
                    ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class U16Type final : public GenericBuiltInIntegerType<false, 16>
{
public:
    explicit U16Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class U32Type final : public GenericBuiltInIntegerType<false, 32>
{
public:
    explicit U32Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class U64Type final : public GenericBuiltInIntegerType<false, 64>
{
public:
    explicit U64Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class S8Type final : public GenericBuiltInIntegerType<true, 8>
{
public:
    explicit S8Type(parse::LocationRange locationRange,
                    ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class S16Type final : public GenericBuiltInIntegerType<true, 16>
{
public:
    explicit S16Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class S32Type final : public GenericBuiltInIntegerType<true, 32>
{
public:
    explicit S32Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class S64Type final : public GenericBuiltInIntegerType<true, 64>
{
public:
    explicit S64Type(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};

class BitType final : public GenericBuiltInIntegerType<false, 1>
{
public:
    explicit BitType(parse::LocationRange locationRange,
                     ConsecutiveComments beforeNameComments) noexcept
        : GenericBuiltInIntegerType(locationRange, beforeNameComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
