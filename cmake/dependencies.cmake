include(cpp-tools/cmake/dependencies/libmodbus.cmake)

if (${BUILD_GTEST})
    include(cpp-tools/cmake/dependencies/gtest.cmake)
endif ()
