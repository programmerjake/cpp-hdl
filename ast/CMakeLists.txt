# Copyright 2018 Jacob Lifshay
#
# This file is part of Cpp-HDL.
#
# Cpp-HDL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Cpp-HDL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cpp-HDL.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

add_library(ast INTERFACE)

set(SOURCES
    assignment_expression.cpp
    binary_expression.cpp
    block_statement.cpp
    break_statement.cpp
    cast_expression.cpp
    cat_expression.cpp
    comment.cpp
    conditional_expression.cpp
    connect_expression.cpp
    const_statement.cpp
    const_statement_part.cpp
    continue_statement.cpp
    empty_statement.cpp
    enum.cpp
    enum_statement.cpp
    expression.cpp
    expression_statement.cpp
    fill_expression.cpp
    flip_type.cpp
    for_statement.cpp
    function.cpp
    function_call_expression.cpp
    function_parameter.cpp
    function_statement.cpp
    function_type.cpp
    if_statement.cpp
    import.cpp
    input_output_statement.cpp
    input_output_statement_part.cpp
    int_type.cpp
    interface.cpp
    interface_statement.cpp
    let_statement.cpp
    let_statement_part.cpp
    list_expression.cpp
    match_pattern.cpp
    match_statement.cpp
    memory_type.cpp
    module.cpp
    module_statement.cpp
    number_expression.cpp
    paren_expression.cpp
    pop_count_expression.cpp
    reg_statement.cpp
    reg_statement_part.cpp
    return_statement.cpp
    scoped_id.cpp
    scoped_id_expression.cpp
    scoped_id_type.cpp
    slice_expression.cpp
    statement.cpp
    symbol_table.cpp
    template_arguments.cpp
    template_parameter.cpp
    template_parameters.cpp
    top_level_module.cpp
    type.cpp
    type_of_type.cpp
    type_statement.cpp
    type_template_parameter.cpp
    unary_expression.cpp
    value_template_parameter.cpp)

foreach(i ${SOURCES})
    target_sources(ast INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${i}")
endforeach(i)

