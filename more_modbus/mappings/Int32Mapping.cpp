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

#include "more_modbus/mappings/Int32Mapping.h"

#include "more_modbus/utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
namespace more_modbus
{
Int32Mapping::Int32Mapping(const std::string& reference, RegisterType registerType,
                           const std::vector<int32_t>& addresses, OperationType operation, bool readRestricted,
                           int16_t slaveAddress, double deadbandValue, std::chrono::milliseconds frequencyFilterValue,
                           std::chrono::milliseconds repeatedWrite, const std::int32_t* defaultValue)
: RegisterMapping(reference, registerType, addresses, OutputType::INT32, operation, readRestricted, slaveAddress,
                  deadbandValue, frequencyFilterValue, repeatedWrite)
{
    if (operation != OperationType::MERGE_BIG_ENDIAN && operation != OperationType::MERGE_LITTLE_ENDIAN)
    {
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }
    if (repeatedWrite.count() > 0 && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("Int32Mapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("Int32Mapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
    {
        m_int32Value = *defaultValue;
        m_byteValues = DataParsers::int32ToRegisters(m_int32Value, operation == OperationType::MERGE_BIG_ENDIAN ?
                                                                     DataParsers::Endian::BIG :
                                                                     DataParsers::Endian::LITTLE);
        m_defaultValue = std::to_string(m_int32Value);
    }
}

bool Int32Mapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::MERGE_BIG_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::MERGE_LITTLE_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::LITTLE);
        break;
    default:
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool Int32Mapping::writeValue(int32_t value)
{
    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::BIG);
    else if (m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::LITTLE);
    else
        throw std::logic_error("Int32Mapping: Illegal operation type set.");

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

    bool success = reader->writeMapping(*this, bytes);
    if (success)
        m_int32Value = value;

    return success;
}

int32_t Int32Mapping::getInt32Value() const
{
    return m_int32Value;
}
}    // namespace more_modbus
}    // namespace wolkabout
