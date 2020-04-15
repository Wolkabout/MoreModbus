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

#include "BoolMapping.h"

#include <stdexcept>

namespace wolkabout
{
BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::COIL || registerType == RegisterType::INPUT_CONTACT))
    {
        throw std::logic_error("BoolMapping: Illegal register type set.");
    }
}

BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         RegisterMapping::OperationType operation, int8_t bitIndex, bool readRestricted,
                         int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, operation, bitIndex, readRestricted, slaveAddress)
{
    if (!(operation == OperationType::TAKE_BIT))
    {
        throw std::logic_error("BoolMapping: Illegal operation type set.");
    }
}

bool BoolMapping::writeValue(bool value)
{
    bool success;
    if (m_operationType == OperationType::TAKE_BIT)
        success = getGroup()->getDevice()->getReader()->writeBitMapping(*this, value);
    else
        success = getGroup()->getDevice()->getReader()->writeMapping(*this, value);

    if (success)
        m_boolValue = value;

    return success;
}
}    // namespace wolkabout
