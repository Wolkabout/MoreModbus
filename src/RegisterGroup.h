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
struct GroupUtility {
    const static char SEPARATOR;

    /**
     * @brief Utility method to fetch the address from claim string
     * @details Address is found in the claim string before the separator
     * @param string claim - indicates the data which mapping requires
     * @return the address as int
     */
    static uint16_t getAddressFromString(const std::string& string);

    /**
     * @brief Utility method to fetch the bit from claim string
     * @details Bit index is found in the claim string after separator, value has to be in 0-15 range.
     * @param string claim - indicates the data which mapping requires
     * @return the bit index as int
     */
    static int16_t getBitFromString(const std::string& string);

    /**
     * @brief Main function that serves a purpose of sorting the elements in the set created by the group.
     */
    bool operator() (const std::pair<std::string, std::shared_ptr<RegisterMapping>>& left,
                     const std::pair<std::string, std::shared_ptr<RegisterMapping>>& right) {
        const auto addressComp = getAddressFromString(right.first) - getAddressFromString(left.first);
        if (addressComp != 0)
            return addressComp > 0;

        return getBitFromString(left.first) < getBitFromString(right.first);
    }
};

typedef std::set<std::pair<std::string, std::shared_ptr<RegisterMapping>>, GroupUtility> MappingsMap;

/**
 * @brief Group serves to merge multiple mappings that can be read with a single Modbus command.
 * @details It groups Mappings of same type, that can be found next to each other.
 *          When values are read, they're assigned to each Mapping as necessary.
 */
class RegisterGroup
{
public:

    /**
     * @brief Default constructor for the Group
     * @param mapping that will be added to the group, their RegisterType will be taken.
     */
    explicit RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping);

    // Overriden default copy constructor to ensure deep copy of objects which we own
    // shared pointers to. This is done because for every group, we want to have
    // copies of mappings, since they're the ones tracking values and their changes
    // for each separate register on each separate slave address.
    RegisterGroup(const RegisterGroup& instance);

    /**
     * @brief Add a mapping to the group.
     * @detail Add a mapping, see if it's address and register count can be added into the group
     *         see if it can stay in continuity.
     * @param mapping
     * @return whether or not the adding is successful
     */
    bool addMapping(const std::shared_ptr<RegisterMapping>& mapping);

    RegisterMapping::RegisterType getRegisterType() const;

    uint16_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    int8_t getSlaveAddress() const;

    void setSlaveAddress(int8_t slaveAddress);

    bool isReadRestricted() const;

    /**
     * @return all the claims strings and mappings in pairs, where the pairs
     *         are sorted by the comparer method (by address, and bit index)
     */
    const MappingsMap& getMappings() const;

    /**
     * @return all the claim strings with mapping in map, where the claims
     *         are sorted by an alphanumeric sort
     */
    std::map<std::string, std::shared_ptr<RegisterMapping>> getMappingsMap() const;

    /**
     * @return all the claim strings for mappings in the group.
     */
    std::vector<std::string> getMappingsClaims() const;

private:
    bool appendMapping(const std::shared_ptr<RegisterMapping>& mapping);

    bool keyExistsInSet(const std::string& key);

    RegisterMapping::RegisterType m_registerType;
    int8_t m_slaveAddress;
    bool m_readRestricted;

    MappingsMap m_mappings;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERGROUP_H
