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


#include "LibModbusSerialRtuClient.h"
#include "utilities/Logger.h"

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <utility>

namespace wolkabout
{
LibModbusSerialRtuClient::LibModbusSerialRtuClient(std::string serialPort, int baudRate, char dataBits, char stopBits,
                                                   BitParity bitParity, std::chrono::milliseconds responseTimeout)
: ModbusClient(std::move(responseTimeout))
, m_serialPort(std::move(serialPort))
, m_baudRate(baudRate)
, m_dataBits(dataBits)
, m_stopBits(stopBits)
{
    switch (bitParity)
    {
    case BitParity::NONE:
        m_bitParity = 'N';
        break;

    case BitParity::EVEN:
        m_bitParity = 'E';
        break;

    case BitParity::ODD:
        m_bitParity = 'O';
        break;
    }
}

LibModbusSerialRtuClient::~LibModbusSerialRtuClient()
{
    destroyContext();
}

bool LibModbusSerialRtuClient::createContext()
{
    LOG(INFO) << "LibModbusClient: Opening serial port '" << m_serialPort << "'  Baud: " << m_baudRate;

    m_modbus = modbus_new_rtu(m_serialPort.c_str(), m_baudRate, m_bitParity, m_dataBits, m_stopBits);
    if (m_modbus == nullptr)
    {
        LOG(ERROR) << "LibModbusClient: Unable to create modbus context - " << modbus_strerror(errno);
        return false;
    }

    m_contextCreated = true;
    return true;
}

bool LibModbusSerialRtuClient::destroyContext()
{
    if (m_modbus)
    {
        LOG(INFO) << "LibModbusClient: Closing serial port '" << m_serialPort << "'";
        disconnect();
        modbus_free(m_modbus);
        m_modbus = nullptr;
        m_contextCreated = false;
    }

    return true;
}

bool LibModbusSerialRtuClient::writeHoldingRegister(int address, uint16_t value)
{
    auto result = ModbusClient::writeHoldingRegister(address, value);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::writeCoil(int address, bool value)
{
    auto result = ModbusClient::writeCoil(address, value);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readInputRegisters(int address, int number, std::vector<uint16_t>& values)
{
    auto result = ModbusClient::readInputRegisters(address, number, values);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readInputContacts(int address, int number, std::vector<bool>& values)
{
    auto result = ModbusClient::readInputContacts(address, number, values);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readHoldingRegister(int address, uint16_t& value)
{
    auto result = ModbusClient::readHoldingRegister(address, value);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readHoldingRegisters(int address, int number, std::vector<uint16_t>& values)
{
    auto result = ModbusClient::readHoldingRegisters(address, number, values);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readCoil(int address, bool& value)
{
    auto result = ModbusClient::readCoil(address, value);

    sleepBetweenModbusMessages();
    return result;
}

bool LibModbusSerialRtuClient::readCoils(int address, int number, std::vector<bool>& values)
{
    auto result = ModbusClient::readCoils(address, number, values);

    sleepBetweenModbusMessages();
    return result;
}

void LibModbusSerialRtuClient::sleepBetweenModbusMessages() const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
}    // namespace wolkabout
