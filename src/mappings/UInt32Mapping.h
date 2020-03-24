//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_UINT32MAPPING_H
#define WOLKABOUT_MODBUS_UINT32MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class UInt32Mapping : public RegisterMapping
{
public:
    UInt32Mapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                  OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(uint32_t value);

    uint32_t getUint32Value() const;

private:
    uint32_t m_uint32Value{};
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_UINT32MAPPING_H
