#
# Copyright 2023 Wolkabout Technology s.r.o.
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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}     -Wall -Wextra -pedantic                                                    \
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

# Setup the options for tests
OPTION(BUILD_GTEST "Build the library with gtest" ON)
OPTION(BUILD_TESTS "Build the unit tests for the library" ON)
if (${BUILD_TESTS})
    if (NOT ${BUILD_GTEST})
        message(FATAL "Cannot Build Tests with BUILD_GTEST set to false.")
    endif ()

    # Setup test coverage if it is necessary
    OPTION(TEST_COVERAGE "Build the library for test coverage" ON)
    if (${TEST_COVERAGE})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g")
    endif ()
endif ()

# Setup the options for the examples
OPTION(BUILD_EXAMPLES "Build the examples/runtimes for testing" ON)

# Check if the paths for output are set, if not, we can set them ourselves
if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif ()

if (NOT DEFINED CMAKE_LIBRARY_INCLUDE_DIRECTORY)
    set(CMAKE_LIBRARY_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/include")
endif ()

if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif ()

if (NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/build")
endif ()

# And this is how we're importing the submodules
add_subdirectory(dependencies)

# And add WolkSDK-Cpp if there is no WolkSDK-Cpp
if (NOT TARGET WolkAboutCoreUtility)
    option(BUILD_CONNECTIVITY "Build the library with Paho MQTT and allow MQTT connection to the platform." OFF)
    option(BUILD_POCO "Build the library with Poco." OFF)
    option(BUILD_AWS_LOG_UPLOADER "Build the library with AwsLogUploader." OFF)

    add_subdirectory(WolkSDK-Cpp)
endif ()

# Add the pthread dependency too
find_package(Threads REQUIRED)

# Set up the paths from where we're going to link the libraries
link_directories(${CMAKE_PREFIX_PATH}/lib)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
include_directories(SYSTEM "${CMAKE_PREFIX_PATH}/include")

# Define the version of the library
set(MOREMODBUS_VERSION_MAJOR 0)
set(MOREMODBUS_VERSION_MINOR 5)
set(MOREMODBUS_VERSION_PATCH 1)
set(MOREMODBUS_VERSION_SUFFIX)

set(MOREMODBUS_VERSION_STRING "${MOREMODBUS_VERSION_MAJOR}.${MOREMODBUS_VERSION_MINOR}.${MOREMODBUS_VERSION_PATCH}")
if (NOT "${MOREMODBUS_VERSION_SUFFIX}" STREQUAL "")
    set(MOREMODBUS_VERSION_STRING "${MOREMODBUS_VERSION_STRING}-${MOREMODBUS_VERSION_SUFFIX}")
endif ()

# Set up the library
set(SOURCE_FILES more_modbus/mappings/BoolMapping.cpp
        more_modbus/mappings/FloatMapping.cpp
        more_modbus/mappings/Int16Mapping.cpp
        more_modbus/mappings/Int32Mapping.cpp
        more_modbus/mappings/StringMapping.cpp
        more_modbus/mappings/UInt16Mapping.cpp
        more_modbus/mappings/UInt32Mapping.cpp
        more_modbus/modbus/LibModbusSerialRtuClient.cpp
        more_modbus/modbus/LibModbusTcpIpClient.cpp
        more_modbus/modbus/ModbusClient.cpp
        more_modbus/modbus/ModbusGroupReader.cpp
        more_modbus/modbus/ModbusMappingReader.cpp
        more_modbus/utilities/DataParsers.cpp
        more_modbus/ModbusDevice.cpp
        more_modbus/ModbusReader.cpp
        more_modbus/RegisterGroup.cpp
        more_modbus/RegisterMapping.cpp)
set(HEADER_FILES more_modbus/mappings/BoolMapping.h
        more_modbus/mappings/FloatMapping.h
        more_modbus/mappings/Int16Mapping.h
        more_modbus/mappings/Int32Mapping.h
        more_modbus/mappings/StringMapping.h
        more_modbus/mappings/UInt16Mapping.h
        more_modbus/mappings/UInt32Mapping.h
        more_modbus/modbus/LibModbusSerialRtuClient.h
        more_modbus/modbus/LibModbusTcpIpClient.h
        more_modbus/modbus/ModbusClient.h
        more_modbus/modbus/ModbusGroupReader.h
        more_modbus/modbus/ModbusMappingReader.h
        more_modbus/utilities/DataParsers.h
        more_modbus/ModbusDevice.h
        more_modbus/ModbusReader.h
        more_modbus/RegisterGroup.h
        more_modbus/RegisterMapping.h)

file(COPY more_modbus/ DESTINATION ${CMAKE_LIBRARY_INCLUDE_DIRECTORY}/more_modbus PATTERN *.cpp EXCLUDE)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} WolkAboutCoreUtility modbus pthread)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_LIBRARY_INCLUDE_DIRECTORY})
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${MOREMODBUS_VERSION_STRING} SOVERSION ${MOREMODBUS_VERSION_MAJOR}.${MOREMODBUS_VERSION_MINOR}.${MOREMODBUS_VERSION_PATCH})
add_dependencies(${PROJECT_NAME} libmodbus)
if (TARGET WolkSDK-Cpp)
    add_dependencies(${PROJECT_NAME} WolkSDK-Cpp)
endif ()

# Tests
if (${BUILD_TESTS})
    set(TEST_SOURCE_FILES tests/ComplexMappingsTests.cpp
            tests/DataParsersTest.cpp
            tests/MappingsTests.cpp
            tests/ModbusClientTests.cpp
            tests/ModbusDeviceTests.cpp
            tests/ModbusReaderTests.cpp
            tests/ProperReadingTest.cpp
            tests/RegisterGroupTests.cpp
            tests/RegisterMappingTests.cpp)
    set(TEST_HEADER_FILES tests/mocks/LibModbusMocking.h
            tests/mocks/ModbusClientMocking.h
            tests/mocks/ModbusDeviceMocking.h
            tests/mocks/ModbusReaderMocking.h
            tests/mocks/RegisterGroupMocking.h
            tests/mocks/RegisterMappingMocking.h)

    add_executable(${PROJECT_NAME}Tests ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})
    target_link_libraries(${PROJECT_NAME}Tests ${PROJECT_NAME} gtest_main gtest gmock_main gmock)
    target_include_directories(${PROJECT_NAME}Tests PRIVATE ${PROJECT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME}Tests PUBLIC ${CMAKE_LIBRARY_INCLUDE_DIRECTORY})
    set_target_properties(${PROJECT_NAME}Tests PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")

    enable_testing()
    add_test(NAME "MoreModbus_Tests" COMMAND ${PROJECT_NAME}Tests)
endif ()

# Example
if (${BUILD_EXAMPLES})
    set(EXAMPLE_SOURCE_FILES example/Application.cpp)

    add_executable(${PROJECT_NAME}Example ${EXAMPLE_SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME}Example ${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME}Example PRIVATE ${PROJECT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME}Example PUBLIC ${CMAKE_LIBRARY_INCLUDE_DIRECTORY})
    set_target_properties(${PROJECT_NAME}Example PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
    set_target_properties(${PROJECT_NAME}Example PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif ()

# Add the format target
if (NOT TARGET format)
    add_custom_target(format
            COMMAND "clang-format" -i -sort-includes -style=file ${HEADER_FILES} ${SOURCE_FILES}
            ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES} ${EXAMPLE_SOURCE_FILES}
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
            COMMENT "[Formatting source code]"
            VERBATIM)
endif ()

# Create the install rule
include(GNUInstallDirs)
install(DIRECTORY ${CMAKE_LIBRARY_INCLUDE_DIRECTORY} DESTINATION ${CMAKE_INSTALL_PREFIX} PATTERN *.h)
install(DIRECTORY ${CMAKE_PREFIX_PATH}/include DESTINATION ${CMAKE_INSTALL_PREFIX} PATTERN *.h)
install(DIRECTORY ${CMAKE_PREFIX_PATH}/lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
