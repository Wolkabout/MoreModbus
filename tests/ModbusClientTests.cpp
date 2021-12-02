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
#include "more_modbus/modbus/LibModbusTcpIpClient.h"
#include "more_modbus/modbus/LibModbusSerialRtuClient.h"
#undef private
#undef protected

#include "mocks/LibModbusMocking.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

std::unique_ptr<LibModbusMock> libModbusMock;

class ModbusSerialRTUClientTest : public ::testing::Test
{
public:
    virtual void SetUp() { libModbusMock.reset(new ::testing::NiceMock<LibModbusMock>()); }

    virtual void TearDown() { libModbusMock.reset(); }
};

class ModbusTCPClientTest : public ::testing::Test
{
public:
    virtual void SetUp() { libModbusMock.reset(new ::testing::NiceMock<LibModbusMock>()); }

    virtual void TearDown() { libModbusMock.reset(); }
};

/*
 * All basic functions
 */
int modbus_set_response_timeout(modbus_t* ctx, uint32_t to_sec, uint32_t to_usec)
{
    return libModbusMock->modbus_set_response_timeout(ctx, to_sec, to_usec);
}

int modbus_connect(modbus_t* ctx)
{
    return libModbusMock->modbus_connect(ctx);
}

int modbus_flush(modbus_t* ctx)
{
    return libModbusMock->modbus_flush(ctx);
}

void modbus_close(modbus_t* ctx)
{
    return libModbusMock->modbus_close(ctx);
}

int modbus_set_slave(modbus_t* ctx, int slave)
{
    return libModbusMock->modbus_set_slave(ctx, slave);
}

/*
 * All read/write functions
 */
int modbus_read_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest)
{
    return libModbusMock->modbus_read_bits(ctx, addr, nb, dest);
}

int modbus_read_input_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest)
{
    return libModbusMock->modbus_read_input_bits(ctx, addr, nb, dest);
}

int modbus_read_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest)
{
    return libModbusMock->modbus_read_registers(ctx, addr, nb, dest);
}

int modbus_read_input_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest)
{
    return libModbusMock->modbus_read_input_registers(ctx, addr, nb, dest);
}

int modbus_write_bit(modbus_t* ctx, int coil_addr, int status)
{
    return libModbusMock->modbus_write_bit(ctx, coil_addr, status);
}

int modbus_write_register(modbus_t* ctx, int reg_addr, int value)
{
    return libModbusMock->modbus_write_register(ctx, reg_addr, value);
}

int modbus_write_bits(modbus_t* ctx, int addr, int nb, const uint8_t* data)
{
    return libModbusMock->modbus_write_bits(ctx, addr, nb, data);
}

int modbus_write_registers(modbus_t* ctx, int addr, int nb, const uint16_t* data)
{
    return libModbusMock->modbus_write_registers(ctx, addr, nb, data);
}

/*
 * All client specific functions
 */
modbus_t* modbus_new_tcp(const char* ip, int port)
{
    return libModbusMock->modbus_new_tcp(ip, port);
}

modbus_t* modbus_new_rtu(const char* port, int baud, char parity, int dataBits, int stopBits)
{
    return libModbusMock->modbus_new_rtu(port, baud, parity, dataBits, stopBits);
}

void modbus_free(modbus_t* ctx)
{
    return libModbusMock->modbus_free(ctx);
}

/*
 * Test simply meant to check if there's still a null check on the modbus_new_tcp call.
 */
TEST_F(ModbusTCPClientTest, NewTCPReturnsNull)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_tcp).Times(1).WillOnce(Return(nullptr));

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, FirstTimeoutReturnsBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, SecondTimeoutReturnsBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(1)).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, ModbusConnectFails)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, AlreadyConnected)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    modbusClient->m_connected = true;

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, HappyFlow)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    // next three called by destructor
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, SetSlaveBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1));

    ASSERT_FALSE(modbusClient->changeSlaveAddress(128));
}

TEST_F(ModbusTCPClientTest, SetSlaveGood)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(1));

    ASSERT_TRUE(modbusClient->changeSlaveAddress(128));
}

TEST_F(ModbusTCPClientTest, WriteHoldingRegister)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_register).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    for (uint32_t i = 0; i < 3; i++)
        EXPECT_EQ(booleans[i], modbusClient->writeHoldingRegister(static_cast<std::int32_t>(i + 1), 1, 0));
}

TEST_F(ModbusTCPClientTest, WriteHoldingRegisters)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_registers).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> data{0, 1, 2, 3};
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->writeHoldingRegisters(i + 1, 1, data));
    }
}

TEST_F(ModbusTCPClientTest, WriteCoil)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_bit).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> data{true, false, false};
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->writeCoil(i + 1, 1, data[i]));
    }
}

TEST_F(ModbusTCPClientTest, ReadInputContacts)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_input_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readInputContacts(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusTCPClientTest, ReadCoil)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    bool values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readCoil(i + 1, 0, values));
    }
}

TEST_F(ModbusTCPClientTest, ReadCoils)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readCoils(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusTCPClientTest, ReadInputRegisters)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_input_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readInputRegisters(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusTCPClientTest, ReadHoldingRegister)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    uint16_t value;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readHoldingRegister(i + 1, 0, value));
    }
}

TEST_F(ModbusTCPClientTest, ReadHoldingRegisters)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readHoldingRegisters(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusSerialRTUClientTest, NewRTUReturnsNull)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(nullptr));

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, FirstTimeoutReturnsBad)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, SecondTimeoutReturnsBad)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(1)).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, ModbusConnectFails)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(-1));
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, AlreadyConnected)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    modbusClient->m_connected = true;

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, HappyFlowBitNone)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    // next three called by destructor
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, HappyFlowBitEven)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::EVEN,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    // next three called by destructor
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, HappyFlowBitOdd)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::ODD,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_new_rtu).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*libModbusMock, modbus_set_response_timeout).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_connect).WillOnce(Return(1));
    // next three called by destructor
    EXPECT_CALL(*libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusSerialRTUClientTest, SetSlaveBad)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1));

    ASSERT_FALSE(modbusClient->changeSlaveAddress(128));
}

TEST_F(ModbusSerialRTUClientTest, SetSlaveGood)
{
    const auto& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(1));

    ASSERT_TRUE(modbusClient->changeSlaveAddress(128));
}

TEST_F(ModbusSerialRTUClientTest, WriteHoldingRegister)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_register).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->writeHoldingRegister(i + 1, 1, 0));
    }
}

TEST_F(ModbusSerialRTUClientTest, WriteHoldingRegisters)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> data{0, 1, 2, 3};
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->writeHoldingRegisters(i + 1, 1, data));
    }
}

TEST_F(ModbusSerialRTUClientTest, WriteCoil)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_write_bit).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> data{true, false, false};
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->writeCoil(i + 1, 1, data[i]));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadInputContacts)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_input_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readInputContacts(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadCoil)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    bool values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readCoil(i + 1, 0, values));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadCoils)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_bits).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<bool> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readCoils(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadInputRegisters)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_input_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readInputRegisters(i + 1, 0, 3, values));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadHoldingRegister)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    uint16_t value;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readHoldingRegister(i + 1, 0, value));
    }
}

TEST_F(ModbusSerialRTUClientTest, ReadHoldingRegisters)
{
    const std::unique_ptr<wolkabout::ModbusClient>& modbusClient =
      std::unique_ptr<wolkabout::LibModbusSerialRtuClient>(new wolkabout::LibModbusSerialRtuClient(
        "/dev/testSerial", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
        std::chrono::milliseconds(500)));
    EXPECT_CALL(*libModbusMock, modbus_set_slave).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*libModbusMock, modbus_read_registers).WillOnce(Return(-1)).WillOnce(Return(1));
    const auto booleans = std::vector<bool>{false, false, true};

    std::vector<uint16_t> values;
    for (uint32_t i = 0; i < 3; i++)
    {
        EXPECT_EQ(booleans[i], modbusClient->readHoldingRegisters(i + 1, 0, 3, values));
    }
}
