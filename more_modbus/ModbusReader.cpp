/**
 * Copyright 2023 Wolkabout Technology s.r.o.
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

#include "more_modbus/ModbusReader.h"

#include "core/utilities/Logger.h"
#include "more_modbus/modbus/ModbusGroupReader.h"
#include "more_modbus/utilities/DataParsers.h"

#include <algorithm>

using namespace wolkabout::legacy;

namespace wolkabout
{
namespace more_modbus
{
ModbusReader::ModbusReader(ModbusClient& modbusClient, const std::chrono::milliseconds& readPeriod)
: m_modbusClient(modbusClient), m_devices(), m_readerShouldRun(false), m_threads(), m_readPeriod(readPeriod)
{
}

ModbusReader::~ModbusReader()
{
    stop();
}

void ModbusReader::addDevice(const std::shared_ptr<ModbusDevice>& device)
{
    m_devices.emplace(device->getSlaveAddress(), device);
    m_deviceActiveStatus.emplace(device->getSlaveAddress(), false);
    m_threads.emplace(device->getSlaveAddress(), nullptr);
    m_rewriteThreads.emplace(device->getSlaveAddress(), nullptr);
    device->setReader(shared_from_this());
    LOG(INFO) << "ModbusReader: Successfully added new device " << device->getName();
}

void ModbusReader::addDevices(const std::vector<std::shared_ptr<ModbusDevice>>& devices)
{
    LOG(INFO) << "ModbusReader: Adding " << devices.size() << " devices.";
    for (const auto& device : devices)
    {
        addDevice(device);
    }
    LOG(INFO) << "ModbusReader: Successfully added " << devices.size() << " devices.";
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
    std::lock_guard<std::mutex> lockGuard{m_deviceActiveMutex};
    return m_deviceActiveStatus;
}

bool ModbusReader::writeMapping(RegisterMapping& mapping, const std::vector<uint16_t>& values)
{
    if (mapping.getRegisterType() != RegisterType::HOLDING_REGISTER)
        throw std::logic_error(
          R"(ModbusReader: You can't write bytes to anything other than HOLDING_REGISTER mapping.)");
    if (mapping.getRegisterCount() != static_cast<int16_t>(values.size()))
        throw std::logic_error("ModbusReader: Received " + std::to_string(values.size()) + " values, but need " +
                               std::to_string(mapping.getRegisterCount()) + ".");
    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
        throw std::logic_error(R"(ModbusReader: Mapping slave address isn't registered with the ModbusReader.)");

    if (!m_modbusClient.writeHoldingRegisters(mapping.getSlaveAddress(), mapping.getStartingAddress(),
                                              const_cast<std::vector<uint16_t>&>(values)))
    {
        LOG(WARN) << "ModbusReader: Unable to write holding register values - Register address : "
                  << mapping.getStartingAddress() << " Registers : " << values.size();
        mapping.setValid(false);
        return false;
    }
    LOG(TRACE) << "ModbusReader: Written value for mapping '" << mapping.getReference() << "'.";
    if (mapping.isAutoUpdateEnabled())
        mapping.update(values);
    return true;
}

bool ModbusReader::writeMapping(RegisterMapping& mapping, bool value)
{
    if (mapping.getRegisterType() != RegisterType::COIL)
        throw std::logic_error(R"(ModbusReader: You can't write bool to anything other than COIL mapping.)");
    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
        throw std::logic_error(R"(ModbusReader: Mapping slave address isn't registered with the ModbusReader.)");

    if (!m_modbusClient.writeCoil(mapping.getSlaveAddress(), mapping.getStartingAddress(), value))
    {
        LOG(WARN) << "ModbusReader: Unable to write coil value - Register address : " << mapping.getStartingAddress()
                  << " Value : " << value;
        mapping.setValid(false);
        return false;
    }
    LOG(TRACE) << "ModbusReader: Written value for mapping '" << mapping.getReference() << "'.";
    mapping.update(value);
    return true;
}

bool ModbusReader::writeBitMapping(RegisterMapping& mapping, bool value)
{
    if (mapping.getRegisterType() != RegisterType::HOLDING_REGISTER)
        throw std::logic_error(R"(ModbusReader: You can't write bit to anything other than HOLDING_REGISTER mapping.)");
    if (mapping.getOperationType() != OperationType::TAKE_BIT)
        throw std::logic_error(R"(ModbusReader: You can't write bit to mapping that isn't TAKE_BIT.)");
    if (m_devices.find(mapping.getSlaveAddress()) == m_devices.end())
        throw std::logic_error(R"(ModbusReader: Mapping slave address isn't registered with the ModbusReader.)");

    // Read value
    uint16_t registerValue;
    if (!m_modbusClient.readHoldingRegister(mapping.getSlaveAddress(), mapping.getStartingAddress(), registerValue))
    {
        LOG(WARN) << "ModbusReader: Unable to read holding register value - Register address : "
                  << mapping.getStartingAddress() << " for purpose of writing a bit into it.";
        mapping.setValid(false);
        return false;
    }
    uint16_t newValue = registerValue;

    // Append bit
    const auto index = static_cast<uint8_t>(mapping.getBitIndex());
    const auto& bits = DataParsers::separateBits(registerValue);
    if (bits[index] != value)
        newValue += (1 << index) * (value ? 1 : -1);

    // Write new value
    if (registerValue != newValue)
    {
        if (!m_modbusClient.writeHoldingRegister(mapping.getSlaveAddress(), mapping.getStartingAddress(), newValue))
        {
            LOG(WARN) << "ModbusReader: Unable to write holding register bit - Register address : "
                      << mapping.getStartingAddress() << " Value : " << newValue;
            mapping.setValid(false);
            return false;
        }
        mapping.update(value);
    }
    return true;
}

bool ModbusReader::start()
{
    if (m_readerShouldRun)
        return true;

    LOG(DEBUG) << "ModbusReader: Starting ModbusReader.";
    // Attempt the first establishment of connection, and start the main thread.
    m_readerShouldRun = true;
    auto connected = m_modbusClient.isConnected();
    if (!connected)
    {
        connected = m_modbusClient.connect();
        if (connected)
            LOG(DEBUG) << "ModbusReader: Connected ModbusClient.";
        else
            LOG(ERROR) << "ModbusReader: Failed to start - Modbus connection failed to establish.";
    }
    if (connected && m_mainReaderThread == nullptr)
    {
        m_mainReaderThread = std::unique_ptr<std::thread>(new std::thread(&ModbusReader::run, this));
        LOG(DEBUG) << "ModbusReader: Started ModbusReader.";
    }
    return connected;
}

void ModbusReader::stop()
{
    if (!m_readerShouldRun)
        return;

    LOG(DEBUG) << "ModbusReader: Stopping ModbusReader.";
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
    for (const auto& thread : m_rewriteThreads)
    {
        if (thread.second != nullptr && thread.second->joinable())
            thread.second->join();
    }

    if (m_mainReaderThread != nullptr && m_mainReaderThread->joinable())
    {
        m_mainReaderThread->join();
    }
    LOG(DEBUG) << "ModbusReader: Stopped ModbusReader.";
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
            LOG(INFO) << "ModbusReader: Attempting to reconnect.";
            m_shouldReconnect = false;
            for (auto& device : m_deviceActiveStatus)
                device.second = false;
            for (const auto& device : m_devices)
            {
                LOG(INFO) << "DeviceStatus: '" << device.second->getName() << "': " << false;

                triggerDeviceStatusUpdate(device.second, false);
            }
            m_modbusClient.disconnect();

            LOG(INFO) << "ModbusReader: Attempting to connect";
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
                device.second = true;
            for (const auto& device : m_devices)
            {
                LOG(INFO) << "DeviceStatus: '" << device.second->getName() << "': " << true;

                triggerDeviceStatusUpdate(device.second, true);
            }
        }
        else
        {
            if (m_modbusClient.isConnected())
            {
                LOG(TRACE) << "ModbusReader: Reading devices.";
                // Start thread foreach device, wait the readPeriod for the threads to execute, and
                // join them back in. Process if some of them reported errors, if all, go to reconnect,
                // if just some are, report them as non-active.
                if (!threadsRunning)
                {
                    for (const auto& device : m_devices)
                    {
                        m_threads[device.second->getSlaveAddress()] =
                          std::unique_ptr<std::thread>(new std::thread(&ModbusReader::readDevice, this, device.second));
                        m_rewriteThreads[device.second->getSlaveAddress()] = std::unique_ptr<std::thread>(
                          new std::thread(&ModbusReader::rewriteDevice, this, device.second));
                        m_deviceActiveStatus[device.second->getSlaveAddress()] = true;
                    }
                    threadsRunning = true;
                }

                std::this_thread::sleep_for(m_readPeriod);

                auto deviceRead = false;
                {
                    auto map = std::map<int16_t, bool>{};
                    {
                        std::lock_guard<std::mutex> lockGuard{m_deviceActiveMutex};
                        map = m_deviceActiveStatus;
                    }

                    for (const auto& device : map)
                    {
                        if (device.second)
                        {
                            deviceRead = true;
                            break;
                        }
                    }
                }

                if (!deviceRead)
                {
                    LOG(WARN) << "ModbusReader: No devices have been read successfully. Reconnecting...";
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
    while (m_readerShouldRun)
    {
        auto start = std::chrono::high_resolution_clock::now();

        if (device->getGroups().empty())
        {
            LOG(WARN) << "ModbusReader: Device " << device->getName() << " has no mappings.";
            return;
        }

        LOG(TRACE) << "ModbusReader: Reading device : " << device->getName();

        // Work on this logic, read all groups and do it properly.
        // Also, parse types and values as necessary.
        uint16_t unreadGroups = 0;

        // Read through all the groups.
        for (const auto& group : device->getGroups())
        {
            if (!ModbusGroupReader::readGroup(m_modbusClient, *group))
            {
                LOG(WARN) << "ModbusReader: Group starting at : " << group->getStartingAddress() << " on slave "
                          << group->getSlaveAddress() << " had error while reading.";
                unreadGroups++;
            }
        }

        // If all the groups had error while reading, report the device as having errors.
        const auto status = unreadGroups != device->getGroups().size();
        std::lock_guard<std::mutex> lockGuard{m_deviceActiveMutex};
        if (m_deviceActiveStatus[device->getSlaveAddress()] != status ||
            !m_deviceStatusReported[device->getSlaveAddress()])
        {
            triggerDeviceStatusUpdate(device, status);
        }

        auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

        if (duration.count() >= m_readPeriod.count())
        {
            LOG(WARN) << "ModbusReader: Thread read device " << device->getName()
                      << " for more than read period."
                         " Skipping sleep. Consider increasing the read period.";
        }
        else
        {
            std::this_thread::sleep_for(m_readPeriod - duration);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ModbusReader::rewriteDevice(const std::shared_ptr<ModbusDevice>& device)
{
    bool connected = true;

    while (m_readerShouldRun)
    {
        if (device->getRewritable().empty())
        {
            LOG(WARN) << "ModbusReader: Device " << device->getName() << " has no rewritable mappings.";
            return;
        }

        // Count the amount of mappings that needed to be rewritten, and the ones that succeeded
        auto requiredMappings = std::uint64_t{0};
        auto succeededMappings = std::uint64_t{0};

        // Read through all the rewritable mappings.
        for (const auto& rewritable : device->getRewritable())
        {
            // Check that the value is not
            const auto repeatedWriteTime = rewritable->getRepeatedWrite();
            if (repeatedWriteTime.count() == 0)
            {
                continue;
            }

            const auto nextUpdateTime = rewritable->getLastUpdateTime() + repeatedWriteTime;
            const auto diff = (std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::high_resolution_clock::now() - nextUpdateTime))
                                .count();
            if (diff > 0)
            {
                ++requiredMappings;

                // Write the current value in
                if (rewritable->getRegisterType() == RegisterType::COIL)
                {
                    if (m_modbusClient.writeCoil(rewritable->getSlaveAddress(), rewritable->getStartingAddress(),
                                                 rewritable->getBoolValue()))
                    {
                        LOG(DEBUG) << "Successfully rewrote '" << rewritable->getReference() << "' - " << diff << "ms.";
                        rewritable->update(rewritable->getBoolValue());
                        ++succeededMappings;
                    }
                    else
                    {
                        LOG(DEBUG) << "Failed to rewrite '" << rewritable->getReference() << "'.";
                    }
                }
                else
                {
                    if (rewritable->getRegisterCount() == 1)
                    {
                        if (m_modbusClient.writeHoldingRegister(rewritable->getSlaveAddress(),
                                                                rewritable->getStartingAddress(),
                                                                rewritable->getBytesValues().front()))
                        {
                            LOG(TRACE) << "Successfully rewrote '" << rewritable->getReference() << "' - " << diff
                                       << "ms.";
                            rewritable->update(rewritable->getBytesValues());
                            ++succeededMappings;
                        }
                        else
                        {
                            LOG(WARN) << "Failed to rewrite '" << rewritable->getReference() << "'.";
                        }
                    }
                    else
                    {
                        if (m_modbusClient.writeHoldingRegisters(
                              rewritable->getSlaveAddress(), rewritable->getStartingAddress(),
                              const_cast<std::vector<std::uint16_t>&>(rewritable->getBytesValues())))
                        {
                            LOG(DEBUG) << "Successfully rewrote '" << rewritable->getReference() << "' - " << diff
                                       << "ms.";
                            rewritable->update(rewritable->getBytesValues());
                            ++succeededMappings;
                        }
                        else
                        {
                            LOG(DEBUG) << "Failed to rewrite '" << rewritable->getReference() << "'.";
                        }
                    }
                }
            }
        }

        // If all the groups had error while reading, report the device as having errors.
        if (requiredMappings > 0)
        {
            const auto status = succeededMappings > 0;
            std::lock_guard<std::mutex> lockGuard{m_deviceActiveMutex};
            if (m_deviceActiveStatus[device->getSlaveAddress()] != status ||
                !m_deviceStatusReported[device->getSlaveAddress()])
            {
                triggerDeviceStatusUpdate(device, status);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ModbusReader::triggerDeviceStatusUpdate(const std::shared_ptr<ModbusDevice>& device, bool status)
{
    m_deviceActiveStatus[device->getSlaveAddress()] = status;
    device->triggerOnStatusChange(status);

    m_deviceStatusReported[device->getSlaveAddress()] = true;
}
}    // namespace more_modbus
}    // namespace wolkabout
