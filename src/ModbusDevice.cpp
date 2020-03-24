//
// Created by Nexyy on 17/03/2020.
//

#include "ModbusDevice.h"

#include <algorithm>

namespace wolkabout
{
ModbusDevice::ModbusDevice(const std::string& name, int8_t slaveAddress,
                           const std::vector<std::shared_ptr<RegisterMapping>>& mappings)
: m_name(name), m_slaveAddress(slaveAddress), m_groups()
{
    std::map<RegisterMapping::RegisterType, std::shared_ptr<RegisterGroup>> readRestrictedGroups;
    std::vector<std::shared_ptr<RegisterMapping>> copyMappings(mappings);

    for (const auto& mapping : copyMappings)
    {
        if (mapping->isReadRestricted())
        {
            // Add the mapping to the readRestricted group for specific type.
            if (readRestrictedGroups[mapping->getRegisterType()] == nullptr)
            {
                readRestrictedGroups[mapping->getRegisterType()] = std::make_shared<RegisterGroup>(mapping);
                m_groups.insert(m_groups.end(), readRestrictedGroups[mapping->getRegisterType()]);
            }
            else
            {
                readRestrictedGroups[mapping->getRegisterType()]->addMapping(mapping);
            }
        }
        else
        {
            const auto group = std::make_shared<RegisterGroup>(mapping);
            group->setSlaveAddress(slaveAddress);
            m_groups.insert(m_groups.end(), group);
        }
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

void ModbusDevice::setOnMappingValueChange(const std::function<void(const RegisterMapping&)>& onMappingValueChange)
{
    m_onMappingValueChange = onMappingValueChange;
}

void ModbusDevice::triggerOnMappingValueChange(const RegisterMapping& mapping)
{
    m_onMappingValueChange(mapping);
}
}    // namespace wolkabout