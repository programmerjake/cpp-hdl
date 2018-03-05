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
#include "../parse/source.h"
#include "../util/string_pool.h"
#include "template_arguments.h"

namespace ast
{
class ScopedId : public Node
{
public:
    ConsecutiveComments beforeColonColonComments;
    bool hasColonColon;
    ScopedId *parentScope;
    ConsecutiveComments beforeNameComments;
    parse::LocationRange nameLocationRange;
    util::StringPool::Entry name;
    TemplateArguments *templateArguments;
    explicit ScopedId(parse::LocationRange locationRange,
                      ConsecutiveComments beforeColonColonComments,
                      bool hasColonColon,
                      ScopedId *parentScope,
                      ConsecutiveComments beforeNameComments,
                      parse::LocationRange nameLocationRange,
                      util::StringPool::Entry name,
                      TemplateArguments *templateArguments) noexcept
        : Node(locationRange),
          beforeColonColonComments(beforeColonColonComments),
          hasColonColon(hasColonColon),
          parentScope(parentScope),
          beforeNameComments(beforeNameComments),
          nameLocationRange(nameLocationRange),
          name(name),
          templateArguments(templateArguments)
    {
    }
};
}
