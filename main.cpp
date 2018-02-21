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

#include <iostream>
#include "tokenizer.h"
#include "string_view.h"
#include "parse_error.h"
#include "bit_vector.h"
#include "parser.h"

int main(int argc, char **argv)
{
    if(argc != 2 || (argv[1][0] == '-' && util::string_view(argv[1]) != "-"))
    {
        std::cerr << "usage: " << argv[0] << " <file>|-\n";
        return 1;
    }
    try
    {
        auto source = Source::makeSourceFromFile(argv[1]);
        ast::Context context;
        auto module = Parser::parseTopLevelModule(Tokenizer(source.get()), context);
        module->dump();
    }
    catch(ParseError &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
