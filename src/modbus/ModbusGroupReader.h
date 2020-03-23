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
    // Discrete values operations
    static void passValuesToGroup(RegisterGroup& group, const std::vector<bool>& values);
    static bool readCoilGroup(ModbusClient& modbusClient, RegisterGroup& group);
    static bool readDiscreteInputGroup(ModbusClient& modbusClient, RegisterGroup& group);
    // Register operations
    static void passValuesToGroup(RegisterGroup& group, const std::vector<uint16_t>& values);
    static bool readHoldingRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group);
    static bool readInputRegisterGroup(ModbusClient& modbusClient, RegisterGroup& group);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MODBUSGROUPREADER_H
