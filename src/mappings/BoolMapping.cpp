//
// Created by astrihale on 24.3.20..
//

#include "BoolMapping.h"

#include <stdexcept>

namespace wolkabout
{
BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::COIL || registerType == RegisterType::INPUT_CONTACT))
    {
        throw std::logic_error("BoolMapping: Illegal register type set.");
    }
}

bool BoolMapping::writeValue(bool value)
{
    bool success = ModbusReader::getInstance()->writeToMapping(*this, value);
    if (success)
        m_boolValue = value;

    return success;
}
}    // namespace wolkabout
