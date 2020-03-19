//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_MODBUSREADER_H
#define WOLKABOUT_MODBUS_MODBUSREADER_H

#include "ModbusDevice.h"
#include "modbus/ModbusClient.h"

#include <atomic>
#include <functional>
#include <thread>

namespace wolkabout
{
// Main workings class. Takes all the devices, and reads them periodically.
// Separates all modbus connection and device trigger logic into a single thread.
// Runs each owns devices modbus readings and data parsing in separate threads.
// Does that periodically, dictated by the passed readPeriod parameter.
class ModbusReader
{
public:
    ModbusReader(ModbusClient& modbusClient, const std::vector<std::shared_ptr<ModbusDevice>>& devices,
                 const std::chrono::milliseconds& readPeriod);

    virtual ~ModbusReader();

    bool isRunning() const;

    void start();

    void stop();

private:
    // Main thread, handles initializing reading of devices, their status, and the modbus connection.
    void run();

    // Does the logic of reading all devices groups, and parsing data to
    // each separate mapping as the mapping requires them.
    void readDevice(const std::shared_ptr<ModbusDevice>& device);

    // Modbus client and device data
    ModbusClient& m_modbusClient;
    std::map<int8_t, std::shared_ptr<ModbusDevice>> m_devices;
    std::map<int8_t, bool> m_deviceActiveStatus;

    // Reconnect logic, modbusClient will after a failed read/connection,
    // try to reconnect in increasing periods of time.
    unsigned long m_timeoutIterator;
    const std::vector<int> m_timeoutDurations = {1, 5, 10, 15, 30, 60, 300, 600, 1800, 3600};
    // All devices that experienced an error reading all the registers, will put their
    // slave address in this vector. And they can be reported offline.
    std::vector<int8_t> m_errorDevices;
    std::atomic_bool m_shouldReconnect{};

    // Threading and reader data
    // Thread killswitch
    std::atomic_bool m_readerShouldRun{};
    // Main thread that handles connection and reconnection.
    // Per-device threads, handles modbusClient call per group and parsing of data.
    std::unique_ptr<std::thread> m_mainReaderThread;
    std::map<int8_t, std::unique_ptr<std::thread>> m_threads;
    std::chrono::milliseconds m_readPeriod;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSREADER_H
