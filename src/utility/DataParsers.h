//
// Created by astrihale on 19.3.20..
//

#ifndef WOLKABOUT_MODBUS_DATAPARSERS_H
#define WOLKABOUT_MODBUS_DATAPARSERS_H

#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

namespace wolkabout
{
class DataParsers
{
public:
    enum class Endian
    {
        BIG = 0,
        LITTLE
    };

    static std::vector<uint16_t> asciiStringToRegisters(const std::string& value);
    static std::vector<uint16_t> unicodeStringToRegisters(const std::string& value);

    static std::vector<uint16_t> int32ToRegisters(int32_t value, Endian endian);
    static std::vector<uint16_t> uint32ToRegisters(uint32_t value, Endian endian);
    static std::vector<uint16_t> floatToRegisters(float value);

    static std::string registersToAsciiString(const std::vector<uint16_t>& value);
    static std::string registersToUnicodeString(const std::vector<uint16_t>& value);

    static int32_t registersToInt32(const std::vector<uint16_t>& value, Endian endian);
    static uint32_t registersToUint32(const std::vector<uint16_t>& value, Endian endian);
    static float registersToFloat(const std::vector<uint16_t>& value);

    static int16_t uint16ToInt16(uint16_t value);
    static uint16_t int16ToUint16(int16_t value);

    static std::bitset<sizeof(uint16_t) * 8> separteBits(uint16_t value);

private:
    static uint8_t MAX_UINT8;
    static uint8_t SHIFT_UINT8;
    static uint16_t MAX_UINT16;
    static uint16_t SHIFT_UINT16;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_DATAPARSERS_H
