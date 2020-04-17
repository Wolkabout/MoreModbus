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

#include <gtest/gtest.h>

#include "utilities/ConsoleLogger.h"
#include "utilities/Logger.h"

namespace
{
class DataParsersTest: public ::testing::Test
{
public:
    void SetUp()
    {
        auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
        logger->setLogLevel(wolkabout::LogLevel::TRACE);
        wolkabout::Logger::setInstance(std::move(logger));
        LOG(DEBUG) << "Started tests.";
    }
};

TEST_F(DataParsersTest, SimpleTest)
{
    LOG(DEBUG) << "Hello World!";
}
}    // namespace
