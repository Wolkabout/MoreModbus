/**
 * Copyright 2021 Wolkabout Technology s.r.o.
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

#include "more_modbus/modbus/LibModbusTcpIpClient.h"

#include "core/utilities/Logger.h"
#include "more_modbus/modbus/ModbusClient.h"

#include <cerrno>
#include <chrono>
#include <modbus/modbus.h>
#include <mutex>
#include <string>
#include <utility>

using namespace wolkabout::legacy;

namespace wolkabout
{
namespace more_modbus
{
LibModbusTcpIpClient::LibModbusTcpIpClient(std::string ipAddress, int port, std::chrono::milliseconds responseTimeout)
: ModbusClient(responseTimeout), m_ipAddress(std::move(ipAddress)), m_port(port)
{
}

LibModbusTcpIpClient::~LibModbusTcpIpClient()
{
    destroyContext();
}

bool LibModbusTcpIpClient::createContext()
{
    LOG(INFO) << "LibModbusClient: Connecting to " << m_ipAddress << ":" << m_port;

    m_modbus = modbus_new_tcp(m_ipAddress.c_str(), m_port);
    if (m_modbus == nullptr)
    {
        LOG(ERROR) << "LibModbusClient: Unable to create modbus context - " << modbus_strerror(errno);
        return false;
    }

    m_contextCreated = true;
    return true;
}

bool LibModbusTcpIpClient::destroyContext()
{
    if (m_modbus)
    {
        LOG(INFO) << "LibModbusClient: Disconnecting from " << m_ipAddress << ":" << m_port;

        std::lock_guard<decltype(m_modbusMutex)> l(m_modbusMutex);
        disconnect();
        modbus_free(m_modbus);
        m_modbus = nullptr;
        m_contextCreated = false;
    }

    return true;
}

}    // namespace more_modbus
}    // namespace wolkabout
