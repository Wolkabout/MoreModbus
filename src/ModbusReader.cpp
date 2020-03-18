//
// Created by nvuletic on 3/11/20.
//

#include "ModbusReader.h"

#include "utility/Logger.h"

namespace wolkabout
{
ModbusReader::ModbusReader(ModbusClient& modbusClient, const std::vector<std::shared_ptr<ModbusDevice>>& devices,
                           const std::chrono::milliseconds& readPeriod)
: m_modbusClient(modbusClient), m_devices(), m_readerShouldRun(false), m_threads(), m_readPeriod(readPeriod)
{
    LOG(INFO) << "Initializing ModbusReader...";
    for (const auto& device : devices)
    {
        m_devices.emplace(device->getSlaveAddress(), device);
        m_deviceActiveStatus.emplace(device->getSlaveAddress(), false);
        m_threads.emplace(device->getSlaveAddress(), nullptr);
    }
    LOG(INFO) << "ModbusReader initialized. Created " << devices.size() << " device(s).";
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

    m_readerShouldRun = true;
    LOG(DEBUG) << "Starting ModbusReader.";
    if (!m_modbusClient.isConnected())
    {
        m_modbusClient.connect();
    }

    m_mainReaderThread = std::unique_ptr<std::thread>(new std::thread(&ModbusReader::run, this));
    LOG(DEBUG) << "Started ModbusReader.";
}

void ModbusReader::stop()
{
    if (!m_readerShouldRun)
        return;

    m_readerShouldRun = false;
    LOG(DEBUG) << "Stopping ModbusReader.";
    if (m_modbusClient.isConnected())
    {
        m_modbusClient.disconnect();
    }

    if (m_mainReaderThread->joinable())
    {
        m_mainReaderThread->join();
    }
    LOG(DEBUG) << "Stopped ModbusReader.";
}

void ModbusReader::run()
{
    while (m_readerShouldRun)
    {
        if (m_shouldReconnect)
        {
            LOG(TRACE) << "We should try to reconnect.";
            m_shouldReconnect = false;

            // Reconnect logic
            for (auto& device : m_deviceActiveStatus)
            {
                device.second = false;
            }
            m_modbusClient.disconnect();

            LOG(INFO) << "ModbusReader: Attempting to connect";
            while (!m_modbusClient.connect())
            {
                std::this_thread::sleep_for(std::chrono::seconds(m_timeoutDurations[m_timeoutIterator]));
                if ((uint)m_timeoutIterator < m_timeoutDurations.size() - 1)
                {
                    m_timeoutIterator++;
                }
            }
            m_shouldReconnect = false;
            m_timeoutIterator = 0;

            for (auto& device : m_deviceActiveStatus)
            {
                device.second = true;
            }
        }
        else
        {
            if (m_modbusClient.isConnected())
            {
                LOG(TRACE) << "Reading devices.";
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
    LOG(DEBUG) << "Reading device " << device->getName();
    bool value;
    if (!m_modbusClient.readCoil(device->getSlaveAddress(), 0, value))
    {
        m_shouldReconnect = true;
    }
}
}    // namespace wolkabout
