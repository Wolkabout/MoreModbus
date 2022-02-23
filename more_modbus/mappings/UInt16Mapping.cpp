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

#include "more_modbus/mappings/UInt16Mapping.h"

#include <stdexcept>

namespace wolkabout
{
namespace more_modbus
{
UInt16Mapping::UInt16Mapping(const std::string& reference, RegisterType registerType, int32_t address,
                             bool readRestricted, int16_t slaveAddress, double deadbandValue,
                             std::chrono::milliseconds frequencyFilterValue, std::chrono::milliseconds repeatedWrite,
                             const std::uint16_t* defaultValue)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress, deadbandValue, frequencyFilterValue,
                  repeatedWrite)
{
    if (!(registerType == RegisterType::INPUT_REGISTER || registerType == RegisterType::HOLDING_REGISTER))
    {
        throw std::logic_error("UInt16Mapping: Illegal register type set.");
    }
    if (repeatedWrite.count() > 0 && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("UInt16Mapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("UInt16Mapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
    {
        m_uint16Value = *defaultValue;
        m_byteValues = {*defaultValue};
        m_defaultValue = std::to_string(m_uint16Value);
    }
}

bool UInt16Mapping::update(const std::vector<uint16_t>& newValues)
{
    m_uint16Value = newValues[0];
    return RegisterMapping::update(newValues);
}

bool UInt16Mapping::writeValue(uint16_t value)
{
    if (getGroup().expired())
        return false;
    const auto group = getGroup().lock();
    if (group == nullptr || group->getDevice().expired())
        return false;
    const auto device = group->getDevice().lock();
    if (device == nullptr || device->getReader().expired())
        return false;
    const auto reader = device->getReader().lock();
    if (reader == nullptr)
        return false;

    std::vector<uint16_t> bytes;
    bytes.emplace_back(value);

    bool success = reader->writeMapping(*this, bytes);
    if (success)
        m_uint16Value = value;

    return success;
}

uint16_t UInt16Mapping::getUint16Value() const
{
    return m_uint16Value;
}
}    // namespace more_modbus
}    // namespace wolkabout
