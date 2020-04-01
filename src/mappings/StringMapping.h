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

#ifndef WOLKABOUT_MODBUS_STRINGMAPPING_H
#define WOLKABOUT_MODBUS_STRINGMAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
/**
 * @brief Class describing a RegisterMapping with output type STRING
 * @details This Mapping merges some registers into a single string value by storing two characters in a single uint16.
 */
class StringMapping : public RegisterMapping
{
public:
    /**
     * @brief Constructor defining two register addresses which can be read and written as two registers.
     * @param reference Name for the Mapping.
     * @param registerType Type, which will accept HOLDING_REGISTER & INPUT_REGISTER
     * @param addresses Modbus register addresses passed as vector of int16_t's
     * @param operation Define which operation will be done to characters, accepts STRINGIFY_ASCII & STRINGIFY_UNICODE
     * @param readRestricted Is the Mapping write only?
     * @param slaveAddress Slave address of device, leave to be assigned by device, default is -1.
     */
    StringMapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                  OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    /**
     * @details Override methods will be executed on devices reading thread, so that this parsing can be done
     *          for each device on their own respective thread.
     */
    bool update(const std::vector<uint16_t>& newValues) override;

    /**
     * @brief Triggers the client to write the value for this register.
     * @param value String value to be written
     * @return Result of the operation, whether or not it was successful
     */
    bool writeValue(const std::string& newValue);

    /**
     * @brief Get the last written/read value of registers, already parsed as STRING.
     */
    const std::string& getStringValue() const;

private:
    std::string m_stringValue;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_STRINGMAPPING_H
