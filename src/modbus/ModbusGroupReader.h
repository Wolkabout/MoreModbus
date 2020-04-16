//
// Created by astrihale on 19.3.20..
//

#ifndef WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
#define WOLKABOUT_MODBUS_MODBUSGROUPREADER_H

#include "ModbusClient.h"
#include "RegisterGroup.h"

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
