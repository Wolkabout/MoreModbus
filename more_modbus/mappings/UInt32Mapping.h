/**
 * Copyright 2021 Wolkabout Technology s.r.o.
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

#ifndef WOLKABOUT_MODBUS_UINT32MAPPING_H
#define WOLKABOUT_MODBUS_UINT32MAPPING_H

#include "more_modbus/ModbusReader.h"

namespace wolkabout::more_modbus
{
/**
 * @brief Class describing a RegisterMapping with output type UINT32
 * @details This Mapping merges two registers into a single int32 value.
 */
class UInt32Mapping : public RegisterMapping
{
public:
    /**
     * @brief Constructor defining two register addresses which can be read and written as two registers.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept HOLDING_REGISTER & INPUT_REGISTER
     * @param addresses Modbus register addresses passed as vector of int16_t's
     * @param operation Operation type, accepts MERGE_BIG_ENDIAN and MERGE_LITTLE_ENDIAN
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     * @param deadbandValue indicates a change in value of the register that is insignificant data.
     * @param frequencyFilterValue changes that occur within the given time (in milliseconds) that will be ignored
     * @param repeatedWrite The minimal time between two writes for a mapping.
     * @param defaultValue The default value for the mapping.
     * @param autoLocalUpdate Whether the local value of the mapping will be automatically updated when written in.
     */
    UInt32Mapping(const std::string& reference, RegisterType registerType, const std::vector<int32_t>& addresses,
                  OperationType operation, bool readRestricted = false, int16_t slaveAddress = -1,
                  double deadbandValue = 0.0,
                  std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0),
                  std::chrono::milliseconds repeatedWrite = std::chrono::milliseconds{0},
                  const std::uint32_t* defaultValue = nullptr, bool autoLocalUpdate = false);

    /**
     * @details Override methods will be executed on devices reading thread, so that this parsing can be done
     *         for each device on their own respective thread.
     */
    bool update(const std::vector<uint16_t>& newValues) override;

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value UINT32 value to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(uint32_t value);

    /**
     * @brief Get the last written/read value of registers, already parsed as UINT32.
     */
    uint32_t getValue() const;

private:
    uint32_t m_uint32Value{};
};
}    // namespace wolkabout::more_modbus

#endif    // WOLKABOUT_MODBUS_UINT32MAPPING_H
