/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef WOLKMODBUS_DESERIALIZERS_H
#define WOLKMODBUS_DESERIALIZERS_H

#include "more_modbus/RegisterMapping.h"

#include <string>

namespace wolkabout
{
namespace more_modbus
{
/**
 * @brief Utility methods for Deserialization of various enumerations.
 * @details These are generally useful if you have to input data of this type as string
 *         from say a JSON or XML file.
 */
class Deserializers
{
public:
    /**
     * @brief Convert Modbus Register types (4 defined by Modbus protocol) from string to own enum value.
     * @param registerType string spelled exactly as the enum cases are named
     * @return corresponding register type (supported: COIL, INPUT_CONTACT, HOLDING_REGISTER, INPUT_REGISTER).
     *         Will throw logic_error if passed an invalid value.
     */
    static RegisterType deserializeRegisterType(const std::string& registerType);

    /**
     * @brief Convert Data types (7 defined in RegisterMapping class, OutputType enum) from string to own enum value.
     * @param dataType string spelled exactly as the enum cases are named
     * @return corresponding output/data type (supported: BOOL, UINT16, INT16, UINT32, INT32, FLOAT, STRING).
     *         Will throw logic_error if passed an invalid value.
     */
    static OutputType deserializeDataType(const std::string& dataType);

    /**
     * @brief Convert Operation types (7 defined in RegisterMapping class, OperationType enum) from string to own enum
     * value.
     * @param dataType string spelled exactly as the enum cases are named
     * @return corresponding output/data type (support: NONE, MERGE_BIG_ENDIAN, MERGE_LITTLE_ENDIAN, MERGE_FLOAT,
     *         STRINGIFY_ASCII, STRINGIFY_UNICODE, TAKE_BIT).
     *         Will throw logic_error if passed an invalid value.
     */
    static OperationType deserializeOperationType(const std::string& dataType);
};
}    // namespace more_modbus
}    // namespace wolkabout

#endif    // WOLKMODBUS_DESERIALIZERS_H
