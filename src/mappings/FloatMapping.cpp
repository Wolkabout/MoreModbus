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

#include "FloatMapping.h"

#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
FloatMapping::FloatMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                           const std::vector<int16_t>& addresses, bool readRestricted, int16_t slaveAddress)
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

    if (getGroup()->getDevice()->getReader().expired())
        return false;

    const auto reader = getGroup()->getDevice()->getReader().lock();
    bool success = reader->writeMapping(*this, bytes);
    if (success)
        m_floatValue = value;

    return success;
}

float FloatMapping::getFloatValue() const
{
    return m_floatValue;
}
}    // namespace wolkabout
