/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
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


#include "Logger.h"

#include <algorithm>
#include <iostream>

namespace wolkabout
{
std::unique_ptr<Logger> Logger::m_instance;

void Logger::operator+=(Log& log)
{
    logEntry(log);
}

void Logger::setInstance(std::unique_ptr<Logger> instance)
{
    m_instance = std::move(instance);
}

Logger* Logger::getInstance()
{
    return m_instance.get();
}

Log::Log(LogLevel level) : m_level{level}, m_message{""} {}

LogLevel Log::getLogLevel() const
{
    return m_level;
}

std::string Log::getMessage() const
{
    return m_message.str();
}

wolkabout::LogLevel from_string(std::string level)
{
    std::transform(level.begin(), level.end(), level.begin(), ::toupper);

    if (level.compare("TRACE") == 0)
    {
        return wolkabout::LogLevel::TRACE;
    }
    else if (level.compare("DEBUG") == 0)
    {
        return wolkabout::LogLevel::DEBUG;
    }
    else if (level.compare("INFO") == 0)
    {
        return wolkabout::LogLevel::INFO;
    }
    else if (level.compare("WARN") == 0)
    {
        return wolkabout::LogLevel::WARN;
    }
    else
    {
        return wolkabout::LogLevel::ERROR;
    }
}
}    // namespace wolkabout
