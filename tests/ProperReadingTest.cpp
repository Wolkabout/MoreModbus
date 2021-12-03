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
#include "more_modbus/ModbusReader.h"
#undef private
#undef protected

#define _registerType wolkabout::RegisterMapping::RegisterType
#define _outputType wolkabout::RegisterMapping::OutputType
#define _operationType wolkabout::RegisterMapping::OperationType
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

#include "core/utilities/Logger.h"
#include "mocks/ModbusClientMocking.h"
#include "mocks/RegisterMappingMocking.h"
#include "more_modbus/mappings/BoolMapping.h"
#include "more_modbus/mappings/FloatMapping.h"
#include "more_modbus/mappings/Int16Mapping.h"
#include "more_modbus/mappings/Int32Mapping.h"
#include "more_modbus/mappings/StringMapping.h"
#include "more_modbus/mappings/UInt16Mapping.h"
#include "more_modbus/mappings/UInt32Mapping.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

class ProperReadingTest : public ::testing::Test
{
public:
    std::shared_ptr<ModbusClientMock> modbusClientMock;

    std::shared_ptr<wolkabout::BoolMapping> bitMapping;
    std::shared_ptr<wolkabout::BoolMapping> coilMapping;
    std::shared_ptr<wolkabout::UInt16Mapping> uint16Mapping;
    std::shared_ptr<wolkabout::StringMapping> stringMapping;
    std::shared_ptr<wolkabout::FloatMapping> nonWriteableFloatMapping;

    std::vector<std::shared_ptr<wolkabout::RegisterMapping>> mappings;
    std::shared_ptr<wolkabout::ModbusDevice> device;

    void SetUpMappings()
    {
        uint16Mapping = std::make_shared<wolkabout::UInt16Mapping>("HR1", _registerType::HOLDING_REGISTER, 0);
        mappings.emplace_back(uint16Mapping);

        mappings.emplace_back(std::make_shared<wolkabout::UInt16Mapping>("HR2", _registerType::HOLDING_REGISTER, 1));
        mappings.emplace_back(std::make_shared<wolkabout::Int16Mapping>("HR3", _registerType::HOLDING_REGISTER, 2));
        mappings.emplace_back(std::make_shared<wolkabout::Int16Mapping>("HR4", _registerType::HOLDING_REGISTER, 3));

        mappings.emplace_back(std::make_shared<wolkabout::UInt32Mapping>(
          "IR1", _registerType::INPUT_REGISTER, std::vector<std::int32_t>{0, 1}, _operationType::MERGE_LITTLE_ENDIAN));
        mappings.emplace_back(std::make_shared<wolkabout::Int32Mapping>(
          "IR2", _registerType::INPUT_REGISTER, std::vector<std::int32_t>{2, 3}, _operationType::MERGE_BIG_ENDIAN));

        nonWriteableFloatMapping = std::make_shared<wolkabout::FloatMapping>("IR3", _registerType::INPUT_REGISTER,
                                                                             std::vector<std::int32_t>{4, 5});
        mappings.emplace_back(nonWriteableFloatMapping);

        bitMapping = std::make_shared<wolkabout::BoolMapping>("HRB1", _registerType::HOLDING_REGISTER, 4,
                                                              _operationType::TAKE_BIT, 0);
        mappings.emplace_back(bitMapping);
        mappings.emplace_back(std::make_shared<wolkabout::BoolMapping>("HRB2", _registerType::HOLDING_REGISTER, 4,
                                                                       _operationType::TAKE_BIT, 2));
        mappings.emplace_back(std::make_shared<wolkabout::BoolMapping>("HRB3", _registerType::HOLDING_REGISTER, 4,
                                                                       _operationType::TAKE_BIT, 4));
        mappings.emplace_back(std::make_shared<wolkabout::BoolMapping>("HRB4", _registerType::HOLDING_REGISTER, 4,
                                                                       _operationType::TAKE_BIT, 6));

        stringMapping = std::make_shared<wolkabout::StringMapping>("HRSTR", _registerType::HOLDING_REGISTER,
                                                                   std::vector<std::int32_t>{5, 6, 7, 8, 9},
                                                                   _operationType::STRINGIFY_ASCII);
        mappings.emplace_back(stringMapping);

        coilMapping = std::make_shared<wolkabout::BoolMapping>("HRCOIL", _registerType::COIL, 0);
        mappings.emplace_back(coilMapping);

        mappings.emplace_back(std::make_shared<wolkabout::BoolMapping>("HRIC", _registerType::INPUT_CONTACT, 0));
    }

    void SetUpDevices()
    {
        device = std::make_shared<wolkabout::ModbusDevice>("TEST DEVICE", 0);
        device->createGroups(mappings);
    }

    void SetUp()
    {
        wolkabout::Logger::init(wolkabout::LogLevel::TRACE, wolkabout::Logger::Type::CONSOLE);

        modbusClientMock = std::make_shared<ModbusClientMock>();
        SetUpMappings();
        SetUpDevices();
    }
};

TEST_F(ProperReadingTest, InitializeReader)
{
    const auto& reader = std::make_shared<wolkabout::ModbusReader>(*modbusClientMock, std::chrono::milliseconds(500));
    EXPECT_NO_THROW(reader->addDevice(device));

    const auto boolData = std::vector<bool>(10, false);
    uint16_t uint16Single = 0;
    const auto uint16Data = std::vector<uint16_t>(10, 0);

    EXPECT_CALL(*modbusClientMock, isConnected).WillOnce(Return(false)).WillRepeatedly(Return(true));
    EXPECT_CALL(*modbusClientMock, connect).WillRepeatedly(Return(true));
    EXPECT_CALL(*modbusClientMock, readCoils).WillRepeatedly(DoAll(SetArgReferee<3>(boolData), Return(true)));
    EXPECT_CALL(*modbusClientMock, readInputContacts).WillRepeatedly(DoAll(SetArgReferee<3>(boolData), Return(true)));
    EXPECT_CALL(*modbusClientMock, readHoldingRegisters)
      .WillRepeatedly(DoAll(SetArgReferee<3>(uint16Data), Return(true)));
    EXPECT_CALL(*modbusClientMock, readHoldingRegister)
      .WillRepeatedly(DoAll(SetArgReferee<2>(uint16Single), Return(true)));
    EXPECT_CALL(*modbusClientMock, readInputRegisters)
      .WillRepeatedly(DoAll(SetArgReferee<3>(uint16Data), Return(true)));

    EXPECT_CALL(*modbusClientMock, writeCoil).WillRepeatedly(Return(true));
    EXPECT_CALL(*modbusClientMock, writeHoldingRegisters).WillRepeatedly(Return(true));

    EXPECT_NO_THROW(reader->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_NO_THROW(bitMapping->writeValue(true));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_NO_THROW(stringMapping->writeValue("Heyo!!!!!!"));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_NO_THROW(reader->m_shouldReconnect = true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_NO_THROW(coilMapping->writeValue(true));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_CALL(*modbusClientMock, isConnected).WillOnce(Return(false)).WillRepeatedly(Return(true));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_ANY_THROW(nonWriteableFloatMapping->writeValue(100.12f));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXPECT_NO_THROW(reader->stop());
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
