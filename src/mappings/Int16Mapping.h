//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_INT16MAPPING_H
#define WOLKABOUT_MODBUS_INT16MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class Int16Mapping : public RegisterMapping
{
public:
    Int16Mapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                 int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(int16_t value);

    int16_t getInt16Value() const;

private:
    int16_t m_int16Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_INT16MAPPING_H
