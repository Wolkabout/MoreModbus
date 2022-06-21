include(ExternalProject)

set(LIB_MODBUS_VERSION "3.1.6")
set(WOLK_SDK_VERSION "d5944a94e5b36452a7bef645b7a114f9ab2e0916")

string(APPEND LIBMODBUS_ADDITIONAL_ARGS --host=${LIBMODBUS_CROSSCOMPILE_HOST})

ExternalProject_Add(libmodbus
        GIT_REPOSITORY "https://github.com/stephane/libmodbus"
        GIT_TAG "v${LIB_MODBUS_VERSION}"
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND cd <SOURCE_DIR> && /bin/sh ./autogen.sh
        COMMAND /bin/sh <SOURCE_DIR>/configure
        --prefix=${CMAKE_PREFIX_PATH}
        --disable-tests
        ${LIBMODBUS_ADDITIONAL_ARGS}
        INSTALL_DIR ${CMAKE_PREFIX_PATH}
        )

# And add WolkSDK-Cpp if there is no WolkSDK-Cpp
if (NOT TARGET WolkAboutCoreUtility)
    ExternalProject_Add(WolkSDK-Cpp
            GIT_REPOSITORY "https://github.com/Wolkabout/WolkSDK-Cpp"
            GIT_TAG "${WOLK_SDK_VERSION}"
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
