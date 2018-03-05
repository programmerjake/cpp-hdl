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
#include "comment.h"
#include "type.h"
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "../util/dump_tree.h"

namespace ast
{
class TypeTemplateParameter final : public TemplateParameter
{
public:
    ConsecutiveComments beforeTypeComments;
    ConsecutiveComments beforeImplementsComments;
    Type *parentType;
    explicit TypeTemplateParameter(parse::LocationRange locationRange,
                                   ConsecutiveComments beforeTypeComments,
                                   ConsecutiveComments beforeNameComments,
                                   parse::LocationRange symbolLocationRange,
                                   util::StringPool::Entry name,
                                   ConsecutiveComments beforeImplementsComments,
                                   Type *parentType,
                                   ConsecutiveComments beforeDotDotDotComments,
                                   bool hasDotDotDot) noexcept
        : TemplateParameter(locationRange,
                            beforeNameComments,
                            symbolLocationRange,
                            name,
                            beforeDotDotDotComments,
                            hasDotDotDot),
          beforeTypeComments(beforeTypeComments),
          beforeImplementsComments(beforeImplementsComments),
          parentType(parentType)
    {
    }
    virtual void dump(util::DumpTree *dumpNode, util::DumpState &state) const override;
};
}
