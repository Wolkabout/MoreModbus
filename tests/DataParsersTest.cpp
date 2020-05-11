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

#define _bits std::vector<bool>
#define _registers std::vector<uint16_t>

#include <gtest/gtest.h>

#include "utilities/ConsoleLogger.h"
#include "utilities/Logger.h"

namespace
{
class DataParsersTest : public ::testing::Test
{
protected:
    std::map<uint16_t, _bits> bitValues;
    std::map<std::string, _registers> stringValues;
    std::map<uint32_t, _registers> uint32ValuesBigEndian;
    std::map<uint32_t, _registers> uint32ValuesLittleEndian;
    std::map<uint32_t, _registers> invalidUInt32;
    std::map<int32_t, _registers> int32ValuesBigEndian;
    std::map<int32_t, _registers> int32ValuesLittleEndian;
    std::map<int32_t, _registers> invalidInt32;
    std::map<float, _registers> floatValues;
    std::map<float, _registers> invalidFloatValues;
    std::map<uint16_t, int16_t> uintToInt;

public:
    void SetUpBitValues()
    {
        bitValues.emplace(747, _bits{0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1});
        bitValues.emplace(2958, _bits{0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0});
        bitValues.emplace(14267, _bits{0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1});
        bitValues.emplace(15274, _bits{0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0});
        bitValues.emplace(20621, _bits{0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1});
        bitValues.emplace(44510, _bits{1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0});
        bitValues.emplace(47701, _bits{1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1});
        bitValues.emplace(50616, _bits{1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0});
        bitValues.emplace(52175, _bits{1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1});
        bitValues.emplace(56402, _bits{1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0});
    }

    void SetUpStringValues()
    {
        stringValues.emplace("Aloha", _registers{16748, 28520, 24832});
        stringValues.emplace("Aloha!", _registers{16748, 28520, 24865});
        stringValues.emplace("ASCII", _registers{16723, 17225, 18688});
        stringValues.emplace("Unicode", _registers{21870, 26979, 28516, 25856});
    }

    void SetUpUInt32Values()
    {
        uint32ValuesBigEndian.emplace(1908, _registers{1908, 0});
        uint32ValuesBigEndian.emplace(66666332, _registers{16220, 1017});
        uint32ValuesBigEndian.emplace(945890348, _registers{9260, 14433});
        uint32ValuesBigEndian.emplace(2313852345, _registers{38329, 35306});

        uint32ValuesLittleEndian.emplace(25953, _registers{0, 25953});
        uint32ValuesLittleEndian.emplace(231589259, _registers{3533, 50571});
        uint32ValuesLittleEndian.emplace(2412900252, _registers{36817, 61340});
        uint32ValuesLittleEndian.emplace(4128992390, _registers{63003, 27782});

        invalidUInt32.emplace(12409, _registers{12409});
        invalidUInt32.emplace(65536, _registers{});
        invalidUInt32.emplace(360923908, _registers{
                                           109,
                                           109,
                                           1,
                                           1,
                                           1,
                                         });
        invalidUInt32.emplace(2155, _registers{2, 1, 5, 5});
    }

    void SetUpInt32Values()
    {
        int32ValuesBigEndian.emplace(-1, _registers{65535, 65535});
        int32ValuesBigEndian.emplace(-103, _registers{65433, 65535});
        int32ValuesBigEndian.emplace(-412980, _registers{45772, 65529});
        int32ValuesBigEndian.emplace(-1012490345, _registers{40855, 50086});

        int32ValuesLittleEndian.emplace(-1, _registers{65535, 65535});
        int32ValuesLittleEndian.emplace(-103, _registers{65535, 65433});
        int32ValuesLittleEndian.emplace(-412980, _registers{65529, 45772});
        int32ValuesLittleEndian.emplace(-1012490345, _registers{50086, 40855});

        invalidInt32.emplace(-99999, _registers{1, 65535, 1245});
        invalidInt32.emplace(-24910, _registers{});
        invalidInt32.emplace(224910, _registers{65535, 65535, 1});
    }

    void SetUpFloatValues()
    {
        floatValues.emplace(4290.243, _registers{17798, 4594});
        floatValues.emplace(-333.6521, _registers{50086, 54136});
        floatValues.emplace(3489.12355, _registers{17754, 4602});
        floatValues.emplace(10.2419599, _registers{16675, 57105});

        invalidFloatValues.emplace(12498, _registers{0, 0, 0, 0, 0, 0});
        invalidFloatValues.emplace(-424.79, _registers{});
        invalidFloatValues.emplace(-1200.01, _registers{1200, 1200, 1200});
    }

    void SetUp16BitValues() { uintToInt.emplace(65535, -1); }

    void SetUp()
    {
        auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
        logger->setLogLevel(wolkabout::LogLevel::WARN);
        wolkabout::Logger::setInstance(std::move(logger));
        LOG(DEBUG) << "Started tests " << ::testing::UnitTest::GetInstance()->current_test_info()->name() << ".";

        SetUpBitValues();
        SetUpStringValues();
        SetUpUInt32Values();
        SetUpInt32Values();
        SetUpFloatValues();
        SetUp16BitValues();
    }
};

TEST_F(DataParsersTest, TestSeparateBits)
{
    if (bitValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : bitValues)
    {
        const auto calculatedBits = wolkabout::DataParsers::separateBits(kvp.first);

        LOG(DEBUG) << kvp.first;
        for (uint i = 0; i < 16; i++)
        {
            EXPECT_EQ(kvp.second[i], calculatedBits[15 - i]);
        }
    }
}

TEST_F(DataParsersTest, TestBytesToStringASCII)
{
    if (stringValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : stringValues)
    {
        auto string = wolkabout::DataParsers::registersToAsciiString(kvp.second);

        if (string[string.length() - 1] == '\0')
        {
            string.pop_back();
        }

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first.length(), string.length());
        EXPECT_EQ(kvp.first, string);
    }
}

TEST_F(DataParsersTest, TestBytesToStringUnicode)
{
    if (stringValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : stringValues)
    {
        auto string = wolkabout::DataParsers::registersToUnicodeString(kvp.second);

        if (string[string.length() - 1] == '\0')
        {
            string.pop_back();
        }

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first.length(), string.length());
        EXPECT_EQ(kvp.first, string);
    }
}

TEST_F(DataParsersTest, TestStringToBytesASCII)
{
    if (stringValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : stringValues)
    {
        const auto bytes = wolkabout::DataParsers::asciiStringToRegisters(kvp.first);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestStringToBytesUnicode)
{
    if (stringValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : stringValues)
    {
        const auto bytes = wolkabout::DataParsers::unicodeStringToRegisters(kvp.first);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestUInt32ToBytesBigEndian)
{
    if (uint32ValuesBigEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uint32ValuesBigEndian)
    {
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(kvp.first, wolkabout::DataParsers::Endian::BIG);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestRegistersToUInt32BigEndian)
{
    if (uint32ValuesBigEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uint32ValuesBigEndian)
    {
        const auto value = wolkabout::DataParsers::registersToUint32(kvp.second, wolkabout::DataParsers::Endian::BIG);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first, value);
    }
}

TEST_F(DataParsersTest, TestUInt32ToBytesLittleEndian)
{
    if (uint32ValuesLittleEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uint32ValuesLittleEndian)
    {
        const auto bytes = wolkabout::DataParsers::uint32ToRegisters(kvp.first, wolkabout::DataParsers::Endian::LITTLE);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestRegistersToUInt32LittleEndian)
{
    if (uint32ValuesLittleEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uint32ValuesLittleEndian)
    {
        const auto value =
          wolkabout::DataParsers::registersToUint32(kvp.second, wolkabout::DataParsers::Endian::LITTLE);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first, value);
    }
}

TEST_F(DataParsersTest, TestInvalidUint32Values)
{
    if (invalidUInt32.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : invalidUInt32)
    {
        LOG(DEBUG) << kvp.first;
        EXPECT_THROW(wolkabout::DataParsers::registersToUint32(kvp.second, wolkabout::DataParsers::Endian::BIG),
                     std::logic_error);
    }
}

TEST_F(DataParsersTest, TestInt32ToBytesBigEndian)
{
    if (int32ValuesBigEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : int32ValuesBigEndian)
    {
        const auto bytes = wolkabout::DataParsers::int32ToRegisters(kvp.first, wolkabout::DataParsers::Endian::BIG);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestRegistersToInt32BigEndian)
{
    if (int32ValuesBigEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : int32ValuesBigEndian)
    {
        const auto value = wolkabout::DataParsers::registersToInt32(kvp.second, wolkabout::DataParsers::Endian::BIG);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first, value);
    }
}

TEST_F(DataParsersTest, TestInt32ToBytesLittleEndian)
{
    if (int32ValuesLittleEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : int32ValuesLittleEndian)
    {
        const auto bytes = wolkabout::DataParsers::int32ToRegisters(kvp.first, wolkabout::DataParsers::Endian::LITTLE);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestRegistersToInt32LittleEndian)
{
    if (int32ValuesLittleEndian.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : int32ValuesLittleEndian)
    {
        const auto value = wolkabout::DataParsers::registersToInt32(kvp.second, wolkabout::DataParsers::Endian::LITTLE);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first, value);
    }
}

TEST_F(DataParsersTest, TestInvalidInt32Values)
{
    if (invalidInt32.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : invalidInt32)
    {
        LOG(DEBUG) << kvp.first;
        EXPECT_THROW(wolkabout::DataParsers::registersToInt32(kvp.second, wolkabout::DataParsers::Endian::BIG),
                     std::logic_error);
    }
}

TEST_F(DataParsersTest, TestFloatToBytes)
{
    if (floatValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : floatValues)
    {
        const auto bytes = wolkabout::DataParsers::floatToRegisters(kvp.first);

        LOG(DEBUG) << kvp.first;
        ASSERT_EQ(kvp.second.size(), bytes.size());

        for (uint i = 0; i < bytes.size(); i++)
        {
            EXPECT_EQ(kvp.second[i], bytes[i]);
        }
    }
}

TEST_F(DataParsersTest, TestBytesToFloat)
{
    if (floatValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : floatValues)
    {
        const auto value = wolkabout::DataParsers::registersToFloat(kvp.second);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.first, value);
    }
}

TEST_F(DataParsersTest, InvalidFloatTest)
{
    if (invalidFloatValues.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : invalidFloatValues)
    {
        EXPECT_THROW(wolkabout::DataParsers::registersToFloat(kvp.second), std::logic_error);
    }
}

TEST_F(DataParsersTest, UInt16ToInt16Test)
{
    if (uintToInt.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uintToInt)
    {
        const auto value = wolkabout::DataParsers::uint16ToInt16(kvp.first);

        LOG(DEBUG) << kvp.first;
        EXPECT_EQ(kvp.second, value);
    }
}

TEST_F(DataParsersTest, Int16ToUInt16Test)
{
    if (uintToInt.empty())
    {
        LOG(WARN) << "DataParsersTest: " << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << " has no data to test with.";
    }

    for (const auto& kvp : uintToInt)
    {
        const auto value = wolkabout::DataParsers::int16ToUint16(kvp.second);

        LOG(DEBUG) << kvp.second;
        EXPECT_EQ(kvp.first, value);
    }
}
}    // namespace
