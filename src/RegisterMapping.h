//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_REGISTERMAPPING_H
#define WOLKABOUT_MODBUS_REGISTERMAPPING_H

#include <cstdint>
#include <string>
#include <vector>

namespace wolkabout
{
// Mapping is a single unit involving one, part of, or multiple Modbus registers.
// Their values are being interpreted in one of available OutputValues types.
class RegisterMapping
{
public:
    // Enumeration used to represent all Modbus register types.
    enum class RegisterType
    {
        COIL = 0,
        INPUT_CONTACT,
        INPUT_REGISTER,
        HOLDING_REGISTER
    };

    // Enumeration used to represent the Outputting type of Mapping.
    enum class OutputType
    {
        BOOL = 0,
        STRING,
        INT16,
        UINT16,
        INT32,
        UINT32,
        FLOAT
    };

    // Enumeration used to represent the operation done
    // over data when it's being written or read.
    enum class OperationType
    {
        NONE = 0,
        MERGE_BIG_ENDIAN,
        MERGE_LITTLE_ENDIAN,
        MERGE_FLOAT,
        STRINGIFY_ASCII,
        STRINGIFY_UNICODE,
        TAKE_BIT
    };

    // Default case
    // Applies default operation, where COIL/INPUT_CONTACT registers are interpreted as BOOL,
    // and HOLDING_REGISTER/INPUT_REGISTER registers are interpreted as INT16.
    RegisterMapping(const std::string& reference, RegisterType registerType, int16_t address,
                    bool readRestricted = false, int8_t slaveAddress = -1);

    // Default case
    // But allows HOLDING_REGISTER/INPUT_REGISTER to be INT16 or UINT16 with given argument.
    RegisterMapping(const std::string& reference, RegisterType registerType, int16_t address, OutputType type,
                    bool readRestricted = false, int8_t slaveAddress = -1);

    // Bit fetch case
    // Choose a HOLDING_REGISTER/INPUT_REGISTER, and take a bit off of it, and return as BOOL.
    RegisterMapping(const std::string& reference, RegisterType registerType, int16_t address, OperationType operation,
                    int8_t bitIndex, bool readRestricted = false, int8_t slaveAddress = -1);

    // For mapping where there's more registers (not discrete types).
    // Can be used in case where you need to merge 2 registers into 32 bit types (such as UINT32, INT32, FLOAT)
    // with MERGE_BIG_ENDIAN, MERGE_LITTLE_ENDIAN or MERGE_FLOAT.
    // Can be also used to merge strings, take `n` amount of registers, interpreting each REGISTER
    // as two ASCII characters, two UNICODE characters or one UTF-16 character.
    RegisterMapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
                    OutputType type, OperationType operation, bool readRestricted = false, int8_t slaveAddress = -1);

    const std::string& getReference() const;

    bool isReadRestricted() const;

    RegisterType getRegisterType() const;

    int16_t getAddress() const;

    const std::vector<int16_t>& getAddresses() const;

    int8_t getSlaveAddress() const;

    int16_t getStartingAddress() const;

    void setSlaveAddress(int8_t slaveAddress);

    int16_t getRegisterCount() const;

    OutputType getOutputType() const;

    OperationType getOperationType() const;

    int8_t getBitIndex() const;

    bool update(const std::vector<uint16_t>& newValues);

    bool update(bool newRegisterValue);

    const std::vector<uint16_t>& getBytesValues() const;

    bool getBoolValue() const;

    void setValid(bool valid);

private:
    // General mapping data
    std::string m_reference;
    bool m_readRestricted;

    // Modbus registers data
    RegisterType m_registerType;
    int16_t m_address = -1;
    std::vector<int16_t> m_addresses;
    int8_t m_slaveAddress = -1;

    // Wolkabout output data
    OutputType m_outputType;
    OperationType m_operationType;
    int8_t m_bitIndex = -1;

    // Value change watching logic
    bool m_boolValue{};
    std::vector<uint16_t> m_byteValues;
    bool m_isInitialized = false;
    bool m_isValid = false;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERMAPPING_H
