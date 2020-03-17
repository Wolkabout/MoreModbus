//
// Created by nvuletic on 3/11/20.
//

#include "Mapping.h"

namespace wolkabout
{
Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType, int16_t address,
                 uint8_t slaveAddress, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_readRestricted(readRestricted)
, m_operationType(OperationType::NONE)
{
    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        m_outputType = OutputType::INT16;
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        m_outputType = OutputType::BOOL;
        break;
    }
}

Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType, int16_t address,
                 uint8_t slaveAddress, OutputType type, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_readRestricted(readRestricted)
, m_outputType(type)
, m_operationType(OperationType::NONE)
{
    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        if (m_outputType != OutputType::INT16 && m_outputType != OutputType::UINT16)
        {
            throw std::logic_error("Single address register mapping can\'t"
                                   " be anything else than INT16, UINT16.");
        }
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        if (m_outputType != OutputType::BOOL)
        {
            throw std::logic_error("Single address discrete register can\'t"
                                   " be anything else than BOOL.");
        }
        break;
    }
}

Mapping::Mapping(const std::string& reference, Mapping::RegisterType registerType, int16_t address,
                 uint8_t slaveAddress, OperationType type, int8_t bitIndex, bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
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
                 const std::vector<int16_t>& addresses, uint8_t slaveAddress, OutputType type, OperationType operation,
                 bool readRestricted)
: m_reference(reference)
, m_registerType(registerType)
, m_addresses(addresses)
, m_slaveAddress(slaveAddress)
, m_readRestricted(readRestricted)
, m_outputType(type)
, m_operationType(operation)
{
    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("Multi register mapping can\'t be COIL or INPUT_CONTACT.");
    }

    if (m_outputType == OutputType::BOOL || m_outputType == OutputType::INT16 || m_outputType == OutputType::UINT16)
    {
        throw std::logic_error("Multi register mapping can\'t be BOOL, INT16 or UINT16.");
    }

    // Can be two registers that are being merged into a 32bit, or multiple ones merging into a string.
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN || m_operationType == OperationType::MERGE_LITTLE_ENDIAN ||
        m_operationType == OperationType::MERGE_FLOAT)
    {
        if (m_addresses.size() != 2)
        {
            throw std::logic_error("Merge operations work only with 2 registers.");
        }

        if (m_outputType != OutputType::INT32 && m_outputType != OutputType::UINT32 &&
            m_outputType != OutputType::FLOAT)
        {
            throw std::logic_error("Merge operations output 32bit types (INT32, UINT32, FLOAT).");
        }
    }
    else if (m_operationType == OperationType::STRINGIFY)
    {
        if (m_outputType != OutputType::STRING)
        {
            throw std::logic_error("Stringify can only return string.");
        }
    }
}

const std::string& Mapping::getReference() const
{
    return m_reference;
}

bool Mapping::isReadRestricted() const
{
    return m_readRestricted;
}

Mapping::RegisterType Mapping::getRegisterType() const
{
    return m_registerType;
}

int16_t Mapping::getAddress() const
{
    return m_address;
}

const std::vector<int16_t>& Mapping::getAddresses() const
{
    return m_addresses;
}

uint8_t Mapping::getSlaveAddress() const
{
    return m_slaveAddress;
}

int16_t Mapping::getStartingAddress() const
{
    if (m_address == -1)
    {
        return m_addresses.at(0);
    }
    return m_address;
}

int16_t Mapping::getRegisterCount() const
{
    if (m_address == -1)
    {
        return m_addresses.size();
    }
    return 1;
}

Mapping::OutputType Mapping::getOutputType() const
{
    return m_outputType;
}

Mapping::OperationType Mapping::getOperationType() const
{
    return m_operationType;
}

int8_t Mapping::getBitIndex() const
{
    return m_bitIndex;
}
}    // namespace wolkabout
