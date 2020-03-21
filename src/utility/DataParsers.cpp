//
// Created by astrihale on 19.3.20..
//

#include "DataParsers.h"
#include "utility/Logger.h"

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
        auto firstChar = static_cast<uint8_t>(value[i]),
             secondChar = static_cast<uint8_t>(value[i + 1]);
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
    return std::vector<uint16_t>();
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

    uint8_t bigValue = (endian == DataParsers::Endian::LITTLE), smallValue = !bigValue;
    return static_cast<int32_t>((value[bigValue] << SHIFT_UINT16) + value[smallValue]);
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

    return 0;
}
}    // namespace wolkabout
