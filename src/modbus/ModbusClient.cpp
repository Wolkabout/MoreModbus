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

#include "ModbusClient.h"

#include "utilities/Logger.h"

namespace wolkabout
{
ModbusClient::ModbusClient(std::chrono::milliseconds responseTimeout)
: m_responseTimeout(responseTimeout), m_connected(false), m_contextCreated(false), m_modbus(nullptr)
{
}

bool ModbusClient::connect()
{
    if (m_connected)
    {
        return true;
    }

    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!m_contextCreated && !createContext())
    {
        return false;
    }

    if (modbus_set_response_timeout(m_modbus, 2, 0) == -1)
    {
        LOG(ERROR) << "LibModbusClient: Unable to set response timeout - " << modbus_strerror(errno);
        destroyContext();
        return false;
    }

    if (modbus_connect(m_modbus) == -1)
    {
        LOG(ERROR) << "LibModbusClient: Unable to connect - " << modbus_strerror(errno);
        return false;
    }
    LOG(INFO) << "LibModbusClient: Connected successfully.";
    m_connected = true;

    auto responseTimeOutSeconds = std::chrono::duration_cast<std::chrono::seconds>(m_responseTimeout);
    auto responseTimeOutMicrosecondsResidue =
      std::chrono::duration_cast<std::chrono::microseconds>(m_responseTimeout) - responseTimeOutSeconds;

    if (modbus_set_response_timeout(m_modbus, static_cast<uint32_t>(responseTimeOutSeconds.count()),
                                    static_cast<uint32_t>(responseTimeOutMicrosecondsResidue.count())) == -1)
    {
        LOG(ERROR) << "LibModbusClient: Unable to set response timeout - " << modbus_strerror(errno);
        destroyContext();
        return false;
    }

    return true;
}

bool ModbusClient::disconnect()
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    modbus_flush(m_modbus);
    modbus_close(m_modbus);
    m_connected = false;
    return true;
}

bool ModbusClient::isConnected()
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    return m_connected;
}

bool ModbusClient::writeHoldingRegister(int slaveAddress, int address, uint16_t value)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return writeHoldingRegister(address, value);
}

bool ModbusClient::writeHoldingRegisters(int slaveAddress, int address, std::vector<uint16_t>& values)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return writeHoldingRegisters(address, values);
}

bool ModbusClient::writeCoil(int slaveAddress, int address, bool value)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return writeCoil(address, value);
}

bool ModbusClient::readInputRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return readInputRegisters(address, number, values);
}

bool ModbusClient::readInputContacts(int slaveAddress, int address, int number, std::vector<bool>& values)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return readInputContacts(address, number, values);
}

bool ModbusClient::readHoldingRegister(int slaveAddress, int address, uint16_t& value)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return readHoldingRegister(address, value);
}

bool ModbusClient::readHoldingRegisters(int slaveAddress, int address, int number, std::vector<uint16_t>& values)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return readHoldingRegisters(address, number, values);
}

bool ModbusClient::readCoil(int slaveAddress, int address, bool& value)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }
    bool read = readCoil(address, value);
    return read;
}

bool ModbusClient::readCoils(int slaveAddress, int address, int number, std::vector<bool>& values)
{
    std::lock_guard<decltype(m_modbusMutex)> l{m_modbusMutex};
    if (!changeSlaveAddress(slaveAddress))
    {
        return false;
    }

    return readCoils(address, number, values);
}

//

bool ModbusClient::writeHoldingRegister(int address, uint16_t value)
{
    if (modbus_write_register(m_modbus, address, value) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to write holding register - " << modbus_strerror(errno);
        return false;
    }

    return true;
}

bool ModbusClient::writeHoldingRegisters(int address, std::vector<uint16_t>& values)
{
    if (modbus_write_registers(m_modbus, address, static_cast<int>(values.size()), values.data()) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to write holding registers - " << modbus_strerror(errno);
        return false;
    }

    return true;
}

bool ModbusClient::writeCoil(int address, bool value)
{
    if (modbus_write_bit(m_modbus, address, value ? TRUE : FALSE) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to write coil - " << modbus_strerror(errno);
        return false;
    }

    return true;
}

bool ModbusClient::readInputRegisters(int address, int number, std::vector<uint16_t>& values)
{
    std::vector<uint16_t> tmpValues(static_cast<uint>(number));

    if (modbus_read_input_registers(m_modbus, address, number, &tmpValues[0]) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read input registers - " << modbus_strerror(errno);
        return false;
    }

    for (const uint16_t& tmpValue : tmpValues)
    {
        values.push_back(tmpValue);
    }

    return true;
}

bool ModbusClient::readInputContacts(int address, int number, std::vector<bool>& values)
{
    int size = (number / 8 + (number % 8 != 0));
    std::vector<std::uint8_t> tmpValues(static_cast<uint>(size));

    int bits_read = modbus_read_input_bits(m_modbus, address, number, &tmpValues[0]);
    if (bits_read == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read input contacts - " << modbus_strerror(errno);
        return false;
    }

    for (uint i = 0; i < static_cast<uint>(bits_read); i++)
    {
        values.push_back(static_cast<bool>(tmpValues[i]));
    }
    return true;
}

bool ModbusClient::readHoldingRegister(int address, uint16_t& value)
{
    if (modbus_read_registers(m_modbus, address, 1, &value) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read holding register - " << modbus_strerror(errno);
        return false;
    }

    return true;
}

bool ModbusClient::readHoldingRegisters(int address, int number, std::vector<uint16_t>& values)
{
    std::vector<uint16_t> tmpValues(static_cast<uint>(number));

    if (modbus_read_registers(m_modbus, address, number, &tmpValues[0]) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read holding registers - " << modbus_strerror(errno);
        return false;
    }

    for (const uint16_t& tmpValue : tmpValues)
    {
        values.push_back(tmpValue);
    }

    return true;
}

bool ModbusClient::readCoil(int address, bool& value)
{
    std::uint8_t tmpValue = 0;

    if (modbus_read_bits(m_modbus, address, 1, &tmpValue) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read coil - " << modbus_strerror(errno);
        return false;
    }

    value = tmpValue != 0;
    return true;
}

bool ModbusClient::readCoils(int address, int number, std::vector<bool>& values)
{
    std::vector<std::uint8_t> tmpValues(static_cast<std::vector<std::uint8_t>::size_type>(number));
    int bits_read = 0;
    bits_read = modbus_read_bits(m_modbus, address, number, &tmpValues[0]);
    if (bits_read == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to read coils - " << modbus_strerror(errno);
        return false;
    }

    for (uint i = 0; i < static_cast<uint>(bits_read); i++)
    {
        values.push_back(static_cast<bool>(tmpValues[i]));
    }
    return true;
}

bool ModbusClient::changeSlaveAddress(int address)
{
    if (modbus_set_slave(m_modbus, address) == -1)
    {
        LOG(DEBUG) << "LibModbusClient: Unable to set slave address - " << modbus_strerror(errno);
        return false;
    }

    return true;
}
}    // namespace wolkabout
