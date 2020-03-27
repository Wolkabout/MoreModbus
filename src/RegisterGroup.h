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

#ifndef WOLKABOUT_MODBUS_REGISTERGROUP_H
#define WOLKABOUT_MODBUS_REGISTERGROUP_H

#include "RegisterMapping.h"

#include <map>
#include <memory>
#include <set>

namespace wolkabout
{
typedef bool (*MappingCompareFunction)(const std::pair<std::string, std::shared_ptr<RegisterMapping>>& left,
                                       const std::pair<std::string, std::shared_ptr<RegisterMapping>>& right);

typedef std::set<std::pair<std::string, std::shared_ptr<RegisterMapping>>, MappingCompareFunction> MappingsMap;

// Group serves to merge multiple mappings that can be read with a single Modbus command.
// It groups Mappings of same type, that can be found next to each other.
// When values are read, they're assigned to each Mapping as necessary.
class RegisterGroup
{
public:
    const static char SEPARATOR;

    explicit RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping);

    // Overriden default copy constructor to ensure deep copy of objects which we own
    // shared pointers to. This is done because for every group, we want to have
    // copies of mappings, since they're the ones tracking values and their changes
    // for each separate register on each separate slave address.
    RegisterGroup(const RegisterGroup& instance);

    bool addMapping(const std::shared_ptr<RegisterMapping>& mapping);

    RegisterMapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    int8_t getSlaveAddress() const;

    void setSlaveAddress(int8_t slaveAddress);

    bool isReadRestricted() const;

    const MappingsMap& getMappings() const;

    std::map<std::string, std::shared_ptr<RegisterMapping>> getMappingsMap() const;

    std::vector<std::string> getMappingsClaims() const;

    static uint16_t getAddressFromString(const std::string& string);

    static int16_t getBitFromString(const std::string& string);

private:
    bool appendMapping(const std::shared_ptr<RegisterMapping>& mapping);

    static bool compareFunction(const std::pair<std::string, std::shared_ptr<RegisterMapping>>& left,
                                const std::pair<std::string, std::shared_ptr<RegisterMapping>>& right);

    bool keyExistsInSet(const std::string& key);

    RegisterMapping::RegisterType m_registerType;
    int8_t m_slaveAddress;
    bool m_readRestricted;

    MappingsMap m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERGROUP_H
