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

#include "../ast/top_level_module.h"
#include "../ast/context.h"
#include "source.h"
#include <functional>
#include <string>
#include "parse_error.h"

namespace parse
{
struct Parser;

inline void defaultParseErrorHandler(LocationRange locationRange, std::string message)
{
    throw ParseError(locationRange, std::move(message));
}

ast::TopLevelModule *parseTopLevelModule(
    ast::Context &context,
    const Source *source,
    std::function<void(LocationRange locationRange, std::string message)> errorHandler =
        defaultParseErrorHandler);
}
