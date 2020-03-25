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

#ifndef WOLKMODBUS_DESERIALIZERS_H
#define WOLKMODBUS_DESERIALIZERS_H

#include "RegisterMapping.h"

#include <string>

namespace wolkabout
{
class Deserializers {
public:
    static RegisterMapping::RegisterType deserializeRegisterType(const std::string& registerType);
    static RegisterMapping::OutputType deserializeDataType(const std::string& dataType);
    static RegisterMapping::OperationType deserializeOperationType(const std::string& dataType);
};
}

#endif //WOLKMODBUS_DESERIALIZERS_H
