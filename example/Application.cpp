//
// Created by Nexyy on 16/03/2020.
//

#include <mappings/BoolMapping.h>
#include "ModbusDevice.h"
#include "ModbusReader.h"
#include "RegisterGroup.h"
#include "mappings/StringMapping.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "utility/ConsoleLogger.h"

int main()
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::DEBUG);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& registerMapping = std::make_shared<wolkabout::RegisterMapping>(
      "MP1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    const auto& anotherRegisterMapping = std::make_shared<wolkabout::RegisterMapping>(
      "MP2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 3);

    const auto& stringMapping = std::make_shared<wolkabout::StringMapping>(
      "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t>{0, 1, 2},
      wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

    const auto& getFirstBitMapping =
      std::make_shared<wolkabout::RegisterMapping>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER,
                                                   4, wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0);

    const auto& getSecondBitMapping =
      std::make_shared<wolkabout::RegisterMapping>("B4-2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER,
                                                   4, wolkabout::RegisterMapping::OperationType::TAKE_BIT, 1);

    const auto& fifthRegister = std::make_shared<wolkabout::RegisterMapping>(
      "MP5", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 5);

    const auto& sixthRegister = std::make_shared<wolkabout::RegisterMapping>(
      "MP6", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 8);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>(
      "Test Device 1", 1,
      std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{registerMapping, anotherRegisterMapping, stringMapping,
                                                               getFirstBitMapping, getSecondBitMapping, fifthRegister,
                                                               sixthRegister});

    device->setOnMappingValueChange([&](const wolkabout::RegisterMapping& mapping) {
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
