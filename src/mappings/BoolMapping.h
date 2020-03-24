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
    BoolMapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted,
                int8_t slaveAddress);

    bool writeValue(bool value);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_BOOLMAPPING_H
