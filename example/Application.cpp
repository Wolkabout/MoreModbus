/*
 * Copyright 2020 WolkAbout Technology s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mappings/BoolMapping.h"
#include "mappings/StringMapping.h"
#include "mappings/UInt16Mapping.h"
#include "ModbusDevice.h"
#include "ModbusReader.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "modbus/LibModbusSerialRtuClient.h"
#include "RegisterGroup.h"
#include "utilities/ConsoleLogger.h"

int main()
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::TRACE);
    wolkabout::Logger::setInstance(std::move(logger));

    // Create a regular Register Mapping
    const auto& normalRegisterMapping =
      std::make_shared<wolkabout::UInt16Mapping>("U16M", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    // Create a regular Discrete Mapping
    const auto& normalContactMapping =
      std::make_shared<wolkabout::BoolMapping>("BM", wolkabout::RegisterMapping::RegisterType::INPUT_CONTACT, 0);

    // Create a String Mapping
    const auto& stringMapping = std::make_shared<wolkabout::StringMapping>(
      "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t>{0, 1, 2},
      wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

    // Create some Bit Mappings
    const auto& getFirstBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0);

    const auto& getSecondBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 1);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>(
      "Test Device 1", 1,
      std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{normalRegisterMapping, normalContactMapping,
                                                               stringMapping, getFirstBitMapping, getSecondBitMapping});

    device->setOnMappingValueChange([](const std::shared_ptr<wolkabout::RegisterMapping>& mapping) {
        // You can do this for all output types.
        if (mapping->getOutputType() == wolkabout::RegisterMapping::OutputType::BOOL)
        {
            const auto& boolean = std::dynamic_pointer_cast<wolkabout::BoolMapping>(mapping);
            LOG(DEBUG) << "Application: Mapping is bool, value : " << boolean->getBoolValue();
        }
        else if (mapping->getOutputType() == wolkabout::RegisterMapping::OutputType::STRING)
        {
            const auto& string = std::dynamic_pointer_cast<wolkabout::StringMapping>(mapping);
            LOG(DEBUG) << "Application: Mapping is string, value : " << string->getStringValue();

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
      std::make_shared<wolkabout::LibModbusTcpIpClient>("192.168.0.20", 502, std::chrono::milliseconds(500));

    const auto& reader = std::make_shared<wolkabout::ModbusReader>(
      *modbusClient, std::vector<std::shared_ptr<wolkabout::ModbusDevice>>{device}, std::chrono::milliseconds(1000));

    reader->start();

    while (reader->isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
