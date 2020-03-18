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

    m_modbusClient.connect();
    //    for (const auto& device : m_devices)
    //    {
    //        m_readerThreads.emplace(device.first, new std::thread(&ModbusReader::run, this));
    //    }
    LOG(DEBUG) << "Started ModbusReader.";
}

void ModbusReader::stop()
{
    if (!m_readerShouldRun)
        return;

    m_readerShouldRun = false;
    LOG(DEBUG) << "Stopping ModbusReader.";

    //    for (auto& thread : m_readerThreads)
    //    {
    //        if (thread.second->joinable())
    //        {
    //            thread.second->join();
    //        }
    //    }

    if (m_modbusClient.isConnected())
    {
        m_modbusClient.disconnect();
    }
    LOG(DEBUG) << "Stopped ModbusReader.";
}

void ModbusReader::run()
{
    while (m_readerShouldRun)
    {
        if (m_modbusClient.isConnected())
        {
            LOG(TRACE) << "Reading device ";

            // Do things with connection.
        }
        std::this_thread::sleep_for(m_readPeriod);
    }
}
}    // namespace wolkabout
