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

#include "StringMapping.h"
#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
StringMapping::StringMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                             const std::vector<int16_t>& addresses, RegisterMapping::OperationType operation,
                             bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::STRING, operation, readRestricted, slaveAddress)
{
    if (operation != OperationType::STRINGIFY_ASCII && operation != OperationType::STRINGIFY_UNICODE)
    {
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }
}

bool wolkabout::StringMapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::STRINGIFY_ASCII:
        m_stringValue = DataParsers::registersToAsciiString(newValues);
        break;
    case OperationType ::STRINGIFY_UNICODE:
        m_stringValue = DataParsers::registersToUnicodeString(newValues);
        break;
    default:
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool StringMapping::writeValue(const std::string& newValue)
{
    if (newValue.size() > static_cast<uint16_t>(getRegisterCount() * 2))
    {
        throw std::logic_error("StringMapping: You can\'t write a string that\'s longer than " +
                               std::to_string(getRegisterCount() * 2) + " characters!");
    }

    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::STRINGIFY_ASCII)
        bytes = DataParsers::asciiStringToRegisters(newValue);
    else if (m_operationType == OperationType::STRINGIFY_UNICODE)
        bytes = DataParsers::unicodeStringToRegisters(newValue);
    else
        throw std::logic_error("StringMapping: Illegal operation type set.");

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_stringValue = newValue;

    return success;
}

const std::string& StringMapping::getStringValue() const
{
    return m_stringValue;
}
}    // namespace wolkabout
