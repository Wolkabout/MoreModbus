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
     *          for each device on their own respective thread.
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
