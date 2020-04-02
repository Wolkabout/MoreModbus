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

#ifndef LIBMODBUSSERIALRTUCLIENT_H
#define LIBMODBUSSERIALRTUCLIENT_H

#include "ModbusClient.h"

#include <chrono>
#include <mutex>
#include <string>

namespace wolkabout
{
/**
 * @brief ModbusClient implementation for Modbus SERIAL/RTU connections.
 * @details Uses modbus_new_rtu method from libmodbus, and takes in a bunch of arguments.
 *          Overrides all the methods to add a timeout after messages.
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
