include(ExternalProject)

set(LIB_MODBUS_VERSION "3.1.6")

string(APPEND LIBMODBUS_ADDITIONAL_ARGS --host=${LIBMODBUS_CROSSCOMPILE_HOST})

ExternalProject_Add(libmodbus
        GIT_REPOSITORY "https://github.com/stephane/libmodbus"
        GIT_TAG "v${LIB_MODBUS_VERSION}"
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND cd <SOURCE_DIR> && ./autogen.sh
        COMMAND <SOURCE_DIR>/configure
        --prefix=${CMAKE_PREFIX_PATH}
        --disable-tests
        ${LIBMODBUS_ADDITIONAL_ARGS}
        INSTALL_DIR ${CMAKE_PREFIX_PATH}
        )

# And add WolkSDK-Cpp if there is no WolkSDK-Cpp
if (NOT TARGET WolkAboutCoreUtility)
    ExternalProject_Add(WolkSDK-Cpp
            GIT_REPOSITORY "https://github.com/Wolkabout/WolkSDK-Cpp"
            GIT_TAG "cc3f9611cb02355815a9944c45f05fe8b4c3e4d8"
            UPDATE_COMMAND ""
            CMAKE_ARGS
            -DBUILD_CONNECTIVITY=OFF
            -DBUILD_POCO=OFF
            -DBUILD_AWS_LOG_UPLOADER=OFF
            -DBUILD_GTEST=${BUILD_GTEST}
            -DBUILD_TESTS=${BUILD_TESTS}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_PREFIX_PATH}
            INSTALL_DIR ${CMAKE_PREFIX_PATH})
endif ()
