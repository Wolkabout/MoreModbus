/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
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

#include "more_modbus/mappings/Int16Mapping.h"

#include "more_modbus/utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
Int16Mapping::Int16Mapping(const std::string& reference, RegisterMapping::RegisterType registerType, int32_t address,
                           bool readRestricted, int16_t slaveAddress, double deadbandValue,
                           std::chrono::milliseconds frequencyFilterValue, std::chrono::milliseconds repeatedWrite,
                           const std::int16_t* defaultValue)
: RegisterMapping(reference, registerType, address, OutputType::INT16, readRestricted, slaveAddress, deadbandValue,
                  frequencyFilterValue, repeatedWrite)
{
    if (defaultValue != nullptr)
    {
        m_int16Value = *defaultValue;
        m_byteValues = {DataParsers::int16ToUint16(m_int16Value)};
    }
}

bool Int16Mapping::update(const std::vector<uint16_t>& newValues)
{
    m_int16Value = DataParsers::uint16ToInt16(newValues[0]);
    return RegisterMapping::update(newValues);
}

bool Int16Mapping::writeValue(int16_t value)
{
    if (getGroup()->getDevice()->getReader().expired())
        return false;

    std::vector<uint16_t> bytes;
    bytes.emplace_back(DataParsers::int16ToUint16(value));

    const auto reader = getGroup()->getDevice()->getReader().lock();
    bool success = reader->writeMapping(*this, bytes);
    if (success)
        m_int16Value = value;

    return success;
}

int16_t Int16Mapping::getInt16Value() const
{
    return m_int16Value;
}
}    // namespace wolkabout
