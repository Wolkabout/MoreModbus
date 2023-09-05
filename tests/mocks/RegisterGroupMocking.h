/**
 * Copyright 2023 Wolkabout Technology s.r.o.
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

#ifndef MOREMODBUS_REGISTERGROUPMOCKING_H
#define MOREMODBUS_REGISTERGROUPMOCKING_H

#include "more_modbus/RegisterGroup.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

using namespace ::testing;

typedef std::set<std::pair<std::string, std::shared_ptr<wolkabout::more_modbus::RegisterMapping>>,
                 wolkabout::more_modbus::GroupUtility>
  MappingsMap;

class RegisterGroupMock : public wolkabout::more_modbus::RegisterGroup
{
    std::string name = "MOCK";

public:
    RegisterGroupMock()
    : RegisterGroup{std::make_shared<wolkabout::more_modbus::RegisterMapping>(
                      "TEST", wolkabout::more_modbus::RegisterType::COIL, 0),
                    nullptr}
    {
    }

    RegisterGroupMock(const std::shared_ptr<wolkabout::more_modbus::RegisterMapping>& mapping,
                      const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>& device)
    : RegisterGroup(mapping, device)
    {
    }

    explicit RegisterGroupMock(const RegisterGroup& instance) : RegisterGroup(instance) {}

    MOCK_METHOD1(addMapping, bool(const std::shared_ptr<wolkabout::more_modbus::RegisterMapping>&));
    MOCK_METHOD0(getRegisterType, wolkabout::more_modbus::RegisterType());
    MOCK_METHOD0(getDevice, const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>&());
    MOCK_METHOD1(setDevice, void(const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>&));
    MOCK_METHOD0(getStartingAddress, int32_t());
    MOCK_METHOD0(getAddressCount, uint16_t());
    MOCK_METHOD0(getSlaveAddress, int16_t());
    MOCK_METHOD1(setSlaveAddress, void(int16_t));
    MOCK_METHOD0(isReadRestricted, bool());
    MOCK_CONST_METHOD0(getMappings, const MappingsMap&());
    MOCK_METHOD0(getMappingsMap, std::map<std::string, std::shared_ptr<wolkabout::more_modbus::RegisterMapping>>());
    MOCK_METHOD0(getMappingsClaims, std::vector<std::string>());
};

#endif    // MOREMODBUS_REGISTERGROUPMOCKING_H
