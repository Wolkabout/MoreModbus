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
    std::map<std::string, std::vector<std::uint16_t>> stringValues;
    std::map<uint32_t, std::vector<std::uint16_t>> uint32Values;
    std::map<int32_t, std::vector<std::uint16_t>> int32Values;
    std::map<float, std::vector<std::uint16_t>> floatValues;

public:
    void SetUpBitValues()
    {
        bitValues.emplace(747, std::vector<bool> {0,0,0,0,0,0,1,0,1,1,1,0,1,0,1,1});
        bitValues.emplace(2958, std::vector<bool> {0,0,0,0,1,0,1,1,1,0,0,0,1,1,1,0});
        bitValues.emplace(14267, std::vector<bool> {0,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1});
        bitValues.emplace(15274, std::vector<bool> {0,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0});
        bitValues.emplace(20621, std::vector<bool> {0,1,0,1,0,0,0,0,1,0,0,0,1,1,0,1});
        bitValues.emplace(44510, std::vector<bool> {1,0,1,0,1,1,0,1,1,1,0,1,1,1,1,0});
        bitValues.emplace(47701, std::vector<bool> {1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1});
        bitValues.emplace(50616, std::vector<bool> {1,1,0,0,0,1,0,1,1,0,1,1,1,0,0,0});
        bitValues.emplace(52175, std::vector<bool> {1,1,0,0,1,0,1,1,1,1,0,0,1,1,1,1});
        bitValues.emplace(56402, std::vector<bool> {1,1,0,1,1,1,0,0,0,1,0,1,0,0,1,0});
    }

    void SetUpStringValues()
    {

    }

    void SetUp()
    {
        auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
        logger->setLogLevel(wolkabout::LogLevel::WARN);
        wolkabout::Logger::setInstance(std::move(logger));
        LOG(DEBUG) << "Started tests.";

        SetUpBitValues();
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
