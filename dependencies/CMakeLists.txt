include(ExternalProject)

set(LIB_MODBUS_VERSION "3.1.6")

string(APPEND LIBMODBUS_ADDITIONAL_ARGS --host=${LIBMODBUS_CROSSCOMPILE_HOST})

if (NOT TARGET libmodbus)
    ExternalProject_Add(libmodbus
            URL ${CMAKE_CURRENT_LIST_DIR}/libmodbus-3.1.6.tar.gz
            UPDATE_COMMAND ""
            CONFIGURE_COMMAND cd <SOURCE_DIR> && /bin/sh ./autogen.sh
            COMMAND /bin/sh <SOURCE_DIR>/configure
            --prefix=${CMAKE_PREFIX_PATH}
            --disable-tests
            ${LIBMODBUS_ADDITIONAL_ARGS}
            INSTALL_DIR ${CMAKE_PREFIX_PATH}
            )
endif ()
