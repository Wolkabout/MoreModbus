//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_MODBUSREADER_H
#define WOLKABOUT_MODBUS_MODBUSREADER_H

#include "ModbusClient.h"

#include <memory>

namespace wolkabout
{
class ModbusReader
{
private:
    std::unique_ptr<ModbusClient> m_modbusClient;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSREADER_H
