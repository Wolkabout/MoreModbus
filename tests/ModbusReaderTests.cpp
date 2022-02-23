/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
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
#include "more_modbus/ModbusReader.h"
#undef private
#undef protected

#include "mocks/ModbusClientMocking.h"
#include "mocks/ModbusDeviceMocking.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

class ModbusReaderTests : public ::testing::Test
{
public:
    void SetUp()
    {
        modbusDeviceMock.reset(new ::testing::NiceMock<ModbusDeviceMock>());
        modbusClientMock.reset(new ::testing::NiceMock<ModbusClientMock>());
    }

    void TearDown()
    {
        modbusDeviceMock.reset();
        modbusClientMock.reset();
    }

    std::unique_ptr<ModbusDeviceMock> modbusDeviceMock;
    std::unique_ptr<ModbusClientMock> modbusClientMock;
};

TEST_F(ModbusReaderTests, ExampleTest)
{
    const auto& reader =
      std::make_shared<wolkabout::more_modbus::ModbusReader>(*modbusClientMock, std::chrono::milliseconds(500));

    EXPECT_CALL(*modbusClientMock, connect).WillRepeatedly(Return(true));
    EXPECT_CALL(*modbusClientMock, isConnected).WillRepeatedly(Return(true));

    EXPECT_NO_THROW(reader->addDevice((const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>&)modbusDeviceMock));
    reader->m_devices.clear();

    EXPECT_NO_THROW(reader->addDevices(std::vector<std::shared_ptr<wolkabout::more_modbus::ModbusDevice>>{
      (const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>&)modbusDeviceMock}));

    EXPECT_EQ(reader->getDevices().size(), 1);
    for (const auto& kvp : reader->getDeviceStatuses())
    {
        EXPECT_FALSE(kvp.second);
    }
    EXPECT_FALSE(reader->isRunning());

    EXPECT_NO_THROW(reader->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_NO_THROW(reader->stop());
}
