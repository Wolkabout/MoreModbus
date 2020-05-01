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
#include "mappings/BoolMapping.h"
#include "mappings/FloatMapping.h"
#include "mappings/Int16Mapping.h"
#include "mappings/Int32Mapping.h"
#include "mappings/StringMapping.h"
#include "mappings/UInt16Mapping.h"
#include "mappings/UInt32Mapping.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "modbus/LibModbusSerialRtuClient.h"
#undef private
#undef protected

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#define _registerType wolkabout::RegisterMapping::RegisterType
#define _outputType wolkabout::RegisterMapping::OutputType
#define _operationType wolkabout::RegisterMapping::OperationType
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

#include "mocks/ModbusClientMock.h"
#include "mocks/ModbusDeviceMocking.h"
#include "mocks/ModbusReaderMocking.h"
#include "mocks/RegisterGroupMocking.h"

class MappingsTests : public ::testing::Test
{
public:
    std::vector<_registerType> registerTypes;
    std::vector<_outputType> outputTypes;
    std::vector<_operationType> operationTypes;

    std::map<_outputType, std::vector<_combination>> winningCombos;

    virtual void SetUp()
    {
        modbusClientMock.reset(new ::testing::NiceMock<ModbusClientMock>());
        registerGroupMock.reset(new ::testing::NiceMock<RegisterGroupMock>());
        modbusDeviceMock.reset(new ::testing::NiceMock<ModbusDeviceMock>());
        modbusReaderMock.reset(new ::testing::NiceMock<ModbusReaderMock>(*modbusClientMock));

        SetUpDefaultValues();
        SetUpWinningCombinations();
    }

    virtual void TearDown()
    {
        modbusDeviceMock.reset();
        modbusReaderMock.reset();
        registerGroupMock.reset();
        modbusClientMock.reset();

        ::testing::FLAGS_gmock_verbose = "error";
    }

    void MovePointers()
    {
        modbusDeviceMock->m_reader = std::move(modbusReaderMock);
        registerGroupMock->m_device = std::move(modbusDeviceMock);
    }

    void MoveBackPointers()
    {
        registerGroupMock.reset(new ::testing::NiceMock<RegisterGroupMock>());
        modbusDeviceMock.reset(new ::testing::NiceMock<ModbusDeviceMock>());
        modbusReaderMock.reset(new ::testing::NiceMock<ModbusReaderMock>(*modbusClientMock));
    }

    void SetUpDefaultValues()
    {
        registerTypes = {_registerType::INPUT_CONTACT, _registerType::COIL, _registerType::INPUT_CONTACT,
                         _registerType::HOLDING_REGISTER};
        outputTypes = {_outputType::BOOL,   _outputType::FLOAT,  _outputType::INT16, _outputType::INT32,
                       _outputType::STRING, _outputType::UINT16, _outputType::UINT32};
        operationTypes = {
          _operationType::NONE,        _operationType::MERGE_BIG_ENDIAN, _operationType::MERGE_LITTLE_ENDIAN,
          _operationType::MERGE_FLOAT, _operationType::STRINGIFY_ASCII,  _operationType::STRINGIFY_UNICODE,
          _operationType::TAKE_BIT};
    }

    void SetUpWinningCombinations()
    {
        winningCombos.emplace(_outputType::BOOL, std::vector<_combination>{
                                                   _makeCombo(INPUT_CONTACT, BOOL, NONE), _makeCombo(COIL, BOOL, NONE),
                                                   _makeCombo(INPUT_REGISTER, BOOL, TAKE_BIT),
                                                   _makeCombo(HOLDING_REGISTER, BOOL, TAKE_BIT)});
    }

    bool IsWinningCombo(_outputType outputType, _combination combo)
    {
        for (const auto& winningCombo : winningCombos[outputType])
        {
            if (combo == winningCombo)
            {
                return true;
            }
        }
        return false;
    }

    static std::unique_ptr<ModbusClientMock> modbusClientMock;
    static std::unique_ptr<ModbusDeviceMock> modbusDeviceMock;
    static std::unique_ptr<ModbusReaderMock> modbusReaderMock;
    static std::unique_ptr<RegisterGroupMock> registerGroupMock;
};

std::unique_ptr<ModbusClientMock> MappingsTests::modbusClientMock;
std::unique_ptr<ModbusDeviceMock> MappingsTests::modbusDeviceMock;
std::unique_ptr<ModbusReaderMock> MappingsTests::modbusReaderMock;
std::unique_ptr<RegisterGroupMock> MappingsTests::registerGroupMock;

TEST_F(MappingsTests, BoolMappingCreation)
{
    const auto& outputType = _outputType::BOOL;
    for (const auto& registerType : registerTypes)
    {
        for (const auto& operationType : operationTypes)
        {
            const auto& combo = _makeComboPure(registerType, outputType, operationType);
            const bool winning = IsWinningCombo(outputType, combo);
            if (operationType == _operationType::NONE)
            {
                if (winning)
                {
                    EXPECT_NO_THROW(wolkabout::BoolMapping("TEST", registerType, 0));
                }
                else
                {
                    EXPECT_THROW(wolkabout::BoolMapping("TEST", registerType, 0), std::logic_error);
                }
            }
            else
            {
                if (winning)
                {
                    EXPECT_NO_THROW(wolkabout::BoolMapping("TEST", registerType, 0, operationType, 0));
                }
                else
                {
                    EXPECT_THROW(wolkabout::BoolMapping("TEST", registerType, 0, operationType, 0), std::logic_error);
                }
            }
        }
    }
}

TEST_F(MappingsTests, BoolMappingsWriteValue)
{
    const auto& outputType = _outputType::BOOL;
    const auto& boolCombos = winningCombos[outputType];
    for (const auto& combo : boolCombos)
    {
        bool value = rand() % 2;
        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        const auto operationType = std::get<2>(combo);
        if (operationType == _operationType::NONE)
        {
            auto mapping = std::make_shared<wolkabout::BoolMapping>("TEST", registerType, 0, false, 0);
            mapping->m_boolValue = !value;
            MovePointers();
            mapping->m_group = std::move(registerGroupMock);
            if (registerType == _registerType::COIL)
            {
                EXPECT_CALL((ModbusReaderMock&)*(mapping->m_group->m_device->m_reader), writeMapping(_, value))
                  .WillOnce(Return(true));
                EXPECT_TRUE(mapping->writeValue(value));

                EXPECT_EQ(value, mapping->getBoolValue());
            }
            else
            {
                EXPECT_FALSE(mapping->writeValue(value));
            }

            MoveBackPointers();
            modbusReaderMock->m_devices.clear();
        }
        else
        {
            auto mapping = std::make_shared<wolkabout::BoolMapping>("TEST", registerType, 0, operationType, 0);
            mapping->m_boolValue = !value;
            MovePointers();
            mapping->m_group = std::move(registerGroupMock);
            if (registerType == _registerType::HOLDING_REGISTER)
            {
                EXPECT_CALL((ModbusReaderMock&)*(mapping->m_group->m_device->m_reader), writeBitMapping)
                  .WillOnce(Return(true));
                EXPECT_TRUE(mapping->writeValue(value));

                EXPECT_EQ(value, mapping->getBoolValue());
            }
            else
            {
                EXPECT_FALSE(mapping->writeValue(true));
            }

            MoveBackPointers();
        }
    }
}
