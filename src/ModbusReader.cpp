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

#include "ModbusReader.h"

#include "modbus/ModbusGroupReader.h"
#include "utilities/DataParsers.h"

#include <algorithm>
#include <iostream>

namespace wolkabout
{
ModbusReader::ModbusReader(ModbusClient& modbusClient, const std::chrono::milliseconds& readPeriod)
: m_modbusClient(modbusClient), m_devices(), m_readerShouldRun(false), m_threads(), m_readPeriod(readPeriod)
{
}

void ModbusReader::addDevice(const std::shared_ptr<ModbusDevice>& device)
{
    m_devices.emplace(device->getSlaveAddress(), device);
    m_deviceActiveStatus.emplace(device->getSlaveAddress(), false);
    m_threads.emplace(device->getSlaveAddress(), nullptr);
    device->setReader(shared_from_this());
    std::cout << "ModbusReader: Successfully added new device " << device->getName() << std::endl;
}

void ModbusReader::addDevices(const std::vector<std::shared_ptr<ModbusDevice>>& devices)
{
    std::cout << "ModbusReader: Adding " << devices.size() << " devices." << std::endl;
    for (const auto& device : devices)
    {
        addDevice(device);
    }
    std::cout << "ModbusReader: Successfully added " << devices.size() << " devices." << std::endl;
}

ModbusReader::~ModbusReader()
{
    stop();
}

bool ModbusReader::isRunning() const
{
    return m_readerShouldRun;
}

const std::map<int16_t, std::shared_ptr<ModbusDevice>>& ModbusReader::getDevices() const
{
    return m_devices;
}

const std::map<int16_t, bool>& ModbusReader::getDeviceStatuses() const
{
    return m_deviceActiveStatus;
}

bool ModbusReader::writeMapping(RegisterMapping& mapping, const std::vector<uint16_t>& values)
{
    if (mapping.getRegisterType() != RegisterMapping::RegisterType::HOLDING_REGISTER)
    {
        throw std::logic_error("ModbusReader: You can\'t write bytes to anything other than HOLDING_REGISTER mapping.");
    }

    if (mapping.getRegisterCount() != static_cast<int16_t>(values.size()))
    {
        throw std::logic_error("ModbusReader: Received " + std::to_string(values.size()) + " values, but need " +
                               std::to_string(mapping.getRegisterCount()) + ".");
    }

    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
    {
        throw std::logic_error("ModbusReader: Mapping slave address isn\'t registered with the ModbusReader.");
    }

    if (!m_modbusClient.writeHoldingRegisters(mapping.getSlaveAddress(), mapping.getStartingAddress(),
                                              const_cast<std::vector<uint16_t>&>(values)))
    {
        std::cout << "ModbusReader: Unable to write holding register values - Register address : "
                  << mapping.getStartingAddress() << " Registers : " << values.size() << std::endl;
        mapping.setValid(false);
        return false;
    }

    mapping.update(values);
    return true;
}

bool ModbusReader::writeMapping(RegisterMapping& mapping, bool value)
{
    if (mapping.getRegisterType() != RegisterMapping::RegisterType::COIL)
    {
        throw std::logic_error("ModbusReader: You can\'t write bool to anything other than COIL mapping.");
    }

    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
    {
        throw std::logic_error("ModbusReader: Mapping slave address isn\'t registered with the ModbusReader.");
    }

    if (!m_modbusClient.writeCoil(mapping.getSlaveAddress(), mapping.getStartingAddress(), value))
    {
        std::cout << "ModbusReader: Unable to write coil value - Register address : " << mapping.getStartingAddress()
                  << " Value : " << value << std::endl;
        mapping.setValid(false);
        return false;
    }

    mapping.update(value);
    return true;
}

bool ModbusReader::writeBitMapping(RegisterMapping& mapping, bool value)
{
    if (mapping.getRegisterType() != RegisterMapping::RegisterType::HOLDING_REGISTER)
    {
        throw std::logic_error("ModbusReader: You can\'t write bit to anything other than HOLDING_REGISTER mapping.");
    }

    if (mapping.getOperationType() != RegisterMapping::OperationType::TAKE_BIT)
    {
        throw std::logic_error("ModbusReader: You can\'t write bit to mapping that isn\'t TAKE_BIT.");
    }

    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
    {
        throw std::logic_error("ModbusReader: Mapping slave address isn\'t registered with the ModbusReader.");
    }

    // Read value
    uint16_t registerValue;
    if (!m_modbusClient.readHoldingRegister(mapping.getSlaveAddress(), mapping.getStartingAddress(), registerValue))
    {
        std::cout << "ModbusReader: Unable to read holding register value - Register address : "
                  << mapping.getStartingAddress() << " for purpose of writing a bit into it." << std::endl;
        mapping.setValid(false);
        return false;
    }

    uint16_t newValue = registerValue;

    // Append bit
    const auto index = static_cast<uint8_t>(mapping.getBitIndex());
    const auto& bits = DataParsers::separateBits(registerValue);
    if (bits[index] != value)
    {
        const int delta = (1 << index) * (value ? 1 : -1);
        newValue += delta;
    }

    // Write new value
    if (registerValue != newValue)
    {
        if (!m_modbusClient.writeHoldingRegister(mapping.getSlaveAddress(), mapping.getStartingAddress(), newValue))
        {
            std::cout << "ModbusReader: Unable to write holding register bit - Register address : "
                      << mapping.getStartingAddress() << " Value : " << newValue << std::endl;
            mapping.setValid(false);
            return false;
        }
        mapping.update(value);
    }

    return false;
}

void ModbusReader::start()
{
    if (m_readerShouldRun)
        return;

    std::cout << "ModbusReader: Starting ModbusReader." << std::endl;
    // Attempt the first establishment of connection, and start the main thread.
    m_readerShouldRun = true;

    if (!m_modbusClient.isConnected())
    {
        m_modbusClient.connect();
    }

    m_mainReaderThread = std::unique_ptr<std::thread>(new std::thread(&ModbusReader::run, this));
    std::cout << "ModbusReader: Started ModbusReader." << std::endl;
}

void ModbusReader::stop()
{
    if (!m_readerShouldRun)
        return;

    std::cout << "ModbusReader: Stopping ModbusReader." << std::endl;
    // Disconnect the modbus devices, and stop the main thread.
    m_readerShouldRun = false;

    if (m_modbusClient.isConnected())
    {
        m_modbusClient.disconnect();
    }

    for (const auto& thread : m_threads)
    {
        if (thread.second != nullptr && thread.second->joinable())
            thread.second->join();
    }

    if (m_mainReaderThread->joinable())
    {
        m_mainReaderThread->join();
    }
    std::cout << "ModbusReader: Stopped ModbusReader." << std::endl;
}

void ModbusReader::run()
{
    auto threadsRunning = false;

    while (m_readerShouldRun)
    {
        if (m_shouldReconnect)
        {
            // Reconnect logic, the thread will be stuck in this if-case until
            // the modbus connection is reestablished.
            // Report all devices as non-active.
            std::cout << "ModbusReader: Attempting to reconnect." << std::endl;
            m_shouldReconnect = false;
            for (auto& device : m_deviceActiveStatus)
            {
                device.second = false;
            }
            m_modbusClient.disconnect();

            std::cout << "ModbusReader: Attempting to connect" << std::endl;
            while (!m_modbusClient.connect())
            {
                // Timing logic, increase the time after which we attempt to reconnect.
                std::this_thread::sleep_for(std::chrono::seconds(m_timeoutDurations[m_timeoutIterator]));
                if ((uint32_t)m_timeoutIterator < m_timeoutDurations.size() - 1)
                {
                    m_timeoutIterator++;
                }
            }
            m_shouldReconnect = false;
            m_timeoutIterator = 0;

            // Report all devices as active.
            for (auto& device : m_deviceActiveStatus)
            {
                device.second = true;
            }
        }
        else
        {
            if (m_modbusClient.isConnected())
            {
                std::cout << "ModbusReader: Reading devices." << std::endl;
                // Start thread foreach device, wait the readPeriod for the threads to execute, and
                // join them back in. Process if some of them reported errors, if all, go to reconnect,
                // if just some are, report them as non-active.
                if (!threadsRunning)
                {
                    for (const auto& device : m_devices)
                    {
                        m_threads[device.second->getSlaveAddress()] =
                          std::unique_ptr<std::thread>(new std::thread(&ModbusReader::readDevice, this, device.second));
                        m_deviceActiveStatus[device.second->getSlaveAddress()] = true;
                    }
                    threadsRunning = true;
                }

                std::this_thread::sleep_for(m_readPeriod);

                auto deviceRead = false;
                for (const auto& device : m_deviceActiveStatus)
                {
                    if (device.second)
                    {
                        deviceRead = true;
                        break;
                    }
                }

                if (!deviceRead)
                {
                    std::cout << "ModbusReader: No devices have been read successfully. Reconnecting..." << std::endl;
                    m_shouldReconnect = true;
                }
            }
            else
            {
                m_shouldReconnect = true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void ModbusReader::readDevice(const std::shared_ptr<ModbusDevice>& device)
{
    bool connected = true;

    while (m_readerShouldRun)
    {
        if (connected)
        {
            auto start = std::chrono::high_resolution_clock::now();

            if (device->getGroups().empty())
            {
                std::cout << "ModbusReader: Device " << device->getName() << " has no mappings." << std::endl;
                return;
            }

            std::cout << "ModbusReader: Reading device : " << device->getName() << std::endl;

            // Work on this logic, read all groups and do it properly.
            // Also, parse types and values as necessary.
            uint16_t unreadGroups = 0;

            // Read through all the groups.
            for (const auto& group : device->getGroups())
            {
                if (!ModbusGroupReader::readGroup(m_modbusClient, *group))
                {
                    std::cout << "ModbusReader: Group starting at : " << group->getStartingAddress() << " on slave "
                              << group->getSlaveAddress() << " had error while reading." << std::endl;
                    unreadGroups++;
                }
            }

            // If all the groups had error while reading, report the device as having errors.
            if (unreadGroups == device->getGroups().size())
            {
                m_deviceActiveStatus[device->getSlaveAddress()] = false;
                device->triggerOnStatusChange(false);
                connected = false;
            }
            else
            {
                m_deviceActiveStatus[device->getSlaveAddress()] = true;
                device->triggerOnStatusChange(true);
            }

            auto duration =
              std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

            if (duration.count() >= m_readPeriod.count())
            {
                std::cout << "ModbusReader: Thread read device " << device->getName()
                          << " for more than read period."
                             " Skipping sleep. Consider increasing the read period."
                          << std::endl;
            }
            else
            {
                std::this_thread::sleep_for(m_readPeriod - duration);
            }
        }
        else
        {
            if (m_deviceActiveStatus[device->getSlaveAddress()])
                connected = true;

            std::this_thread::sleep_for(m_readPeriod);
        }
    }
}
}    // namespace wolkabout
