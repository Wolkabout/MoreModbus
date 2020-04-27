/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
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

#define private public
#define protected public
#include "modbus/LibModbusTcpIpClient.h"
#undef private
#undef protected

#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/LibModbusMocking.h"
#include "modbus/libmodbus/modbus.h"

class ModbusTCPClientTest : public ::testing::Test
{
public:
    virtual void SetUp() { _libModbusMock.reset(new ::testing::NiceMock<LibModbusMock>()); }

    virtual void TearDown() { _libModbusMock.reset(); }

    static std::unique_ptr<LibModbusMock> _libModbusMock;
};

std::unique_ptr<LibModbusMock> ModbusTCPClientTest::_libModbusMock;

/*
 * All basic functions
 */
int modbus_set_response_timeout(modbus_t* ctx, uint32_t to_sec, uint32_t to_usec)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_set_response_timeout(ctx, to_sec, to_usec);
}

int modbus_connect(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_connect(ctx);
}

int modbus_flush(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_flush(ctx);
}

void modbus_close(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_close(ctx);
}

int modbus_set_slave(modbus_t* ctx, int slave)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_set_slave(ctx, slave);
}

/*
 * All read/write functions
 */
int modbus_read_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_read_bits(ctx, addr, nb, dest);
}

int modbus_read_input_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_read_input_bits(ctx, addr, nb, dest);
}

int modbus_read_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_read_registers(ctx, addr, nb, dest);
}

int modbus_read_input_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_read_input_registers(ctx, addr, nb, dest);
}

int modbus_write_bit(modbus_t* ctx, int coil_addr, int status)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_write_bit(ctx, coil_addr, status);
}

int modbus_write_register(modbus_t* ctx, int reg_addr, int value)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_write_register(ctx, reg_addr, value);
}

int modbus_write_bits(modbus_t* ctx, int addr, int nb, const uint8_t* data)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_write_bits(ctx, addr, nb, data);
}

int modbus_write_registers(modbus_t* ctx, int addr, int nb, const uint16_t* data)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_write_registers(ctx, addr, nb, data);
}

/*
 * All client specific functions
 */
modbus_t* modbus_new_tcp(const char* ip, int port)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_new_tcp(ip, port);
}

modbus_t* modbus_new_rtu(const char* device, int baud, char parity, int data_bit, int stop_bit)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_new_rtu(device, baud, parity, data_bit, stop_bit);
}

void modbus_free(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_free(ctx);
}

/*
 * Test simply meant to check if there's still a null check on the modbus_new_tcp call.
 */
TEST_F(ModbusTCPClientTest, TestConnectionFailure)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));

    EXPECT_CALL(*_libModbusMock, modbus_new_tcp).Times(1).WillOnce(Return(nullptr));
    EXPECT_FALSE(modbusClient->connect());
}
