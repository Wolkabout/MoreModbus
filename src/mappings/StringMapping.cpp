/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
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

    bool success = getGroup()->getDevice()->getReader()->writeMapping(*this, bytes);
    if (success)
        m_stringValue = newValue;

    return success;
}

const std::string& StringMapping::getStringValue() const
{
    return m_stringValue;
}
}    // namespace wolkabout
