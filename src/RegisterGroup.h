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
    const static char SEPARATOR;

    explicit RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping);

    // Overriden default copy constructor to ensure deep copy of objects which we own
    // shared pointers to. This is done because for every group, we want to have
    // copies of mappings, since they're the ones tracking values and their changes
    // for each separate register on each separate slave address.
    RegisterGroup(const RegisterGroup& instance);

    bool addMapping(const std::shared_ptr<RegisterMapping>& mapping);

    RegisterMapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    int8_t getSlaveAddress() const;

    void setSlaveAddress(int8_t slaveAddress);

    const std::map<std::string, std::shared_ptr<RegisterMapping>>& getMappingsMap() const;

    std::vector<std::string> getMappingsClaims() const;

    std::vector<std::shared_ptr<RegisterMapping>> getMappings() const;

    static uint16_t getAddressFromString(const std::string& string);

    static int16_t getBitFromString(const std::string& string);

private:
    RegisterMapping::RegisterType m_registerType;
    int8_t m_slaveAddress;

    std::map<std::string, std::shared_ptr<RegisterMapping>> m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERGROUP_H
