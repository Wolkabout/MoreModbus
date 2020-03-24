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

BoolMapping::BoolMapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                         RegisterMapping::OperationType operation, int8_t bitIndex, bool readRestricted,
                         int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, operation, bitIndex, readRestricted, slaveAddress)
{
    if (!(operation == OperationType::TAKE_BIT))
    {
        throw std::logic_error("BoolMapping: Illegal operation type set.");
    }
}

bool BoolMapping::writeValue(bool value)
{
    bool success;
    if (m_operationType == OperationType::TAKE_BIT)
        success = ModbusReader::getInstance()->writeBitMapping(*this, value);
    else
        success = ModbusReader::getInstance()->writeMapping(*this, value);

    if (success)
        m_boolValue = value;

    return success;
}
}    // namespace wolkabout
