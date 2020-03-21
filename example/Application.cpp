//
// Created by Nexyy on 16/03/2020.
//

#include "ModbusDevice.h"
#include "ModbusReader.h"
#include "RegisterGroup.h"
#include "modbus/LibModbusTcpIpClient.h"
#include "utility/ConsoleLogger.h"
#include "utility/DataParsers.h"

#include <bitset>

template <class T>
void printBits(T value)
{
    std::bitset<sizeof(T) * 8> set(value);
    LOG(TRACE) << set;
}

int main(int argc, char** argv)
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::TRACE);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& registerMapping = std::make_shared<wolkabout::RegisterMapping>(
      "MP1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    const auto& anotherRegisterMapping = std::make_shared<wolkabout::RegisterMapping>(
      "MP2", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 3);

    const auto& stringMapping = std::make_shared<wolkabout::RegisterMapping>(
      "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t>{0, 1, 2},
      wolkabout::RegisterMapping::OutputType::STRING, wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

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

    const auto& group = std::make_shared<wolkabout::RegisterGroup>(anotherRegisterMapping);
    group->addMapping(registerMapping);
    group->addMapping(stringMapping);
    group->addMapping(getFirstBitMapping);
    group->addMapping(getSecondBitMapping);
    group->addMapping(fifthRegister);
    group->addMapping(sixthRegister);

    const auto& device = std::make_shared<wolkabout::ModbusDevice>("Test Device 1", 1);
    device->addGroup(group);

    const auto& device2 = std::make_shared<wolkabout::ModbusDevice>("Test Device 2", 2);
    device->addGroup(group);

    const auto& modbusClient =
      std::make_shared<wolkabout::LibModbusTcpIpClient>("192.168.0.101", 502, std::chrono::milliseconds(500));

    const auto& reader = std::make_shared<wolkabout::ModbusReader>(
      *modbusClient, std::vector<std::shared_ptr<wolkabout::ModbusDevice>>{device}, std::chrono::milliseconds(1000));

    const auto& bytes = std::vector<uint16_t>{16706, 17184};
    LOG(DEBUG) << wolkabout::DataParsers::registersToAsciiString(bytes);
    LOG(DEBUG) << wolkabout::DataParsers::registersToUnicodeString(bytes);

    const auto& newBytes = std::vector<uint16_t>{32768, 0};
    LOG(DEBUG) << wolkabout::DataParsers::registersToInt32(newBytes, wolkabout::DataParsers::Endian::BIG);
    LOG(DEBUG) << wolkabout::DataParsers::registersToInt32(newBytes, wolkabout::DataParsers::Endian::LITTLE);
    LOG(DEBUG) << wolkabout::DataParsers::registersToUint32(newBytes, wolkabout::DataParsers::Endian::BIG);
    LOG(DEBUG) << wolkabout::DataParsers::registersToUint32(newBytes, wolkabout::DataParsers::Endian::LITTLE);

    const auto& array = wolkabout::DataParsers::asciiStringToRegisters("Aloha!");
    LOG(DEBUG) << wolkabout::DataParsers::registersToAsciiString(array);
    LOG(DEBUG) << wolkabout::DataParsers::registersToUnicodeString(array);

    int32_t attempt = -1;
    auto uintAttempt = static_cast<uint32_t>(attempt);

    printBits(attempt);
    printBits(uintAttempt);

    //    reader->start();
    //
    //    while (reader->isRunning())
    //    {
    //        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //    }

    return 0;
}
