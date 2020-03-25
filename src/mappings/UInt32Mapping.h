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

#ifndef WOLKABOUT_MODBUS_UINT32MAPPING_H
#define WOLKABOUT_MODBUS_UINT32MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class UInt32Mapping : public RegisterMapping
{
public:
    UInt32Mapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                  OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(uint32_t value);

    uint32_t getUint32Value() const;

private:
    uint32_t m_uint32Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_UINT32MAPPING_H