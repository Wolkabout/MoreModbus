//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_FLOATMAPPING_H
#define WOLKABOUT_MODBUS_FLOATMAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class FloatMapping : public RegisterMapping
{
public:
    FloatMapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                 bool readRestricted = false, int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(float value);

    float getFloatValue() const;

private:
    float m_floatValue;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_FLOATMAPPING_H
