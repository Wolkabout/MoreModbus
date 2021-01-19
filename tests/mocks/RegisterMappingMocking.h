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

#ifndef MOREMODBUS_REGISTERMAPPINGMOCKING_H
#define MOREMODBUS_REGISTERMAPPINGMOCKING_H

#include "RegisterMapping.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include <memory>

using namespace ::testing;

class RegisterMappingMock : public wolkabout::RegisterMapping
{
public:
    RegisterMappingMock(const std::string& name, RegisterType registerType, int32_t address)
    : RegisterMapping(name, registerType, address)
    {
    }

    RegisterMappingMock(const std::string& name, RegisterType registerType, int32_t address, int16_t slaveAddress, bool readRestricted)
      : RegisterMapping(name, registerType, address, readRestricted, slaveAddress)
    {
    }

    RegisterMappingMock(const std::string& name, RegisterType registerType, int32_t address, OutputType outputType)
    : RegisterMapping(name, registerType, address, outputType)
    {
    }

    RegisterMappingMock(const std::string& name, RegisterType registerType, int32_t address, OutputType outputType, double deadbandValue)
    : RegisterMapping(name, registerType, address, outputType, deadbandValue)
    {
    }

    RegisterMappingMock(const std::string& name, RegisterType registerType, const std::vector<int32_t>& addresses,
                        OutputType outputType, OperationType operationType)
    : RegisterMapping(name, registerType, addresses, outputType, operationType)
    {
    }

    RegisterMappingMock(const std::string& name, RegisterType registerType, int32_t address,
                        OperationType operationType, int8_t bitIndex)
    : RegisterMapping(name, registerType, address, operationType, bitIndex)
    {
    }

    MOCK_METHOD0(getGroup, const std::shared_ptr<wolkabout::RegisterGroup>&());
    MOCK_METHOD0(setGroup, const std::shared_ptr<wolkabout::RegisterGroup>&());
    MOCK_METHOD0(getReference, const std::string&());
//    MOCK_METHOD0(isReadRestricted, bool());
    MOCK_METHOD0(getRegisterType, RegisterType());
    MOCK_METHOD0(getAddress, int32_t());
    MOCK_METHOD0(getAddresses, std::vector<int32_t>&());
    MOCK_METHOD0(getSlaveAddress, int16_t());
    MOCK_METHOD0(getStartingAddress, int32_t());
    MOCK_METHOD1(setSlaveAddress, void(int16_t));
    MOCK_METHOD0(getRegisterCount, int16_t());
    MOCK_METHOD0(getOutputType, OutputType());
    MOCK_METHOD0(getOperationType, OperationType());
    MOCK_METHOD0(getBitIndex, int8_t());
    MOCK_METHOD1(update, bool(const std::vector<uint16_t>&));
    MOCK_METHOD1(doesUpdate, bool(const std::vector<uint16_t>&));
    MOCK_METHOD1(update, bool(bool));
    MOCK_METHOD0(getBytesValues, std::vector<uint16_t>&());
    MOCK_METHOD0(getBoolValue, bool());
    MOCK_METHOD0(isInitialized, bool());
    MOCK_METHOD0(isValid, bool());
    MOCK_METHOD1(setValid, void(bool));
};

#endif    // MOREMODBUS_REGISTERMAPPINGMOCKING_H
