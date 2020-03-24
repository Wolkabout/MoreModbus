//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_BOOLMAPPING_H
#define WOLKABOUT_MODBUS_BOOLMAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class BoolMapping : public RegisterMapping
{
public:
    BoolMapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                int8_t slaveAddress = -1);

    BoolMapping(const std::string& reference, RegisterType registerType, int16_t address, OperationType operation,
                int8_t bitIndex, bool readRestricted = false, int8_t slaveAddress = -1);

    bool writeValue(bool value);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_BOOLMAPPING_H
