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

#include "more_modbus/RegisterMapping.h"

#include "more_modbus/ModbusDevice.h"
#include "more_modbus/RegisterGroup.h"
#include "more_modbus/utilities/DataParsers.h"

#include <chrono>
#include <stdexcept>
#include <utility>

namespace wolkabout
{
namespace more_modbus
{
RegisterMapping::RegisterMapping(std::string reference, RegisterMapping::RegisterType registerType, int32_t address,
                                 bool readRestricted, int16_t slaveAddress, double deadbandValue,
                                 std::chrono::milliseconds frequencyFilterValue,
                                 std::chrono::milliseconds repeatedWrite)
: m_reference(std::move(reference))
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_operationType(OperationType::NONE)
, m_byteValues(1)
, m_repeatedWrite(repeatedWrite)
, m_deadbandValue(deadbandValue)
, m_frequencyFilterValue(frequencyFilterValue)
{
    if (readRestricted && (static_cast<uint16_t>(registerType) % 2 == 1))
    {
        throw std::logic_error("RegisterMapping: Absurd mapping, can\'t be readRestricted and read-only.");
    }

    // On default, assign default OutputType for type of register.
    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        m_outputType = OutputType::UINT16;
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        m_outputType = OutputType::BOOL;
        break;
    }
}

RegisterMapping::RegisterMapping(std::string reference, RegisterMapping::RegisterType registerType, int32_t address,
                                 OutputType type, bool readRestricted, int16_t slaveAddress, double deadbandValue,
                                 std::chrono::milliseconds frequencyFilterValue,
                                 std::chrono::milliseconds repeatedWrite)
: m_reference(std::move(reference))
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_outputType(type)
, m_operationType(OperationType::NONE)
, m_byteValues(1)
, m_repeatedWrite(repeatedWrite)
, m_deadbandValue(deadbandValue)
, m_frequencyFilterValue(frequencyFilterValue)
{
    if (readRestricted && (static_cast<uint16_t>(registerType) % 2 == 1))
    {
        throw std::logic_error("RegisterMapping: Absurd mapping, can\'t be readRestricted and read-only.");
    }

    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        // Allow Registers to be INT16, UINT16.
        if (m_outputType != OutputType::INT16 && m_outputType != OutputType::UINT16)
        {
            throw std::logic_error(
              "RegisterMapping: Single address register mapping can\'t be anything else than INT16, UINT16.");
        }
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        if (m_outputType != OutputType::BOOL)
        {
            throw std::logic_error(
              "RegisterMapping: Single address discrete register can\'t be anything else than BOOL.");
        }
    }
}

RegisterMapping::RegisterMapping(std::string reference, RegisterMapping::RegisterType registerType, int32_t address,
                                 OperationType type, int8_t bitIndex, bool readRestricted, int16_t slaveAddress,
                                 std::chrono::milliseconds frequencyFilterValue,
                                 std::chrono::milliseconds repeatedWrite)
: m_reference(std::move(reference))
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_outputType(OutputType::BOOL)
, m_operationType(OperationType::TAKE_BIT)
, m_bitIndex(bitIndex)
, m_repeatedWrite(repeatedWrite)
, m_frequencyFilterValue(frequencyFilterValue)
{
    if (readRestricted && (static_cast<uint16_t>(registerType) % 2 == 1))
    {
        throw std::logic_error("RegisterMapping: Absurd mapping, can\'t be readRestricted and read-only.");
    }

    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("RegisterMapping: Take bit can\'t be done over COIL/INPUT_CONTACT.");
    }
}

RegisterMapping::RegisterMapping(std::string reference, RegisterType registerType, std::vector<int32_t> addresses,
                                 OutputType type, OperationType operation, bool readRestricted, int16_t slaveAddress,
                                 double deadbandValue, std::chrono::milliseconds frequencyFilterValue,
                                 std::chrono::milliseconds repeatedWrite)
: m_reference(std::move(reference))
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_addresses(std::move(addresses))
, m_slaveAddress(slaveAddress)
, m_outputType(type)
, m_operationType(operation)
, m_repeatedWrite(repeatedWrite)
, m_deadbandValue(deadbandValue)
, m_frequencyFilterValue(frequencyFilterValue)
{
    if (readRestricted && (static_cast<uint16_t>(registerType) % 2 == 1))
    {
        throw std::logic_error("RegisterMapping: Absurd mapping, can\'t be readRestricted and read-only.");
    }

    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("RegisterMapping: Multi register mapping can\'t be COIL or INPUT_CONTACT.");
    }

    if (m_outputType == OutputType::BOOL || m_outputType == OutputType::INT16 || m_outputType == OutputType::UINT16)
    {
        throw std::logic_error("RegisterMapping: Multi register mapping can\'t be BOOL, INT16 or UINT16.");
    }

    // Can be two registers that are being merged into a 32bit, or multiple ones merging into a string.
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN || m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
    {
        if (m_addresses.size() != 2)
        {
            throw std::logic_error("RegisterMapping: Merge operations work only with 2 registers.");
        }

        if (m_outputType != OutputType::INT32 && m_outputType != OutputType::UINT32)
        {
            throw std::logic_error(
              "RegisterMapping: Merge operations (with endians) output 32bit types (INT32, UINT32).");
        }
    }
    else if (m_operationType == OperationType::MERGE_FLOAT)
    {
        if (m_addresses.size() != 2)
        {
            throw std::logic_error("RegisterMapping: Merge operations work only with 2 registers.");
        }

        if (m_outputType != OutputType::FLOAT)
        {
            throw std::logic_error("RegisterMapping: Merge for floats can only output FLOAT.");
        }
    }
    else if (m_operationType == OperationType::STRINGIFY_ASCII || m_operationType == OperationType::STRINGIFY_UNICODE)
    {
        if (m_outputType != OutputType::STRING)
        {
            throw std::logic_error("RegisterMapping: Stringify can only return string.");
        }
    }
    else
    {
        throw std::logic_error("RegisterMapping: You can\'t have an multiple register mapping do that!");
    }

    m_byteValues = std::vector<uint16_t>(m_addresses.size());
}

const std::string& RegisterMapping::getReference() const
{
    return m_reference;
}

bool RegisterMapping::isReadRestricted() const
{
    return m_readRestricted;
}

RegisterMapping::RegisterType RegisterMapping::getRegisterType() const
{
    return m_registerType;
}

int32_t RegisterMapping::getAddress() const
{
    return m_address;
}

const std::vector<int32_t>& RegisterMapping::getAddresses() const
{
    return m_addresses;
}

int16_t RegisterMapping::getSlaveAddress() const
{
    return m_slaveAddress;
}

void RegisterMapping::setSlaveAddress(int16_t slaveAddress)
{
    m_slaveAddress = slaveAddress;
}

int32_t RegisterMapping::getStartingAddress() const
{
    if (m_address == -1)
    {
        return m_addresses.at(0);
    }
    return m_address;
}

int16_t RegisterMapping::getRegisterCount() const
{
    if (m_address == -1)
    {
        return static_cast<int16_t>(m_addresses.size());
    }
    return 1;
}

RegisterMapping::OutputType RegisterMapping::getOutputType() const
{
    return m_outputType;
}

RegisterMapping::OperationType RegisterMapping::getOperationType() const
{
    return m_operationType;
}

int8_t RegisterMapping::getBitIndex() const
{
    return m_bitIndex;
}

bool RegisterMapping::doesUpdate(const std::vector<uint16_t>& newValues) const
{
    if (newValues.size() != m_byteValues.size())
    {
        throw std::logic_error("RegisterMapping: The value array has to be the same size, it cannot change.");
    }

    if (!m_isInitialized || !m_isValid)
    {
        return true;    // initial value
    }

    bool different = false;
    uint32_t i = 0;
    while (i < newValues.size())
    {
        if (m_byteValues[i] != newValues[i])
        {
            different = true;
            break;
        }
        ++i;
    }

    if (m_frequencyFilterValue == std::chrono::milliseconds(0) && m_deadbandValue == 0.0)
    {
        return different;    // no data filtering
    }

    if (!different)
    {
        return false;
    }

    if (m_frequencyFilterValue != std::chrono::milliseconds(0))
    {
        bool frequentUpdate = std::chrono::high_resolution_clock::now() < m_lastUpdateTime + m_frequencyFilterValue;

        if (m_deadbandValue == 0.0)
        {
            return !frequentUpdate;    // freq filter
        }
        else
        {
            if (frequentUpdate)
            {
                return false;    // both filters, but freq filter caught it
            }
        }
    }

    return deadbandFilter(newValues);
}

bool RegisterMapping::update(const std::vector<uint16_t>& newValues)
{
    if (newValues.size() != m_byteValues.size())
    {
        throw std::logic_error("RegisterMapping: The value array has to be the same size, it cannot change.");
    }

    bool different = false;
    uint32_t i = 0;
    while (!different && i < newValues.size())
    {
        if (m_byteValues[i] != newValues[i])
        {
            different = true;
        }
        ++i;
    }
    m_byteValues = newValues;

    bool isValueInitialized = m_isInitialized;
    m_isInitialized = true;

    bool isValid = m_isValid;
    m_isValid = true;

    m_lastUpdateTime = std::chrono::high_resolution_clock::now();

    return !isValueInitialized || different || !isValid;
}

bool RegisterMapping::doesUpdate(bool newRegisterValue) const
{
    if (!m_isInitialized || !m_isValid)
    {
        return true;
    }

    if (m_frequencyFilterValue != std::chrono::milliseconds(0))
    {
        bool frequentUpdate = std::chrono::high_resolution_clock::now() < m_lastUpdateTime + m_frequencyFilterValue;

        return !m_isInitialized || !frequentUpdate;
    }

    return m_boolValue != newRegisterValue || !m_isInitialized || !m_isValid;
}

bool RegisterMapping::update(bool newRegisterValue)
{
    bool different = m_boolValue != newRegisterValue;
    m_boolValue = newRegisterValue;

    bool isValueInitialized = m_isInitialized;
    m_isInitialized = true;

    bool isValid = m_isValid;
    m_isValid = true;

    m_lastUpdateTime = std::chrono::high_resolution_clock::now();

    return !isValueInitialized || different || !isValid;
}

const std::vector<uint16_t>& RegisterMapping::getBytesValues() const
{
    return m_byteValues;
}

bool RegisterMapping::getBoolValue() const
{
    return m_boolValue;
}

bool RegisterMapping::isInitialized() const
{
    return m_isInitialized;
}

bool RegisterMapping::isValid() const
{
    return m_isValid;
}

void RegisterMapping::setValid(bool valid)
{
    m_isValid = valid;
}

std::weak_ptr<RegisterGroup> RegisterMapping::getGroup() const
{
    return m_group;
}

void RegisterMapping::setGroup(const std::shared_ptr<RegisterGroup>& group)
{
    m_group = group;
}

const std::chrono::milliseconds& RegisterMapping::getRepeatedWrite() const
{
    return m_repeatedWrite;
}

void RegisterMapping::setRepeatedWrite(const std::chrono::milliseconds& repeatedWrite)
{
    // Check if the repeated write needs to be added now into the device's list
    if ((m_repeatedWrite.count() == 0 && repeatedWrite.count() > 0) ||
        (m_repeatedWrite.count() > 0 && repeatedWrite.count() == 0))
    {
        if (auto group = m_group.lock())
        {
            if (auto device = group->getDevice().lock())
            {
                if (m_repeatedWrite.count() == 0 && repeatedWrite.count() > 0)
                    device->addRewritable(shared_from_this());
                else if (m_repeatedWrite.count() > 0 && repeatedWrite.count() == 0)
                    device->removeRewritable(shared_from_this());
            }
        }
    }

    m_repeatedWrite = repeatedWrite;
}

const std::string& RegisterMapping::getDefaultValue() const
{
    return m_defaultValue;
}

const std::chrono::high_resolution_clock::time_point& RegisterMapping::getLastUpdateTime() const
{
    return m_lastUpdateTime;
}

bool RegisterMapping::deadbandFilter(const std::vector<uint16_t>& newValues) const
{
    bool significantChange = false;

    switch (getOutputType())
    {
    case OutputType::UINT16:
    {
        uint16_t currentValueUint16 = m_byteValues[0];
        uint16_t newValueUint16 = newValues[0];

        significantChange = newValueUint16 >= currentValueUint16 + m_deadbandValue ||
                            newValueUint16 <= currentValueUint16 - m_deadbandValue;
    }
    break;

    case OutputType::INT16:
    {
        int16_t currentValueInt16 = DataParsers::uint16ToInt16(m_byteValues[0]);
        int16_t newValueInt16 = DataParsers::uint16ToInt16(newValues[0]);

        significantChange =
          newValueInt16 >= currentValueInt16 + m_deadbandValue || newValueInt16 <= currentValueInt16 - m_deadbandValue;
    }
    break;

    case OutputType::UINT32:
    {
        uint32_t currentValueUint32;
        uint32_t newValueUint32;
        if (getOperationType() == OperationType::MERGE_BIG_ENDIAN)
        {
            currentValueUint32 = DataParsers::registersToUint32(m_byteValues, DataParsers::Endian::BIG);
            newValueUint32 = DataParsers::registersToUint32(newValues, DataParsers::Endian::BIG);
        }
        else
        {
            currentValueUint32 = DataParsers::registersToUint32(m_byteValues, DataParsers::Endian::LITTLE);
            newValueUint32 = DataParsers::registersToUint32(newValues, DataParsers::Endian::LITTLE);
        }

        significantChange = newValueUint32 >= currentValueUint32 + m_deadbandValue ||
                            newValueUint32 <= currentValueUint32 - m_deadbandValue;
    }
    break;

    case OutputType::INT32:
    {
        int32_t currentValueInt32;
        int32_t newValueInt32;
        if (getOperationType() == OperationType::MERGE_BIG_ENDIAN)
        {
            currentValueInt32 = DataParsers::registersToInt32(m_byteValues, DataParsers::Endian::BIG);
            newValueInt32 = DataParsers::registersToInt32(newValues, DataParsers::Endian::BIG);
        }
        else
        {
            currentValueInt32 = DataParsers::registersToInt32(m_byteValues, DataParsers::Endian::LITTLE);
            newValueInt32 = DataParsers::registersToInt32(newValues, DataParsers::Endian::LITTLE);
        }

        significantChange =
          newValueInt32 >= currentValueInt32 + m_deadbandValue || newValueInt32 <= currentValueInt32 - m_deadbandValue;
    }
    break;

    case OutputType::FLOAT:
    {
        float currentValueFloat = DataParsers::registersToFloat(m_byteValues);
        float newValueFloat = DataParsers::registersToFloat(newValues);

        significantChange =
          newValueFloat >= currentValueFloat + m_deadbandValue || newValueFloat <= currentValueFloat - m_deadbandValue;
    }
    break;
    }

    return significantChange;
}
}    // namespace more_modbus
}    // namespace wolkabout
