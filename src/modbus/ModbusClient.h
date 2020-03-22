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

#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include "modbus/libmodbus/modbus.h"

#include <chrono>
#include <mutex>
#include <string>
#include <vector>

namespace wolkabout
{
class ModbusClient
{
public:
    explicit ModbusClient(std::chrono::milliseconds responseTimeout);
    virtual ~ModbusClient() = default;

    bool connect();
    bool disconnect();

    bool isConnected();

    bool writeHoldingRegister(int slaveAddress, int address, uint16_t value);
    bool writeHoldingRegisters(int slaveAddress, int address, std::vector<uint16_t>& values);

    bool writeCoil(int slaveAddress, int address, bool value);

    bool readInputContacts(int slaveAddress, int address, int number, std::vector<bool>& values);

    bool readHoldingRegister(int slaveAddress, int address, uint16_t& value);
    bool readHoldingRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values);

    bool readInputRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values);

    bool readCoil(int slaveAddress, int address, bool& value);
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
