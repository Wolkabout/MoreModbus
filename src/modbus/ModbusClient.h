/*
 *  Copyright (C) 2020 WolkAbout Technology s.r.o.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include "modbus/libmodbus/modbus.h"

#include <chrono>
#include <mutex>
#include <string>
#include <vector>

namespace wolkabout
{
/**
 * @brief Main interface class for Clients to inherit.
 * @details Describes all methods necessary for the ModbusGroupReader to use while reading
 *          a group. Uses a mutex so calls don't overlap.
 */
class ModbusClient
{
public:
    explicit ModbusClient(std::chrono::milliseconds responseTimeout);
    virtual ~ModbusClient() = default;

    /**
     * @return Returns whether or not the client successfully established a modbus connection
     *          or if the connection is already established.
     */
    bool connect();

    /**
     * @return Returns true if connection has been successfully stopped.
     */
    bool disconnect();

    /**
     * @return Returns whether or not the client has established a modbus client.
     */
    bool isConnected();

    /**
     * @brief Writes a single uint16_t value into a HOLDING REGISTER, targeting the address.
     * @details Modbus code function 6 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param value
     * @return Returns whether or not the operation was successful.
     */
    bool writeHoldingRegister(int slaveAddress, int address, uint16_t value);

    /**
     * @brief Writes multiple uint16_t values into HOLDING REGISTERS, targeting the address, and however many registers
     *        after, dictated by the length of vector passed as value.
     * @details Modbus code function 16 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param values
     * @return Returns whether or not the operation was successful.
     */
    bool writeHoldingRegisters(int slaveAddress, int address, std::vector<uint16_t>& values);

    /**
     * @brief Writes a single bool value to a COIL, targeting the address.
     * @details Modbus code function 5 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param value
     * @return Returns whether or not the operation was successful.
     */
    bool writeCoil(int slaveAddress, int address, bool value);

    /**
     * @brief Reads from multiple INPUT_CONTACTS, starting from address,
     *        reading as much as passed argument number dictates.
     * @details Modbus code function 2 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param number
     * @param values
     * @return Returns whether or not the operation was successful.
     */
    bool readInputContacts(int slaveAddress, int address, int number, std::vector<bool>& values);

    /**
     * @brief Reads from a single HOLDING_REGISTER, targeting the address.
     * @details Modbus code function 3 is being used in this call, but only reads a single register.
     * @param slaveAddress
     * @param address
     * @param value
     * @return Returns whether or not the operation was successful.
     */
    bool readHoldingRegister(int slaveAddress, int address, uint16_t& value);

    /**
     * @brief Reads from multiple HOLDING_REGISTERS, targeting the address,
     *        reading as much as passed argument number dictates.
     * @details Modbus code function 3 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param number
     * @param values
     * @return Returns whether or not the operation was successful.
     */
    bool readHoldingRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values);

    /**
     * @brief Reads from multiple INPUT_REGISTERS, targeting the address,
     *        reading as much as passed argument number dictates.
     * @details Modbus code function 4 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param number
     * @param values
     * @return Returns whether or not the operation was successful.
     */
    bool readInputRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values);

    /**
     * @brief Reads from a single COIL, targeting the address.
     * @details Modbus code function 1 is being used in this call, but only reads a single register.
     * @param slaveAddress
     * @param address
     * @param value
     * @return Returns whether or not the operation was successful.
     */
    bool readCoil(int slaveAddress, int address, bool& value);

    /**
     * @brief Reads from multiple COILS, targeting the address,
     *        reading as much as passed argument number dictates.
     * @details Modbus code function 1 is being used in this call.
     * @param slaveAddress
     * @param address
     * @param number
     * @param values
     * @return Returns whether or not the operation was successful.
     */
    bool readCoils(int slaveAddress, int address, int number, std::vector<bool>& values);

protected:
    virtual bool createContext() = 0;
    virtual bool destroyContext() = 0;

    virtual bool writeHoldingRegister(int address, uint16_t value);
    virtual bool writeHoldingRegisters(int address, std::vector<uint16_t>& values);

    virtual bool writeCoil(int address, bool value);

    virtual bool readHoldingRegister(int address, uint16_t& value);
    virtual bool readHoldingRegisters(int address, int number, std::vector<uint16_t>& values);

    virtual bool readInputRegisters(int address, int number, std::vector<uint16_t>& values);

    virtual bool readCoil(int address, bool& value);
    virtual bool readCoils(int address, int number, std::vector<bool>& values);

    virtual bool readInputContacts(int address, int number, std::vector<bool>& values);

    virtual bool changeSlaveAddress(int address);
    std::chrono::milliseconds m_responseTimeout;

    bool m_connected;
    bool m_contextCreated;
    std::mutex m_modbusMutex;
    modbus_t* m_modbus;
};
}    // namespace wolkabout

#endif
