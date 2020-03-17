//
// Created by nvuletic on 3/11/20.
//

#include "Group.h"

#include "utility/Logger.h"

namespace wolkabout
{
Group::Group(const std::shared_ptr<Mapping>& mapping) : m_registerType(mapping->getRegisterType()), m_mappings()
{
    addMapping(mapping);
}

bool Group::addMapping(std::shared_ptr<Mapping> mapping)
{
    if (mapping->getRegisterType() != m_registerType)
    {
        throw std::logic_error("You can\'t add different typed registers in a group.");
    }

    if (!m_mappings.empty())
    {
        const auto firstGroupAddress = getStartingAddress();
        const auto groupAddressCount = getAddressCount();
        const auto firstMappingAddress = mapping->getStartingAddress();
        const auto mappingAddressCount = mapping->getRegisterCount();

        if (mapping->getOperationType() == Mapping::OperationType::TAKE_BIT)
        {
            // If we're just adding bits, we don't need to apply same ruling.
            if (m_mappings.find(std::to_string(mapping->getStartingAddress())) != m_mappings.end())
            {
                // The address we're targeting is already fully claimed.
                LOG(WARN) << "Mapping " << mapping->getReference()
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
                    LOG(WARN) << "Mapping " << mapping->getReference()
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
                    LOG(WARN) << "Mapping " << mapping->getReference() << " is after the group by more than 1 address.";
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
                    LOG(WARN) << "Mapping " << mapping->getReference() << "(" << mapping->getStartingAddress()
                              << ") does not fit in the group (" << firstMappingAddress << " with "
                              << mappingAddressCount << " address(es) ... " << firstGroupAddress << ", address gap is "
                              << diff << ")";
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
                    LOG(WARN) << "Mapping " << mapping->getReference() << "(" << mapping->getStartingAddress()
                              << ") does not fit in the group (" << firstGroupAddress << " -> "
                              << (firstGroupAddress + groupAddressCount - 1) << " ... " << firstMappingAddress
                              << ", address gap is " << diff << ")";
                    return false;
                }
            }
        }
    }

    if (mapping->getOperationType() == Mapping::OperationType::TAKE_BIT)
    {
        m_mappings.emplace(std::to_string(mapping->getStartingAddress()) + "." + std::to_string(mapping->getBitIndex()),
                           mapping);
        return true;
    }
    else
    {
        for (uint i = 0; i < mapping->getRegisterCount(); i++)
        {
            m_mappings.emplace(std::to_string(mapping->getStartingAddress() + i), mapping);
        }
        return true;
    }
}

Mapping::RegisterType Group::getRegisterType() const
{
    return m_registerType;
}

uint16_t Group::getStartingAddress() const
{
    return getAddressFromString(m_mappings.begin()->first);
}

uint16_t Group::getAddressCount() const
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
    return mappings.size();
}

const std::map<std::string, std::shared_ptr<Mapping>>& Group::getMappings() const
{
    return m_mappings;
}

uint16_t Group::getAddressFromString(const std::string& string)
{
    auto firstAddressString = std::string(string);
    auto dotIndex = firstAddressString.find('.');
    if (dotIndex != -1)
    {
        firstAddressString = firstAddressString.substr(0, dotIndex);
    }
    return std::stoul(firstAddressString);
}
}    // namespace wolkabout
