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

#include "RegisterGroup.h"
#include "utilities/Logger.h"

#include <algorithm>

namespace wolkabout
{
const char RegisterGroup::SEPARATOR = '.';

bool RegisterGroup::compareFunction(const std::pair<std::string, std::shared_ptr<RegisterMapping>>& left,
                                    const std::pair<std::string, std::shared_ptr<RegisterMapping>>& right)
{
    const auto addressComp = getAddressFromString(right.first) - getAddressFromString(left.first);
    if (addressComp != 0)
        return addressComp > 0;

    return getBitFromString(right.first) - getBitFromString(left.first);
}

bool RegisterGroup::keyExistsInSet(const std::string& key)
{
    for (const auto& pair : m_mappings)
    {
        if (pair.first == key)
            return true;
    }
    return false;
}

RegisterGroup::RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping)
: m_registerType(mapping->getRegisterType())
, m_slaveAddress(mapping->getSlaveAddress())
, m_readRestricted(mapping->isReadRestricted())
, m_mappings(&RegisterGroup::compareFunction)
{
    addMapping(mapping);
}

RegisterGroup::RegisterGroup(const RegisterGroup& instance)
: m_registerType(instance.getRegisterType())
, m_slaveAddress(instance.getSlaveAddress())
, m_readRestricted(instance.isReadRestricted())
, m_mappings(&RegisterGroup::compareFunction)
{
    for (const auto& mapping : instance.getMappingsMap())
    {
        m_mappings.emplace(std::string(mapping.first), std::make_shared<RegisterMapping>(*(mapping.second)));
    }
}

bool RegisterGroup::addMapping(const std::shared_ptr<RegisterMapping>& mapping)
{
    if (mapping->getRegisterType() != m_registerType)
    {
        throw std::logic_error("RegisterGroup: You can\'t add different typed registers in a group.");
    }

    if (mapping->isReadRestricted() && !m_readRestricted)
    {
        throw std::logic_error("RegisterGroup: Read restricted mappings have to have groups of their own!");
    }

    mapping->setSlaveAddress(m_slaveAddress);

    if (mapping->isReadRestricted() && m_readRestricted)
    {
        return appendMapping(mapping);
    }

    if (!m_mappings.empty())
    {
        const auto firstGroupAddress = getStartingAddress();
        const auto groupAddressCount = getAddressCount();
        const auto firstMappingAddress = mapping->getStartingAddress();
        const auto mappingAddressCount = mapping->getRegisterCount();

        if (mapping->getOperationType() == RegisterMapping::OperationType::TAKE_BIT)
        {
            // If we're just adding bits, we don't need to apply same ruling.
            if (keyExistsInSet(std::to_string(mapping->getStartingAddress())))
            {
                // The address we're targeting is already fully claimed.
                LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference()
                          << " can\'t take a bit. Other register"
                             "is already taking the full value.";
                return false;
            }

            // If it's before the group, diff must be -1.
            if (firstMappingAddress < firstGroupAddress)
            {
                const auto diff = firstGroupAddress - firstMappingAddress;
                if (diff != 1)
                {
                    // It must be exactly one address before.
                    LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference()
                              << " is ahead of the group by more than 1 "
                                 " address.";
                    return false;
                }
            }
            else if (firstMappingAddress > (firstGroupAddress + groupAddressCount - 1))
            {
                const auto diff = firstMappingAddress - (firstGroupAddress + groupAddressCount - 1);
                if (diff != 1)
                {
                    // It must be exactly one address after.
                    LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference()
                              << " is after the group by more than 1 address.";
                    return false;
                }
            }
        }
        else
        {
            // Get status, if first mapping register count is lower.
            // If it is lower (for 1, and it's register count is 1), add it.
            // If it is lower (for 2, and it's register count is 2), add it.
            // If the value is higher, check start register and count, see if one/two can be added.
            if (firstMappingAddress < firstGroupAddress)
            {
                // Address is before first one. Check with mappings address count to see where it first.
                const auto diff = firstGroupAddress - (firstMappingAddress + mappingAddressCount);
                if (diff != 0)
                {
                    // They're going to have a gap in between. That's not allowed in a group.
                    LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference() << "("
                              << mapping->getStartingAddress() << ") does not fit in the group (" << firstMappingAddress
                              << " with " << mappingAddressCount << " address(es) ... " << firstGroupAddress
                              << ", address gap is " << diff << ")";
                    return false;
                }
            }
            else
            {
                // Address is after first one. Check with groups address count to see where the last address is.
                const auto diff = firstMappingAddress - (firstGroupAddress + groupAddressCount);
                if (diff != 0)
                {
                    // They're also going to have a gap in between.
                    LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference() << "("
                              << mapping->getStartingAddress() << ") does not fit in the group (" << firstGroupAddress
                              << " -> " << (firstGroupAddress + groupAddressCount - 1) << " ... " << firstMappingAddress
                              << ", address gap is " << diff << ")";
                    return false;
                }
            }
        }
    }

    return appendMapping(mapping);
}

bool RegisterGroup::appendMapping(const std::shared_ptr<RegisterMapping>& mapping)
{
    if (mapping->getOperationType() == RegisterMapping::OperationType::TAKE_BIT)
    {
        const auto key =
          std::to_string(mapping->getStartingAddress()) + SEPARATOR + std::to_string(mapping->getBitIndex());
        if (keyExistsInSet(std::to_string(mapping->getStartingAddress())))
        {
            LOG(WARN) << "RegisterGroup: Mapping " << mapping->getReference() << "(" << mapping->getStartingAddress()
                      << ") requests a bit that is already occupied.";
            return false;
        }
        m_mappings.emplace(key, mapping);
        return true;
    }
    else
    {
        for (uint16_t i = 0; i < mapping->getRegisterCount(); i++)
        {
            m_mappings.emplace(std::to_string(mapping->getStartingAddress() + i), mapping);
        }
        return true;
    }
}

RegisterMapping::RegisterType RegisterGroup::getRegisterType() const
{
    return m_registerType;
}

uint16_t RegisterGroup::getStartingAddress() const
{
    return getAddressFromString(m_mappings.begin()->first);
}

uint16_t RegisterGroup::getAddressCount() const
{
    std::vector<int16_t> mappings;
    for (const auto& pair : m_mappings)
    {
        const auto address = getAddressFromString(pair.first);
        if (std::find(mappings.begin(), mappings.end(), address) == mappings.end())
        {
            mappings.emplace_back(address);
        }
    }
    return static_cast<uint16_t>(mappings.size());
}

int8_t RegisterGroup::getSlaveAddress() const
{
    return m_slaveAddress;
}

void RegisterGroup::setSlaveAddress(int8_t slaveAddress)
{
    m_slaveAddress = slaveAddress;
    for (const auto& mapping : m_mappings)
    {
        mapping.second->setSlaveAddress(slaveAddress);
    }
}

bool RegisterGroup::isReadRestricted() const
{
    return m_readRestricted;
}

std::map<std::string, std::shared_ptr<RegisterMapping>> RegisterGroup::getMappingsMap() const
{
    std::map<std::string, std::shared_ptr<RegisterMapping>> map;
    for (const auto& pair : m_mappings)
    {
        map.emplace(pair.first, pair.second);
    }
    return map;
}

std::vector<std::string> RegisterGroup::getMappingsClaims() const
{
    std::vector<std::string> claims;
    for (const auto& mappingPair : m_mappings)
    {
        claims.emplace_back(mappingPair.first);
    }
    return claims;
}

std::vector<std::shared_ptr<RegisterMapping>> RegisterGroup::getMappings() const
{
    std::vector<std::shared_ptr<RegisterMapping>> mappings;
    for (const auto& mappingPair : m_mappings)
    {
        mappings.emplace_back(mappingPair.second);
    }
    return mappings;
}

uint16_t RegisterGroup::getAddressFromString(const std::string& string)
{
    auto firstAddressString = std::string(string);
    auto dotIndex = firstAddressString.find(SEPARATOR);
    if (dotIndex != std::string::npos)
    {
        firstAddressString = firstAddressString.substr(0, dotIndex);
    }
    return static_cast<uint16_t>(std::stoul(firstAddressString));
}

int16_t RegisterGroup::getBitFromString(const std::string& string)
{
    auto firstAddressString = std::string(string);
    auto dotIndex = firstAddressString.find(SEPARATOR);
    if (dotIndex == std::string::npos)
    {
        return -1;
    }
    return static_cast<int16_t>(std::stoi(firstAddressString.substr(dotIndex + 1)));
}
}    // namespace wolkabout
