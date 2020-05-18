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

#ifndef MOREMODBUS_MODBUSDEVICEMOCKING_H
#define MOREMODBUS_MODBUSDEVICEMOCKING_H

#include "ModbusDevice.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include <memory>

using namespace ::testing;

class ModbusDeviceMock : public wolkabout::ModbusDevice
{
    std::string name = "MOCK";
public:
    ModbusDeviceMock() : ModbusDevice("TEST", 0) { }

    MOCK_METHOD1(createGroups, void(const std::vector<std::shared_ptr<wolkabout::RegisterMapping>>&));
    MOCK_CONST_METHOD0(getName, const std::string&());
    MOCK_METHOD0(getStatus, bool());
    MOCK_METHOD0(getReader, const std::shared_ptr<wolkabout::ModbusReader>&());
    MOCK_METHOD1(setReader, void(const std::shared_ptr<wolkabout::ModbusReader>&));
    MOCK_METHOD0(getSlaveAddress, int16_t());
    MOCK_METHOD0(getGroups, const std::vector<std::shared_ptr<wolkabout::RegisterGroup>>&());
    MOCK_METHOD1(setOnMappingValueChange, void(const std::function<void(const std::shared_ptr<wolkabout::RegisterMapping>&)>&));
    MOCK_METHOD1(setOnStatusChange, void(const std::function<void(bool)>&));
    MOCK_METHOD1(triggerOnMappingValueChange, void(const std::shared_ptr<wolkabout::RegisterMapping>&));
    MOCK_METHOD1(triggerOnStatusChange, void(bool));
};

#endif    // MOREMODBUS_MODBUSDEVICEMOCKING_H
