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
    std::bitset<32> bits(*reinterpret_cast<unsigned long*>(&value));
    std::bitset<32> good(65535);
    std::bitset<16> secondBits((bits & good).to_ulong());
    std::bitset<16> firstBits((bits >> 16 & good).to_ulong());
    return std::vector<uint16_t>{static_cast<uint16_t>(firstBits.to_ulong()),
                                 static_cast<uint16_t>(secondBits.to_ulong())};
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

    uint8_t bigValue = (endian == DataParsers::Endian::BIG), smallValue = !bigValue;
    return static_cast<uint32_t>((value[bigValue] << SHIFT_UINT16) + value[smallValue]);
}

float DataParsers::registersToFloat(const std::vector<uint16_t>& value)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Float.");

    std::bitset<32> newBits(0);
    newBits = newBits | std::bitset<32>(value[0]);
    newBits <<= 16;
    newBits = newBits | std::bitset<32>(value[1]);

    auto newValue = newBits.to_ullong();
    return *reinterpret_cast<float*>(&newValue);
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
