#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>

enum class LoggerLevel { trace, debug, info, warning, error, fatal };
enum class LoggerInterface { internal, client };

struct LoggerInfo
{
	std::vector<spdlog::logger> loggers;
	std::vector<spdlog::sink_ptr> loggerSinks;
	std::vector<std::string> formatString;
	std::string filePath   = "Logs/";
	std::string fileName   = "Log.txt";
	LoggerLevel logLevel   = LoggerLevel::trace;
	LoggerLevel flushLevel = LoggerLevel::trace;
};