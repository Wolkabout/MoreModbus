include(ExternalProject)

set(LIB_MODBUS_VERSION "3.1.6")
set(WOLK_SDK_VERSION "162dbbc37e906412df48b588ce6ba1039f94eaef")

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
