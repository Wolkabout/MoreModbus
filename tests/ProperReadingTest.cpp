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

#define private public
#define protected public
#include "ModbusReader.h"
#undef private
#undef protected

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#define _registerType wolkabout::RegisterMapping::RegisterType
#define _outputType wolkabout::RegisterMapping::OutputType
#define _operationType wolkabout::RegisterMapping::OperationType
#define _combination std::tuple<_registerType, _outputType, _operationType>
#define _makeCombo(x, y, z) _combination(_registerType::x, _outputType::y, _operationType::z)
#define _makeComboPure(x, y, z) _combination(x, y, z)

#include "mocks/ModbusClientMocking.h"
#include "mocks/RegisterMappingMocking.h"

class ProperReadingTest : public ::testing::Test
{
public:
    std::shared_ptr<ModbusClientMock> modbusClientMock;

    std::vector<std::shared_ptr<RegisterMappingMock>> mappings;
    std::vector<std::shared_ptr<wolkabout::ModbusDevice>> devices;

    void SetUpWinningCombos()
    {

    }

    void SetUpMappings()
    {

    }

    void SetUpDevices()
    {

    }

    void SetUp()
    {
        SetUpWinningCombos();

        modbusClientMock = std::make_shared<ModbusClientMock>();
    }
};

TEST_F(ProperReadingTest, InitializeReader)
{
    const auto& reader = std::make_shared<wolkabout::ModbusReader>(*modbusClientMock, std::chrono::milliseconds(500));
}
