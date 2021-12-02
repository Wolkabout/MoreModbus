/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef WOLKABOUT_MODBUS_MODBUSDEVICE_H
#define WOLKABOUT_MODBUS_MODBUSDEVICE_H

#include "more_modbus/RegisterGroup.h"

#include <functional>

namespace wolkabout
{
class ModbusReader;

/**
 * @brief Utility struct defining a function used by the std::set inside of ModbusDevice to sort the mappings before
 *       an attempt at forming ModbusGroup instances for such device.
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
 *         The groups will be created by the device, and all the slaveAddresses will be set when the
 *          device is created.
 */
class ModbusDevice : public std::enable_shared_from_this<ModbusDevice>
{
public:
    /**
     * @brief Default constructor for the device
     * @param name unique as identification
     * @param slaveAddress unique to device, and is going to set all groups to this slaveAddress
     */
    ModbusDevice(const std::string& name, int16_t slaveAddress);

    /**
     * @brief Copy constructor for the device, does complete deep copy for device and groups.
     */
    ModbusDevice(const ModbusDevice& device);

    /**
     * @brief Create all the RegisterGroup that this device will have by providing all mappings.
     * @param mappings container of all mappings the user wishes this device has.
     */
    void createGroups(const std::vector<std::shared_ptr<RegisterMapping>>& mappings);

    const std::string& getName() const;

    bool getStatus() const;

    const std::weak_ptr<ModbusReader>& getReader() const;

    void setReader(const std::shared_ptr<ModbusReader>& reader);

    int16_t getSlaveAddress() const;

    const std::vector<std::shared_ptr<RegisterGroup>>& getGroups() const;

    void setOnMappingValueChange(
      const std::function<void(const std::shared_ptr<RegisterMapping>&, bool)>& onMappingValueChangeBool);

    /**
     * @brief Event that will trigger when one of the devices mappings value changes.
     * @param onMappingValueChange the callback function for callback, executed on the devices reading thread.
     */
    void setOnMappingValueChange(const std::function<void(const std::shared_ptr<RegisterMapping>&,
                                                          const std::vector<uint16_t>&)>& onMappingValueChange);

    /**
     * @brief Event that will trigger when the devices status changes.
     * @param onStatusChange the callback function for callback, executed on the devices reading thread.
     */
    void setOnStatusChange(const std::function<void(bool)>& onStatusChange);

    void triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping, bool data);

    void triggerOnMappingValueChange(const std::shared_ptr<RegisterMapping>& mapping,
                                     const std::vector<uint16_t>& data);

    void triggerOnStatusChange(bool status);

private:
    std::string m_name;
    bool m_status;
    int16_t m_slaveAddress;
    std::vector<std::shared_ptr<RegisterGroup>> m_groups;

    std::weak_ptr<ModbusReader> m_reader;

    std::function<void(const std::shared_ptr<RegisterMapping>&, bool data)> m_onMappingValueChangeBool;
    std::function<void(const std::shared_ptr<RegisterMapping>&, const std::vector<uint16_t>& data)>
      m_onMappingValueChangeBytes;
    std::function<void(bool)> m_onStatusChange;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSDEVICE_H
