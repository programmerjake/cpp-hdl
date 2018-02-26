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

#include "bit_vector_type_forward.h"
#include "../tokenizer.h"
#include "../math/bit_vector.h"
#include "type.h"
#include "type_pool.h"
#include "../util/string_view.h"
#include "ast_macros.h"

namespace ast
{
struct BitVectorTypeBuiltinAlias
{
    TokenType name;
    math::BitVector::Kind kind;
    std::size_t bitWidth;
    constexpr BitVectorTypeBuiltinAlias(TokenType name,
                                        math::BitVector::Kind kind,
                                        std::size_t bitWidth) noexcept : name(name),
                                                                         kind(kind),
                                                                         bitWidth(bitWidth)
    {
    }
};

class BitVectorType final : public Type
{
    friend class TypePool;

private:
    struct PrivateAccessTag
    {
        friend class TypePool;

    private:
        constexpr PrivateAccessTag() = default;
    };

public:
    using Direction = BitVectorTypeDirection;
    static constexpr Direction flipDirection(Direction v) noexcept
    {
        switch(v)
        {
        case Direction::Input:
            return Direction::Output;
        case Direction::Output:
            return Direction::Input;
        default:
            return v;
        }
    }
    static constexpr util::string_view getDirectionName(Direction v) noexcept
    {
        using namespace util::string_view_literals;
        switch(v)
        {
        case Direction::Input:
            return "input"_sv;
        case Direction::Output:
            return "output"_sv;
        case Direction::Reg:
            return "reg"_sv;
        }
        assert(v == Direction::Input);
        return ""_sv;
    }
    static constexpr bool isStatelessDirection(Direction v) noexcept
    {
        switch(v)
        {
        case Direction::Input:
        case Direction::Output:
            return false;
        case Direction::Reg:
            return true;
        }
        assert(v == Direction::Input);
        return true;
    }

public:
    Direction direction;
    math::BitVector::Kind kind;
    std::size_t bitWidth;

private:
    BitVectorType *flippedType;

public:
    BitVectorType(Direction direction,
                  math::BitVector::Kind kind,
                  std::size_t bitWidth,
                  BitVectorType *flippedType,
                  PrivateAccessTag)
        : Type({}, isStatelessDirection(direction)),
          direction(),
          kind(kind),
          bitWidth(bitWidth),
          flippedType(flippedType)
    {
        if(flippedType)
            flippedType->flippedType = this;
    }
    virtual const Type *getFlippedType() const noexcept override
    {
        return flippedType;
    }

public:
    using BuiltinAlias = BitVectorTypeBuiltinAlias;
    class BuiltinAliases
    {
    public:
        typedef const BuiltinAlias *iterator;
        typedef const BuiltinAlias *const_iterator;

    private:
        const BuiltinAlias *array;
        std::size_t count;

    public:
        template <std::size_t N>
        explicit constexpr BuiltinAliases(const BuiltinAlias(&array)[N]) noexcept : array(array),
                                                                                    count(N)
        {
        }
        constexpr const_iterator begin() const noexcept
        {
            return array;
        }
        constexpr const_iterator end() const noexcept
        {
            return array + count;
        }
        constexpr std::size_t size() const noexcept
        {
            return count;
        }
    };
    static BuiltinAliases getBuiltinAliases() noexcept
    {
        using namespace util::string_view_literals;
        static constexpr BuiltinAlias builtinAliases[] = {
            BuiltinAlias(TokenType::Bit, math::BitVector::Kind::Unsigned, 1),
            BuiltinAlias(TokenType::U8, math::BitVector::Kind::Unsigned, 8),
            BuiltinAlias(TokenType::U16, math::BitVector::Kind::Unsigned, 16),
            BuiltinAlias(TokenType::U32, math::BitVector::Kind::Unsigned, 32),
            BuiltinAlias(TokenType::U64, math::BitVector::Kind::Unsigned, 64),
            BuiltinAlias(TokenType::S8, math::BitVector::Kind::Signed, 8),
            BuiltinAlias(TokenType::S16, math::BitVector::Kind::Signed, 16),
            BuiltinAlias(TokenType::S32, math::BitVector::Kind::Signed, 32),
            BuiltinAlias(TokenType::S64, math::BitVector::Kind::Signed, 64),
        };
        return BuiltinAliases(builtinAliases);
    }
    AST_NODE_DECLARE_VISITOR()
};
}



