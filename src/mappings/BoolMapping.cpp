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

#include "BoolMapping.h"

#include <stdexcept>

namespace wolkabout
{
BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::COIL || registerType == RegisterType::INPUT_CONTACT))
    {
        throw std::logic_error("BoolMapping: Illegal register type set.");
    }
}

BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         RegisterMapping::OperationType operation, int8_t bitIndex, bool readRestricted,
                         int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, operation, bitIndex, readRestricted, slaveAddress)
{
    if (!(operation == OperationType::TAKE_BIT))
    {
        throw std::logic_error("BoolMapping: Illegal operation type set.");
    }
}

bool BoolMapping::writeValue(bool value)
{
    bool success;
    if (m_operationType == OperationType::TAKE_BIT)
        success = getGroup()->getDevice()->getReader()->writeBitMapping(*this, value);
    else
        success = getGroup()->getDevice()->getReader()->writeMapping(*this, value);

    if (success)
        m_boolValue = value;

    return success;
}
}    // namespace wolkabout
