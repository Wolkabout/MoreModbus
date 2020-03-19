//
// Created by astrihale on 19.3.20..
//

#ifndef WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
#define WOLKABOUT_MODBUS_MODBUSGROUPREADER_H

#include "ModbusClient.h"
#include "RegisterGroup.h"

namespace wolkabout
{
class ModbusGroupReader
{
public:
    static bool readGroup(ModbusClient& modbusClient, RegisterGroup& group);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
