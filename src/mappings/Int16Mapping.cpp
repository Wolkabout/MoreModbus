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
