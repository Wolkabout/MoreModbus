//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_GROUP_H
#define WOLKABOUT_MODBUS_GROUP_H

#include "Mapping.h"

#include <map>

namespace wolkabout
{
class Group
{
public:
    explicit Group(const std::shared_ptr<Mapping>& mapping);

    void addMapping(std::shared_ptr<Mapping> mapping);

    Mapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    const std::map<std::string, std::shared_ptr<Mapping>>& getMappings() const;

private:
    Mapping::RegisterType m_registerType;
    uint16_t m_startingAddress;
    uint16_t m_addressCount;

    std::map<std::string, std::shared_ptr<Mapping>> m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_GROUP_H
