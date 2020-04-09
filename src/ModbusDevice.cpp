/*
 * Copyright 2020 WolkAbout Technology s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ModbusDevice.h"
#include "utilities/Logger.h"

#include <algorithm>
#include <set>

namespace wolkabout
{
ModbusDevice::ModbusDevice(const std::string& name, int8_t slaveAddress)
: m_name(name), m_slaveAddress(slaveAddress), m_groups()
{
}

void ModbusDevice::createGroups(const std::vector<std::shared_ptr<RegisterMapping>>& mappings)
{
    std::map<RegisterMapping::RegisterType, std::shared_ptr<RegisterGroup>> readRestrictedGroups;
    std::set<std::shared_ptr<RegisterMapping>, CompareFunction> set(mappings.begin(), mappings.end());

    std::shared_ptr<RegisterGroup> previousGroup = nullptr;
    for (const auto& mapping : set)
    {
        if (mapping->isReadRestricted())
        {
            // Add the mapping to the readRestricted group for specific type.
            if (readRestrictedGroups[mapping->getRegisterType()] == nullptr)
            {
                readRestrictedGroups[mapping->getRegisterType()] =
                  std::make_shared<RegisterGroup>(mapping, shared_from_this());
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
                previousGroup = std::make_shared<RegisterGroup>(mapping, shared_from_this());
                previousGroup->setSlaveAddress(m_slaveAddress);
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

            previousGroup = std::make_shared<RegisterGroup>(mapping, shared_from_this());
            previousGroup->setSlaveAddress(m_slaveAddress);
            m_groups.insert(m_groups.end(), previousGroup);
        }
    }

    LOG(DEBUG) << "ModbusDevice: Created " << m_groups.size() << " groups for device " << m_name << ".";
}

const std::string& ModbusDevice::getName() const
{
    return m_name;
}

bool ModbusDevice::getStatus() const
{
    return m_status;
}

int8_t ModbusDevice::getSlaveAddress() const
{
    return m_slaveAddress;
}

const std::vector<std::shared_ptr<RegisterGroup>>& ModbusDevice::getGroups() const
{
    return m_groups;
}

void ModbusDevice::setOnMappingValueChange(
  const std::function<void(const std::shared_ptr<RegisterMapping>&)>& onMappingValueChange)
{
    m_onMappingValueChange = onMappingValueChange;
}

void ModbusDevice::setOnStatusChange(const std::function<void(bool)>& onStatusChange)
{
    m_onStatusChange = onStatusChange;
}

void ModbusDevice::triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping)
{
    if (m_onMappingValueChange != nullptr)
        m_onMappingValueChange(mapping);
}

void ModbusDevice::triggerOnStatusChange(bool status)
{
    if (m_onStatusChange != nullptr && m_status != status)
        m_onStatusChange(status);

    m_status = status;
}
}    // namespace wolkabout