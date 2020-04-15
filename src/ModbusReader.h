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
/**
 * @brief Main functional class, that accepts all devices and reads them periodcally.
 * @details Function of the class is to periodically trigger the reading of all devices,
 *          of course, each and every on their separate threads. Where the connection
 *          gets split between them all, and data parsing for mappings is execute on their own
 *          thread. And also, the on value change for mappings event is invoked.
 */
class ModbusReader : public std::enable_shared_from_this<ModbusReader>
{
public:

    /**
     * @brief Main constructor for the reader, that prepares all the necessary data for reading.
     * @param modbusClient one of implementations of the abstract class
     * @param readPeriod time period for cycling reads
     */
    ModbusReader(ModbusClient& modbusClient, const std::chrono::milliseconds& readPeriod);

    virtual ~ModbusReader();

    /**
     * @brief Method that adds a single device for the reader to read.
     * @param device shared pointer to the device
     */
    void addDevice(const std::shared_ptr<ModbusDevice>& device);

    /**
     * @brief Methods that adds multiple devices for the reader to read.
     * @param devices vector containing all shared pointers for devices
     */
    void addDevices(const std::vector<std::shared_ptr<ModbusDevice>>& devices);

    /**
     * @brief Force the reader to write to a mapping (uint16_t values)
     * @param mapping reference to mapping from one of devices that the reader was passed in constructor
     * @param values array of uint16_t's, needs to contain exact amount of values as the mapping has in register count.
     * @return whether or not the operation was successful
     */
    bool writeMapping(RegisterMapping& mapping, const std::vector<uint16_t>& values);

    /**
     * @brief Force the reader to write to a mapping (bool value)
     * @param mapping reference to mapping from one of device that the reader was passed in constructor
     * @param value single boolean
     * @return whether or not the operation was successful
     */
    bool writeMapping(RegisterMapping& mapping, bool value);

    /**
     * @brief Force the reader to write to a mapping that interacts with a bit from an 16bit register
     * @param mapping reference to bit mapping from one of the device that the reader was passed in constructor
     * @param value single boolean
     * @return whether or not the operation was successful
     */
    bool writeBitMapping(RegisterMapping& mapping, bool value);

    /**
     * @brief Indicates whether or not the threads that read devices are running
     * @return thread running status
     */
    bool isRunning() const;

    const std::map<int8_t, std::shared_ptr<ModbusDevice>>& getDevices() const;

    const std::map<int8_t, bool>& getDeviceStatuses() const;

    /**
     * @brief Initializes the modbus connection, will also reconnect if it isn't working,
     *          or it disconnected in the meanwhile. If the connection is up, it will read
     *          the devices.
     */
    void start();

    /**
     * @brief Halts the modbus connection and stops all device reading threads.
     */
    void stop();

private:
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
