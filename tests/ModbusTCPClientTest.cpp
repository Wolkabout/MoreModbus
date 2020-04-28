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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "mocks/LibModbusMocking.h"
#include "modbus/libmodbus/modbus.h"

namespace
{
class ModbusTCPClientTest : public ::testing::Test
{
};

/*
 * Test simply meant to check if there's still a null check on the modbus_new_tcp call.
 */
TEST_F(ModbusTCPClientTest, NewTCPReturnsNull)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_new_tcp).Times(1).WillOnce(Return(nullptr));

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, FirstTimeoutReturnsBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_response_timeout).WillOnce(Return(-1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, SecondTimeoutReturnsBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_response_timeout).WillOnce(Return(1)).WillOnce(Return(-1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_connect).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_FALSE(modbusClient->connect());
    ASSERT_FALSE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, ModbusConnectFails)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_response_timeout).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_connect).WillOnce(Return(-1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_close).WillOnce(Return());

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
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_new_tcp).WillOnce(Return(reinterpret_cast<modbus_t*>(0x1)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_response_timeout).WillRepeatedly(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_connect).WillOnce(Return(1));
    // next three called by destructor
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_free).WillOnce(Return());
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_flush).WillOnce(Return(1));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_close).WillOnce(Return());

    ASSERT_TRUE(modbusClient->connect());
    ASSERT_TRUE(modbusClient->isConnected());
}

TEST_F(ModbusTCPClientTest, SetSlaveBad)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_slave).WillOnce(Return(-1));

    ASSERT_FALSE(modbusClient->changeSlaveAddress(128));
}

TEST_F(ModbusTCPClientTest, SetSlaveGood)
{
    const auto& modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
      new wolkabout::LibModbusTcpIpClient("TEST IP ADDRESS", 551, std::chrono::milliseconds(500)));
    EXPECT_CALL(*LibModbusMock::_libModbusMock, modbus_set_slave).WillOnce(Return(1));

    ASSERT_TRUE(modbusClient->changeSlaveAddress(128));
}
}    // namespace
