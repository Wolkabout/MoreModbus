//
// Created by Nexyy on 17/03/2020.
//

#include "ModbusDevice.h"
#include "utility/Logger.h"

#include <algorithm>
#include <set>

namespace wolkabout
{
ModbusDevice::ModbusDevice(const std::string& name, int8_t slaveAddress,
                           const std::vector<std::shared_ptr<RegisterMapping>>& mappings)
: m_name(name), m_slaveAddress(slaveAddress), m_groups()
{
    std::map<RegisterMapping::RegisterType, std::shared_ptr<RegisterGroup>> readRestrictedGroups;
    std::set<std::shared_ptr<RegisterMapping>, CompareFunction> set(mappings.begin(), mappings.end(), &ModbusDevice::compareMappings);

    std::shared_ptr<RegisterGroup> previousGroup = nullptr;
    for (const auto& mapping : set)
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
            if (previousGroup == nullptr)
            {
                previousGroup = std::make_shared<RegisterGroup>(mapping);
                previousGroup->setSlaveAddress(slaveAddress);
                m_groups.insert(m_groups.end(), previousGroup);
                continue;
            }

            if (previousGroup->getRegisterType() == mapping->getRegisterType())
            {
                if (previousGroup->addMapping(mapping))
                {
                    continue;
                }
            }

            previousGroup = std::make_shared<RegisterGroup>(mapping);
            previousGroup->setSlaveAddress(slaveAddress);
            m_groups.insert(m_groups.end(), previousGroup);
        }
    }

    LOG(DEBUG) << "ModbusDevice: Created " << m_groups.size() << " groups for device " << m_name << ".";
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

bool ModbusDevice::compareMappings(const std::shared_ptr<RegisterMapping>& left,
                                   const std::shared_ptr<RegisterMapping>& right)
{
    const auto typeDiff = (int)right->getRegisterType() - (int)left->getRegisterType();
    if (typeDiff != 0)
        return typeDiff > 0;

    const auto startDiff = right->getStartingAddress() - left->getStartingAddress();
    if (startDiff != 0)
        return startDiff > 0;

    const auto lengthDiff = right->getRegisterCount() - left->getRegisterCount();
    if (lengthDiff != 0)
        return lengthDiff > 0;

    const auto outputDiff = (int)right->getOutputType() - (int)left->getOutputType();
    if (outputDiff != 0)
        return outputDiff > 0;

    return left->getBitIndex() < right->getBitIndex();
}
}    // namespace wolkabout