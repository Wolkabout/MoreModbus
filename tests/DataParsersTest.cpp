/*
 * Copyright 2020 WolkAbout Technology s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define private public
#define protected public
#include "utilities/DataParsers.h"
#undef private
#undef protected

#include <gtest/gtest.h>

#include "utilities/ConsoleLogger.h"
#include "utilities/Logger.h"

namespace
{
class DataParsersTest : public ::testing::Test
{
protected:
    std::map<uint16_t, std::vector<bool>> bitValues;

public:
    void SetUp()
    {
        auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
        logger->setLogLevel(wolkabout::LogLevel::WARN);
        wolkabout::Logger::setInstance(std::move(logger));
        LOG(DEBUG) << "Started tests.";

        bitValues.emplace(747, std::vector<bool> {0,0,0,0,0,0,1,0,1,1,1,0,1,0,1,1});
        bitValues.emplace(50616, std::vector<bool> {1,1,0,0,0,1,0,1,1,0,1,1,1,0,0,0});
        bitValues.emplace(14267, std::vector<bool> {0,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1});
        bitValues.emplace(2958, std::vector<bool> {0,0,0,0,1,0,1,1,1,0,0,0,1,1,1,0});
        bitValues.emplace(15274, std::vector<bool> {0,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0});
        bitValues.emplace(52175, std::vector<bool> {1,1,0,0,1,0,1,1,1,1,0,0,1,1,1,1});
        bitValues.emplace(44510, std::vector<bool> {1,0,1,0,1,1,0,1,1,1,0,1,1,1,1,0});
        bitValues.emplace(56402, std::vector<bool> {1,1,0,1,1,1,0,0,0,1,0,1,0,0,1,0});
        bitValues.emplace(20621, std::vector<bool> {0,1,0,1,0,0,0,0,1,0,0,0,1,1,0,1});
        bitValues.emplace(47701, std::vector<bool> {1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1});
    }
};

TEST_F(DataParsersTest, TestSeparateBits)
{
    for (const auto& kvp: bitValues)
    {
        const auto bits = wolkabout::DataParsers::separateBits(kvp.first);

        LOG(DEBUG) << kvp.first;
        for (uint i = 0; i < 16; i++)
        {
            EXPECT_EQ(kvp.second[i], bits[15 - i]);
        }
    }
}
}    // namespace
