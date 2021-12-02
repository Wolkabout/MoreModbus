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
