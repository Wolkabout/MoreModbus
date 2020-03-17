//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_REGISTERGROUP_H
#define WOLKABOUT_MODBUS_REGISTERGROUP_H

#include "RegisterMapping.h"

#include <map>
#include <memory>

namespace wolkabout
{
// Group serves to merge multiple mappings that can be read with a single Modbus command.
// It groups Mappings of same type, that can be found next to each other.
// When values are read, they're assigned to each Mapping as necessary.
class RegisterGroup
{
public:
    explicit RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping);

    RegisterGroup(const RegisterGroup& instance);

    bool addMapping(std::shared_ptr<RegisterMapping> mapping);

    RegisterMapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    int8_t getSlaveAddress() const;

    void setSlaveAddress(int8_t slaveAddress);

    const std::map<std::string, std::shared_ptr<RegisterMapping>>& getMappings() const;

    static uint16_t getAddressFromString(const std::string& string);

private:
    RegisterMapping::RegisterType m_registerType;
    int8_t m_slaveAddress;

    std::map<std::string, std::shared_ptr<RegisterMapping>> m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERGROUP_H
