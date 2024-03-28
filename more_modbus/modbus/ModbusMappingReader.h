#ifndef MOREMODBUS_MODBUSMAPPINGREADER_H
#define MOREMODBUS_MODBUSMAPPINGREADER_H

#include "more_modbus/RegisterMapping.h"
#include "more_modbus/modbus/ModbusClient.h"

namespace wolkabout::more_modbus
{
class ModbusMappingReader
{
public:
    static bool readRegister(ModbusClient& modbusClient, RegisterMapping& registerMapping);

private:
    static bool readCoil(ModbusClient& client, RegisterMapping& mapping);

    static bool readInputContact(ModbusClient& client, RegisterMapping& mapping);

    static bool readHoldingRegister(ModbusClient& client, RegisterMapping& mapping);

    static bool readInputRegister(ModbusClient& client, RegisterMapping& mapping);
};
}    // namespace wolkabout::more_modbus

#endif    // MOREMODBUS_MODBUSMAPPINGREADER_H
