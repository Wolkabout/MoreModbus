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

#include "more_modbus/ModbusDevice.h"

#include "core/utilities/Logger.h"

#include <algorithm>
#include <set>

namespace wolkabout
{
namespace more_modbus
{
ModbusDevice::ModbusDevice(const std::string& name, int16_t slaveAddress)
: m_name(name), m_status(false), m_slaveAddress(slaveAddress), m_groups()
{
}

ModbusDevice::ModbusDevice(const ModbusDevice& device)
: m_name(device.m_name)
, m_status(device.m_status)
, m_groups()
, m_reader(device.m_reader)
, m_onMappingValueChangeBool(device.m_onMappingValueChangeBool)
, m_onMappingValueChangeBytes(device.m_onMappingValueChangeBytes)
, m_onStatusChange(device.m_onStatusChange)
{
    for (const auto& group : device.m_groups)
    {
        m_groups.emplace_back(std::make_shared<RegisterGroup>(*group));
    }
}

void ModbusDevice::createGroups(const std::vector<std::shared_ptr<RegisterMapping>>& mappings)
{
    std::map<RegisterMapping::RegisterType, std::shared_ptr<RegisterGroup>> readRestrictedGroups;
    std::set<std::shared_ptr<RegisterMapping>, CompareFunction> set(mappings.begin(), mappings.end());

    std::shared_ptr<RegisterGroup> previousGroup = nullptr;
    for (const auto& mapping : set)
    {
        // Add the mapping to rewrite vector if it needs to be rewritten
        if (mapping->getRepeatedWrite().count() > 0)
            m_rewrite.emplace_back(mapping);

        if (mapping->isReadRestricted())
        {
            // Add the mapping to the readRestricted group for specific type.
            if (readRestrictedGroups[mapping->getRegisterType()] == nullptr)
            {
                const auto newGroup = std::make_shared<RegisterGroup>(mapping, shared_from_this());
                readRestrictedGroups[mapping->getRegisterType()] = newGroup;
                m_groups.insert(m_groups.end(), newGroup);
                mapping->setGroup(newGroup);
            }
            else
            {
                const auto group = readRestrictedGroups[mapping->getRegisterType()];
                group->addMapping(mapping);
                mapping->setGroup(group);
            }
        }
        else
        {
            if (previousGroup == nullptr)
            {
                previousGroup = std::make_shared<RegisterGroup>(mapping, shared_from_this());
                previousGroup->setSlaveAddress(m_slaveAddress);
                m_groups.insert(m_groups.end(), previousGroup);
                mapping->setGroup(previousGroup);
                continue;
            }

            if (previousGroup->getRegisterType() == mapping->getRegisterType())
            {
                if (previousGroup->addMapping(mapping))
                {
                    mapping->setGroup(previousGroup);
                    continue;
                }
            }

            previousGroup = std::make_shared<RegisterGroup>(mapping, shared_from_this());
            previousGroup->setSlaveAddress(m_slaveAddress);
            m_groups.insert(m_groups.end(), previousGroup);
            mapping->setGroup(previousGroup);
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

int16_t ModbusDevice::getSlaveAddress() const
{
    return m_slaveAddress;
}

const std::vector<std::shared_ptr<RegisterGroup>>& ModbusDevice::getGroups() const
{
    return m_groups;
}

std::vector<std::shared_ptr<RegisterMapping>> ModbusDevice::getRewritable() const
{
    std::lock_guard<std::mutex> lockGuard{m_rewriteMutex};

    return m_rewrite;
}

void ModbusDevice::addRewritable(const std::shared_ptr<RegisterMapping>& mapping)
{
    std::lock_guard<std::mutex> lockGuard{m_rewriteMutex};

    if (std::find(m_rewrite.cbegin(), m_rewrite.cend(), mapping) == m_rewrite.cend())
        m_rewrite.emplace_back(mapping);
}

void ModbusDevice::removeRewritable(const std::shared_ptr<RegisterMapping>& mapping)
{
    std::lock_guard<std::mutex> lockGuard{m_rewriteMutex};

    const auto it = std::find(m_rewrite.cbegin(), m_rewrite.cend(), mapping);
    if (it != m_rewrite.cend())
        m_rewrite.erase(it);
}

void ModbusDevice::setOnMappingValueChange(
  const std::function<void(const std::shared_ptr<RegisterMapping>&, bool)>& onMappingValueChangeBool)
{
    m_onMappingValueChangeBool = onMappingValueChangeBool;
}

void ModbusDevice::setOnMappingValueChange(
  const std::function<void(const std::shared_ptr<RegisterMapping>&, const std::vector<uint16_t>&)>&
    onMappingValueChange)
{
    m_onMappingValueChangeBytes = onMappingValueChange;
}

void ModbusDevice::setOnStatusChange(const std::function<void(bool)>& onStatusChange)
{
    m_onStatusChange = onStatusChange;
}

void ModbusDevice::triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping,
                                               const std::vector<uint16_t>& data)
{
    if (m_onMappingValueChangeBytes)
        m_onMappingValueChangeBytes(mapping, data);
}

void ModbusDevice::triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping, bool data)
{
    if (m_onMappingValueChangeBool)
        m_onMappingValueChangeBool(mapping, data);
}

void ModbusDevice::triggerOnStatusChange(bool status)
{
    if (m_onStatusChange != nullptr && m_status != status)
        m_onStatusChange(status);

    m_status = status;
}

const std::weak_ptr<ModbusReader>& ModbusDevice::getReader() const
{
    return m_reader;
}

void ModbusDevice::setReader(const std::shared_ptr<ModbusReader>& reader)
{
    m_reader = reader;
}
}    // namespace more_modbus
}    // namespace wolkabout
