#include "more_modbus/modbus/ModbusMappingReader.h"

#include "core/utilities/Logger.h"
#include "more_modbus/ModbusDevice.h"
#include "more_modbus/RegisterGroup.h"

using namespace wolkabout;
using namespace wolkabout::legacy;

namespace wolkabout::more_modbus
{
bool ModbusMappingReader::readRegister(ModbusClient& modbusClient, RegisterMapping& registerMapping)
{
    if (registerMapping.isReadRestricted())
        return true;

    switch (registerMapping.getRegisterType())
    {
    case RegisterType::COIL:
        return readCoil(modbusClient, registerMapping);
    case RegisterType::INPUT_CONTACT:
        return readInputContact(modbusClient, registerMapping);
    case RegisterType::HOLDING_REGISTER:
        return readHoldingRegister(modbusClient, registerMapping);
    case RegisterType::INPUT_REGISTER:
        return readInputRegister(modbusClient, registerMapping);
    default:
        return false;
    }
}

bool ModbusMappingReader::readCoil(ModbusClient& client, RegisterMapping& mapping)
{
    auto value = false;
    if (!client.readCoil(mapping.getSlaveAddress(), mapping.getAddress(), value))
    {
        LOG(ERROR) << "ModbusMappingReader: Failed to read coil mapping - Modbus error.";
        return false;
    }

    if (mapping.doesUpdate(value))
    {
        if (!mapping.update(value))
        {
            LOG(ERROR) << "ModbusMappingReader: Failed to update coil mapping.";
            return false;
        }

        if (auto group = mapping.getGroup().lock())
            if (auto device = group->getDevice().lock())
                device->triggerOnMappingValueChange(mapping.shared_from_this(), value);
    }
    return true;
}

bool ModbusMappingReader::readInputContact(ModbusClient& client, RegisterMapping& mapping)
{
    auto values = std::vector<bool>{};
    if (!client.readInputContacts(mapping.getSlaveAddress(), mapping.getAddress(), 1, values) || values.empty())
    {
        LOG(ERROR) << "ModbusMappingReader: Failed to read input contact mapping - Modbus error.";
        return false;
    }

    const auto value = values.front();
    if (mapping.doesUpdate(value))
    {
        if (!mapping.update(value))
        {
            LOG(ERROR) << "ModbusMappingReader: Failed to update input contact mapping.";
            return false;
        }

        if (auto group = mapping.getGroup().lock())
            if (auto device = group->getDevice().lock())
                device->triggerOnMappingValueChange(mapping.shared_from_this(), value);
    }
    return true;
}

bool ModbusMappingReader::readHoldingRegister(ModbusClient& client, RegisterMapping& mapping)
{
    auto value = std::vector<std::uint16_t>{};
    if (!client.readHoldingRegisters(mapping.getSlaveAddress(), mapping.getAddress(), mapping.getRegisterCount(),
                                     value) ||
        value.empty())
    {
        LOG(ERROR) << "ModbusMappingReader: Failed to read holding register mapping - Modbus error.";
        return false;
    }

    if (mapping.doesUpdate(value))
    {
        if (!mapping.update(value))
        {
            LOG(ERROR) << "ModbusMappingReader: Failed to update holding register mapping.";
            return false;
        }

        if (auto group = mapping.getGroup().lock())
            if (auto device = group->getDevice().lock())
                device->triggerOnMappingValueChange(mapping.shared_from_this(), value);
    }
    return true;
}

bool ModbusMappingReader::readInputRegister(ModbusClient& client, RegisterMapping& mapping)
{
    auto value = std::vector<std::uint16_t>{};
    if (!client.readInputRegisters(mapping.getSlaveAddress(), mapping.getAddress(), mapping.getRegisterCount(),
                                   value) ||
        value.empty())
    {
        LOG(ERROR) << "ModbusMappingReader: Failed to read holding register mapping - Modbus error.";
        return false;
    }

    if (mapping.doesUpdate(value))
    {
        if (!mapping.update(value))
        {
            LOG(ERROR) << "ModbusMappingReader: Failed to update holding register mapping.";
            return false;
        }

        if (auto group = mapping.getGroup().lock())
            if (auto device = group->getDevice().lock())
                device->triggerOnMappingValueChange(mapping.shared_from_this(), value);
    }
    return true;
}
}    // namespace wolkabout::more_modbus
