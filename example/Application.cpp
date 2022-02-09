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

#include "core/utilities/Logger.h"
#include "more_modbus/ModbusDevice.h"
#include "more_modbus/ModbusReader.h"
#include "more_modbus/RegisterGroup.h"
#include "more_modbus/mappings/BoolMapping.h"
#include "more_modbus/mappings/StringMapping.h"
#include "more_modbus/mappings/UInt16Mapping.h"
#include "more_modbus/modbus/LibModbusSerialRtuClient.h"
#include "more_modbus/modbus/LibModbusTcpIpClient.h"
#include "more_modbus/utilities/DataParsers.h"

int main()
{
    // Setup logger
    wolkabout::Logger::init(wolkabout::LogLevel::TRACE, wolkabout::Logger::Type::CONSOLE);

    // Create a regular Register Mapping
    auto defaultValue = std::uint16_t{123};
    const auto normalRegisterMapping = std::make_shared<wolkabout::UInt16Mapping>(
      "U16M", wolkabout::RegisterType::HOLDING_REGISTER, 0, false, -1, 0.0,
      std::chrono::milliseconds{0}, std::chrono::milliseconds{10000}, &defaultValue);

    // Create a regular Discrete Mapping
    const auto normalContactMapping =
      std::make_shared<wolkabout::BoolMapping>("BM", wolkabout::RegisterType::INPUT_CONTACT, 0);

    // Create a String Mapping
    const auto stringMapping = std::make_shared<wolkabout::StringMapping>(
      "STR1", wolkabout::RegisterType::HOLDING_REGISTER, std::vector<std::int32_t>{0, 1, 2},
      wolkabout::OperationType::STRINGIFY_ASCII, false, -1, std::chrono::milliseconds{0},
      std::chrono::milliseconds{3000}, "Hello!");

    // Create some Bit Mappings
    const auto getFirstBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-1", wolkabout::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::OperationType::TAKE_BIT, 0);

    const auto getSecondBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-2", wolkabout::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::OperationType::TAKE_BIT, 1);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>("Test Device 1", 1);

    device->createGroups(std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{
      normalRegisterMapping, normalContactMapping, stringMapping, getFirstBitMapping, getSecondBitMapping});

    device->setOnMappingValueChange([](const std::shared_ptr<wolkabout::RegisterMapping>& mapping, bool data) {
        // You can do this for all output types.
        const auto& boolean = std::dynamic_pointer_cast<wolkabout::BoolMapping>(mapping);
        LOG(DEBUG) << "Application: Mapping is bool, old value was : " << boolean->getBoolValue()
                   << ", new value is : " << data;
    });

    device->setOnMappingValueChange(
      [](const std::shared_ptr<wolkabout::RegisterMapping>& mapping, const std::vector<uint16_t>& bytes) {
          // You can do this for all output types.
          if (mapping->getOutputType() == wolkabout::OutputType::STRING)
          {
              const auto& string = std::dynamic_pointer_cast<wolkabout::StringMapping>(mapping);
              LOG(DEBUG) << "Application: Mapping is string, old value was : '" << string->getStringValue() << "'"
                         << ", new value is : '" << wolkabout::DataParsers::registersToAsciiString(bytes) << "'.";

              if (string->getStringValue().empty())
                  string->writeValue("Test");
          }
          else
          {
              LOG(DEBUG) << "Application: Mapping " << mapping->getReference() << " value changed.";
          }
      });

    device->setOnStatusChange([&](bool status) {
        LOG(DEBUG) << "Application: Device " << device->getName() << " is now " << (status ? "online" : "offline")
                   << ".";
    });

    // Serial RTU client
    //    const auto& modbusClient = std::make_shared<wolkabout::LibModbusSerialRtuClient>(
    //      "/dev/tty0", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE,
    //      std::chrono::milliseconds(500));

    // TCP IP client
    const auto& modbusClient =
      std::make_shared<wolkabout::LibModbusTcpIpClient>("127.0.0.1", 502, std::chrono::milliseconds(500));

    const auto& reader = std::make_shared<wolkabout::ModbusReader>(*modbusClient, std::chrono::milliseconds(1000));

    reader->addDevice(device);

    reader->start();

    while (reader->isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
