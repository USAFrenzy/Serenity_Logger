#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <filesystem>

#include "serenity/Defines.hpp"

enum class LoggerLevel
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal,
	off
};
enum class LoggerInterface
{
	internal = INTERFACE_INTERNAL,
	client   = INTERFACE_CLIENT
};

namespace serenity
{
	using MappedLevel     = spdlog::level::level_enum;
	namespace file_helper = std::filesystem;

	struct logger_info
	{
		std::string                  loggerName = "Logger";
		std::string                  logName    = "Log.txt";
		LoggerLevel                  level      = LoggerLevel::off;
		file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs" };
	};

}  // namespace serenity


// struct LoggerInfo
//{
//	std::vector<spdlog::logger> loggers;
//	std::vector<spdlog::sink_ptr> loggerSinks;
//	std::vector<std::string> formatString;
//	std::string filePath   = "Logs/";
//	std::string fileName   = "Log.txt";
//	LoggerLevel logLevel   = LoggerLevel::trace;
//	LoggerLevel flushLevel = LoggerLevel::trace;
//};