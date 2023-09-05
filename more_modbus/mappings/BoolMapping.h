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

#ifndef WOLKABOUT_MODBUS_BOOLMAPPING_H
#define WOLKABOUT_MODBUS_BOOLMAPPING_H

#include "more_modbus/ModbusReader.h"

namespace wolkabout::more_modbus
{
/**
 * @brief Class describing a RegisterMapping which has an OutputType BOOLEAN
 * @details This is useful for creating mappings of RegisterType COIL or INPUT_DISCRETE,
 *         or ones returning a BOOL by taking a bit from a register.
 */
class BoolMapping : public RegisterMapping
{
public:
    /**
     * @brief Constructor for COIL/INPUT_CONTACT type of Mappings.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept COIL & INPUT_CONTACT
     * @param address Modbus register address
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     * @param frequencyFilterValue changes that occur within the given time (in milliseconds) that will be ignored
     * @param repeatedWrite The minimal time between two writes for a mapping.
     * @param defaultValue The default value for the mapping.
     * @param autoLocalUpdate Whether the local value of the mapping will be automatically updated when written in.
     */
    BoolMapping(const std::string& reference, RegisterType registerType, int32_t address, bool readRestricted = false,
                int16_t slaveAddress = -1,
                std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0),
                std::chrono::milliseconds repeatedWrite = std::chrono::milliseconds{0},
                const bool* defaultValue = nullptr, bool autoLocalUpdate = false);

    /**
     * @brief Constructor for TAKE_BIT type of Mappings.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept COIL & INPUT_CONTACT, and also INPUT_REGISTER & HOLDING_REGISTER
     * @param address Modbus register address
     * @param operation Takes TAKE_BIT
     * @param bitIndex And bit index, as for which bit will we present out.
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     * @param frequencyFilterValue changes that occur within the given time (in milliseconds) that will be ignored
     * @param repeatedWrite The minimal time between two writes for a mapping.
     * @param defaultValue The default value for the mapping.
     * @param autoLocalUpdate Whether the local value of the mapping will be automatically updated when written in.
     */
    BoolMapping(const std::string& reference, RegisterType registerType, int32_t address, OperationType operation,
                int8_t bitIndex, bool readRestricted = false, int16_t slaveAddress = -1,
                std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0),
                std::chrono::milliseconds repeatedWrite = std::chrono::milliseconds{0},
                const bool* defaultValue = nullptr, bool autoLocalUpdate = false);

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value Bool to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(bool value);

    bool getValue() const;
};
}    // namespace wolkabout::more_modbus

#endif    // WOLKABOUT_MODBUS_BOOLMAPPING_H
