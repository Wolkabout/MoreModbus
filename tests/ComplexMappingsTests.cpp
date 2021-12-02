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
#include "more_modbus/mappings/FloatMapping.h"
#include "more_modbus/mappings/Int32Mapping.h"
#include "more_modbus/mappings/StringMapping.h"
#include "more_modbus/mappings/UInt32Mapping.h"
#include "more_modbus/modbus/LibModbusTcpIpClient.h"
#include "more_modbus/modbus/LibModbusSerialRtuClient.h"
#undef private
#undef protected

#define _registerType wolkabout::RegisterMapping::RegisterType
#define _outputType wolkabout::RegisterMapping::OutputType
#define _operationType wolkabout::RegisterMapping::OperationType
#define _endian wolkabout::DataParsers::Endian
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

#include "mocks/ModbusClientMocking.h"
#include "mocks/ModbusDeviceMocking.h"
#include "mocks/ModbusReaderMocking.h"
#include "mocks/RegisterGroupMocking.h"
#include "more_modbus/utilities/DataParsers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

class ComplexMappingsTests : public ::testing::Test
{
public:
    std::vector<_registerType> registerTypes;
    std::vector<_outputType> outputTypes;
    std::vector<_operationType> operationTypes;
    std::vector<_endian> endians;
    std::map<_operationType, _endian> endianForOperation;

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
        registerTypes = {_registerType::INPUT_CONTACT, _registerType::COIL, _registerType::INPUT_REGISTER,
                         _registerType::HOLDING_REGISTER};
        outputTypes = {_outputType::BOOL,   _outputType::FLOAT,  _outputType::INT16, _outputType::INT32,
                       _outputType::STRING, _outputType::UINT16, _outputType::UINT32};
        operationTypes = {
          _operationType::NONE,        _operationType::MERGE_BIG_ENDIAN, _operationType::MERGE_LITTLE_ENDIAN,
          _operationType::MERGE_FLOAT, _operationType::STRINGIFY_ASCII,  _operationType::STRINGIFY_UNICODE,
          _operationType::TAKE_BIT};

        endians = {_endian::BIG, _endian::LITTLE};

        endianForOperation = {std::pair<_operationType, _endian>(_operationType::MERGE_BIG_ENDIAN, _endian::BIG),
                              std::pair<_operationType, _endian>(_operationType::MERGE_LITTLE_ENDIAN, _endian::LITTLE)};
    }

    void SetUpWinningCombinations()
    {
        winningCombos.emplace(_outputType::UINT32,
                              std::vector<_combination>{_makeCombo(HOLDING_REGISTER, UINT32, MERGE_BIG_ENDIAN),
                                                        _makeCombo(HOLDING_REGISTER, UINT32, MERGE_LITTLE_ENDIAN),
                                                        _makeCombo(INPUT_REGISTER, UINT32, MERGE_BIG_ENDIAN),
                                                        _makeCombo(INPUT_REGISTER, UINT32, MERGE_LITTLE_ENDIAN)});

        winningCombos.emplace(_outputType::INT32,
                              std::vector<_combination>{_makeCombo(HOLDING_REGISTER, INT32, MERGE_BIG_ENDIAN),
                                                        _makeCombo(HOLDING_REGISTER, INT32, MERGE_LITTLE_ENDIAN),
                                                        _makeCombo(INPUT_REGISTER, INT32, MERGE_BIG_ENDIAN),
                                                        _makeCombo(INPUT_REGISTER, INT32, MERGE_LITTLE_ENDIAN)});

        winningCombos.emplace(_outputType::FLOAT,
                              std::vector<_combination>{_makeCombo(HOLDING_REGISTER, FLOAT, MERGE_FLOAT),
                                                        _makeCombo(INPUT_REGISTER, FLOAT, MERGE_FLOAT)});

        winningCombos.emplace(_outputType::STRING,
                              std::vector<_combination>{_makeCombo(HOLDING_REGISTER, STRING, STRINGIFY_ASCII),
                                                        _makeCombo(INPUT_REGISTER, STRING, STRINGIFY_ASCII),
                                                        _makeCombo(HOLDING_REGISTER, STRING, STRINGIFY_UNICODE),
                                                        _makeCombo(INPUT_REGISTER, STRING, STRINGIFY_UNICODE)});
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
    static std::shared_ptr<ModbusReaderMock> modbusReaderMock;
    static std::unique_ptr<RegisterGroupMock> registerGroupMock;
};

std::unique_ptr<ModbusClientMock> ComplexMappingsTests::modbusClientMock;
std::unique_ptr<ModbusDeviceMock> ComplexMappingsTests::modbusDeviceMock;
std::shared_ptr<ModbusReaderMock> ComplexMappingsTests::modbusReaderMock;
std::unique_ptr<RegisterGroupMock> ComplexMappingsTests::registerGroupMock;

TEST_F(ComplexMappingsTests, UInt32MappingsCreation)
{
    const auto& outputType = _outputType::UINT32;
    for (const auto& registerType : registerTypes)
    {
        for (const auto& operationType : operationTypes)
        {
            const auto& combo = _makeComboPure(registerType, outputType, operationType);
            const bool winning = IsWinningCombo(outputType, combo);

            if (winning)
            {
                EXPECT_NO_THROW(
                  wolkabout::UInt32Mapping("TEST", registerType, std::vector<std::int32_t>{0, 1}, operationType));
            }
            else
            {
                EXPECT_THROW(
                  wolkabout::UInt32Mapping("TEST", registerType, std::vector<std::int32_t>{0, 1}, operationType),
                  std::logic_error);
            }
        }
    }
}

TEST_F(ComplexMappingsTests, UInt32MappingsWriteValue)
{
    const auto& outputType = _outputType::UINT32;
    const auto& uintCombos = winningCombos[outputType];
    for (const auto& combo : uintCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(std::get<2>(combo));
        const auto value = static_cast<uint32_t>(rand());
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(value, endian);
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::UInt32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                  operationType);
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group->m_device->m_reader.expired());
        const auto reader = mapping->m_group->m_device->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, bytes)).WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getUint32Value());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(ComplexMappingsTests, UInt32MappingsInitUpdateValid)
{
    const auto& outputType = _outputType::UINT32;
    const auto& uintCombos = winningCombos[outputType];
    for (const auto& combo : uintCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<uint32_t>(rand());
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(value, endian);
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::UInt32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                  operationType);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getUint32Value());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        // Redundant if checks
        mapping->m_operationType = _operationType::STRINGIFY_UNICODE;

        EXPECT_THROW(mapping->writeValue(value), std::logic_error);
        EXPECT_THROW(mapping->update(bytes), std::logic_error);
    }
}

TEST_F(ComplexMappingsTests, UInt32MappingsDeadband)
{
    const auto& outputType = _outputType::UINT32;
    const auto& uintCombos = winningCombos[outputType];
    for (const auto& combo : uintCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<uint32_t>(rand());
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(value, endian);
        double deadbandValue = 2.0;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::UInt32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                  operationType, false, -1, deadbandValue);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getUint32Value());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        EXPECT_FALSE(mapping->doesUpdate(wolkabout::DataParsers::uint32ToRegisters(value + 1, endian)));
        EXPECT_TRUE(mapping->doesUpdate(wolkabout::DataParsers::uint32ToRegisters(value + 3, endian)));
    }
}

TEST_F(ComplexMappingsTests, FrequencyFilter)
{
    const auto& outputType = _outputType::UINT32;
    const auto& uintCombos = winningCombos[outputType];
    for (const auto& combo : uintCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<uint32_t>(rand());
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(value, endian);
        std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(100);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::UInt32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                  operationType, false, -1, 0.0, frequencyFilterValue);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getUint32Value());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        EXPECT_FALSE(mapping->doesUpdate(wolkabout::DataParsers::uint32ToRegisters(value + 1, endian)));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_TRUE(mapping->doesUpdate(wolkabout::DataParsers::uint32ToRegisters(value + 3, endian)));
    }
}

TEST_F(ComplexMappingsTests, Int32MappingsCreation)
{
    const auto& outputType = _outputType::INT32;
    for (const auto& registerType : registerTypes)
    {
        for (const auto& operationType : operationTypes)
        {
            const auto& combo = _makeComboPure(registerType, outputType, operationType);
            const bool winning = IsWinningCombo(outputType, combo);

            if (winning)
            {
                EXPECT_NO_THROW(
                  wolkabout::Int32Mapping("TEST", registerType, std::vector<std::int32_t>{0, 1}, operationType));
            }
            else
            {
                EXPECT_THROW(
                  wolkabout::Int32Mapping("TEST", registerType, std::vector<std::int32_t>{0, 1}, operationType),
                  std::logic_error);
            }
        }
    }
}

TEST_F(ComplexMappingsTests, Int32MappingsWriteValue)
{
    const auto& outputType = _outputType::INT32;
    const auto& intCombos = winningCombos[outputType];
    for (const auto& combo : intCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<int32_t>(rand());
        const auto bytes = wolkabout::DataParsers::int32ToRegisters(value, endian);
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::Int32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                 operationType);
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group->m_device->m_reader.expired());
        const auto reader = mapping->m_group->m_device->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, bytes)).WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getInt32Value());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(ComplexMappingsTests, Int32MappingsInitUpdateValid)
{
    const auto& outputType = _outputType::INT32;
    const auto& intCombos = winningCombos[outputType];
    for (const auto& combo : intCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<int32_t>(rand());
        const auto bytes = wolkabout::DataParsers::int32ToRegisters(value, endian);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::Int32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                 operationType);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getInt32Value());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        // Redundant if checks
        mapping->m_operationType = _operationType::TAKE_BIT;

        EXPECT_THROW(mapping->writeValue(value), std::logic_error);
        EXPECT_THROW(mapping->update(bytes), std::logic_error);
    }
}

TEST_F(ComplexMappingsTests, Int32MappingsDeadband)
{
    const auto& outputType = _outputType::INT32;
    const auto& intCombos = winningCombos[outputType];
    for (const auto& combo : intCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto endian = endianForOperation.at(operationType);
        const auto value = static_cast<int32_t>(rand());
        const auto bytes = wolkabout::DataParsers::int32ToRegisters(value, endian);
        double deadbandValue = 2.0;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::Int32Mapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                 operationType, false, -1, deadbandValue);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getInt32Value());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        EXPECT_FALSE(mapping->doesUpdate(wolkabout::DataParsers::int32ToRegisters(value + 1, endian)));
        EXPECT_TRUE(mapping->doesUpdate(wolkabout::DataParsers::int32ToRegisters(value + 3, endian)));
    }
}

TEST_F(ComplexMappingsTests, FloatMappingsCreation)
{
    const auto& outputType = _outputType::FLOAT;
    for (const auto& registerType : registerTypes)
    {
        const auto& combo = _makeComboPure(registerType, outputType, _operationType::MERGE_FLOAT);
        const bool winning = IsWinningCombo(outputType, combo);

        if (winning)
        {
            EXPECT_NO_THROW(wolkabout::FloatMapping("TEST", registerType, std::vector<std::int32_t>{0, 1}));
        }
        else
        {
            EXPECT_THROW(wolkabout::FloatMapping("TEST", registerType, std::vector<std::int32_t>{0, 1}),
                         std::logic_error);
        }
    }
}

TEST_F(ComplexMappingsTests, FloatMappingsWriteValue)
{
    const auto& outputType = _outputType::FLOAT;
    const auto& floatCombos = winningCombos[outputType];
    for (const auto& combo : floatCombos)
    {
        const auto value = static_cast<float>(rand());
        const auto bytes = wolkabout::DataParsers::floatToRegisters(value);
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::FloatMapping>("TEST", registerType, std::vector<std::int32_t>{0, 1});
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group->m_device->m_reader.expired());
        const auto reader = mapping->m_group->m_device->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, bytes)).WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getFloatValue());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(ComplexMappingsTests, FloatMappingsInitUpdateValid)
{
    const auto& outputType = _outputType::FLOAT;
    const auto& floatCombos = winningCombos[outputType];
    for (const auto& combo : floatCombos)
    {
        const auto value = static_cast<float>(rand());
        const auto bytes = wolkabout::DataParsers::floatToRegisters(value);
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::FloatMapping>("TEST", registerType, std::vector<std::int32_t>{0, 1});

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getFloatValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        mapping->m_operationType = _operationType::MERGE_BIG_ENDIAN;

        EXPECT_THROW(mapping->writeValue(value), std::logic_error);
        EXPECT_THROW(mapping->update(bytes), std::logic_error);
    }
}

TEST_F(ComplexMappingsTests, FloatMappingsDeadband)
{
    const auto& outputType = _outputType::FLOAT;
    const auto& floatCombos = winningCombos[outputType];
    for (const auto& combo : floatCombos)
    {
        const auto value = static_cast<float>(15.0);
        double deadbandValue = 2.0;
        const auto bytes = wolkabout::DataParsers::floatToRegisters(value);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::FloatMapping>("TEST", registerType, std::vector<std::int32_t>{0, 1},
                                                                 false, -1, deadbandValue);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getFloatValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        EXPECT_FALSE(mapping->doesUpdate(wolkabout::DataParsers::floatToRegisters(value + 1.0)));
        EXPECT_TRUE(mapping->doesUpdate(wolkabout::DataParsers::floatToRegisters(value + 3.0)));
    }
}

TEST_F(ComplexMappingsTests, StringMappingsCreation)
{
    const auto& outputType = _outputType::STRING;
    for (const auto& registerType : registerTypes)
    {
        for (const auto& operationType : operationTypes)
        {
            const auto& combo = _makeComboPure(registerType, outputType, operationType);
            const bool winning = IsWinningCombo(outputType, combo);

            if (winning)
            {
                EXPECT_NO_THROW(wolkabout::StringMapping(
                  "TEST", registerType, std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, operationType));
            }
            else
            {
                EXPECT_THROW(
                  wolkabout::StringMapping("TEST", registerType,
                                           std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, operationType),
                  std::logic_error);
            }
        }
    }
}

std::string random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const char charset[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

TEST_F(ComplexMappingsTests, StringMappingsWriteValue)
{
    const auto& outputType = _outputType::STRING;
    const auto& stringCombos = winningCombos[outputType];
    for (const auto& combo : stringCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto addresses = std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto value = random_string(addresses.size() * 2);
        auto bytes = std::vector<uint16_t>();
        //        std::cout << "Testing with " << value << std::endl;

        if (operationType == _operationType::STRINGIFY_ASCII)
        {
            bytes = {wolkabout::DataParsers::asciiStringToRegisters(value)};
        }
        else
        {
            bytes = {wolkabout::DataParsers::unicodeStringToRegisters(value)};
        }

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::StringMapping>("TEST", registerType, addresses, operationType);
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group->m_device->m_reader.expired());
        const auto reader = mapping->m_group->m_device->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, bytes)).WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getStringValue());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(ComplexMappingsTests, StringMappingsInitUpdateValid)
{
    const auto& outputType = _outputType::STRING;
    const auto& stringCombos = winningCombos[outputType];
    for (const auto& combo : stringCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto addresses = std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto value = random_string(addresses.size() * 2);
        const auto bytes = wolkabout::DataParsers::asciiStringToRegisters(value);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::StringMapping>("TEST", registerType, addresses, operationType);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getStringValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        mapping->m_operationType = _operationType::MERGE_LITTLE_ENDIAN;

        EXPECT_THROW(mapping->writeValue(value), std::logic_error);
        EXPECT_THROW(mapping->update(bytes), std::logic_error);
    }
}

TEST_F(ComplexMappingsTests, StringMappingsFrequencyFilter)
{
    const auto& outputType = _outputType::STRING;
    const auto& stringCombos = winningCombos[outputType];
    for (const auto& combo : stringCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto addresses = std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto value = random_string(addresses.size() * 2);
        auto bytes = wolkabout::DataParsers::asciiStringToRegisters(value);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::StringMapping>("TEST", registerType, addresses, operationType, false,
                                                                  -1, std::chrono::milliseconds(100));

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update(bytes));
        EXPECT_EQ(value, mapping->getStringValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());

        mapping->m_operationType = _operationType::MERGE_LITTLE_ENDIAN;

        value = random_string(addresses.size() * 2);
        bytes = wolkabout::DataParsers::asciiStringToRegisters(value);

        EXPECT_FALSE(mapping->doesUpdate(bytes));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_TRUE(mapping->doesUpdate(bytes));
    }
}

TEST_F(ComplexMappingsTests, StringMappingsUpdateFailCauseSize)
{
    const auto& outputType = _outputType::STRING;
    const auto& stringCombos = winningCombos[outputType];
    for (const auto& combo : stringCombos)
    {
        const auto operationType = std::get<2>(combo);
        const auto addresses = std::vector<std::int32_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        const auto value = random_string(addresses.size() * 2 + 4);

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::StringMapping>("TEST", registerType, addresses, operationType);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_THROW(mapping->writeValue(value), std::logic_error);
    }
}
