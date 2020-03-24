//
// Created by astrihale on 24.3.20..
//

#ifndef WOLKABOUT_MODBUS_STRINGMAPPING_H
#define WOLKABOUT_MODBUS_STRINGMAPPING_H

#include "ModbusReader.h"

namespace wolkabout
{
class StringMapping : public RegisterMapping
{
public:
    StringMapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                  OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    bool update(const std::vector<uint16_t>& newValues) override;

    bool writeValue(const std::string& newValue);

    const std::string& getStringValue() const;

private:
    std::string m_stringValue;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_STRINGMAPPING_H
