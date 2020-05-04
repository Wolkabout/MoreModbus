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

#ifndef MOREMODBUS_MODBUSCLIENTMOCKING_H
#define MOREMODBUS_MODBUSCLIENTMOCKING_H

#define private public
#define protected public
#include "modbus/ModbusClient.h"
#undef private
#undef protected

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

class ModbusClientMock : public wolkabout::ModbusClient
{
    std::string name = "MOCK";
public:
    ModbusClientMock() : ModbusClient(std::chrono::milliseconds(500))
    {
    }

    bool createContext() override { return true; }
    bool destroyContext() override { return true; }

    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(disconnect, bool());
    MOCK_METHOD0(isConnected, bool());
    MOCK_METHOD3(writeHoldingRegister, bool(int, int, uint16_t));
    MOCK_METHOD3(writeHoldingRegisters, bool(int, int, std::vector<uint16_t>));
    MOCK_METHOD3(writeCoil, bool(int, int, bool));
    MOCK_METHOD4(readInputContacts, bool(int, int, int, std::vector<bool>&));
    MOCK_METHOD4(readInputRegisters, bool(int, int, int, std::vector<uint16_t>&));
    MOCK_METHOD3(readHoldingRegister, bool(int, int, uint16_t&));
    MOCK_METHOD4(readHoldingRegisters, bool(int, int, int, std::vector<uint16_t>&));
    MOCK_METHOD3(readCoil, bool(int, int, bool&));
    MOCK_METHOD4(readCoils, bool(int, int, int, std::vector<bool>&));
    MOCK_METHOD1(changeSlaveAddress, bool(int));
};

#endif    // MOREMODBUS_MODBUSCLIENTMOCKING_H
