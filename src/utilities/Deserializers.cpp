/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
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
