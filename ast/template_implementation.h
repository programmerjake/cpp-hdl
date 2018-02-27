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

#include "template_parameter.h"
#include <vector>
#include "template_argument.h"
#include "context.h"

namespace ast
{
class TemplateImplementation
{
public:
    virtual ~TemplateImplementation() = default;
    std::vector<const TemplateParameter *> templateParameters;
    TemplateImplementation(std::vector<const TemplateParameter *> templateParameters)
        : templateParameters(std::move(templateParameters))
    {
    }
    virtual const Node *instantiate(std::vector<const TemplateArgument *> templateArguments,
                                    ast::Context &context) const = 0;
};
}
