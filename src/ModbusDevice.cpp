//
// Created by Nexyy on 17/03/2020.
//

#include "ModbusDevice.h"

namespace wolkabout
{
ModbusDevice::ModbusDevice(const std::string& name, int8_t slaveAddress)
: m_name(name), m_slaveAddress(slaveAddress), m_groups(), m_mappings()
{
}

void ModbusDevice::addGroup(const std::shared_ptr<RegisterGroup>& group)
{
    const auto copyGroup = std::make_shared<RegisterGroup>(*group);
    copyGroup->setSlaveAddress(m_slaveAddress);
    m_groups.emplace_back(copyGroup);

    for (const auto& mapping : copyGroup->getMappings())
    {
        m_mappings.emplace(mapping.first, mapping.second);
    }
}

const std::string& ModbusDevice::getName() const
{
    return m_name;
}

int8_t ModbusDevice::getSlaveAddress() const
{
    return m_slaveAddress;
}

const std::vector<std::shared_ptr<RegisterGroup>>& ModbusDevice::getGroups() const
{
    return m_groups;
}

const std::map<std::string, std::shared_ptr<RegisterMapping>>& ModbusDevice::getMappings() const
{
    return m_mappings;
}
}    // namespace wolkabout