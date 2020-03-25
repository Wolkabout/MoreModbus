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

#include "ModbusDevice.h"
#include "ModbusReader.h"
#include "RegisterGroup.h"
#include "mappings/BoolMapping.h"
#include "mappings/StringMapping.h"
#include "mappings/UInt16Mapping.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "utilities/ConsoleLogger.h"

int main()
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::DEBUG);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& registerMapping =
      std::make_shared<wolkabout::UInt16Mapping>("MP1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    const auto& anotherRegisterMapping =
      std::make_shared<wolkabout::UInt16Mapping>("MP2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 3);

    const auto& stringMapping = std::make_shared<wolkabout::StringMapping>(
      "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t>{0, 1, 2},
      wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

    const auto& getFirstBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0);

    const auto& getSecondBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 1);

    const auto& fifthRegister =
      std::make_shared<wolkabout::UInt16Mapping>("MP5", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 5);

    const auto& sixthRegister =
      std::make_shared<wolkabout::UInt16Mapping>("MP6", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 8);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>(
      "Test Device 1", 1,
      std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{registerMapping, anotherRegisterMapping, stringMapping,
                                                               getFirstBitMapping, getSecondBitMapping, fifthRegister,
                                                               sixthRegister});

    device->setOnMappingValueChange([](const wolkabout::RegisterMapping& mapping) {
        // You can do this for all output types.
        if (mapping.getOutputType() == wolkabout::RegisterMapping::OutputType::BOOL)
        {
            auto& boolean = (wolkabout::BoolMapping&)mapping;
            LOG(DEBUG) << "Application: Mapping is bool, value : " << boolean.getBoolValue();
            boolean.writeValue(false);
        }
        else if (mapping.getOutputType() == wolkabout::RegisterMapping::OutputType::STRING)
        {
            auto& string = (wolkabout::StringMapping&)mapping;
            LOG(DEBUG) << "Application: Mapping is string, value : " << string.getStringValue();
            string.writeValue("Fruit!");
        }
        else
        {
            LOG(DEBUG) << "Application: Mapping " << mapping.getReference() << " value changed.";
        }
    });

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
