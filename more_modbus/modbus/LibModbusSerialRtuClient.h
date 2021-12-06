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

#ifndef LIBMODBUSSERIALRTUCLIENT_H
#define LIBMODBUSSERIALRTUCLIENT_H

#include "more_modbus/modbus/ModbusClient.h"

#include <chrono>
#include <mutex>
#include <string>

namespace wolkabout
{
/**
 * @brief ModbusClient implementation for Modbus SERIAL/RTU connections.
 * @details Uses modbus_new_rtu method from libmodbus, and takes in a bunch of arguments.
 *         Overrides all the methods to add a timeout after messages.
 */
class LibModbusSerialRtuClient : public ModbusClient
{
public:
    /**
     * @brief Indicates the bitParity of slaves.
     */
    enum class BitParity
    {
        NONE,
        EVEN,
        ODD
    };

    /**
     * @brief Constructor for the client
     * @param serialPort that is used to access all the slaves
     * @param baudRate used to read the devices
     * @param dataBits
     * @param stopBits
     * @param bitParity
     * @param responseTimeout
     */
    LibModbusSerialRtuClient(std::string serialPort, int baudRate, char dataBits, char stopBits, BitParity bitParity,
                             std::chrono::milliseconds responseTimeout);

    ~LibModbusSerialRtuClient() override;

private:
    bool createContext() override;
    bool destroyContext() override;

    bool writeHoldingRegister(int address, uint16_t value) override;

    bool writeCoil(int address, bool value) override;

    bool readInputRegisters(int address, int number, std::vector<uint16_t>& values) override;

    bool readInputContacts(int address, int number, std::vector<bool>& values) override;

    bool readHoldingRegister(int address, uint16_t& value) override;
    bool readHoldingRegisters(int address, int number, std::vector<uint16_t>& values) override;

    bool readCoil(int address, bool& value) override;
    bool readCoils(int address, int number, std::vector<bool>& values) override;

    void sleepBetweenModbusMessages() const;

    std::string m_serialPort;
    int m_baudRate;
    char m_dataBits;
    char m_stopBits;
    char m_bitParity;
};
}    // namespace wolkabout

#endif
