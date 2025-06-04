include(WolkSDK-Cpp/cpp-tools/cmake/dependencies/libmodbus.cmake)

if (MOREMODBUS_BUILD_TESTS)
    include(WolkSDK-Cpp/cpp-tools/cmake/dependencies/gtest.cmake)
endif ()
