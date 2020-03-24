//
// Created by astrihale on 24.3.20..
//

#include "FloatMapping.h"
#include "utility/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
FloatMapping::FloatMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                           const std::vector<int16_t>& addresses, bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::FLOAT, OperationType::MERGE_FLOAT, readRestricted,
                  slaveAddress)
{
}

bool FloatMapping::update(const std::vector<uint16_t>& newValues)
{
    if (m_operationType != OperationType::MERGE_FLOAT)
        throw std::logic_error("FloatMapping: Illegal operation type set.");

    m_floatValue = DataParsers::registersToFloat(newValues);
    return RegisterMapping::update(newValues);
}

bool FloatMapping::writeValue(float value)
{
    std::vector<uint16_t> bytes;

    if (m_operationType != OperationType::MERGE_FLOAT)
        throw std::logic_error("FloatMapping: Illegal operation type set.");
    bytes = DataParsers::floatToRegisters(value);

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_floatValue = value;

    return success;
}

float FloatMapping::getFloatValue() const
{
    return m_floatValue;
}
}    // namespace wolkabout
