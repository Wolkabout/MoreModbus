/*
 *  Copyright (C) 2020 WolkAbout Technology s.r.o.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "ConsoleLogger.h"

#include <iostream>

namespace wolkabout
{
ConsoleLogger::ConsoleLogger() : m_level{LogLevel::ERROR} {}

void ConsoleLogger::logEntry(Log& log)
{
    if (static_cast<int>(log.getLogLevel()) >= static_cast<int>(m_level.load()))
    {
        switch (log.getLogLevel())
        {
        case LogLevel::TRACE:
        {
            std::cout << getFormattedDateTime() << "[T]" << log.getMessage() << "\n";
            break;
        }
        case LogLevel::DEBUG:
        {
            std::cout << getFormattedDateTime() << "[D]" << log.getMessage() << "\n";
            break;
        }
        case LogLevel::INFO:
        {
            std::cout << getFormattedDateTime() << "[I]" << log.getMessage() << "\n";
            break;
        }
        case LogLevel::WARN:
        {
            std::cout << getFormattedDateTime() << "[W]" << log.getMessage() << "\n";
            break;
        }
        case LogLevel::ERROR:
        {
            std::cout << getFormattedDateTime() << "[E]" << log.getMessage() << "\n";
            break;
        }
        }
    }
}

void ConsoleLogger::setLogLevel(wolkabout::LogLevel level)
{
    m_level = level;
}

std::string ConsoleLogger::getFormattedDateTime()
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char _retval[20];
    strftime(_retval, sizeof(_retval), "%Y-%m-%d %H:%M:%S", timeinfo);

    return _retval;
}
}    // namespace wolkabout
