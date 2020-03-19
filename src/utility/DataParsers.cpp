//
// Created by astrihale on 19.3.20..
//

#include "DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
std::vector<uint16_t> DataParsers::asciiStringToRegisters(const std::string& value)
{
    return std::vector<uint16_t>();
}

std::vector<uint16_t> DataParsers::unicodeStringToRegisters(const std::string& value)
{
    return std::vector<uint16_t>();
}

std::vector<uint16_t> DataParsers::utf16StringToRegisters(const std::string& value)
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
    return std::__cxx11::string();
}

std::string DataParsers::registersToUnicodeString(const std::vector<uint16_t>& value)
{
    return std::__cxx11::string();
}

std::string DataParsers::registersToUtf16String(const std::vector<uint16_t>& value)
{
    return std::__cxx11::string();
}

int32_t DataParsers::registersToInt32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Int32.");

    return 0;
}

uint32_t DataParsers::registersToUint32(const std::vector<uint16_t>& value, DataParsers::Endian endian)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an UInt32.");

    return 0;
}

float DataParsers::registersToFloat(const std::vector<uint16_t>& value)
{
    if (value.size() != 2)
        throw std::logic_error("DataParsers: You must pass exactly 2 values to parse into an Float.");

    return 0;
}
}    // namespace wolkabout
