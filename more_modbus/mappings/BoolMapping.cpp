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

#include "more_modbus/mappings/BoolMapping.h"

#include <stdexcept>

namespace wolkabout
{
BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int32_t address,
                         bool readRestricted, int16_t slaveAddress, std::chrono::milliseconds frequencyFilterValue,
                         std::chrono::milliseconds repeatedWrite, const bool* defaultValue)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress, 0.0, frequencyFilterValue,
                  repeatedWrite)
{
    if (!(registerType == RegisterType::COIL || registerType == RegisterType::INPUT_CONTACT))
    {
        throw std::logic_error("BoolMapping: Illegal register type set.");
    }
    if (repeatedWrite.count() > 0 && registerType == RegisterMapping::RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("BoolMapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterMapping::RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("BoolMapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
    {
        m_boolValue = *defaultValue;
        m_defaultValue = m_boolValue ? "true" : "false";
    }
    else
    {
        m_boolValue = false;
    }
}

BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int32_t address,
                         RegisterMapping::OperationType operation, int8_t bitIndex, bool readRestricted,
                         int16_t slaveAddress, std::chrono::milliseconds frequencyFilterValue,
                         std::chrono::milliseconds repeatedWrite, const bool* defaultValue)
: RegisterMapping(reference, registerType, address, operation, bitIndex, readRestricted, slaveAddress,
                  frequencyFilterValue, repeatedWrite)
{
    if (operation != OperationType::TAKE_BIT)
    {
        throw std::logic_error("BoolMapping: Illegal operation type set.");
    }
    if (repeatedWrite.count() > 0 && registerType == RegisterMapping::RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("BoolMapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterMapping::RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("BoolMapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
        m_boolValue = defaultValue;
    else
        m_boolValue = false;
}

bool BoolMapping::writeValue(bool value)
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

    bool success;

    if (m_operationType == OperationType::TAKE_BIT)
        success = reader->writeBitMapping(*this, value);
    else
        success = reader->writeMapping(*this, value);

    if (success)
        m_boolValue = value;

    return success;
}
}    // namespace wolkabout
