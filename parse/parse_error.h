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

#include "source.h"
#include <stdexcept>
#include <string>
#include "../util/string_view.h"

namespace parse
{
class ParseError : public std::runtime_error
{
public:
    LocationRange errorLocation;
    static std::string makeErrorMessage(LocationRange errorLocation, util::string_view message);
    ParseError(LocationRange errorLocation, util::string_view message)
        : runtime_error(makeErrorMessage(errorLocation, message)), errorLocation(errorLocation)
    {
    }
};
}
