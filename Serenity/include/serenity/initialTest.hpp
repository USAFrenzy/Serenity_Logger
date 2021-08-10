#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <map>
#include <string>

// Just an Initial Test function
void PrintHello();
// currently in global namespace
using MappedLevel = spdlog::level::level_enum;

class Logger
{   
    public:
    Logger();
    ~Logger();
    static void Init();
    void SetLoggerLevel(LoggerLevel logLevel);

    static std::shared_ptr<spdlog::logger>& GetInternalLogger() {return m_internalLogger;}
    static std::shared_ptr<spdlog::logger>& GetClientSideLogger() {return m_clientLogger; }

    private:
    inline static MappedLevel MapLogLevel(LoggerLevel level);
    static std::shared_ptr<spdlog::logger> m_internalLogger;
    static std::shared_ptr<spdlog::logger> m_clientLogger;
    MappedLevel m_level;
};

enum class LoggerLevel
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

// Implementing Here First and then will clean up and extract to .cpp

void Logger::SetLoggerLevel(LoggerLevel level)
{
    this->m_level = MapLogLevel(level);
}

MappedLevel Logger::MapLogLevel(LoggerLevel level)
{
    std::map<LoggerLevel, MappedLevel> levelMap = 
    {
        {LoggerLevel::trace, MappedLevel::trace},
        {LoggerLevel::info, MappedLevel::info},
        {LoggerLevel::debug, MappedLevel::debug},
        {LoggerLevel::warning, MappedLevel::warn},
        {LoggerLevel::error, MappedLevel::err},
        {LoggerLevel::fatal, MappedLevel::critical}
    };
    MappedLevel result = MappedLevel::off;
    auto iterator = levelMap.find(level);
    if(iterator != levelMap.end()){result = iterator->second;}
    return result;
}
