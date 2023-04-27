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

#include "more_modbus/mappings/FloatMapping.h"

#include "more_modbus/utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
namespace more_modbus
{
FloatMapping::FloatMapping(const std::string& reference, RegisterType registerType,
                           const std::vector<int32_t>& addresses, bool readRestricted, int16_t slaveAddress,
                           double deadbandValue, std::chrono::milliseconds frequencyFilterValue,
                           std::chrono::milliseconds repeatedWrite, const float* defaultValue)
: RegisterMapping(reference, registerType, addresses, OutputType::FLOAT, OperationType::MERGE_FLOAT_BIG_ENDIAN, readRestricted,
                  slaveAddress, deadbandValue, frequencyFilterValue, repeatedWrite)
{
    if (repeatedWrite.count() > 0 && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("FloatMapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("FloatMapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
    {
        m_floatValue = *defaultValue;
        auto endianness = m_operationType == OperationType::MERGE_FLOAT_BIG_ENDIAN? DataParsers::Endian::BIG : DataParsers::Endian::LITTLE;
        m_byteValues = DataParsers::floatToRegisters(m_floatValue, endianness);
        m_defaultValue = std::to_string(m_floatValue);
    }
}

FloatMapping::FloatMapping(const std::string& reference, RegisterType registerType,
                           const std::vector<int32_t>& addresses, OperationType operation, bool readRestricted,
                           int16_t slaveAddress, double deadbandValue, std::chrono::milliseconds frequencyFilterValue,
                           std::chrono::milliseconds repeatedWrite, const float* defaultValue)
: RegisterMapping(reference, registerType, addresses, OutputType::FLOAT, operation, readRestricted,
                  slaveAddress, deadbandValue, frequencyFilterValue, repeatedWrite)
{
    if (repeatedWrite.count() > 0 && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("FloatMapping: Can not set a repeated write value for a read-only register.");
    }
    if (defaultValue != nullptr && registerType == RegisterType::INPUT_REGISTER)
    {
        throw std::logic_error("FloatMapping: Can not set a default value for a read-only register.");
    }

    if (defaultValue != nullptr)
    {
        m_floatValue = *defaultValue;
        auto endianness = m_operationType == OperationType::MERGE_FLOAT_BIG_ENDIAN? DataParsers::Endian::BIG : DataParsers::Endian::LITTLE;
        m_byteValues = DataParsers::floatToRegisters(m_floatValue, endianness);
        m_defaultValue = std::to_string(m_floatValue);
    }
}

bool FloatMapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
        case OperationType::MERGE_FLOAT_BIG_ENDIAN:
        {
            m_floatValue = DataParsers::registersToFloat(newValues, DataParsers::Endian::BIG);
            break;
        }
        case OperationType::MERGE_FLOAT_LITTLE_ENDIAN:
        {
            m_floatValue = DataParsers::registersToFloat(newValues, DataParsers::Endian::LITTLE);
            break;
        }
        default:
            throw std::logic_error("FloatMapping: Illegal operation type set.");
    }
    return RegisterMapping::update(newValues);
}

bool FloatMapping::writeValue(float value)
{
    std::vector<uint16_t> bytes;

    if (m_operationType != OperationType::MERGE_FLOAT_BIG_ENDIAN && m_operationType != OperationType::MERGE_FLOAT_LITTLE_ENDIAN)
        throw std::logic_error("FloatMapping: Illegal operation type set.");

    if (m_operationType == OperationType::MERGE_FLOAT_BIG_ENDIAN)
    {
        bytes = DataParsers::floatToRegisters(value, DataParsers::Endian::BIG);
    }
    else
    {
        bytes = DataParsers::floatToRegisters(value, DataParsers::Endian::LITTLE);
    }

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
        m_floatValue = value;

    return success;
}

float FloatMapping::getValue() const
{
    return m_floatValue;
}
}    // namespace more_modbus
}    // namespace wolkabout
