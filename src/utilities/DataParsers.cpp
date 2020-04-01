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

#include "DataParsers.h"

#include <bitset>
#include <sstream>
#include <stdexcept>
#include <string>

namespace wolkabout
{
uint8_t DataParsers::MAX_UINT8 = 255;
uint8_t DataParsers::SHIFT_UINT8 = 8;
uint16_t DataParsers::MAX_UINT16 = 65535;
uint16_t DataParsers::SHIFT_UINT16 = 16;

std::vector<uint16_t> DataParsers::asciiStringToRegisters(const std::string& value)
{
    std::vector<uint16_t> values;
    for (uint i = 0; i < value.size(); i = i + 2)
    {
        char firstChar = value[i], secondChar = value[i + 1];
        values.emplace_back((firstChar << SHIFT_UINT8) + secondChar);
    }
    return values;
}

std::vector<uint16_t> DataParsers::unicodeStringToRegisters(const std::string& value)
{
    std::vector<uint16_t> values;
    for (uint i = 0; i < value.size(); i = i + 2)
    {
        auto firstChar = static_cast<uint8_t>(value[i]), secondChar = static_cast<uint8_t>(value[i + 1]);
        values.emplace_back((firstChar << SHIFT_UINT8) + secondChar);
    }
    return values;
}

std::vector<uint16_t> DataParsers::int32ToRegisters(int32_t value, DataParsers::Endian endian)
{
    return uint32ToRegisters(static_cast<uint32_t>(value), endian);
}

std::vector<uint16_t> DataParsers::uint32ToRegisters(uint32_t value, DataParsers::Endian endian)
{
    uint16_t bigValue = value & MAX_UINT16;
    uint16_t smallValue = (value >> SHIFT_UINT16) & MAX_UINT16;
    if (endian == Endian::BIG)
        return std::vector<uint16_t>{bigValue, smallValue};
    else
        return std::vector<uint16_t>{smallValue, bigValue};
}

std::vector<uint16_t> DataParsers::floatToRegisters(float value)
{
    union {
        float floatValue;
        uint16_t registerValues[2];
    } converter{};
    converter.floatValue = value;
    return std::vector<uint16_t>{converter.registerValues[0], converter.registerValues[1]};
}

std::string DataParsers::registersToAsciiString(const std::vector<uint16_t>& value)
{
    std::stringstream stream;
    for (auto twoBytes : value)
    {
        char firstByte = static_cast<char>(twoBytes & MAX_UINT8);
        char secondByte = static_cast<char>(static_cast<uint8_t>(twoBytes >> SHIFT_UINT8) & MAX_UINT8);
        stream << secondByte << firstByte;
    }
    return stream.str();
}

std::string DataParsers::registersToUnicodeString(const std::vector<uint16_t>& value)
{
    std::stringstream stream;
    for (auto twoBytes : value)
    {
        uint8_t firstByte = twoBytes & MAX_UINT8;
        uint8_t secondByte = static_cast<uint8_t>(twoBytes >> SHIFT_UINT8) & MAX_UINT8;
        stream << secondByte << firstByte;
    }
    return stream.str();
}

int32_t DataParsers::registersToInt32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Int32.");

    return static_cast<int32_t>(registersToUint32(value, endian));
}

uint32_t DataParsers::registersToUint32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an UInt32.");

    uint8_t bigValue = (endian == DataParsers::Endian::LITTLE), smallValue = !bigValue;
    return static_cast<uint32_t>((value[bigValue] << SHIFT_UINT16) + value[smallValue]);
}

float DataParsers::registersToFloat(const std::vector<uint16_t>& value)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Float.");

    union {
        float floatValue;
        uint16_t registerValues[2];
    } converter{};
    converter.registerValues[0] = value[0];
    converter.registerValues[1] = value[1];
    return converter.floatValue;
}

int16_t DataParsers::uint16ToInt16(uint16_t value)
{
    return static_cast<int16_t>(value);
}

uint16_t DataParsers::int16ToUint16(int16_t value)
{
    return static_cast<uint16_t>(value);
}

std::bitset<sizeof(uint16_t) * 8> DataParsers::separateBits(uint16_t value)
{
    return {value};
}
}    // namespace wolkabout
