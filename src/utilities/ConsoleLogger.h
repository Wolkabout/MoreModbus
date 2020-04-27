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

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "Logger.h"

#include <atomic>

namespace wolkabout
{
/**
 * @brief Implementation of Logger class, using iostream to output to stdout.
 */
class ConsoleLogger : public Logger
{
public:
    ConsoleLogger();

    void logEntry(Log& log) override;
    void setLogLevel(wolkabout::LogLevel level) override;

private:
    static std::string getFormattedDateTime();

    std::atomic<LogLevel> m_level;
};
}    // namespace wolkabout

#endif
