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

#include "type_pool.h"
#include "bit_vector_type.h"

namespace ast
{
const BitVectorType *TypePool::getBitVectorType(BitVectorType::Direction direction,
                                                math::BitVector::Kind kind,
                                                std::size_t bitWidth)
{
    auto &retval = bitVectorTypes[static_cast<std::underlying_type_t<BitVectorType::Direction>>(
        direction)][static_cast<std::underlying_type_t<math::BitVector::Kind>>(kind)][bitWidth];
    if(!retval)
    {
        auto flippedDirection = BitVectorType::flipDirection(direction);
        assert(BitVectorType::flipDirection(flippedDirection) == direction);
        retval = typeArena.create<BitVectorType>(
            direction, kind, bitWidth, nullptr, BitVectorType::PrivateAccessTag{});
        auto &flippedType =
            bitVectorTypes[static_cast<std::underlying_type_t<BitVectorType::Direction>>(
                flippedDirection)][static_cast<std::underlying_type_t<math::BitVector::Kind>>(kind)]
                          [bitWidth];
        if(!flippedType)
            flippedType = typeArena.create<BitVectorType>(
                flippedDirection, kind, bitWidth, nullptr, BitVectorType::PrivateAccessTag{});
        flippedType->flippedType = retval;
        retval->flippedType = flippedType;
    }
    return retval;
}
}



