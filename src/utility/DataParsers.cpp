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
std::vector<uint16_t> DataParsers::asciiStringToRegisters(const std::string& value)
{
    std::vector<uint16_t> values;
    for (uint i = 0; i < value.size(); i = i + 2)
    {
        values.emplace_back((value[i] << 8) + value[i + 1]);
    }
    return values;
}

std::vector<uint16_t> DataParsers::unicodeStringToRegisters(const std::string& value)
{
    return std::vector<uint16_t>();
}

std::vector<uint16_t> DataParsers::int32ToRegisters(int32_t value, DataParsers::Endian endian)
{
    return std::vector<uint16_t>();
}

std::vector<uint16_t> DataParsers::uint32ToRegisters(uint32_t value, DataParsers::Endian endian)
{
    return std::vector<uint16_t>();
}

std::vector<uint16_t> DataParsers::floatToRegisters(float value)
{
    return std::vector<uint16_t>();
}

std::string DataParsers::registersToAsciiString(const std::vector<uint16_t>& value)
{
    std::stringstream stream;
    uint8_t max = 255;
    uint8_t shift = 8;
    for (auto twoBytes : value)
    {
        char firstByte = static_cast<char>(twoBytes & max);
        char secondByte = static_cast<char>(static_cast<uint8_t>(twoBytes >> shift) & max);
        stream << secondByte << firstByte;
    }
    return stream.str();
}

std::string DataParsers::registersToUnicodeString(const std::vector<uint16_t>& value)
{
    std::stringstream stream;
    uint8_t max = 255;
    uint8_t shift = 8;
    for (auto twoBytes : value)
    {
        uint8_t firstByte = twoBytes & max;
        uint8_t secondByte = static_cast<uint8_t>(twoBytes >> shift) & max;
        stream << secondByte << firstByte;
    }
    return stream.str();
}

int32_t DataParsers::registersToInt32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Int32.");

    int32_t newValue = 0;
    if (endian == DataParsers::Endian::BIG)
    {
        newValue += value[0] * 65536;
        newValue += value[1];
    }
    else
    {
        newValue += value[1] * 65536;
        newValue += value[0];
    }
    return newValue;
}

uint32_t DataParsers::registersToUint32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an UInt32.");

    uint32_t newValue = 0;
    if (endian == DataParsers::Endian::BIG)
    {
        newValue += value[0] * 65536;
        newValue += value[1];
    }
    else
    {
        newValue += value[1] * 65536;
        newValue += value[0];
    }
    return newValue;
}

float DataParsers::registersToFloat(const std::vector<uint16_t>& value)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Float.");

    return 0;
}
}    // namespace wolkabout
