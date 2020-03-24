//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_INT32MAPPING_H
#define WOLKABOUT_MODBUS_INT32MAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class Int32Mapping : public RegisterMapping
{
public:
    Int32Mapping(const std::string &reference, RegisterType registerType, const std::vector<int16_t> &addresses,
                 OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t> &newValues) override;

    bool writeValue(int32_t value);

    int32_t getInt32Value() const;

private:
    int32_t m_int32Value{};
};
}

#endif //WOLKABOUT_MODBUS_INT32MAPPING_H
