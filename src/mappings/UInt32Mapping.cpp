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


#include "UInt32Mapping.h"
#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
UInt32Mapping::UInt32Mapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                             const std::vector<int16_t>& addresses, RegisterMapping::OperationType operation,
                             bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::UINT32, operation, readRestricted, slaveAddress)
{
    if (operation != OperationType::MERGE_BIG_ENDIAN && operation != OperationType::MERGE_LITTLE_ENDIAN)
    {
        throw std::logic_error("UInt32Mapping: Illegal operation type set.");
    }
}

bool UInt32Mapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::MERGE_BIG_ENDIAN:
        m_uint32Value = DataParsers::registersToUint32(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::MERGE_LITTLE_ENDIAN:
        m_uint32Value = DataParsers::registersToUint32(newValues, DataParsers::Endian::LITTLE);
        break;
    default:
        throw std::logic_error("UInt32Mapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool UInt32Mapping::writeValue(uint32_t value)
{
    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN)
        bytes = DataParsers::uint32ToRegisters(value, DataParsers::Endian::BIG);
    else if (m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
        bytes = DataParsers::uint32ToRegisters(value, DataParsers::Endian::LITTLE);
    else
        throw std::logic_error("UInt32Mapping: Illegal operation type set.");

    bool success = getGroup()->getDevice()->getReader()->writeMapping(*this, bytes);
    if (success)
        m_uint32Value = value;

    return success;
}

uint32_t UInt32Mapping::getUint32Value() const
{
    return m_uint32Value;
}
}    // namespace wolkabout
