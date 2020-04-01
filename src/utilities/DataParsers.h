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

#ifndef WOLKABOUT_MODBUS_DATAPARSERS_H
#define WOLKABOUT_MODBUS_DATAPARSERS_H

#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

namespace wolkabout
{
/**
 * @brief Collection of Utility methods, necessary for parsing data, between uint16_t, and
 *          wanted user types.
 */
class DataParsers
{
public:
    /**
     * @brief Enum used for parsing methods where Endian needs to be defined.
     */
    enum class Endian
    {
        BIG = 0,
        LITTLE
    };

    /**
     * @brief Convert passed string value, where each character can be interpreted as an ASCII character
     *          into an uint16_t vector, where one uint16_t holds two characters, one as first 8 bits,
     *          and next one as other 8 bits.
     * @param value String value to be parsed.
     * @return std::vector containing (number of value characters / 2) uint16_t values.
     */
    static std::vector<uint16_t> asciiStringToRegisters(const std::string& value);

    /**
     * @brief Convert passed string value, where each character can be interpreted as an UNICODE (only 0-255)
     *          character into an uint16_t vector, where one uint16_t holds two characters,
     *          one as first 8 bits, and next one as other 8 bits.
     * @param value String value to be parsed.
     * @return std::vector containing (number of value characters / 2) uint16_t values.
     */
    static std::vector<uint16_t> unicodeStringToRegisters(const std::string& value);

    /**
     * @brief Convert passed 32-bit Integer value, where we separate it to two 16-bit uint16_t,
     *        order dictated by the endian mode passed as argument.
     * @param value
     * @param endian 
     * @return
     */
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

    static std::bitset<sizeof(uint16_t) * 8> separateBits(uint16_t value);

private:
    static uint8_t MAX_UINT8;
    static uint8_t SHIFT_UINT8;
    static uint16_t MAX_UINT16;
    static uint16_t SHIFT_UINT16;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_DATAPARSERS_H
