//
// Created by Nexyy on 16/03/2020.
//

#include "../src/RegisterGroup.h"
#include "../src/utility/ConsoleLogger.h"

int main(int argc, char** argv)
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::DEBUG);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& registerMapping = std::make_shared<wolkabout::RegisterMapping>("MP1",
                                                                               wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

    const auto& anotherRegisterMapping = std::make_shared<wolkabout::RegisterMapping>("MP2",
                                                                                      wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 3);

    const auto& stringMapping = std::make_shared<wolkabout::RegisterMapping>("STR1",
                                                                             wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t> {0, 1, 2},
                                                                             wolkabout::RegisterMapping::OutputType::STRING, wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);

    const auto& getFirstBitMapping = std::make_shared<wolkabout::RegisterMapping>("B4-1",
                                                                                  wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4, wolkabout::RegisterMapping::OperationType::TAKE_BIT,
                                                                                  0);

    const auto& getSecondBitMapping = std::make_shared<wolkabout::RegisterMapping>("B4-2",
                                                                                   wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4, wolkabout::RegisterMapping::OperationType::TAKE_BIT,
                                                                                   1);

    const auto& fifthRegister = std::make_shared<wolkabout::RegisterMapping>("MP5",
                                                                             wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 5);

    const auto& sixthRegister = std::make_shared<wolkabout::RegisterMapping>("MP6",
                                                                             wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 8);

    const auto& group = std::make_shared<wolkabout::RegisterGroup>(anotherRegisterMapping);
    group->addMapping(registerMapping);
    group->addMapping(stringMapping);
    group->addMapping(getFirstBitMapping);
    group->addMapping(getSecondBitMapping);
    group->addMapping(fifthRegister);
    group->addMapping(sixthRegister);

    return 0;
}
