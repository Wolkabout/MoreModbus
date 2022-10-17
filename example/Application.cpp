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

using namespace wolkabout;
using namespace wolkabout::more_modbus;

int main()
{
    // Setup logger
    Logger::init(LogLevel::DEBUG, Logger::Type::CONSOLE);

    // Create a regular Register Mapping
    auto defaultValue = std::uint16_t{123};
    const auto normalRegisterMapping =
      std::make_shared<UInt16Mapping>("U16M", RegisterType::HOLDING_REGISTER, 0, false, -1, 0.0,
                                      std::chrono::milliseconds{0}, std::chrono::milliseconds{10000}, &defaultValue);

    // Create a regular Discrete Mapping
    const auto normalContactMapping = std::make_shared<BoolMapping>("BM", RegisterType::INPUT_CONTACT, 0);

    // Create a String Mapping
    const auto stringMapping =
      std::make_shared<StringMapping>("STR1", RegisterType::HOLDING_REGISTER, std::vector<std::int32_t>{0, 1, 2},
                                      OperationType::STRINGIFY_ASCII_BIG_ENDIAN, false, -1,
                                      std::chrono::milliseconds{0}, std::chrono::milliseconds{3000}, "Hello!");

    // Create some Bit Mappings
    const auto getFirstBitMapping =
      std::make_shared<BoolMapping>("B4-1", RegisterType::HOLDING_REGISTER, 4, OperationType::TAKE_BIT, 0);

    const auto getSecondBitMapping =
      std::make_shared<BoolMapping>("B4-2", RegisterType::HOLDING_REGISTER, 4, OperationType::TAKE_BIT, 1);

    const auto& device = std::make_shared<ModbusDevice>("Test Device 1", 1);

    device->createGroups(std::vector<std::shared_ptr<RegisterMapping>>{
      normalRegisterMapping, normalContactMapping, stringMapping, getFirstBitMapping, getSecondBitMapping});

    device->setOnMappingValueChange([](const std::shared_ptr<RegisterMapping>& mapping, bool data) {
        // You can do this for all output types.
        const auto& boolean = std::dynamic_pointer_cast<BoolMapping>(mapping);
        LOG(DEBUG) << "Application: Received BoolMapping update : " << (data ? "true" : "false") << ".";
    });

    device->setOnMappingValueChange(
      [](const std::shared_ptr<RegisterMapping>& mapping, const std::vector<uint16_t>& bytes) {
          // You can do this for all output types.
          if (mapping->getOutputType() == OutputType::STRING)
          {
              const auto& string = std::dynamic_pointer_cast<StringMapping>(mapping);
              LOG(DEBUG) << "Application: Received StringMapping update: '"
                         << DataParsers::registersToAsciiString(bytes) << "'.";

              if (string->getStringValue().empty())
                  string->writeValue("Test");
          }
          else
          {
              LOG(DEBUG) << "Application: Received '" << mapping->getReference() << "' update.";
          }
      });

    device->setOnStatusChange([&](bool status) {
        LOG(DEBUG) << "Application: Device " << device->getName() << " is now " << (status ? "online" : "offline")
                   << ".";
    });

    // Serial RTU client
    //    const auto& modbusClient = std::make_shared<LibModbusSerialRtuClient>(
    //      "/dev/tty0", 115200, 8, 1, LibModbusSerialRtuClient::BitParity::NONE,
    //      std::chrono::milliseconds(500));

    // TCP IP client
    const auto modbusClient = std::make_shared<LibModbusTcpIpClient>("127.0.0.1", 502, std::chrono::milliseconds(500));

    const auto reader = std::make_shared<ModbusReader>(*modbusClient, std::chrono::milliseconds(1000));

    reader->addDevice(device);

    reader->start();

    while (reader->isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
