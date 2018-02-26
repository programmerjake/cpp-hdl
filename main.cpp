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
#include <fstream>
#include "tokenizer.h"
#include "util/string_view.h"
#include "parse_error.h"
#include "math/bit_vector.h"
#include "parser.h"
#include "output_generator.h"

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
        module->dump(std::cerr);
        std::cerr << std::endl;
        for(auto &outputGeneratorCreator : outputGeneratorCreators)
        {
            std::string outputFileName = argv[1];
            outputFileName += '.';
            outputFileName += outputGeneratorCreator.outputFileExtension;
            std::ofstream os(outputFileName);
            os.exceptions(std::ios::badbit | std::ios::failbit);
            auto params = outputGeneratorCreator.createParams();
            params->outputStream = &os;
            outputGeneratorCreator.create(std::move(params))->run(module);
        }
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
