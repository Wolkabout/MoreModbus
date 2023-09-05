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

#include "more_modbus/mappings/StringMapping.h"

#include "more_modbus/utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout::more_modbus
{
StringMapping::StringMapping(const std::string& reference, RegisterType registerType,
                             const std::vector<int32_t>& addresses, OperationType operation, bool readRestricted,
                             int16_t slaveAddress, std::chrono::milliseconds frequencyFilterValue,
                             std::chrono::milliseconds repeatedWrite, const std::string& defaultValue,
                             bool autoLocalUpdate)
: RegisterMapping(reference, registerType, addresses, OutputType::STRING, operation, readRestricted, slaveAddress, 0.0,
                  frequencyFilterValue, repeatedWrite, autoLocalUpdate)
{
    if (operation != OperationType::STRINGIFY_ASCII_BIG_ENDIAN &&
        operation != OperationType::STRINGIFY_ASCII_LITTLE_ENDIAN &&
        operation != OperationType::STRINGIFY_UNICODE_BIG_ENDIAN &&
        operation != OperationType::STRINGIFY_UNICODE_LITTLE_ENDIAN)
    {
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }
    if (repeatedWrite.count() > 0 && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("StringMapping: Can not set a repeated write value for a read-only register.");
    }
    if (!defaultValue.empty() && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("StringMapping: Can not set a default value for a read-only register.");
    }

    if (!defaultValue.empty() && (defaultValue.size() <= static_cast<uint16_t>(getRegisterCount() * 2)))
    {
        m_stringValue = defaultValue;
        if (operation == OperationType::STRINGIFY_ASCII_BIG_ENDIAN)
            m_byteValues = DataParsers::asciiStringToRegisters(defaultValue);
        else
            m_byteValues = DataParsers::unicodeStringToRegisters(defaultValue);
        m_defaultValue = m_stringValue;
    }
}

bool StringMapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::STRINGIFY_ASCII_BIG_ENDIAN:
        m_stringValue = DataParsers::registersToAsciiString(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::STRINGIFY_ASCII_LITTLE_ENDIAN:
        m_stringValue = DataParsers::registersToAsciiString(newValues, DataParsers::Endian::LITTLE);
        break;
    case OperationType ::STRINGIFY_UNICODE_BIG_ENDIAN:
        m_stringValue = DataParsers::registersToUnicodeString(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::STRINGIFY_UNICODE_LITTLE_ENDIAN:
        m_stringValue = DataParsers::registersToUnicodeString(newValues, DataParsers::Endian::LITTLE);
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
    if (m_operationType == OperationType::STRINGIFY_ASCII_BIG_ENDIAN)
    {
        bytes = DataParsers::asciiStringToRegisters(newValue, DataParsers::Endian::BIG);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else if (m_operationType == OperationType::STRINGIFY_ASCII_LITTLE_ENDIAN)
    {
        bytes = DataParsers::asciiStringToRegisters(newValue, DataParsers::Endian::LITTLE);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else if (m_operationType == OperationType::STRINGIFY_UNICODE_BIG_ENDIAN)
    {
        bytes = DataParsers::unicodeStringToRegisters(newValue, DataParsers::Endian::BIG);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else if (m_operationType == OperationType::STRINGIFY_UNICODE_LITTLE_ENDIAN)
    {
        bytes = DataParsers::unicodeStringToRegisters(newValue, DataParsers::Endian::LITTLE);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else
        throw std::logic_error("StringMapping: Illegal operation type set.");

    while (bytes.size() < this->m_addresses.size())
        bytes.emplace_back(0);

    const auto success = RegisterMapping::writeValue(bytes);
    if (success)
        m_stringValue = newValue;

    return success;
}

const std::string& StringMapping::getValue() const
{
    return m_stringValue;
}
}    // namespace wolkabout::more_modbus
