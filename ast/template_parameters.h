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

#include "node.h"
#include "comment.h"
#include "template_parameter.h"
#include <vector>
#include "../parse/source.h"
#include "../util/dump_tree.h"

namespace ast
{
class TemplateParameters final : public Node
{
public:
    struct Part final
    {
        ConsecutiveComments beforeCommaComments;
        TemplateParameter *templateParameter;
        constexpr Part(ConsecutiveComments beforeCommaComments,
                       TemplateParameter *templateParameter) noexcept
            : beforeCommaComments(beforeCommaComments),
              templateParameter(templateParameter)
        {
        }
    };
    ConsecutiveComments beforeLBraceComments;
    TemplateParameter *firstTemplateParameter;
    std::vector<Part> parts;
    ConsecutiveComments beforeRBraceComments;
    explicit TemplateParameters(parse::LocationRange locationRange,
                                ConsecutiveComments beforeLBraceComments,
                                TemplateParameter *firstTemplateParameter,
                                std::vector<Part> parts,
                                ConsecutiveComments beforeRBraceComments) noexcept
        : Node(locationRange),
          beforeLBraceComments(beforeLBraceComments),
          firstTemplateParameter(firstTemplateParameter),
          parts(std::move(parts)),
          beforeRBraceComments(beforeRBraceComments)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
