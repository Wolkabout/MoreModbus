#
# Copyright (C) 2020 WolkAbout Technology s.r.o.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#


cmake_minimum_required(VERSION 3.5)
project(MoreModbus C CXX)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}    -Wall -Wextra -pedantic                                                    \
                                            -pedantic-errors -Wcast-align                                              \
                                            -Wcast-qual -Wconversion                                                   \
                                            -Wdisabled-optimization                                                    \
                                            -Wfloat-equal -Wformat=2                                                   \
                                            -Werror=init-self -Werror=return-type                                      \
                                            -Werror=missing-field-initializers -Wmissing-format-attribute              \
                                            -Wmissing-include-dirs -Wmissing-noreturn                                  \
                                            -Werror=pointer-arith                                                      \
                                            -Wno-packed  -Wno-padded -Wredundant-decls                                 \
                                            -Werror=shadow -Werror=stack-protector                                     \
                                            -Wstrict-aliasing=2                                                        \
                                            -Werror=unreachable-code -Wno-unused                                       \
                                            -Wvariadic-macros                                                          \
                                            -Wwrite-strings -Werror=non-virtual-dtor                                   \
                                            -fprofile-arcs -ftest-coverage -g")

# library output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_INCLUDE_DIRECTORY ${CMAKE_PREFIX_PATH}/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(dependencies)

# required dependency
find_package(Threads REQUIRED)

# include files
include_directories("src")

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} modbus pthread)
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
add_dependencies(${PROJECT_NAME} libmodbus libgtest)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")

# Tests
include_directories("tests")

file(GLOB_RECURSE TEST_HEADER_FILES "tests/*.h" "tests/*.hpp")
file(GLOB_RECURSE TEST_SOURCE_FILES "tests/*.c" "tests/*.cpp")

add_executable(${PROJECT_NAME}Tests ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})
target_link_libraries(${PROJECT_NAME}Tests ${PROJECT_NAME} gtest_main gtest gmock_main gmock)
target_include_directories(${PROJECT_NAME}Tests PUBLIC ${CMAKE_LIBRARY_INCLUDE_DIRECTORY})
set_target_properties(${PROJECT_NAME}Tests PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

add_test(gtest ${PROJECT_NAME}Tests)

# Example
include_directories("example")

file(GLOB_RECURSE EXAMPLE_HEADER_FILES "example/*.h" "example/*.hpp")
file(GLOB_RECURSE EXAMPLE_SOURCE_FILES "example/*.c" "example/*.cpp")

add_executable(${PROJECT_NAME}Example ${EXAMPLE_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}Example ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}Example PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

add_subdirectory(cmake)
