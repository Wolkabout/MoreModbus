//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_WOLKABOUTREGISTERGROUP_H
#define WOLKABOUT_MODBUS_WOLKABOUTREGISTERGROUP_H

#include "WolkaboutRegisterMapping.h"

#include <map>

namespace wolkabout
{
// Group serves to merge multiple mappings that can be read with a single Modbus command.
// It groups Mappings of same type, that can be found next to each other.
// When values are read, they're assigned to each Mapping as necessary.
class WolkaboutRegisterGroup
{
public:
    explicit WolkaboutRegisterGroup(const std::shared_ptr<WolkaboutRegisterMapping>& mapping);

    bool addMapping(std::shared_ptr<WolkaboutRegisterMapping> mapping);

    WolkaboutRegisterMapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    const std::map<std::string, std::shared_ptr<WolkaboutRegisterMapping>>& getMappings() const;

    static uint16_t getAddressFromString(const std::string& string);

private:
    WolkaboutRegisterMapping::RegisterType m_registerType;

    std::map<std::string, std::shared_ptr<WolkaboutRegisterMapping>> m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_WOLKABOUTREGISTERGROUP_H
