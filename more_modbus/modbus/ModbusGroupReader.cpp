/**
 * Copyright 2021 Wolkabout Technology s.r.o.
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

#include "more_modbus/modbus/ModbusGroupReader.h"

#include "core/utilities/Logger.h"
#include "more_modbus/ModbusReader.h"
#include "more_modbus/utilities/DataParsers.h"

using namespace wolkabout::legacy;

namespace wolkabout::more_modbus
{
bool ModbusGroupReader::readGroup(ModbusClient& modbusClient, RegisterGroup& group)
{
    if (group.isReadRestricted())
        return true;

    switch (group.getRegisterType())
    {
    case RegisterType::COIL:
        return readCoilGroup(modbusClient, group);
    case RegisterType::INPUT_CONTACT:
        return readDiscreteInputGroup(modbusClient, group);
    case RegisterType::INPUT_REGISTER:
        return readInputRegisterGroup(modbusClient, group);
    case RegisterType::HOLDING_REGISTER:
        return readHoldingRegisterGroup(modbusClient, group);
    default:
        return false;
    }
}

bool ModbusGroupReader::readCoilGroup(ModbusClient& modbusClient, RegisterGroup& group)
{
    std::vector<bool> boolValues;
    if (!modbusClient.readCoils(group.getSlaveAddress(), group.getStartingAddress(), group.getAddressCount(),
                                boolValues))
    {
        LOG(WARN) << "ModbusGroupReader: Unable to read coil group on device " << group.getSlaveAddress()
                  << ", starting on " << group.getStartingAddress() << " counting " << group.getAddressCount()
                  << " addresses.";
        return false;
    }

    passValuesToGroup(group, boolValues);
    return true;
}

bool ModbusGroupReader::readDiscreteInputGroup(ModbusClient& modbusClient, RegisterGroup& group)
{
    std::vector<bool> boolValues;
    if (!modbusClient.readInputContacts(group.getSlaveAddress(), group.getStartingAddress(), group.getAddressCount(),
                                        boolValues))
    {
        LOG(WARN) << "ModbusGroupReader: Unable to read discrete input group on device " << group.getSlaveAddress()
                  << ", starting on " << group.getStartingAddress() << " counting " << group.getAddressCount()
                  << " addresses.";
        return false;
    }

    passValuesToGroup(group, boolValues);
    return true;
}

bool ModbusGroupReader::readHoldingRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group)
{
    std::vector<uint16_t> registerValues;
    if (!modbusClient.readHoldingRegisters(group.getSlaveAddress(), group.getStartingAddress(), group.getAddressCount(),
                                           registerValues))
    {
        LOG(WARN) << "ModbusGroupReader: Unable to read holding register group on device " << group.getSlaveAddress()
                  << ", starting on " << group.getStartingAddress() << " counting " << group.getAddressCount()
                  << " addresses.";
        return false;
    }

    passValuesToGroup(group, registerValues);
    return true;
}

bool ModbusGroupReader::readInputRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group)
{
    std::vector<uint16_t> registerValues;
    if (!modbusClient.readInputRegisters(group.getSlaveAddress(), group.getStartingAddress(), group.getAddressCount(),
                                         registerValues))
    {
        LOG(WARN) << "ModbusGroupReader: Unable to read input register group on device " << group.getSlaveAddress()
                  << ", starting on " << group.getStartingAddress() << " counting " << group.getAddressCount()
                  << " addresses.";
        return false;
    }

    passValuesToGroup(group, registerValues);
    return true;
}

void ModbusGroupReader::passValuesToGroup(RegisterGroup& group, const std::vector<bool>& values)
{
    uint32_t i = 0;
    const auto mappingMap = group.getMappingsMap();
    for (const auto& mapping : mappingMap)
    {
        bool newValue = values[i++];
        if (mapping.second->doesUpdate(newValue))
        {
            mapping.second->update(newValue);
            if (auto device = group.getDevice().lock())
                device->triggerOnMappingValueChange(mapping.second, newValue);
            LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '" << mapping.second->getReference()
                      << "' Value: '" << mapping.second->getBoolValue() << "'";
        }
    }
}

void ModbusGroupReader::passValuesToGroup(RegisterGroup& group, const std::vector<uint16_t>& values)
{
    const auto claims = group.getMappingsClaims();
    const auto& mappingsSet = group.getMappings();
    const auto& mappings =
      std::vector<std::pair<std::string, std::shared_ptr<RegisterMapping>>>(mappingsSet.begin(), mappingsSet.end());

    uint32_t skipMappings = 0, valueCounter = 0, mappingCounter = 0;
    for (const auto& mapping : mappings)
    {
        if (skipMappings > 0)
        {
            ++mappingCounter;
            --skipMappings;
            continue;
        }

        const auto address = GroupUtility::getAddressFromString(mapping.first);
        if (mapping.first.find(GroupUtility::SEPARATOR) != std::string::npos)
        {
            const auto& bits = DataParsers::separateBits(values[valueCounter++]);
            uint32_t shift = 0;
            while (mappingCounter + shift <= group.getMappings().size() - 1 &&
                   address == GroupUtility::getAddressFromString(claims[mappingCounter + shift]))
            {
                if (shift > 0)
                    skipMappings++;

                const auto bitIndex = GroupUtility::getBitFromString(claims[mappingCounter + shift]);
                const auto bitValue = bits[static_cast<uint16_t>(bitIndex)];

                const auto& bitMapping = mappings.at(mappingCounter + shift);

                if (bitMapping.second->doesUpdate(bitValue))
                {
                    bitMapping.second->update(bitValue);
                    if (auto device = group.getDevice().lock())
                        device->triggerOnMappingValueChange(bitMapping.second, bitValue);
                    LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '"
                              << bitMapping.second->getReference() << "' Value: '" << bitValue << "'";
                }
                ++shift;
            }
        }
        else
        {
            // If the mapping has multiple values, they will get added now, and their loop iterations
            // will be skipped.
            std::vector<uint16_t> data;
            std::string loggingString;
            for (int i = 0; i < mapping.second->getRegisterCount(); i++)
            {
                if (i > 0)
                    skipMappings++;

                loggingString.append(std::to_string(values[valueCounter]) + " ");
                data.emplace_back(values[valueCounter++]);
            }

            if (mapping.second->doesUpdate(data))
            {
                mapping.second->update(data);
                if (auto device = group.getDevice().lock())
                    device->triggerOnMappingValueChange(mapping.second, data);
                LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '" << mapping.second->getReference()
                          << "' Values: " << loggingString;
            }
        }

        ++mappingCounter;
    }
}
}    // namespace wolkabout::more_modbus
