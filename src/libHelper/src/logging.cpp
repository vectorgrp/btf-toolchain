/* logging.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/


#include "helper/logging.h"

#include <fstream>
#include <memory>
#include <filesystem>

namespace helper::logging
{

/*!
    @brief Class for the Logger.
*/
class GlobalLogger
{
  public:
    /*!
      @brief Constructor of the class GlobalLogger.
    */
    GlobalLogger() noexcept
    {
        try
        {
            nullout_ = std::make_unique<std::ofstream>();
        }
        catch (...)
        {
            std::puts("GlobalLogger: could not create null logger\n");
            std::terminate();
        }
    }

    /*!
      @brief Opens the log file.
      @param[in] path Path to the log file.
    */
    void openLogFile(const std::string& path)
    {
        log_file_ = std::make_unique<std::ofstream>(path);
        if (!log_file_->good())
        {
            closeLogFile();
        }
    }

    /*!
      @brief Closes the log file.
    */
    void closeLogFile() noexcept
    {
        log_file_.reset(nullptr);
    }

    /*!
      @brief Gets the logger (ostream).
      @return The log file, if available, else cout.
    */
    std::ostream& getLogger() noexcept
    {
        if (log_file_ != nullptr)
        {
            return *log_file_;
        }
        return std::cout;
    }

    /*!
      @brief Gets the null logger (ostream).
      @return The null logger.
    */
    std::ostream& getNullLogger() noexcept
    {
        return *nullout_;
    }

  private:
    std::unique_ptr<std::ofstream> log_file_;
    std::unique_ptr<std::ofstream> nullout_;
};

static LogLevel glog_level{LogLevel::warning};
static GlobalLogger glogger;

void initLogging(LogLevel log_level, const std::string& log_file_path)
{
    glog_level = log_level;
    if (!log_file_path.empty())
    {
        glogger.openLogFile(log_file_path);
    }
}

std::ostream& printError(const source_location location)
{
    return printLog(LogLevel::error, location);
}

std::ostream& printWarning(const source_location location)
{
    return printLog(LogLevel::warning, location);
}

std::ostream& printTrace(const source_location location)
{
    return printLog(LogLevel::trace, location);
}

std::string logLevelToString(LogLevel log_level)
{
    switch (log_level)
    {
    case LogLevel::error:
        return "Error";
    case LogLevel::warning:
        return "Warning";
    case LogLevel::trace:
        return "Trace";
    case LogLevel::none:
        return "";
    }
    return "";
}

std::string stripSourceFileName(const std::string& file_name)
{
    const std::string_view root_project_folder_name{"btf-toolchain"};
    auto pos{file_name.rfind(root_project_folder_name)};
    if (pos != std::string::npos)
    {
        return file_name.substr(pos + root_project_folder_name.length() + 1);
    }
    return file_name;
}

std::ostream& printLog(LogLevel log_level, const source_location location)
{
    if (glog_level >= log_level)
    {
        glogger.getLogger() << logLevelToString(log_level) << " [file: " << stripSourceFileName(location.file_name()) << ": " << location.line() << " `"
                            << location.function_name() << "`] ";
        return glogger.getLogger();
    }
    return glogger.getNullLogger();
}

void flush()
{
    glogger.getLogger() << std::flush;
}

} // namespace helper::logging