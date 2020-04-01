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

#ifndef WOLKMODBUS_DESERIALIZERS_H
#define WOLKMODBUS_DESERIALIZERS_H

#include "RegisterMapping.h"

#include <string>

namespace wolkabout
{
/**
 * @brief Utility methods for Deserialization of various enumerations.
 * @details These are generally useful if you have to input data of this type as string
 *          from say a JSON or XML file.
 */
class Deserializers
{
public:
    /**
     * @brief Convert Modbus Register types (4 defined by Modbus protocol) from string to own enum value.
     * @param registerType string spelled exactly as the enum cases are named
     * @return corresponding register type (supported: COIL, INPUT_CONTACT, HOLDING_REGISTER, INPUT_REGISTER).
     *          Will throw logic_error if passed an invalid value.
     */
    static RegisterMapping::RegisterType deserializeRegisterType(const std::string& registerType);

    /**
     * @brief Convert Data types (7 defined in RegisterMapping class, OutputType enum) from string to own enum value.
     * @param dataType string spelled exactly as the enum cases are named
     * @return corresponding output/data type (supported: BOOL, UINT16, INT16, UINT32, INT32, FLOAT, STRING).
     *          Will throw logic_error if passed an invalid value.
     */
    static RegisterMapping::OutputType deserializeDataType(const std::string& dataType);

    /**
     * @brief Convert Operation types (7 defined in RegisterMapping class, OperationType enum) from string to own enum value.
     * @param dataType string spelled exactly as the enum cases are named
     * @return corresponding output/data type (support: NONE, MERGE_BIG_ENDIAN, MERGE_LITTLE_ENDIAN, MERGE_FLOAT,
     *          STRINGIFY_ASCII, STRINGIFY_UNICODE, TAKE_BIT).
     *          Will throw logic_error if passed an invalid value.
     */
    static RegisterMapping::OperationType deserializeOperationType(const std::string& dataType);
};
}    // namespace wolkabout

#endif    // WOLKMODBUS_DESERIALIZERS_H
