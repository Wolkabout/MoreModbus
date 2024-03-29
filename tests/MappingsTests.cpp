/**
 * Copyright 2023 Wolkabout Technology s.r.o.
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
#include "more_modbus/mappings/BoolMapping.h"
#include "more_modbus/mappings/Int16Mapping.h"
#include "more_modbus/mappings/UInt16Mapping.h"
#include "more_modbus/modbus/LibModbusSerialRtuClient.h"
#include "more_modbus/modbus/LibModbusTcpIpClient.h"
#undef private
#undef protected

#include "mocks/ModbusClientMocking.h"
#include "mocks/ModbusDeviceMocking.h"
#include "mocks/ModbusReaderMocking.h"
#include "mocks/RegisterGroupMocking.h"
#include "more_modbus/mappings/FloatMapping.h"
#include "more_modbus/mappings/Int32Mapping.h"
#include "more_modbus/mappings/StringMapping.h"
#include "more_modbus/mappings/UInt32Mapping.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#define _registerType wolkabout::more_modbus::RegisterType
#define _outputType wolkabout::more_modbus::OutputType
#define _operationType wolkabout::more_modbus::OperationType
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

class MappingsTests : public ::testing::Test
{
public:
    std::vector<_registerType> registerTypes;
    std::vector<_outputType> outputTypes;
    std::vector<_operationType> operationTypes;

    std::map<_outputType, std::vector<_combination>> winningCombos;

    virtual void SetUp()
    {
        modbusClientMock = std::make_shared<::testing::NiceMock<ModbusClientMock>>();
        registerGroupMock = std::make_shared<::testing::NiceMock<RegisterGroupMock>>();
        modbusDeviceMock = std::make_shared<::testing::NiceMock<ModbusDeviceMock>>();
        modbusReaderMock = std::make_shared<::testing::NiceMock<ModbusReaderMock>>(*modbusClientMock);

        SetUpDefaultValues();
        SetUpWinningCombinations();
    }

    virtual void TearDown()
    {
        modbusDeviceMock.reset();
        modbusReaderMock.reset();
        registerGroupMock.reset();
        modbusClientMock.reset();
    }

    void MovePointers()
    {
        modbusDeviceMock->m_reader = modbusReaderMock;
        registerGroupMock->m_device = modbusDeviceMock;
    }

    void MoveBackPointers()
    {
        registerGroupMock = std::make_shared<::testing::NiceMock<RegisterGroupMock>>();
        modbusDeviceMock = std::make_shared<::testing::NiceMock<ModbusDeviceMock>>();
        modbusReaderMock = std::make_shared<::testing::NiceMock<ModbusReaderMock>>(*modbusClientMock);
    }

    void SetUpDefaultValues()
    {
        registerTypes = {_registerType::INPUT_CONTACT, _registerType::COIL, _registerType::INPUT_REGISTER,
                         _registerType::HOLDING_REGISTER};
        outputTypes = {_outputType::BOOL,   _outputType::FLOAT,  _outputType::INT16, _outputType::INT32,
                       _outputType::STRING, _outputType::UINT16, _outputType::UINT32};
        operationTypes = {_operationType::NONE,
                          _operationType::MERGE_BIG_ENDIAN,
                          _operationType::MERGE_LITTLE_ENDIAN,
                          _operationType::MERGE_FLOAT_BIG_ENDIAN,
                          _operationType::STRINGIFY_ASCII_BIG_ENDIAN,
                          _operationType::STRINGIFY_UNICODE_BIG_ENDIAN,
                          _operationType::TAKE_BIT};
    }

    void SetUpWinningCombinations()
    {
        winningCombos.emplace(_outputType::BOOL, std::vector<_combination>{
                                                   _makeCombo(INPUT_CONTACT, BOOL, NONE), _makeCombo(COIL, BOOL, NONE),
                                                   _makeCombo(INPUT_REGISTER, BOOL, TAKE_BIT),
                                                   _makeCombo(HOLDING_REGISTER, BOOL, TAKE_BIT)});

        winningCombos.emplace(_outputType::UINT16, std::vector<_combination>{_makeCombo(HOLDING_REGISTER, UINT16, NONE),
                                                                             _makeCombo(INPUT_REGISTER, UINT16, NONE)});

        winningCombos.emplace(_outputType::INT16, std::vector<_combination>{_makeCombo(HOLDING_REGISTER, INT16, NONE),
                                                                            _makeCombo(INPUT_REGISTER, INT16, NONE)});
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

    static std::shared_ptr<ModbusClientMock> modbusClientMock;
    static std::shared_ptr<ModbusDeviceMock> modbusDeviceMock;
    static std::shared_ptr<ModbusReaderMock> modbusReaderMock;
    static std::shared_ptr<RegisterGroupMock> registerGroupMock;
};

std::shared_ptr<ModbusClientMock> MappingsTests::modbusClientMock;
std::shared_ptr<ModbusDeviceMock> MappingsTests::modbusDeviceMock;
std::shared_ptr<ModbusReaderMock> MappingsTests::modbusReaderMock;
std::shared_ptr<RegisterGroupMock> MappingsTests::registerGroupMock;

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
                    EXPECT_NO_THROW(wolkabout::more_modbus::BoolMapping("TEST", registerType, 0));
                }
                else
                {
                    EXPECT_THROW(wolkabout::more_modbus::BoolMapping("TEST", registerType, 0), std::logic_error);
                }
            }
            else
            {
                if (winning)
                {
                    EXPECT_NO_THROW(wolkabout::more_modbus::BoolMapping("TEST", registerType, 0, operationType, 0));
                }
                else
                {
                    EXPECT_THROW(wolkabout::more_modbus::BoolMapping("TEST", registerType, 0, operationType, 0),
                                 std::logic_error);
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
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        const auto operationType = std::get<2>(combo);
        if (operationType == _operationType::NONE)
        {
            auto mapping = std::make_shared<wolkabout::more_modbus::BoolMapping>("TEST", registerType, 0, false, 0);
            mapping->m_boolValue = !value;
            MovePointers();
            mapping->m_group = std::move(registerGroupMock);
            ASSERT_FALSE(mapping->m_group.expired());
            ASSERT_FALSE(mapping->m_group.lock()->m_device.expired());
            ASSERT_FALSE(mapping->m_group.lock()->m_device.lock()->m_reader.expired());
            const auto reader = mapping->m_group.lock()->m_device.lock()->m_reader.lock();
            if (registerType == _registerType::COIL)
            {
                EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, value)).WillOnce(Return(true));
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
            auto mapping =
              std::make_shared<wolkabout::more_modbus::BoolMapping>("TEST", registerType, 0, operationType, 0);
            mapping->m_boolValue = !value;
            MovePointers();
            mapping->m_group = std::move(registerGroupMock);
            ASSERT_FALSE(mapping->m_group.expired());
            ASSERT_FALSE(mapping->m_group.lock()->m_device.expired());
            ASSERT_FALSE(mapping->m_group.lock()->m_device.lock()->m_reader.expired());
            const auto reader = mapping->m_group.lock()->m_device.lock()->m_reader.lock();
            if (registerType == _registerType::HOLDING_REGISTER)
            {
                EXPECT_CALL((ModbusReaderMock&)*reader, writeBitMapping).WillOnce(Return(true));
                EXPECT_TRUE(mapping->writeValue(value));
                EXPECT_TRUE(mapping->update(value));

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

TEST_F(MappingsTests, UInt16MappingCreation)
{
    const auto& outputType = _outputType::UINT16;
    for (const auto& registerType : registerTypes)
    {
        const auto& combo = _makeComboPure(registerType, outputType, _operationType::NONE);
        const bool winning = IsWinningCombo(outputType, combo);

        if (winning)
        {
            EXPECT_NO_THROW(wolkabout::more_modbus::UInt16Mapping("TEST", registerType, 0));
        }
        else
        {
            EXPECT_THROW(wolkabout::more_modbus::UInt16Mapping("TEST", registerType, 0), std::logic_error);
        }
    }
}

TEST_F(MappingsTests, UInt16MappingsWriteValue)
{
    const auto& outputType = _outputType::UINT16;
    const auto& boolCombos = winningCombos[outputType];
    for (const auto& combo : boolCombos)
    {
        const auto value = static_cast<uint16_t>(rand());
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::more_modbus::UInt16Mapping>("TEST", registerType, 0);
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group.expired());
        ASSERT_FALSE(mapping->m_group.lock()->m_device.expired());
        ASSERT_FALSE(mapping->m_group.lock()->m_device.lock()->m_reader.expired());
        const auto reader = mapping->m_group.lock()->m_device.lock()->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader, writeMapping(_, std::vector<uint16_t>{value}))
              .WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getValue());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(MappingsTests, UInt16MappingsInitUpdateValid)
{
    const auto& outputType = _outputType::UINT16;
    const auto& boolCombos = winningCombos[outputType];
    for (const auto& combo : boolCombos)
    {
        const auto value = static_cast<uint16_t>(rand());
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::more_modbus::UInt16Mapping>("TEST", registerType, 0);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update({value}));
        EXPECT_EQ(value, mapping->getValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());
    }
}

TEST_F(MappingsTests, Int16MappingCreation)
{
    const auto& outputType = _outputType::INT16;
    for (const auto& registerType : registerTypes)
    {
        const auto& combo = _makeComboPure(registerType, outputType, _operationType::NONE);
        const bool winning = IsWinningCombo(outputType, combo);

        if (winning)
        {
            EXPECT_NO_THROW(wolkabout::more_modbus::Int16Mapping("TEST", registerType, 0));
        }
        else
        {
            EXPECT_THROW(wolkabout::more_modbus::Int16Mapping("TEST", registerType, 0), std::logic_error);
        }
    }
}

TEST_F(MappingsTests, Int16MappingsWriteValue)
{
    const auto& outputType = _outputType::INT16;
    const auto& boolCombos = winningCombos[outputType];
    for (const auto& combo : boolCombos)
    {
        const auto value = static_cast<int16_t>(rand());
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::more_modbus::Int16Mapping>("TEST", registerType, 0);
        MovePointers();
        mapping->m_group = std::move(registerGroupMock);
        ASSERT_FALSE(mapping->m_group.expired());
        ASSERT_FALSE(mapping->m_group.lock()->m_device.expired());
        ASSERT_FALSE(mapping->m_group.lock()->m_device.lock()->m_reader.expired());
        const auto reader = mapping->m_group.lock()->m_device.lock()->m_reader.lock();
        if (registerType == _registerType::HOLDING_REGISTER)
        {
            EXPECT_CALL((ModbusReaderMock&)*reader,
                        writeMapping(_, std::vector<uint16_t>{static_cast<uint16_t>(value)}))
              .WillOnce(Return(true));
            EXPECT_TRUE(mapping->writeValue(value));

            EXPECT_EQ(value, mapping->getValue());
        }
        else
        {
            EXPECT_FALSE(mapping->writeValue(value));
        }

        MoveBackPointers();
        modbusReaderMock->m_devices.clear();
    }
}

TEST_F(MappingsTests, Int16MappingsInitUpdateValid)
{
    const auto& outputType = _outputType::INT16;
    const auto& boolCombos = winningCombos[outputType];
    for (const auto& combo : boolCombos)
    {
        const auto value = static_cast<int16_t>(rand());
        //        std::cout << "Testing with " << value << std::endl;

        const auto registerType = std::get<0>(combo);
        auto mapping = std::make_shared<wolkabout::more_modbus::Int16Mapping>("TEST", registerType, 0);

        EXPECT_FALSE(mapping->isInitialized());
        EXPECT_FALSE(mapping->isValid());

        EXPECT_NO_THROW(mapping->update({static_cast<uint16_t>(value)}));
        EXPECT_EQ(value, mapping->getValue());

        EXPECT_TRUE(mapping->isInitialized());
        EXPECT_TRUE(mapping->isValid());
    }
}

TEST_F(MappingsTests, BoolMappingRepeatAndDefaultButReadOnly)
{
    auto testValue = false;
    EXPECT_THROW(
      wolkabout::more_modbus::BoolMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_CONTACT, 0, false, 0,
                                          std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::BoolMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_CONTACT, 0, false, 0,
                                          std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, &testValue),
      std::logic_error);
    EXPECT_THROW(wolkabout::more_modbus::BoolMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0,
                                                     wolkabout::more_modbus::OperationType::TAKE_BIT, 0, false, 0,
                                                     std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
                 std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::BoolMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0,
                                          wolkabout::more_modbus::OperationType::TAKE_BIT, 0, false, 0,
                                          std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, &testValue),
      std::logic_error);
}

TEST_F(MappingsTests, FloatMappingRepeatAndDefaultButReadOnly)
{
    auto testValue = 1.0f;
    EXPECT_THROW(
      wolkabout::more_modbus::FloatMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1}, false,
                                           0, 0.0, std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(wolkabout::more_modbus::FloatMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER,
                                                      {0, 1}, false, 0, 0.0, std::chrono::milliseconds{0},
                                                      std::chrono::milliseconds{0}, &testValue),
                 std::logic_error);
}

TEST_F(MappingsTests, Int16MappingRepeatAndDefaultButReadOnly)
{
    auto testValue = std::int16_t{10};
    EXPECT_THROW(
      wolkabout::more_modbus::Int16Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0, false, 0,
                                           0.0, std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::Int16Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0, false, 0,
                                           0.0, std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, &testValue),
      std::logic_error);
}

TEST_F(MappingsTests, Int32MappingRepeatAndDefaultButReadOnly)
{
    auto testValue = std::int32_t{10};
    EXPECT_THROW(
      wolkabout::more_modbus::Int32Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                           wolkabout::more_modbus::OperationType::MERGE_BIG_ENDIAN, false, 0, 0.0,
                                           std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::Int32Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                           wolkabout::more_modbus::OperationType::MERGE_BIG_ENDIAN, false, 0, 0.0,
                                           std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, &testValue),
      std::logic_error);
}

TEST_F(MappingsTests, StringMappingRepeatAndDefaultButReadOnly)
{
    auto testValue = std::string("Test");
    EXPECT_THROW(
      wolkabout::more_modbus::StringMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                            wolkabout::more_modbus::OperationType::STRINGIFY_ASCII_BIG_ENDIAN, false, 0,
                                            std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::StringMapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                            wolkabout::more_modbus::OperationType::STRINGIFY_ASCII_BIG_ENDIAN, false, 0,
                                            std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, testValue),
      std::logic_error);
}

TEST_F(MappingsTests, UInt16MappingRepeatAndDefaultButReadOnly)
{
    auto testValue = std::uint16_t{10};
    EXPECT_THROW(
      wolkabout::more_modbus::UInt16Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0, false, 0,
                                            0.0, std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(wolkabout::more_modbus::UInt16Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, 0,
                                                       false, 0, 0.0, std::chrono::milliseconds{0},
                                                       std::chrono::milliseconds{0}, &testValue),
                 std::logic_error);
}

TEST_F(MappingsTests, UInt32MappingRepeatAndDefaultButReadOnly)
{
    auto testValue = std::uint32_t{10};
    EXPECT_THROW(
      wolkabout::more_modbus::UInt32Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                            wolkabout::more_modbus::OperationType::MERGE_BIG_ENDIAN, false, 0, 0.0,
                                            std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}),
      std::logic_error);
    EXPECT_THROW(
      wolkabout::more_modbus::UInt32Mapping("TEST", wolkabout::more_modbus::RegisterType::INPUT_REGISTER, {0, 1},
                                            wolkabout::more_modbus::OperationType::MERGE_BIG_ENDIAN, false, 0, 0.0,
                                            std::chrono::milliseconds{0}, std::chrono::milliseconds{0}, &testValue),
      std::logic_error);
}
