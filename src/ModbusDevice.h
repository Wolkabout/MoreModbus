//
// Created by Nexyy on 17/03/2020.
//

#ifndef WOLKABOUT_MODBUS_MODBUSDEVICE_H
#define WOLKABOUT_MODBUS_MODBUSDEVICE_H

#include "RegisterGroup.h"

#include <functional>

namespace wolkabout
{
typedef bool (*CompareFunction)(const std::shared_ptr<RegisterMapping>&, const std::shared_ptr<RegisterMapping>&);

class ModbusDevice
{
public:
    ModbusDevice(const std::string& name, int8_t slaveAddress,
                 const std::vector<std::shared_ptr<RegisterMapping>>& mappings);

    const std::string& getName() const;

    int8_t getSlaveAddress() const;

    const std::vector<std::shared_ptr<RegisterGroup>>& getGroups() const;

    void setOnMappingValueChange(const std::function<void(const RegisterMapping&)>& onMappingValueChange);

    void triggerOnMappingValueChange(const RegisterMapping& mapping);

    static bool compareMappings(const std::shared_ptr<RegisterMapping>& left,
                                const std::shared_ptr<RegisterMapping>& right);

private:
    std::string m_name;
    int8_t m_slaveAddress;
    std::vector<std::shared_ptr<RegisterGroup>> m_groups;

    std::function<void(const RegisterMapping&)> m_onMappingValueChange;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSDEVICE_H
