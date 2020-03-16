//
// Created by nvuletic on 3/11/20.
//

#include "Group.h"

namespace wolkabout
{
Group::Group(const std::shared_ptr<Mapping>& mapping)
: m_registerType(mapping->getRegisterType())
, m_mappings()
, m_startingAddress(mapping->getStartingAddress())
, m_addressCount(mapping->getRegisterCount())
{
    addMapping(mapping);
}

void Group::addMapping(std::shared_ptr<Mapping> mapping)
{
    if (mapping->getOperationType() == Mapping::OperationType::TAKE_BIT)
    {
        m_mappings.emplace(std::to_string(mapping->getStartingAddress()) + "." + std::to_string(mapping->getBitIndex()),
                           mapping);
    }
    else
    {
        m_mappings.emplace(std::to_string(mapping->getStartingAddress()), mapping);
    }
}

Mapping::RegisterType Group::getRegisterType() const
{
    return m_registerType;
}

uint16_t Group::getStartingAddress() const
{
    return m_startingAddress;
}

uint16_t Group::getAddressCount() const
{
    return m_addressCount;
}

const std::map<std::string, std::shared_ptr<Mapping>>& Group::getMappings() const
{
    return m_mappings;
}
}    // namespace wolkabout
