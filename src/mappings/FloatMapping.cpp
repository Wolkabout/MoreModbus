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

#include "FloatMapping.h"
#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
FloatMapping::FloatMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                           const std::vector<int16_t>& addresses, bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::FLOAT, OperationType::MERGE_FLOAT, readRestricted,
                  slaveAddress)
{
}

bool FloatMapping::update(const std::vector<uint16_t>& newValues)
{
    if (m_operationType != OperationType::MERGE_FLOAT)
        throw std::logic_error("FloatMapping: Illegal operation type set.");

    m_floatValue = DataParsers::registersToFloat(newValues);
    return RegisterMapping::update(newValues);
}

bool FloatMapping::writeValue(float value)
{
    std::vector<uint16_t> bytes;

    if (m_operationType != OperationType::MERGE_FLOAT)
        throw std::logic_error("FloatMapping: Illegal operation type set.");
    bytes = DataParsers::floatToRegisters(value);

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_floatValue = value;

    return success;
}

float FloatMapping::getFloatValue() const
{
    return m_floatValue;
}
}    // namespace wolkabout
