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
#include "more_modbus/RegisterMapping.h"
#undef private
#undef protected

#define _registerType wolkabout::more_modbus::RegisterMapping::RegisterType
#define _outputType wolkabout::more_modbus::RegisterMapping::OutputType
#define _operationType wolkabout::more_modbus::RegisterMapping::OperationType
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

class RegisterMappingTests : public ::testing::Test
{
public:
    std::vector<_registerType> registerTypes;
    std::vector<_outputType> outputTypes;
    std::vector<_operationType> operationTypes;

    std::map<uint16_t, std::vector<_combination>> winningCombos;

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
    }

    void SetUpWinningCombinations()
    {
        winningCombos.emplace(
          0, std::vector<_combination>{_makeCombo(INPUT_CONTACT, BOOL, NONE), _makeCombo(COIL, BOOL, NONE),
                                       _makeCombo(INPUT_REGISTER, UINT16, NONE),
                                       _makeCombo(HOLDING_REGISTER, UINT16, NONE)});

        winningCombos.emplace(1,
                              std::vector<_combination>{
                                _makeCombo(INPUT_CONTACT, BOOL, NONE), _makeCombo(COIL, BOOL, NONE),
                                _makeCombo(INPUT_REGISTER, UINT16, NONE), _makeCombo(INPUT_REGISTER, INT16, NONE),
                                _makeCombo(HOLDING_REGISTER, UINT16, NONE), _makeCombo(HOLDING_REGISTER, INT16, NONE)});

        winningCombos.emplace(2, std::vector<_combination>{_makeCombo(INPUT_REGISTER, BOOL, TAKE_BIT),
                                                           _makeCombo(HOLDING_REGISTER, BOOL, TAKE_BIT)});

        winningCombos.emplace(3, std::vector<_combination>{_makeCombo(INPUT_REGISTER, UINT32, MERGE_BIG_ENDIAN),
                                                           _makeCombo(INPUT_REGISTER, INT32, MERGE_BIG_ENDIAN),
                                                           _makeCombo(INPUT_REGISTER, UINT32, MERGE_LITTLE_ENDIAN),
                                                           _makeCombo(INPUT_REGISTER, INT32, MERGE_LITTLE_ENDIAN),
                                                           _makeCombo(HOLDING_REGISTER, UINT32, MERGE_BIG_ENDIAN),
                                                           _makeCombo(HOLDING_REGISTER, INT32, MERGE_BIG_ENDIAN),
                                                           _makeCombo(HOLDING_REGISTER, UINT32, MERGE_LITTLE_ENDIAN),
                                                           _makeCombo(HOLDING_REGISTER, INT32, MERGE_LITTLE_ENDIAN),
                                                           _makeCombo(INPUT_REGISTER, FLOAT, MERGE_FLOAT),
                                                           _makeCombo(HOLDING_REGISTER, FLOAT, MERGE_FLOAT),
                                                           _makeCombo(INPUT_REGISTER, STRING, STRINGIFY_ASCII),
                                                           _makeCombo(INPUT_REGISTER, STRING, STRINGIFY_UNICODE),
                                                           _makeCombo(HOLDING_REGISTER, STRING, STRINGIFY_ASCII),
                                                           _makeCombo(HOLDING_REGISTER, STRING, STRINGIFY_UNICODE)});
    }

    bool IsWinningCombo(uint16_t set, _combination combo)
    {
        for (const auto& winningCombo : winningCombos[set])
        {
            if (combo == winningCombo)
            {
                return true;
            }
        }
        return false;
    }

    void SetUp() override
    {
        SetUpDefaultValues();
        SetUpWinningCombinations();
    }
};

TEST_F(RegisterMappingTests, ApsurdMappingsTests)
{
    EXPECT_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", _registerType::INPUT_CONTACT, 0, true),
                 std::logic_error);

    EXPECT_THROW(
      std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", _registerType::INPUT_CONTACT, 0, _outputType::BOOL, true),
      std::logic_error);

    EXPECT_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", _registerType::INPUT_REGISTER, 0,
                                                              _operationType::TAKE_BIT, 0, true),
                 std::logic_error);

    EXPECT_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", _registerType::INPUT_REGISTER,
                                                              std::vector<std::int32_t>{0, 1, 2}, _outputType::STRING,
                                                              _operationType::STRINGIFY_ASCII, true),
                 std::logic_error);
}

TEST_F(RegisterMappingTests, IllegalCombosCtorZero)
{
    for (const auto& registerType : registerTypes)
    {
        const auto combo = _makeComboPure(
          registerType, (int32_t)registerType < 2 ? _outputType::BOOL : _outputType::UINT16, _operationType::NONE);
        EXPECT_NO_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", registerType, 0));
    }
}

TEST_F(RegisterMappingTests, IllegalCombosCtorOne)
{
    for (const auto& registerType : registerTypes)
    {
        for (const auto& outputType : outputTypes)
        {
            const auto combo = _makeComboPure(registerType, outputType, _operationType::NONE);
            const auto winning = IsWinningCombo(1, combo);

            if (winning)
            {
                EXPECT_NO_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", registerType, 0, outputType));
            }
            else
            {
                EXPECT_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", registerType, 0, outputType),
                             std::logic_error);
            }
        }
    }
}

TEST_F(RegisterMappingTests, IllegalCombosCtorTwo)
{
    for (const auto& registerType : registerTypes)
    {
        const auto combo = _makeComboPure(registerType, _outputType::BOOL, _operationType::TAKE_BIT);
        const auto winning = IsWinningCombo(2, combo);

        if (winning)
        {
            EXPECT_NO_THROW(
              std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", registerType, 0, _operationType::TAKE_BIT, 0));
        }
        else
        {
            EXPECT_THROW(
              std::make_shared<wolkabout::more_modbus::RegisterMapping>("TEST", registerType, 0, _operationType::TAKE_BIT, 0),
              std::logic_error);
        }
    }
}

TEST_F(RegisterMappingTests, IllegalCombosCtorThree)
{
    for (const auto& registerType : registerTypes)
    {
        for (const auto& outputType : outputTypes)
        {
            for (const auto& operationType : operationTypes)
            {
                const auto combo = _makeComboPure(registerType, outputType, operationType);
                const auto winning = IsWinningCombo(3, combo);

                //                std::cout << (int32_t)registerType << ", " << (int32_t)outputType << ", " <<
                //                (int32_t)operationType << std::endl;

                if (winning)
                {
                    EXPECT_NO_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>(
                      "TEST", registerType, std::vector<std::int32_t>{0, 1}, outputType, operationType));
                }
                else
                {
                    EXPECT_THROW(std::make_shared<wolkabout::more_modbus::RegisterMapping>(
                                   "TEST", registerType, std::vector<std::int32_t>{0, 1}, outputType, operationType),
                                 std::logic_error);
                }
            }
        }
    }
}
