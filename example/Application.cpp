//
// Created by Nexyy on 16/03/2020.
//

#include "../src/Group.h"
#include "../src/utility/ConsoleLogger.h"

int main(int argc, char** argv)
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::DEBUG);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& registerMapping = std::make_shared<wolkabout::Mapping>("MP1",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 0, 1);

    const auto& anotherRegisterMapping = std::make_shared<wolkabout::Mapping>("MP2",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 3, 1);

    const auto& stringMapping = std::make_shared<wolkabout::Mapping>("STR1",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t> {0, 1, 2}, 1,
                    wolkabout::Mapping::OutputType::STRING, wolkabout::Mapping::OperationType::STRINGIFY);

    const auto& getFirstBitMapping = std::make_shared<wolkabout::Mapping>("B4-1",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 4, 1, wolkabout::Mapping::OperationType::TAKE_BIT,
            0);

    const auto& getSecondBitMapping = std::make_shared<wolkabout::Mapping>("B4-2",
          wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 4, 1, wolkabout::Mapping::OperationType::TAKE_BIT,
          1);

    const auto& fifthRegister = std::make_shared<wolkabout::Mapping>("MP5",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 5, 1);

    const auto& sixthRegister = std::make_shared<wolkabout::Mapping>("MP6",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 8, 1);

    const auto& group = std::make_shared<wolkabout::Group>(anotherRegisterMapping);
    group->addMapping(registerMapping);
    group->addMapping(stringMapping);
    group->addMapping(getFirstBitMapping);
    group->addMapping(getSecondBitMapping);
    group->addMapping(fifthRegister);
    group->addMapping(sixthRegister);

    LOG(DEBUG) << "Aloha!";

    return 0;
}
