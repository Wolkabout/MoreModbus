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


#ifndef WOLKABOUT_MODBUS_BOOLMAPPING_H
#define WOLKABOUT_MODBUS_BOOLMAPPING_H

#include "ModbusReader.h"

namespace wolkabout
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
     */
    BoolMapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                int8_t slaveAddress = -1);

    /**
     * @brief Constructor for TAKE_BIT type of Mappings.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept COIL & INPUT_CONTACT, and also INPUT_REGISTER & HOLDING_REGISTER
     * @param address Modbus register address
     * @param operation Takes TAKE_BIT
     * @param bitIndex And bit index, as for which bit will we present out.
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     */
    BoolMapping(const std::string& reference, RegisterType registerType, int16_t address, OperationType operation,
                int8_t bitIndex, bool readRestricted = false, int8_t slaveAddress = -1);

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value Bool to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(bool value);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_BOOLMAPPING_H
