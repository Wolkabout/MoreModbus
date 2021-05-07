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

#include "ModbusDevice.h"
#include "ModbusReader.h"
#include "RegisterGroup.h"
#include "mappings/BoolMapping.h"
#include "mappings/StringMapping.h"
#include "mappings/UInt16Mapping.h"
#include "modbus/LibModbusSerialRtuClient.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "utilities/DataParsers.h"

#include <iostream>

int main()
{
    // Create a regular Register Mapping
    const auto& normalRegisterMapping =
      std::make_shared<wolkabout::UInt16Mapping>("U16M", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    // Create a regular Discrete Mapping
    const auto& normalContactMapping =
      std::make_shared<wolkabout::BoolMapping>("BM", wolkabout::RegisterMapping::RegisterType::INPUT_CONTACT, 0);

    // Create a String Mapping
    const auto& stringMapping = std::make_shared<wolkabout::StringMapping>(
      "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<std::int32_t>{0, 1, 2},
      wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

    // Create some Bit Mappings
    const auto& getFirstBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0);

    const auto& getSecondBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 1);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>("Test Device 1", 1);

    device->createGroups(std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{
      normalRegisterMapping, normalContactMapping, stringMapping, getFirstBitMapping, getSecondBitMapping});

    device->setOnMappingValueChange(
      [](const std::shared_ptr<wolkabout::RegisterMapping>& mapping, bool data)
      {
          // You can do this for all output types.
          const auto& boolean = std::dynamic_pointer_cast<wolkabout::BoolMapping>(mapping);
          std::cout << "Application: Mapping is bool, old value was : " << boolean->getBoolValue()
                    << ", new value is : " << data << std::endl;
      });

    device->setOnMappingValueChange(
      [](const std::shared_ptr<wolkabout::RegisterMapping>& mapping, const std::vector<uint16_t>& bytes)
      {
          // You can do this for all output types.
          if (mapping->getOutputType() == wolkabout::RegisterMapping::OutputType::STRING)
          {
              const auto& string = std::dynamic_pointer_cast<wolkabout::StringMapping>(mapping);
              std::cout << "Application: Mapping is string, old value was : '" << string->getStringValue() << "'"
                        << ", new value is : '" << wolkabout::DataParsers::registersToAsciiString(bytes) << "'."
                        << std::endl;

              if (string->getStringValue().empty())
                  string->writeValue("Test");
          }
          else
          {
              std::cout << "Application: Mapping " << mapping->getReference() << " value changed." << std::endl;
          }
      });

    device->setOnStatusChange(
      [&](bool status)
      {
          std::cout << "Application: Device " << device->getName() << " is now " << (status ? "online" : "offline")
                    << "." << std::endl;
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
