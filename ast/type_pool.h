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

#include "../util/arena.h"
#include <unordered_map>
#include <type_traits>
#include "../math/bit_vector.h"
#include "bit_vector_type_forward.h"

namespace ast
{
class TypePool final
{
private:
    util::Arena typeArena;
    std::unordered_map<std::underlying_type_t<BitVectorTypeDirection>,
                       std::unordered_map<std::underlying_type_t<math::BitVector::Kind>,
                                          std::unordered_map<std::size_t, BitVectorType *>>>
        bitVectorTypes;

public:
    const BitVectorType *getBitVectorType(BitVectorTypeDirection direction,
                                          math::BitVector::Kind kind,
                                          std::size_t bitWidth);
};
}
