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

#include "more_modbus/utilities/Deserializers.h"

#include <stdexcept>

namespace wolkabout
{
namespace more_modbus
{
RegisterType Deserializers::deserializeRegisterType(const std::string& registerType)
{
    if (registerType == "INPUT_REGISTER")
        return RegisterType::INPUT_REGISTER;
    else if (registerType == "HOLDING_REGISTER")
        return RegisterType::HOLDING_REGISTER;
    else if (registerType == "INPUT_CONTACT")
        return RegisterType::INPUT_CONTACT;
    else if (registerType == "COIL")
        return RegisterType::COIL;

    throw std::logic_error("Deserializers: Unknown register type: " + registerType);
}

OutputType Deserializers::deserializeDataType(const std::string& dataType)
{
    if (dataType == "BOOL")
        return OutputType::BOOL;
    else if (dataType == "UINT16")
        return OutputType::UINT16;
    else if (dataType == "INT16")
        return OutputType::INT16;
    else if (dataType == "UINT32")
        return OutputType::UINT32;
    else if (dataType == "INT32")
        return OutputType::INT32;
    else if (dataType == "FLOAT")
        return OutputType::FLOAT;
    else if (dataType == "STRING")
        return OutputType::STRING;

    throw std::logic_error("Deserializers: Unknown data type: " + dataType);
}

OperationType Deserializers::deserializeOperationType(const std::string& dataType)
{
    if (dataType == "NONE")
        return OperationType::NONE;
    else if (dataType == "MERGE_BIG_ENDIAN")
        return OperationType::MERGE_BIG_ENDIAN;
    else if (dataType == "MERGE_LITTLE_ENDIAN")
        return OperationType::MERGE_LITTLE_ENDIAN;
    else if (dataType == "MERGE_FLOAT")
        return OperationType::MERGE_FLOAT;
    else if (dataType == "STRINGIFY_ASCII")
        return OperationType::STRINGIFY_ASCII;
    else if (dataType == "STRINGIFY_UNICODE")
        return OperationType::STRINGIFY_UNICODE;
    else if (dataType == "TAKE_BIT")
        return OperationType::TAKE_BIT;

    throw std::logic_error("Deserializers: Unknown operation type: " + dataType);
}
}    // namespace more_modbus
}    // namespace wolkabout
