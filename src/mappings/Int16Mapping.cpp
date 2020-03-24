//
// Created by astrihale on 24.3.20..
//

#include "Int16Mapping.h"
#include "utility/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
Int16Mapping::Int16Mapping(const std::string& reference, RegisterMapping::RegisterType registerType, int16_t address,
                           bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, address, OutputType::INT16, readRestricted, slaveAddress)
{
    if (!(registerType == RegisterType::INPUT_REGISTER || registerType == RegisterType::HOLDING_REGISTER))
    {
        throw std::logic_error("UInt16Mapping: Illegal register type set.");
    }
}

bool Int16Mapping::update(const std::vector<uint16_t>& newValues)
{
    if (m_operationType != OperationType::NONE)
        throw std::logic_error("Int16Mapping: Illegal operation type set.");

    m_int16Value = DataParsers::uint16ToInt16(newValues[0]);
    return RegisterMapping::update(newValues);
}

bool Int16Mapping::writeValue(int16_t value)
{
    std::vector<uint16_t> bytes;
    bytes.emplace_back(DataParsers::int16ToUint16(value));

    bool success = ModbusReader::getInstance()->writeToMapping(*this, bytes);
    if (success)
        m_int16Value = value;

    return success;
}

int16_t Int16Mapping::getInt16Value() const
{
    return m_int16Value;
}
}    // namespace wolkabout
