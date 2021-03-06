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

add_library(parse INTERFACE)

set(SOURCES
    parse_error.cpp
    parser.cpp
    source.cpp
    token.cpp
    tokenizer.cpp)

foreach(i ${SOURCES})
    target_sources(parse INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${i}")
endforeach(i)

target_link_libraries(parse INTERFACE Threads::Threads)
