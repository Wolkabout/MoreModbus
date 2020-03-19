cmake_minimum_required(VERSION 3.5)
project(wolkabout_modbus C CXX)

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
                                            -Wwrite-strings -Werror=non-virtual-dtor")

find_package(Threads REQUIRED)
include_directories("src")

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.cpp)

link_directories(${PROJECT_SOURCE_DIR}/out/lib)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} modbus pthread)

file(GLOB_RECURSE EXAMPLE_SOURCE_FILES example/*.cpp)

add_executable(example ${EXAMPLE_SOURCE_FILES})
target_link_libraries(example ${PROJECT_NAME})

add_subdirectory("cmake")