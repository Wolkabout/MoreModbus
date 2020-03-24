//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_UINT16MAPPING_H
#define WOLKABOUT_MODBUS_UINT16MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class UInt16Mapping : public RegisterMapping
{
public:
    UInt16Mapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false,
                  int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(uint16_t value);

    uint16_t getUint16Value() const;

private:
    uint16_t m_uint16Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_UINT16MAPPING_H
