//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_GROUP_H
#define WOLKABOUT_MODBUS_GROUP_H

#include "Mapping.h"

#include <map>

namespace wolkabout
{
// Group serves to merge multiple mappings that can be read with a single Modbus command.
// It groups Mappings of same type, that can be found next to each other.
// When values are read, they're assigned to each Mapping as necessary.
class Group
{
public:
    explicit Group(const std::shared_ptr<Mapping>& mapping);

    bool addMapping(std::shared_ptr<Mapping> mapping);

    Mapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    const std::map<std::string, std::shared_ptr<Mapping>>& getMappings() const;

    static uint16_t getAddressFromString(const std::string& string);

private:
    Mapping::RegisterType m_registerType;

    std::map<std::string, std::shared_ptr<Mapping>> m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_GROUP_H
