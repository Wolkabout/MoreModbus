//
// Created by nvuletic on 3/11/20.
//

#include "ModbusReader.h"

#include "utility/Logger.h"

namespace wolkabout
{
ModbusReader::ModbusReader(ModbusClient& modbusClient, const std::vector<std::shared_ptr<ModbusDevice>>& devices,
                           const std::chrono::milliseconds& readPeriod)
: m_modbusClient(modbusClient), m_devices(), m_readerShouldRun(false), m_readPeriod(readPeriod)
{
    LOG(INFO) << "Initializing ModbusReader...";
    for (const auto& device : devices)
    {
        m_devices.emplace(device->getSlaveAddress(), device);
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
        }
        else
        {
            if (m_modbusClient.isConnected())
            {
                LOG(TRACE) << "Reading device ";

                // Do things with connection.
            }
            else
            {
                m_shouldReconnect = true;
            }
        }
        std::this_thread::sleep_for(m_readPeriod);
    }
}

void ModbusReader::readDevice(const std::shared_ptr<ModbusDevice>& device) {}
}    // namespace wolkabout
