/**
 * Copyright 2023 Wolkabout Technology s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MOREMODBUS_LIBMODBUSMOCKING_H
#define MOREMODBUS_LIBMODBUSMOCKING_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <modbus/modbus-rtu.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus.h>

using namespace ::testing;

class LibModbusMock
{
public:
    /*
     * General necessary modbus functions
     */
    MOCK_METHOD3(modbus_set_response_timeout, int(modbus_t*, uint32_t, uint32_t));
    MOCK_METHOD1(modbus_connect, int(modbus_t*));
    MOCK_METHOD1(modbus_flush, int(modbus_t*));
    MOCK_METHOD1(modbus_close, void(modbus_t*));
    MOCK_METHOD2(modbus_set_slave, int(modbus_t*, int));

    /*
     * All the read/write functions
     */
    MOCK_METHOD4(modbus_read_bits, int(modbus_t*, int, int, uint8_t*));
    MOCK_METHOD4(modbus_read_input_bits, int(modbus_t*, int, int, uint8_t*));
    MOCK_METHOD4(modbus_read_registers, int(modbus_t*, int, int, uint16_t*));
    MOCK_METHOD4(modbus_read_input_registers, int(modbus_t*, int, int, uint16_t*));
    MOCK_METHOD3(modbus_write_bit, int(modbus_t*, int, int));
    MOCK_METHOD3(modbus_write_register, int(modbus_t*, int, const uint16_t));
    MOCK_METHOD4(modbus_write_bits, int(modbus_t*, int, int, const uint8_t*));
    MOCK_METHOD4(modbus_write_registers, int(modbus_t*, int, int, const uint16_t*));

    /*
     * Ones necessary for specific ModbusClient implementations
     */
    MOCK_METHOD2(modbus_new_tcp, modbus_t*(const char*, int));
    MOCK_METHOD5(modbus_new_rtu, modbus_t*(const char*, int, char, int, int));
    MOCK_METHOD1(modbus_free, void(modbus_t*));
};

#endif    // MOREMODBUS_LIBMODBUSMOCKING_H
