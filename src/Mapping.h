//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_MAPPING_H
#define WOLKABOUT_MODBUS_MAPPING_H

#include <cstdint>
#include <string>
#include <vector>

namespace wolkabout
{
class Mapping
{
public:
    enum class RegisterType
    {
        COIL = 0,
        INPUT_CONTACT,
        INPUT_REGISTER,
        HOLDING_REGISTER_ACTUATOR
    };

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

    enum class OperationType
    {
        NONE = 0,
        MERGE_BIG_ENDIAN,
        MERGE_LITTLE_ENDIAN,
        MERGE_FLOAT,
        STRINGIFY,
        TAKE_BIT
    };

    // Default case
    Mapping(const std::string& reference, RegisterType registerType, int16_t address, bool readRestricted = false);

    // Default, with override (allows HR/IR to be both UINT16 and INT16)
    Mapping(const std::string& reference, RegisterType registerType, int16_t address, OutputType type,
            bool readRestricted = false);

    // For mappings where you need to fetch a single bit off of a HR/IR.
    Mapping(const std::string& reference, RegisterType registerType, int16_t address, int8_t bitIndex,
            bool readRestricted = false);

    // For mapping where there's more registers (not discrete types).
    Mapping(const std::string& reference, RegisterType registerType, const std::vector<int16_t>& addresses,
            OutputType type, OperationType operation, bool readRestricted = false);

    const std::string& getReference() const;

    bool isReadRestricted() const;

    RegisterType getRegisterType() const;

    int16_t getAddress() const;

    const std::vector<int16_t>& getAddresses() const;

    int16_t getStartingAddress() const;

    int16_t getRegisterCount() const;

    OutputType getOutputType() const;

    OperationType getOperationType() const;

    int8_t getBitIndex() const;

private:
    // General reference data
    std::string m_reference;
    bool m_readRestricted;

    // Modbus registers data
    RegisterType m_registerType;
    int16_t m_address = -1;
    std::vector<int16_t> m_addresses;

    // Wolkabout output data
    OutputType m_outputType;
    OperationType m_operationType;
    int8_t m_bitIndex = -1;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_MAPPING_H
