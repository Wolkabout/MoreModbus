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

#ifndef WOLKABOUT_MODBUS_MODBUSDEVICE_H
#define WOLKABOUT_MODBUS_MODBUSDEVICE_H

#include "RegisterGroup.h"

#include <functional>

namespace wolkabout
{
/**
 * @brief Utility struct defining a function used by the std::set inside of ModbusDevice to sort the mappings before
 *        an attempt at forming ModbusGroup instances for such device.
 */
struct CompareFunction
{
    bool operator()(const std::shared_ptr<RegisterMapping>& left, const std::shared_ptr<RegisterMapping>& right)
    {
        const auto typeDiff = (int)right->getRegisterType() - (int)left->getRegisterType();
        if (typeDiff != 0)
            return typeDiff > 0;

        const auto startDiff = right->getStartingAddress() - left->getStartingAddress();
        if (startDiff != 0)
            return startDiff > 0;

        const auto lengthDiff = right->getRegisterCount() - left->getRegisterCount();
        if (lengthDiff != 0)
            return lengthDiff > 0;

        const auto outputDiff = (int)right->getOutputType() - (int)left->getOutputType();
        if (outputDiff != 0)
            return outputDiff > 0;

        return left->getBitIndex() < right->getBitIndex();
    }
};

/**
 * @brief Collection of ModbusGroups for a single Modbus server/slave.
 * @details The device contains the slaveAddress, and all the groups to be read for the slave address.
 *          The groups will be created by the device, and all the slaveAddresses will be set when the
 *           device is created.
 */
class ModbusDevice : public std::enable_shared_from_this<ModbusDevice>
{
public:
    /**
     * @brief Default constructor for the device
     * @param name unique as identification
     * @param slaveAddress unique to device, and is going to set all groups to this slaveAddress
     */
    ModbusDevice(const std::string& name, int8_t slaveAddress);

    /**
     * @brief Create all the RegisterGroup that this device will have by providing all mappings.
     * @param mappings container of all mappings the user wishes this device has.
     */
    void createGroups(const std::vector<std::shared_ptr<RegisterMapping>>& mappings);

    const std::string& getName() const;

    bool getStatus() const;

    int8_t getSlaveAddress() const;

    const std::vector<std::shared_ptr<RegisterGroup>>& getGroups() const;

    /**
     * @brief Event that will trigger when one of the devices mappings value changes.
     * @param onMappingValueChange the callback function for callback, executed on the devices reading thread.
     */
    void setOnMappingValueChange(
      const std::function<void(const std::shared_ptr<RegisterMapping>&)>& onMappingValueChange);

    /**
     * @brief Event that will trigger when the devices status changes.
     * @param onStatusChange the callback function for callback, executed on the devices reading thread.
     */
    void setOnStatusChange(const std::function<void(bool)>& onStatusChange);

    void triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping);

    void triggerOnStatusChange(bool status);

private:
    std::string m_name;
    bool m_status;
    int8_t m_slaveAddress;
    std::vector<std::shared_ptr<RegisterGroup>> m_groups;

    std::function<void(const std::shared_ptr<RegisterMapping>&)> m_onMappingValueChange;
    std::function<void(bool)> m_onStatusChange;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSDEVICE_H
