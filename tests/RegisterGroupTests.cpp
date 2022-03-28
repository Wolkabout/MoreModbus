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
#include "more_modbus/RegisterGroup.h"
#undef private
#undef protected

#include "mocks/ModbusDeviceMocking.h"
#include "mocks/RegisterMappingMocking.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

class RegisterGroupTests : public ::testing::Test
{
public:
    std::vector<std::shared_ptr<wolkabout::more_modbus::RegisterMapping>> mappings;
    std::vector<std::shared_ptr<wolkabout::more_modbus::RegisterMapping>> readRestrictedMappings;

    void SetUpMappings()
    {
        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("U16M", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 0));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("BM", wolkabout::more_modbus::RegisterType::INPUT_CONTACT, 0));

        mappings.emplace_back(std::make_shared<RegisterMappingMock>(
          "STR1", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, std::vector<std::int32_t>{0, 1, 2},
          wolkabout::more_modbus::OutputType::STRING, wolkabout::more_modbus::OperationType::STRINGIFY_ASCII_BIG_ENDIAN));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("U16M", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 3));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("B4-1", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 4,
                                                wolkabout::more_modbus::OperationType::TAKE_BIT, 0));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("B4-2", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 4,
                                                wolkabout::more_modbus::OperationType::TAKE_BIT, 1));

        mappings.emplace_back(
          std::make_shared<RegisterMappingMock>("U1610", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 10));

        readRestrictedMappings.emplace_back(
          std::make_shared<RegisterMappingMock>("BMRR1", wolkabout::more_modbus::RegisterType::COIL, 0, 0, true));

        readRestrictedMappings.emplace_back(
          std::make_shared<RegisterMappingMock>("BMRR2", wolkabout::more_modbus::RegisterType::COIL, 1, 0, true));

        readRestrictedMappings.emplace_back(std::make_shared<RegisterMappingMock>(
          "BMRR3", wolkabout::more_modbus::RegisterType::HOLDING_REGISTER, 1, 0, true));
    }

    void SetUp() { SetUpMappings(); }
};

TEST_F(RegisterGroupTests, InitialExampleTest)
{
    std::shared_ptr<wolkabout::more_modbus::RegisterGroup> soloGroup;
    EXPECT_NO_THROW(soloGroup = std::make_shared<wolkabout::more_modbus::RegisterGroup>(mappings[0], nullptr));

    std::shared_ptr<wolkabout::more_modbus::RegisterGroup> moreGroup;
    EXPECT_NO_THROW(moreGroup = std::make_shared<wolkabout::more_modbus::RegisterGroup>(mappings[2], nullptr));
    EXPECT_TRUE(moreGroup->addMapping(mappings[3]));
    EXPECT_TRUE(moreGroup->addMapping(mappings[4]));
    EXPECT_TRUE(moreGroup->addMapping(mappings[5]));
    // This one is of wrong type
    EXPECT_THROW(moreGroup->addMapping(mappings[1]), std::logic_error);
    // This one is already added
    EXPECT_FALSE(moreGroup->addMapping(mappings[4]));
    // This one is too far
    EXPECT_FALSE(moreGroup->addMapping(mappings[6]));
    EXPECT_THROW(moreGroup->addMapping(readRestrictedMappings[2]), std::logic_error);

    std::shared_ptr<wolkabout::more_modbus::RegisterGroup> willNotAddBeforeGroup;
    EXPECT_NO_THROW(moreGroup = std::make_shared<wolkabout::more_modbus::RegisterGroup>(mappings[6], nullptr));
    EXPECT_FALSE(moreGroup->addMapping(mappings[0]));

    const auto& copy = wolkabout::more_modbus::RegisterGroup(*moreGroup);
    EXPECT_EQ(copy.getAddressCount(), moreGroup->getAddressCount());
    EXPECT_EQ(copy.getSlaveAddress(), moreGroup->getSlaveAddress());
    EXPECT_EQ(copy.isReadRestricted(), moreGroup->isReadRestricted());

    std::shared_ptr<wolkabout::more_modbus::RegisterGroup> readOnlyGroup;
    EXPECT_NO_THROW(readOnlyGroup =
                      std::make_shared<wolkabout::more_modbus::RegisterGroup>(readRestrictedMappings[0], nullptr));
    EXPECT_TRUE(readOnlyGroup->addMapping(readRestrictedMappings[1]));
    EXPECT_THROW(readOnlyGroup->addMapping(mappings[4]), std::logic_error);

    EXPECT_NO_THROW(readOnlyGroup->setDevice(std::make_shared<ModbusDeviceMock>()));
}
