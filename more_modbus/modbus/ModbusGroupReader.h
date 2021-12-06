/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
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

#ifndef WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
#define WOLKABOUT_MODBUS_MODBUSGROUPREADER_H

#include "more_modbus/RegisterGroup.h"
#include "more_modbus/modbus/ModbusClient.h"

namespace wolkabout
{
/**
 * @brief Collection of utility methods used by ModbusReader to read a group.
 */
class ModbusGroupReader
{
public:
    /**
     * @brief The main method used to read a group, and it calls other methods based on the output type of group.
     * @param modbusClient
     * @param group
     * @return Whether or not the group reading has been successful.
     */
    static bool readGroup(ModbusClient& modbusClient, RegisterGroup& group);

private:
    // Discrete values operations
    /**
     * @brief Helping method that aggregates read bool values to each mapping inside a group.
     * @param group
     * @param values
     */
    static void passValuesToGroup(RegisterGroup& group, const std::vector<bool>& values);

    /**
     * @brief Read a group of COIL mappings, and aggregate read values to each mapping, using passValuesToGroup().
     * @param modbusClient
     * @param group
     * @return Whether or not the group reading has been successful.
     */
    static bool readCoilGroup(ModbusClient& modbusClient, RegisterGroup& group);

    /**
     * @brief Read a group of INPUT_CONTACT mappings,
     *       and aggregate read values to each mapping, using passValuesToGroup()
     * @param modbusClient
     * @param group
     * @return Whether or not the group reading has been successful.
     */
    static bool readDiscreteInputGroup(ModbusClient& modbusClient, RegisterGroup& group);

    // Register operations
    /**
     * @brief Helping method that aggregates read uint16_t values to each mapping inside a group.
     * @param group
     * @param values
     */
    static void passValuesToGroup(RegisterGroup& group, const std::vector<uint16_t>& values);

    /**
     * @brief Read a group of HOLDING_REGISTER mappings,
     *       and aggregates read values to each mapping, using passValuesToGroup()
     * @param modbusClient
     * @param group
     * @return Whether or not the group reading has been successful.
     */
    static bool readHoldingRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group);

    /**
     * @brief Read a group of INPUT_REGISTER mappings,
     *       and aggregates read values to each mapping, using passValuesToGroup()
     * @param modbusClient
     * @param group
     * @return Whether or not the group reading has been successful.
     */
    static bool readInputRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
