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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cpp-hdl CXX C)

set(CMAKE_THREAD_PREFER_PTHREAD 1)
set(THREADS_PREFER_PTHREAD_FLAG 1)
find_package(Threads REQUIRED)

find_package(GMP REQUIRED)

if(WIN32)
    add_definitions(NOMINMAX)
endif()

add_compile_options(-Wall)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Werror "-Wno-error=#warnings")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Werror "-Wno-error=cpp")
endif()

set(SOURCES main.cpp)

add_executable(hdlc ${SOURCES})

set(SUBDIRS
    math
    parse
    util)

foreach(i ${SUBDIRS})
    add_subdirectory(${i})
    target_link_libraries(hdlc ${i})
endforeach(i)
