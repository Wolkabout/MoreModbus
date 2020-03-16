//
// Created by nvuletic on 3/11/20.
//

#include "Mapping.h"

namespace wolkabout
{
Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType,
        uint16_t address, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_readRestricted(readRestricted)
, m_operationType(OperationType::NONE)
, m_bitIndex(0)
{
    switch (m_registerType) {
        case RegisterType::INPUT_REGISTER:
        case RegisterType::HOLDING_REGISTER_ACTUATOR:
            m_outputType = OutputType::INT16;
            break;
        case RegisterType::COIL:
        case RegisterType::INPUT_CONTACT:
            m_outputType = OutputType::BOOL;
            break;
    }
}

Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType,
        uint16_t address, OutputType type, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_readRestricted(readRestricted)
, m_operationType(OperationType::NONE)
, m_bitIndex(0)
{
    switch (m_registerType) {
        case RegisterType::INPUT_REGISTER:
        case RegisterType::HOLDING_REGISTER_ACTUATOR:
            if (type != OutputType::INT16 && type != OutputType::UINT16)
            {
                throw std::logic_error("Single address register mapping can\'t"
                                       " be anything else than INT16, UINT16.");
            }
            m_outputType = type;
            break;
        case RegisterType::COIL:
        case RegisterType::INPUT_CONTACT:
            if (type != OutputType::BOOL)
            {
                throw std::logic_error("Single address discrete register can\'t"
                                       " be anything else than BOOL.");
            }
            m_outputType = type;
            break;
    }
}

Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType,
        uint16_t address, uint8_t bitIndex, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_readRestricted(readRestricted)
, m_bitIndex(bitIndex)
, m_outputType(OutputType::BOOL)
, m_operationType(OperationType::TAKE_BIT)
{
    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("Take bit can\'t be done over COIL/INPUT_CONTACT.");
    }
}

Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType,
        const std::vector<uint16_t>& addresses, OutputType type,
        OperationType operation, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_addresses(addresses)
, m_readRestricted(readRestricted)
{
    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("Multi register mapping can\'t be COIL or INPUT_CONTACT.");
    }

    if (type == OutputType::BOOL || type == OutputType::INT16 || type == OutputType::UINT16)
    {
        throw std::logic_error("Multi register mapping can\'t be BOOl, INT16 or UINT16.");
    }
}

}
