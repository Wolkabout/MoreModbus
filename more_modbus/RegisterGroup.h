/**
 * Copyright (C) 2021 WolkAbout Technology s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef WOLKABOUT_MODBUS_REGISTERGROUP_H
#define WOLKABOUT_MODBUS_REGISTERGROUP_H

#include "more_modbus/RegisterMapping.h"

#include <map>
#include <memory>
#include <set>

namespace wolkabout
{
class ModbusDevice;

/**
 * @brief Structure containing various utility data/methods for RegisterGroup class.
 * @details Contains the helper methods for capturing address and bitIndex from the claims string.
 *         Also, the SEPARATOR being used for creating such strings, and the sort method as operator()
 *         used by the set for mappings.
 */
struct GroupUtility
{
    const static char SEPARATOR;

    /**
     * @brief Utility method to fetch the address from claim string
     * @details Address is found in the claim string before the separator
     * @param string claim - indicates the data which mapping requires
     * @return the address as int
     */
    static int32_t getAddressFromString(const std::string& string);

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
    bool operator()(const std::pair<std::string, std::shared_ptr<RegisterMapping>>& left,
                    const std::pair<std::string, std::shared_ptr<RegisterMapping>>& right)
    {
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
 *         When values are read, they're assigned to each Mapping as necessary.
 */
class RegisterGroup
{
public:
    /**
     * @brief Default constructor for the Group
     * @param mapping that will be added to the group, their RegisterType will be taken.
     */
    RegisterGroup(const std::shared_ptr<RegisterMapping>& mapping, const std::shared_ptr<ModbusDevice>& device);

    /**
     * @brief Overridden copy constructor that does deep copy of RegisterMapping instances it owns
     */
    RegisterGroup(const RegisterGroup& instance);

    /**
     * @brief Add a mapping to the group.
     * @detail Add a mapping, see if it's address and register count can be added into the group
     *        see if it can stay in continuity.
     * @param mapping
     * @return whether or not the adding is successful
     */
    bool addMapping(const std::shared_ptr<RegisterMapping>& mapping);

    RegisterMapping::RegisterType getRegisterType() const;

    /**
     * @brief Internal getter for the device that owns this groups
     * @return weak pointer to that device
     */
    std::weak_ptr<ModbusDevice> getDevice() const;

    void setDevice(const std::shared_ptr<ModbusDevice>& device);

    int32_t getStartingAddress() const;

    uint16_t getAddressCount() const;

    int16_t getSlaveAddress() const;

    void setSlaveAddress(int16_t slaveAddress);

    bool isReadRestricted() const;

    /**
     * @return all the claims strings and mappings in pairs, where the pairs
     *        are sorted by the comparer method (by address, and bit index)
     */
    const MappingsMap& getMappings() const;

    /**
     * @return all the claim strings with mapping in map, where the claims
     *        are sorted by an alphanumeric sort
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
    int16_t m_slaveAddress;
    bool m_readRestricted;

    std::weak_ptr<ModbusDevice> m_device;

    MappingsMap m_mappings;

    friend class RegisterMapping;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERGROUP_H
