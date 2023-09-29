#pragma once

/* logging.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/


#include <iostream>

#ifdef __clang__
#include <experimental/source_location>
using std::experimental::source_location;
#else
#ifdef __GNUG__ 
#include <experimental/source_location>
using std::experimental::source_location;
#else
#include <source_location>
using std::source_location;
#endif
#endif

#define FATAL_INTERNAL_ERROR_MSG(MSG)                                                                                                                          \
    {                                                                                                                                                          \
        const source_location location = source_location::current();                                                                                           \
        std::cout << "FATAL INTERNAL ERROR in <" << location.function_name() << "> at <" << location.line() << ">\n";                                          \
        std::cout << MSG;                                                                                                                                      \
        std::terminate();                                                                                                                                      \
    }

namespace helper
{
namespace logging
{

/*!
    @brief Currently supported log levels for the logging feature.
*/
enum class LogLevel
{
    /// No log level is selected.
    none = 0,
    /// Adds "Error" before the log message.
    error = 1,
    /// Adds "Warning" before the log message.
    warning = 2,
    /// Adds "Trace" before the log message.
    trace = 3
};

/**
 * @brief Initalizes the logging feature.
 * 
 * @param[in] log_level The max log level that will be written to the log.
 * @param[in] log_file_path The file path to the log file. If it is empty cout is used.
 */
void initLogging(LogLevel log_level, const std::string& log_file_path = "");

/**
 * @brief Flushes the log.
 */
void flush();

/**
 * @brief Writes an error message to the log.
 * @param[in] location The source code location. Using the default value gives the current source code location.
 * 
 * @return An ostream with the error message to the log.
 */
std::ostream& printError(const source_location location = source_location::current());

/**
 * @brief Writes a warning message to the log.
 * @param[in] location The source code location. Using the default value gives the current source code location.
 *
 * @return An ostream with the warning message to the log.
 */
std::ostream& printWarning(const source_location location = source_location::current());

/**
 * @brief Writes a trace message to the log.
 * @param[in] location The source code location. Using the default value gives the current source code location.
 *
 * @return An ostream with the trace message to the log.
 */
std::ostream& printTrace(const source_location location = source_location::current());

/**
 * @brief Prints a log message to the log.
 * 
 * @param[in] log_level The log level of the message to be written.
 * @param[in] location The source code location. Using the default value gives the current source code location.
 *
 * @return An ostream with the log message to the log.
 */
std::ostream& printLog(LogLevel log_level, const source_location location);

/**
 *  @brief Converts the enum LogLevel into string.
 *  @param[in] log_level LogLevel enum that will be converted.
 *  
 *  @return LogLevel as string.
*/
std::string logLevelToString(LogLevel log_level);

/**
 *  @brief Gets the source file name out of the path.
 *  @param[in] file_name Source file name with the complete path.
 *  
 *  @return Source file name.
*/
std::string stripSourceFileName(const std::string& file_name);

} // namespace logging
} // namespace helper