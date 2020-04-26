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
    virtual void SetUp()
    {
        _libModbusMock.reset(new ::testing::NiceMock<LibModbusMock>());

        modbusClient = std::unique_ptr<wolkabout::LibModbusTcpIpClient>(
          new wolkabout::LibModbusTcpIpClient("IP", 552, std::chrono::milliseconds(500)));
    }

    virtual void TearDown()
    {
        _libModbusMock.reset();
    }

    std::unique_ptr<wolkabout::LibModbusTcpIpClient> modbusClient;

    static std::unique_ptr<LibModbusMock> _libModbusMock;
};

std::unique_ptr<LibModbusMock> ModbusTCPClientTest::_libModbusMock;

int modbus_flush(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_flush(ctx);
}

void modbus_close(modbus_t* ctx)
{
    return ModbusTCPClientTest::_libModbusMock->modbus_close(ctx);
}

TEST_F(ModbusTCPClientTest, FirstTest)
{
    EXPECT_CALL(*_libModbusMock, modbus_flush).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_libModbusMock, modbus_close).Times(1).WillOnce(Return());
    modbusClient->disconnect();
}
