/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
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

#ifndef WOLKABOUT_MODBUS_INT16MAPPING_H
#define WOLKABOUT_MODBUS_INT16MAPPING_H

#include "more_modbus/ModbusReader.h"

namespace wolkabout
{
namespace more_modbus
{
/**
 * @brief Class describing a RegisterMapping with output type INT16
 * @details This Mapping just converts a single uint16_t into a int16_t.
 */
class Int16Mapping : public RegisterMapping
{
public:
    /**
     * @brief Constructor defining a mapping with a single register address.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept INPUT_REGISTER & HOLDING_REGISTER
     * @param address Modbus register address
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     * @param deadbandValue indicates a change in value of the register that is insignificant data.
     * @param frequencyFilterValue changes that occur within the given time (in milliseconds) that will be ignored
     * @param repeatedWrite The minimal time between two writes for a mapping.
     * @param defaultValue The default value for the mapping.
     */
    Int16Mapping(const std::string& reference, RegisterType registerType, int32_t address, bool readRestricted = false,
                 int16_t slaveAddress = -1, double deadbandValue = 0.0,
                 std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0),
                 std::chrono::milliseconds repeatedWrite = std::chrono::milliseconds{0},
                 const std::int16_t* defaultValue = nullptr);

    /**
     * @details Override methods will be executed on devices reading thread, so that this parsing can be done
     *         for each device on their own respective thread.
     */
    bool update(const std::vector<uint16_t>& newValues) override;

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value INT16 to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(int16_t value);

    /**
     * @brief Get the last written/read value of registers, already parsed as INT16.
     */
    int16_t getValue() const;

private:
    int16_t m_int16Value{};
};
}    // namespace more_modbus
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_INT16MAPPING_H
