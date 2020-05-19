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
#include "ModbusDevice.h"
#undef private
#undef protected

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "mocks/ModbusClientMocking.h"
#include "mocks/ModbusReaderMocking.h"
#include "mocks/RegisterMappingMocking.h"

class ModbusDeviceTests : public ::testing::Test
{
public:
    std::vector<std::shared_ptr<wolkabout::RegisterMapping>> mappings;

    void SetUpMappings()
    {
        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("U16M", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("BM", wolkabout::RegisterMapping::RegisterType::INPUT_CONTACT, 0));

        mappings.emplace_back(std::make_shared<RegisterMappingMock>(
		  "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<std::int32_t>{0, 1, 2},
          wolkabout::RegisterMapping::OutputType::STRING, wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                                wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("B4-2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                                wolkabout::RegisterMapping::OperationType::TAKE_BIT, 1));

        mappings.emplace_back(std::make_shared<RegisterMappingMock>(
          "BMRR1", wolkabout::RegisterMapping::RegisterType::COIL, 0, 0, true));

        mappings.emplace_back(std::make_shared<RegisterMappingMock>(
          "BMRR2", wolkabout::RegisterMapping::RegisterType::COIL, 1, 0, true));
    }

    void SetUp() { SetUpMappings(); }
};

TEST_F(ModbusDeviceTests, InitialExampleTest)
{
    const auto& name = "TEST";
    const auto& slaveAddress = 0;

    const auto& device = std::make_shared<wolkabout::ModbusDevice>(name, slaveAddress);
    ASSERT_NO_THROW(device->createGroups(mappings));
    ASSERT_NO_THROW(std::make_shared<wolkabout::ModbusDevice>(*device));
    EXPECT_EQ(device->m_groups.size(), 5);

    EXPECT_EQ(name, device->getName());
    EXPECT_FALSE(device->getStatus());
    EXPECT_EQ(slaveAddress, device->getSlaveAddress());

    const auto& clientMock = new ::testing::NiceMock<ModbusClientMock>();
    const auto& readerMock = std::make_shared<ModbusReaderMock>(*clientMock);

    EXPECT_NO_THROW(device->setReader(readerMock));

    bool valueChangeSuccess = false;
    bool statusChangeSuccess = false;

    const auto& valueChange = [&](const std::shared_ptr<wolkabout::RegisterMapping>& mapping) {
        if (mapping == nullptr)
        {
            valueChangeSuccess = true;
        }
    };

    const auto& statusChange = [&](bool status) {
        if (status == true)
        {
            statusChangeSuccess = true;
        }
    };

    ASSERT_NO_THROW(device->setOnMappingValueChange(valueChange));
    ASSERT_NO_THROW(device->setOnStatusChange(statusChange));

    ASSERT_NO_THROW(device->triggerOnMappingValueChange(nullptr));
    ASSERT_NO_THROW(device->triggerOnStatusChange(true));

    EXPECT_TRUE(valueChangeSuccess);
    EXPECT_TRUE(statusChangeSuccess);
}
