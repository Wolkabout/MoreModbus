//
// Created by nvuletic on 3/11/20.
//

#include "ModbusReader.h"
#include "modbus/ModbusGroupReader.h"
#include "utility/Logger.h"

namespace wolkabout
{
ModbusReader::ModbusReader(ModbusClient& modbusClient, const std::vector<std::shared_ptr<ModbusDevice>>& devices,
                           const std::chrono::milliseconds& readPeriod)
: m_modbusClient(modbusClient), m_devices(), m_readerShouldRun(false), m_threads(), m_readPeriod(readPeriod)
{
    LOG(INFO) << "ModbusReader: Initializing ModbusReader...";
    // Initialize everything necessary for devices, assume they're at first offline,
    // and don't have any running threads.
    for (const auto& device : devices)
    {
        m_devices.emplace(device->getSlaveAddress(), device);
        m_deviceActiveStatus.emplace(device->getSlaveAddress(), false);
        m_threads.emplace(device->getSlaveAddress(), nullptr);
    }
    LOG(INFO) << "ModbusReader: ModbusReader initialized. Created " << devices.size() << " device(s).";
}

ModbusReader::~ModbusReader()
{
    stop();
}

bool ModbusReader::isRunning() const
{
    return m_readerShouldRun;
}

void ModbusReader::start()
{
    if (m_readerShouldRun)
        return;

    LOG(DEBUG) << "ModbusReader: Starting ModbusReader.";
    // Attempt the first establishment of connection, and start the main thread.
    m_readerShouldRun = true;

    if (!m_modbusClient.isConnected())
    {
        m_modbusClient.connect();
    }

    m_mainReaderThread = std::unique_ptr<std::thread>(new std::thread(&ModbusReader::run, this));
    LOG(DEBUG) << "ModbusReader: Started ModbusReader.";
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

    if (m_mainReaderThread->joinable())
    {
        m_mainReaderThread->join();
    }
    LOG(DEBUG) << "ModbusReader: Stopped ModbusReader.";
}

void ModbusReader::run()
{
    while (m_readerShouldRun)
    {
        if (m_shouldReconnect)
        {
            // Reconnect logic, the thread will be stuck in this if-case until
            // the modbus connection is reestablished.
            // Report all devices as non-active.
            LOG(TRACE) << "ModbusReader: Attempting to reconnect.";
            m_shouldReconnect = false;
            for (auto& device : m_deviceActiveStatus)
            {
                device.second = false;
            }
            m_modbusClient.disconnect();

            LOG(INFO) << "ModbusReader: Attempting to connect";
            while (!m_modbusClient.connect())
            {
                // Timing logic, increase the time after which we attempt to reconnect.
                std::this_thread::sleep_for(std::chrono::seconds(m_timeoutDurations[m_timeoutIterator]));
                if ((uint)m_timeoutIterator < m_timeoutDurations.size() - 1)
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
                LOG(DEBUG) << "ModbusReader: Reading devices.";
                // Start thread foreach device, wait the readPeriod for the threads to execute, and
                // join them back in. Process if some of them reported errors, if all, go to reconnect,
                // if just some are, report them as non-active.
                for (const auto& device : m_devices)
                {
                    m_threads[device.second->getSlaveAddress()] =
                      std::unique_ptr<std::thread>(new std::thread(&ModbusReader::readDevice, this, device.second));
                }

                std::this_thread::sleep_for(m_readPeriod);

                for (const auto& device : m_devices)
                {
                    m_threads[device.second->getSlaveAddress()]->join();
                }

                if (m_errorDevices.size() == m_devices.size())
                {
                    m_shouldReconnect = true;
                    LOG(WARN) << "ModbusReader: No devices have been read successfully. Reconnecting...";
                }
                else if (!m_errorDevices.empty())
                {
                    for (const auto& errorDevice : m_errorDevices)
                    {
                        m_deviceActiveStatus[errorDevice] = false;
                    }
                    LOG(WARN) << "ModbusReader: Some devices haven't been read successfully.";
                }
                else
                {
                    LOG(DEBUG) << "ModbusReader: All devices have been read successfully.";
                }

                m_errorDevices.clear();
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
    if (unreadGroups == device->getGroups().size())
    {
        m_errorDevices.emplace_back(device->getSlaveAddress());
    }
}
}    // namespace wolkabout
