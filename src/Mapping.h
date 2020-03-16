//
// Created by nvuletic on 3/11/20.
//

#ifndef WOLKABOUT_MODBUS_MAPPING_H
#define WOLKABOUT_MODBUS_MAPPING_H

#include "utility/json.hpp"

namespace wolkabout {
using nlohmann::json;

class Mapping {
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
        STRINGIFY,
        TAKE_BIT
    };

    // Default case
    Mapping(const std::string& reference, RegisterType registerType, uint16_t address,
            bool readRestricted = false);

    // Default, with override (allows HR/IR to be both UINT16 and INT16)
    Mapping(const std::string& reference, RegisterType registerType, uint16_t address, OutputType type,
            bool readRestricted = false);

    // For mappings where you need to fetch a single bit off of a HR/IR.
    Mapping(const std::string& reference, RegisterType registerType, uint16_t address, uint8_t bitIndex,
            bool readRestricted = false);

    // For mapping where there's more registers (not discrete types), .
    Mapping(const std::string& reference, RegisterType registerType, const std::vector<uint16_t>& addresses,
            OutputType type, OperationType operation, bool readRestricted = false);

    explicit Mapping(json::object_t json);
    explicit Mapping(const std::string& filePath);
private:
    // General reference data
    std::string m_reference;
    bool m_readRestricted;

    // Modbus registers data
    RegisterType m_registerType;
    uint16_t m_address = -1;
    std::vector<uint16_t> m_addresses;

    // Wolkabout output data
    OutputType m_outputType;
    OperationType m_operationType;
    uint8_t m_bitIndex = -1;
};
}

#endif    // WOLKABOUT_MODBUS_MAPPING_H
