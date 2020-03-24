//
// Created by astrihale on 19.3.20..
//

#include "ModbusGroupReader.h"
#include "utility/DataParsers.h"
#include "utility/Logger.h"

namespace wolkabout
{
bool ModbusGroupReader::readGroup(wolkabout::ModbusClient& modbusClient, wolkabout::RegisterGroup& group)
{
    if (group.isReadRestricted())
        return true;

    switch (group.getRegisterType())
    {
    case RegisterMapping::RegisterType::COIL:
        return readCoilGroup(modbusClient, group);
    case RegisterMapping::RegisterType::INPUT_CONTACT:
        return readDiscreteInputGroup(modbusClient, group);
    case RegisterMapping::RegisterType::INPUT_REGISTER:
        return readInputRegisterGroup(modbusClient, group);
    case RegisterMapping::RegisterType::HOLDING_REGISTER:
        return readHoldingRegisterGroup(modbusClient, group);
    default:
        return false;
    }
}

void ModbusGroupReader::passValuesToGroup(RegisterGroup& group, const std::vector<bool>& values)
{
    uint i = 0;
    for (const auto& mapping : group.getMappingsMap())
    {
        bool newValue = values[i++];
        LOG(TRACE) << "ModbusGroupReader: Old value = " << mapping.second->getBoolValue()
                   << ". New value = " << newValue;
        if (mapping.second->update(newValue))
        {
            LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '" << mapping.second->getReference()
                      << "' Value: '" << mapping.second->getBoolValue() << "'";

            // Notify of value change.
            // TODO External
        }
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

void ModbusGroupReader::passValuesToGroup(RegisterGroup& group, const std::vector<uint16_t>& values)
{
    const auto& claims = group.getMappingsClaims();
    const auto& mappings = group.getMappingsMap();

    uint skipMappings = 0, valueCounter = 0, mappingCounter = 0;
    for (const auto& mapping : mappings)
    {
        if (skipMappings > 0)
        {
            ++mappingCounter;
            --skipMappings;
            continue;
        }

        const auto address = RegisterGroup::getAddressFromString(mapping.first);
        if (mapping.first.find(RegisterGroup::SEPARATOR) != std::string::npos)
        {
            const auto& bits = DataParsers::separteBits(values[valueCounter++]);
            uint shift = 0;
            while (address == RegisterGroup::getAddressFromString(claims[mappingCounter + shift]))
            {
                if (shift > 0)
                    skipMappings++;

                const auto bitIndex = RegisterGroup::getBitFromString(claims[mappingCounter + shift]);
                const auto bitValue = bits[static_cast<uint16_t>(bitIndex)];

                const auto& bitMapping = mappings.at(claims[mappingCounter + shift]);
                if (bitMapping->update(bitValue))
                {
                    LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '" << bitMapping->getReference()
                              << "' Value: '" << bitValue << "'";
                    // Notify of value change.
                    // TODO External
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

            if (mapping.second->update(data))
            {
                LOG(INFO) << "ModbusGroupReader: Mapping value changed - Reference: '" << mapping.second->getReference()
                          << "' Values: " << loggingString;
                // Notify of value change.
                // TODO External
            }
        }

        ++mappingCounter;
    }
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
}    // namespace wolkabout
