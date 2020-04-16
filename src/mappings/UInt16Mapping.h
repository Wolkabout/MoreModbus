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


#ifndef WOLKABOUT_MODBUS_UINT16MAPPING_H
#define WOLKABOUT_MODBUS_UINT16MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
/**
 * @brief Class describing a RegisterMapping with output type UINT16
 * @details This Mapping just keeps a single uint16_t.
 */
class UInt16Mapping : public RegisterMapping
{
public:
    /**
     * @brief Constructor defining a mapping with a single register address.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept INPUT_REGISTER & HOLDING_REGISTER
     * @param address Modbus register address
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     */
    UInt16Mapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                  int8_t slaveAddress = -1);

    /**
     * @details Override methods will be executed on devices reading thread, so that this parsing can be done
     *         for each device on their own respective thread.
     */
    bool update(const std::vector<uint16_t>& newValues) override;

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value UINT16 value to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(uint16_t value);

    /**
     * @brief Get the last written/read value of registers, already parsed as UINT16.
     */
    uint16_t getUint16Value() const;

private:
    uint16_t m_uint16Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_UINT16MAPPING_H
