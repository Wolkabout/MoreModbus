//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_MODBUSREADER_H
#define WOLKABOUT_MODBUS_MODBUSREADER_H

#include "ModbusDevice.h"
#include "modbus/ModbusClient.h"

#include <thread>

namespace wolkabout
{
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
    void run();

    // Modbus client and device data
    ModbusClient& m_modbusClient;
    std::map<int8_t, std::shared_ptr<ModbusDevice>> m_devices;

    // Threading and reader data
    std::atomic_bool m_readerShouldRun;
    std::unique_ptr<std::thread> m_mainReaderThread;
    std::map<int8_t, std::unique_ptr<std::thread>> m_readerThreads;
    std::chrono::milliseconds m_readPeriod;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSREADER_H
