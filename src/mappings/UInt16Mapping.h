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
class UInt16Mapping : public RegisterMapping
{
public:
    UInt16Mapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                  int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(uint16_t value);

    uint16_t getUint16Value() const;

private:
    uint16_t m_uint16Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_UINT16MAPPING_H
