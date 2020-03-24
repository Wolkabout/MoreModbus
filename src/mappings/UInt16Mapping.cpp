//
// Created by astrihale on 24.3.20..
//

#include "UInt16Mapping.h"

#include <stdexcept>

namespace wolkabout
{
UInt16Mapping::UInt16Mapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                             bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::INPUT_REGISTER || registerType == RegisterType::HOLDING_REGISTER))
    {
        throw std::logic_error("UInt16Mapping: Illegal register type set.");
    }
}

bool UInt16Mapping::update(const std::vector<uint16_t>& newValues)
{
    m_uint16Value = newValues[0];
    return RegisterMapping::update(newValues);
}

bool UInt16Mapping::writeValue(uint16_t value)
{
    std::vector<uint16_t> bytes;
    bytes.emplace_back(value);

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_uint16Value = value;

    return success;
}

uint16_t UInt16Mapping::getUint16Value() const
{
    return m_uint16Value;
}
}    // namespace wolkabout
