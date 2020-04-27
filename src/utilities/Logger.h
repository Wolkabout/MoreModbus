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

#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <sstream>

namespace wolkabout
{
class Log;

enum class LogLevel
{
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

/**
 * @brief Converts a log level from string.
 * @param level log level as string
 * @return corresponding log level (supported: ERROR, WARN, INFO, DEBUG and
 * TRACE. All other will be considered as ERROR)
 */
wolkabout::LogLevel from_string(std::string level);

/**
 * @brief The Logger class
 *
 * Abstract class that should be extended by specific logger. Logger is accessed
 * as a singleton. But this single instance must be set via #setInstance.
 */
class Logger
{
public:
    /**
     * @brief Logger destructor.
     */
    virtual ~Logger() = default;

    /**
     * @brief Prints a log message to a standard output.
     * @param log log to be printed
     */
    void operator+=(Log& log);

    virtual void logEntry(Log& log) = 0;

    /**
     * @brief Sets the log level.
     * @param level log level to be set
     */
    virtual void setLogLevel(wolkabout::LogLevel level) = 0;

    /**
     * @brief Sets the Logger single instance
     * @param instance instance of logger to be used as singleton
     */
    static void setInstance(std::unique_ptr<Logger> instance);

    /**
     * @brief Provides a logger singleton instance.
     * @return logger instance
     */
    static Logger* getInstance();

private:
    static std::unique_ptr<Logger> m_instance;
};

/**
 * @brief The Log class
 *
 * This class holds a message that should be printed in the log file or console.
 */
class Log
{
public:
    /**
     * @brief Log constructor.
     * @param level log level that will be used for this log
     */
    Log(wolkabout::LogLevel level);

    /**
     * @brief This is log appender for string value
     * @param value value to be appended
     * @return reference to appended log instance
     */
    template <typename T> Log& operator<<(T value);

    /**
     * @brief Provides a log level for this log.
     * @return log level
     */
    wolkabout::LogLevel getLogLevel() const;

    /**
     * @brief Provides a message for this log.
     * @return message
     */
    std::string getMessage() const;

private:
    const wolkabout::LogLevel m_level;
    std::stringstream m_message;
};

template <typename T> Log& Log::operator<<(T value)
{
    m_message << value;
    return *this;
}

#define METHOD_INFO __FILE__ << ":" << __func__ << ":" << __LINE__ << ": "

#define PREPEND_NAMED_SCOPE(logLevel) wolkabout::LogLevel::logLevel

#define LOG_(level)                       \
    if (wolkabout::Logger::getInstance()) \
    (*wolkabout::Logger::getInstance()) += wolkabout::Log(level)

#define LOG(level) LOG_(PREPEND_NAMED_SCOPE(level))
}    // namespace wolkabout

#endif    // LOGGER_H
