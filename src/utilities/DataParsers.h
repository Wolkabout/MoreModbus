/*
 *  Copyright (C) 2020 WolkAbout Technology s.r.o.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
     * @param value Int32 value to be parsed
     * @param endian Endian mode to be used to order uint16_t's
     * @return vector containing two uint16_t values, ordered for endian
     */
    static std::vector<uint16_t> int32ToRegisters(int32_t value, Endian endian);

    /**
     * @brief Convert passed 32-bit unsigned Integer value, where we separate it to two 16-bit uint16_t,
     *        order dictated by the endian mode passed as argument.
     * @param value UInt32 value to be parsed
     * @param endian Endian mode to be used to order uint16_t's
     * @return vector containing two uint16_t values, ordered for endian
     */
    static std::vector<uint16_t> uint32ToRegisters(uint32_t value, Endian endian);

    /**
     * @brief Convert passed 32-bit Float value, where we separate it to two 16-bit uint16_t,
     *        order dictated by the endian mode passed as argument.
     * @param value Float value to be parsed
     * @return vector containing two uint16_t values, separated bits
     */
    static std::vector<uint16_t> floatToRegisters(float value);

    /**
     * @brief Parse inputted uint16_t values, as a string, where each half of a uint16_t is a ASCII char.
     * @param value vector of uint16_t values, most often acquired by reading with modbus client.
     * @return string value, comprised of ascii characters.
     */
    static std::string registersToAsciiString(const std::vector<uint16_t>& value);

    /**
     * @brief Parse inputted uint16_t values, as a string, where each half of a uint16_t is a ASCII char.
     * @param value vector of uint16_t values, most often acquired by reading with modbus client.
     * @return string value, comprised of ascii characters.
     */
    static std::string registersToUnicodeString(const std::vector<uint16_t>& value);

    /**
     * @brief Parse inputted uint16_t values, to a 32 bit Integer,
     * where uint16 order is interpreted by passed Endian value.
     * @param value vector of uint16_t values, most often acquired by reading with modbus client.
     * @param endian indicates way to interpret uint16_t values.
     * @return parsed, merged 32 bit Integer.
     */
    static int32_t registersToInt32(const std::vector<uint16_t>& value, Endian endian);

    /**
     * @brief Parse inputted uint16_t values, to a 32 Bit Unsigned Integer,
     * where uint16 order is interpreted by passed Endian value.
     * @param value vector of uint16_t values, most often acquired by reading with modbus client.
     * @param endian indicates way to interpret uint16_t values.
     * @return parsed, merged 32 bit Unsigned Integer
     */
    static uint32_t registersToUint32(const std::vector<uint16_t>& value, Endian endian);

    /**
     * @brief Parse inputted uint16_t values, to a 32 bit Float.
     * @param value vector of uint16_t values, most often acquired by reading with modbus client.
     * @return parsed 32 bit Float
     */
    static float registersToFloat(const std::vector<uint16_t>& value);

    /**
     * @brief Static cast of uint16_t value to int16_t.
     * @param value uint16_t value (0 <=> (2^16 - 1))
     * @return int16_t value (- (2^15) <=> (2^15) - 1)
     */
    static int16_t uint16ToInt16(uint16_t value);

    /**
     * @brief Static cast of int16_t value to uint16_t.
     * @param int16_t value (- (2^15) <=> (2^15) - 1)
     * @return value uint16_t value (0 <=> (2^16 - 1))
     */
    static uint16_t int16ToUint16(int16_t value);

    /**
     * @brief Break up uint16_t into bits, as bitset, where bits are accessible with indexes.
     * @param value uint16_t value received through reading with modbus client.
     * @return bitset, containing 16 bits of value.
     */
    static std::bitset<sizeof(uint16_t) * 8> separateBits(uint16_t value);

private:
    /**
     * @brief Value built with 8 bits where each bit is 1.
     */
    static uint8_t MAX_UINT8;

    /**
     * @brief Number of shifts necessary for a uint8_t to move.
     */
    static uint8_t SHIFT_UINT8;

    /**
     * @brief Value built with 16 bits where each bit is 1.
     */
    static uint16_t MAX_UINT16;

    /**
     * @brief Number of shifts necessary for a uint16_t to move.
     */
    static uint16_t SHIFT_UINT16;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_DATAPARSERS_H
