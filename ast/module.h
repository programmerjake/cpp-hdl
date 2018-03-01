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

#include "statement.h"
#include "symbol.h"
#include "comment.h"
#include "template_parameters.h"
#include "type.h"
#include <vector>
#include "../parse/source.h"
#include "../util/string_pool.h"

namespace ast
{
class Module final : public Statement, public Symbol
{
public:
    ConsecutiveComments beforeModuleComments;
    ConsecutiveComments beforeNameComments;
    TemplateParameters *templateParameters;
    ConsecutiveComments beforeImplementsComments;
    Type *parentType;
    ConsecutiveComments beforeLBraceComments;
    std::vector<Statement *> statements;
    ConsecutiveComments beforeRBraceComments;
    explicit Module(parse::LocationRange locationRange,
                    ConsecutiveComments beforeModuleComments,
                    ConsecutiveComments beforeNameComments,
                    parse::LocationRange symbolLocationRange,
                    util::StringPool::Entry name,
                    TemplateParameters *templateParameters,
                    ConsecutiveComments beforeImplementsComments,
                    Type *parentType,
                    ConsecutiveComments beforeLBraceComments,
                    std::vector<Statement *> statements,
                    ConsecutiveComments beforeRBraceComments) noexcept
        : Statement(locationRange),
          Symbol(symbolLocationRange, name),
          beforeModuleComments(beforeModuleComments),
          beforeNameComments(beforeNameComments),
          templateParameters(templateParameters),
          beforeImplementsComments(beforeImplementsComments),
          parentType(parentType),
          beforeLBraceComments(beforeLBraceComments),
          statements(std::move(statements)),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
};
}
