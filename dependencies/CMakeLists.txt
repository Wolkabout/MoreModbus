include(ExternalProject)

set(LIB_MODBUS_VERSION "3.1.6")
set(GTEST_VERSION "1.10.0")

string(APPEND LIBMODBUS_ADDITIONAL_ARGS --host=${LIBMODBUS_CROSSCOMPILE_HOST})

ExternalProject_Add(libmodbus
        GIT_REPOSITORY "https://github.com/stephane/libmodbus"
        GIT_TAG "v${LIB_MODBUS_VERSION}"
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND cd <SOURCE_DIR> && ./autogen.sh
        COMMAND <SOURCE_DIR>/configure
        --prefix=${CMAKE_PREFIX_PATH}
        --libdir=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        --disable-tests
        ${LIBMODBUS_ADDITIONAL_ARGS}
        INSTALL_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )

ExternalProject_Add(libgtest
        GIT_REPOSITORY "https://github.com/google/googletest"
        GIT_TAG "release-${GTEST_VERSION}"
        CMAKE_ARGS
        -DCMAKE_INSTALL_FULL_LIBDIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        -DCMAKE_INSTALL_LIBDIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_PREFIX_PATH}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DBUILD_SHARED_LIBS=ON
        INSTALL_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )
