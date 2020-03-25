/*
 * Copyright 2020 WolkAbout Technology s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Deserializers.h"

#include <stdexcept>

namespace wolkabout
{
RegisterMapping::RegisterType Deserializers::deserializeRegisterType(const std::string& registerType)
{
    if (registerType == "INPUT_REGISTER")
        return RegisterMapping::RegisterType::INPUT_REGISTER;
    else if (registerType == "HOLDING_REGISTER")
        return RegisterMapping::RegisterType::HOLDING_REGISTER;
    else if (registerType == "INPUT_CONTACT")
        return RegisterMapping::RegisterType::INPUT_CONTACT;
    else if (registerType == "COIL")
        return RegisterMapping::RegisterType::COIL;

    throw std::logic_error("Deserializers: Unknown register type: " + registerType);
}

RegisterMapping::OutputType Deserializers::deserializeDataType(const std::string& dataType)
{
    if (dataType == "BOOL")
        return RegisterMapping::OutputType::BOOL;
    else if (dataType == "UINT16")
        return RegisterMapping::OutputType::UINT16;
    else if (dataType == "INT16")
        return RegisterMapping::OutputType::INT16;
    else if (dataType == "UINT32")
        return RegisterMapping::OutputType::UINT32;
    else if (dataType == "INT32")
        return RegisterMapping::OutputType::INT32;
    else if (dataType == "FLOAT")
        return RegisterMapping::OutputType::FLOAT;
    else if (dataType == "STRING")
        return RegisterMapping::OutputType::STRING;

    throw std::logic_error("Deserializers: Unknown data type: " + dataType);
}

RegisterMapping::OperationType Deserializers::deserializeOperationType(const std::string& dataType)
{
    if (dataType == "NONE")
        return RegisterMapping::OperationType::NONE;
    else if (dataType == "MERGE_BIG_ENDIAN")
        return RegisterMapping::OperationType::MERGE_BIG_ENDIAN;
    else if (dataType == "MERGE_LITTLE_ENDIAN")
        return RegisterMapping::OperationType::MERGE_LITTLE_ENDIAN;
    else if (dataType == "MERGE_FLOAT")
        return RegisterMapping::OperationType::MERGE_FLOAT;
    else if (dataType == "STRINGIFY_ASCII")
        return RegisterMapping::OperationType::STRINGIFY_ASCII;
    else if (dataType == "STRINGIFY_UNICODE")
        return RegisterMapping::OperationType::STRINGIFY_UNICODE;
    else if (dataType == "TAKE_BIT")
        return RegisterMapping::OperationType::TAKE_BIT;

    throw std::logic_error("Deserializers: Unknown operation type: " + dataType);
}
}    // namespace wolkabout
