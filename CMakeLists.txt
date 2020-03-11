cmake_minimum_required(VERSION 3.5)
project(wolkabout_modbus C CXX)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE HEADER_FILES src/*.h src/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.cpp)

link_directories(${PROJECT_SOURCE_DIR}/out/lib)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} modbus)

add_subdirectory("cmake")