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

#include "Int32Mapping.h"
#include "utility/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
Int32Mapping::Int32Mapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                           const std::vector<int16_t>& addresses, RegisterMapping::OperationType operation,
                           bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::INT32, operation, readRestricted, slaveAddress)
{
    if (operation != OperationType::MERGE_BIG_ENDIAN && operation != OperationType::MERGE_LITTLE_ENDIAN)
    {
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }
}

bool Int32Mapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::MERGE_BIG_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::MERGE_LITTLE_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::LITTLE);
        break;
    default:
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool Int32Mapping::writeValue(int32_t value)
{
    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::BIG);
    else if (m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::LITTLE);
    else
        throw std::logic_error("Int32Mapping: Illegal operation type set.");

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_int32Value = value;

    return success;
}

int32_t Int32Mapping::getInt32Value() const
{
    return m_int32Value;
}
}    // namespace wolkabout
