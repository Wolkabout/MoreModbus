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

#ifndef MOREMODBUS_GROUPREADERMOCK_H
#define MOREMODBUS_GROUPREADERMOCK_H

#include "more_modbus/ModbusReader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

using namespace ::testing;

class ModbusReaderMock : public wolkabout::more_modbus::ModbusReader
{
    std::string name = "MOCK";

public:
    ModbusReaderMock(wolkabout::more_modbus::ModbusClient& client)
    : ModbusReader(client, std::chrono::milliseconds(500))
    {
    }

    MOCK_METHOD1(addDevice, void(const std::shared_ptr<wolkabout::more_modbus::ModbusDevice>&));
    MOCK_METHOD1(addDevices, void(const std::vector<std::shared_ptr<wolkabout::more_modbus::ModbusDevice>>&));
    MOCK_METHOD2(writeMapping, bool(wolkabout::more_modbus::RegisterMapping&, bool));
    MOCK_METHOD2(writeMapping, bool(wolkabout::more_modbus::RegisterMapping&, const std::vector<uint16_t>&));
    MOCK_METHOD2(writeBitMapping, bool(wolkabout::more_modbus::RegisterMapping&, bool));
    MOCK_METHOD0(isRunning, bool());
    MOCK_CONST_METHOD0(getDevices, const std::map<int16_t, std::shared_ptr<wolkabout::more_modbus::ModbusDevice>>&());
    MOCK_CONST_METHOD0(getDeviceStatuses, const std::map<int16_t, bool>&());
    MOCK_METHOD0(start, void());
    MOCK_METHOD0(stop, void());
};

#endif    // MOREMODBUS_GROUPREADERMOCK_H
