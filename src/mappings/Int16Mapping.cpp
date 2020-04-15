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

#include "Int16Mapping.h"
#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
Int16Mapping::Int16Mapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                           bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, OutputType::INT16, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::INPUT_REGISTER || registerType == RegisterType::HOLDING_REGISTER))
    {
        throw std::logic_error("UInt16Mapping: Illegal register type set.");
    }
}

bool Int16Mapping::update(const std::vector<uint16_t>& newValues)
{
    if (m_operationType != OperationType::NONE)
        throw std::logic_error("Int16Mapping: Illegal operation type set.");

    m_int16Value = DataParsers::uint16ToInt16(newValues[0]);
    return RegisterMapping::update(newValues);
}

bool Int16Mapping::writeValue(int16_t value)
{
    std::vector<uint16_t> bytes;
    bytes.emplace_back(DataParsers::int16ToUint16(value));

    bool success = getGroup()->getDevice()->getReader()->writeMapping(*this, bytes);
    if (success)
        m_int16Value = value;

    return success;
}

int16_t Int16Mapping::getInt16Value() const
{
    return m_int16Value;
}
}    // namespace wolkabout
