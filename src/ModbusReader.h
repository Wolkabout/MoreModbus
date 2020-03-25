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
    static ModbusReader* getInstance();

    ModbusReader(ModbusClient& modbusClient, const std::vector<std::shared_ptr<ModbusDevice>>& devices,
                 const std::chrono::milliseconds& readPeriod);

    virtual ~ModbusReader();

    bool writeMapping(RegisterMapping& mapping, const std::vector<uint16_t>& values);

    bool writeMapping(RegisterMapping& mapping, bool value);

    bool writeBitMapping(RegisterMapping& mapping, bool value);

    bool isRunning() const;

    const std::map<int8_t, std::shared_ptr<ModbusDevice>>& getDevices() const;

    const std::map<int8_t, bool>& getDeviceStatuses() const;

    void start();

    void stop();

    void setOnIterationStatuses(const std::function<void(std::map<int8_t, bool>)>& onIterationStatuses);

private:
    // Singleton
    static ModbusReader* INSTANCE;

    // Main thread, handles initializing reading of devices, their status, and the modbus connection.
    void run();

    // Does the logic of reading all devices groups, and parsing data to
    // each separate mapping as the mapping requires them.
    void readDevice(const std::shared_ptr<ModbusDevice>& device);

    std::function<void(std::map<int8_t, bool>)> m_onIterationStatuses;

    // Modbus client and device data
    ModbusClient& m_modbusClient;
    std::map<int8_t, std::shared_ptr<ModbusDevice>> m_devices;
    std::map<int8_t, bool> m_deviceActiveStatus;

    // Reconnect logic, modbusClient will after a failed read/connection,
    // try to reconnect in increasing periods of time.
    unsigned long m_timeoutIterator{};
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
