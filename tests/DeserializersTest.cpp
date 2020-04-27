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
#include "utilities/Deserializers.h"
#undef private
#undef protected

#define RegisterType wolkabout::RegisterMapping::RegisterType
#define OutputType wolkabout::RegisterMapping::OutputType
#define OperationType wolkabout::RegisterMapping::OperationType

#include <gtest/gtest.h>

#include "utilities/ConsoleLogger.h"
#include "utilities/Logger.h"

namespace
{
class DeserializersTest : public ::testing::Test
{
protected:
    std::map<RegisterType, std::string> registerTypes;
    std::map<OutputType, std::string> dataTypes;
    std::map<OperationType, std::string> operationTypes;

public:
    void SetUpRegisterTypes()
    {
        registerTypes.emplace(RegisterType::COIL, "COIL");
        registerTypes.emplace(RegisterType::INPUT_CONTACT, "INPUT_CONTACT");
        registerTypes.emplace(RegisterType::HOLDING_REGISTER, "HOLDING_REGISTER");
        registerTypes.emplace(RegisterType::INPUT_REGISTER, "INPUT_REGISTER");
    }

    void SetUpDataTypes()
    {
        dataTypes.emplace(OutputType::BOOL, "BOOL");
        dataTypes.emplace(OutputType::UINT16, "UINT16");
        dataTypes.emplace(OutputType::INT16, "INT16");
        dataTypes.emplace(OutputType::UINT32, "UINT32");
        dataTypes.emplace(OutputType::INT32, "INT32");
        dataTypes.emplace(OutputType::FLOAT, "FLOAT");
        dataTypes.emplace(OutputType::STRING, "STRING");
    }

    void SetUpOperationTypes()
    {
        operationTypes.emplace(OperationType::NONE, "NONE");
        operationTypes.emplace(OperationType::MERGE_BIG_ENDIAN, "MERGE_BIG_ENDIAN");
        operationTypes.emplace(OperationType::MERGE_LITTLE_ENDIAN, "MERGE_LITTLE_ENDIAN");
        operationTypes.emplace(OperationType::MERGE_FLOAT, "MERGE_FLOAT");
        operationTypes.emplace(OperationType::STRINGIFY_ASCII, "STRINGIFY_ASCII");
        operationTypes.emplace(OperationType::STRINGIFY_UNICODE, "STRINGIFY_UNICODE");
        operationTypes.emplace(OperationType::TAKE_BIT, "TAKE_BIT");
    }

    void SetUp()
    {
        auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
        logger->setLogLevel(wolkabout::LogLevel::WARN);
        wolkabout::Logger::setInstance(std::move(logger));
        LOG(DEBUG) << "Started tests " << ::testing::UnitTest::GetInstance()->current_test_info()->name() << ".";

        SetUpRegisterTypes();
        SetUpDataTypes();
        SetUpOperationTypes();
    }
};

TEST_F(DeserializersTest, TestRegisterTypeDeserialization)
{
    if (registerTypes.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : registerTypes)
    {
        const auto type = wolkabout::Deserializers::deserializeRegisterType(kvp.second);

        EXPECT_EQ(kvp.first, type);
    }
}

TEST_F(DeserializersTest, TestDataTypeDeserialization)
{
    if (dataTypes.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : dataTypes)
    {
        const auto type = wolkabout::Deserializers::deserializeDataType(kvp.second);

        EXPECT_EQ(kvp.first, type);
    }
}

TEST_F(DeserializersTest, TestOperationTypeDeserialization)
{
    if (operationTypes.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : operationTypes)
    {
        const auto type = wolkabout::Deserializers::deserializeOperationType(kvp.second);

        EXPECT_EQ(kvp.first, type);
    }
}
}