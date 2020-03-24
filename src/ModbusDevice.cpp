//
// Created by Nexyy on 17/03/2020.
//

#include "ModbusDevice.h"
#include "utility/Logger.h"

#include <algorithm>

namespace wolkabout
{
ModbusDevice::ModbusDevice(const std::string& name, int8_t slaveAddress,
                           const std::vector<std::shared_ptr<RegisterMapping>>& mappings)
: m_name(name), m_slaveAddress(slaveAddress), m_groups(), m_mappings()
{
    for (const auto& mapping : mappings)
    {
        const auto group = std::make_shared<RegisterGroup>(mapping);
        group->setSlaveAddress(slaveAddress);
        m_groups.insert(m_groups.end(), group);

        for (const auto& groupMapping : group->getMappingsMap())
        {
            m_mappings.emplace(groupMapping.first, groupMapping.second);
        }
    }

    // Figure out groups from mappings.
    //    std::sort(mappings.begin(), mappings.end(),
    //              [](const std::shared_ptr<RegisterMapping>& first, const std::shared_ptr<RegisterMapping>& second) {
    //                  if (first->getRegisterType() != second->getRegisterType())
    //                      return first->getRegisterType() < second->getRegisterType();
    //                  if (first->isReadRestricted())
    //                      return true;
    //                  return first->getStartingAddress() < second->getStartingAddress();
    //              });
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

void ModbusDevice::setOnMappingValueChange(const std::function<void(const RegisterMapping&)>& onMappingValueChange)
{
    m_onMappingValueChange = onMappingValueChange;
}

void ModbusDevice::triggerOnMappingValueChange(const RegisterMapping& mapping)
{
    m_onMappingValueChange(mapping);
}
}    // namespace wolkabout