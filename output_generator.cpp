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

#include "output_generator.h"
#include "verilog_output_generator.h"

std::pair<const OutputGeneratorCreator *, std::size_t>
    OutputGeneratorCreators::getOutputGeneratorCreators() noexcept
{
    using namespace util::string_view_literals;
    static constexpr std::initializer_list<OutputGeneratorCreator> outputGeneratorCreators = {
        {"verilog"_sv, "v"_sv, VerilogOutputGenerator::create, VerilogOutputGenerator::createParams},
    };
    return {outputGeneratorCreators.begin(), outputGeneratorCreators.size()};
}
