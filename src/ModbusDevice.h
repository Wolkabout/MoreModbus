//
// Created by Nexyy on 17/03/2020.
//

#ifndef WOLKABOUT_MODBUS_MODBUSDEVICE_H
#define WOLKABOUT_MODBUS_MODBUSDEVICE_H

#include "RegisterGroup.h"

#include <functional>

namespace wolkabout
{
class ModbusDevice
{
public:
    ModbusDevice(const std::string& name, int8_t slaveAddress,
                 const std::vector<std::shared_ptr<RegisterMapping>>& mappings);

    const std::string& getName() const;

    int8_t getSlaveAddress() const;

    const std::vector<std::shared_ptr<RegisterGroup>>& getGroups() const;

    const std::map<std::string, std::shared_ptr<RegisterMapping>>& getMappings() const;

    void setOnMappingValueChange(const std::function<void(const RegisterMapping&)>& onMappingValueChange);

    void triggerOnMappingValueChange(const RegisterMapping& mapping);

private:
    std::string m_name;
    int8_t m_slaveAddress;
    std::vector<std::shared_ptr<RegisterGroup>> m_groups;
    std::map<std::string, std::shared_ptr<RegisterMapping>> m_mappings;

    std::function<void(const RegisterMapping&)> m_onMappingValueChange;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSDEVICE_H
