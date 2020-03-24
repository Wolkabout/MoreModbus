//
// Created by astrihale on 24.3.20..
//

#include "StringMapping.h"
#include "utility/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
StringMapping::StringMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                             const std::vector<int16_t>& addresses, RegisterMapping::OperationType operation,
                             bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::STRING, operation, readRestricted, slaveAddress)
{
    if (operation != OperationType::STRINGIFY_ASCII && operation != OperationType::STRINGIFY_UNICODE)
    {
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }
}

bool wolkabout::StringMapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::STRINGIFY_ASCII:
        m_stringValue = DataParsers::registersToAsciiString(newValues);
        break;
    case OperationType ::STRINGIFY_UNICODE:
        m_stringValue = DataParsers::registersToUnicodeString(newValues);
        break;
    default:
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool StringMapping::writeValue(const std::string& newValue)
{
    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::STRINGIFY_ASCII)
        bytes = DataParsers::asciiStringToRegisters(newValue);
    else if (m_operationType == OperationType::STRINGIFY_UNICODE)
        bytes = DataParsers::unicodeStringToRegisters(newValue);
    else
        throw std::logic_error("StringMapping: Illegal operation type set.");

    bool success = ModbusReader::getInstance()->writeToMapping(*this, bytes);
    if (success)
        m_stringValue = newValue;

    return success;
}

const std::string& StringMapping::getStringValue() const
{
    return m_stringValue;
}
}    // namespace wolkabout
