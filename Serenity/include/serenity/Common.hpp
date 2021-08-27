#pragma once

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
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
	using MappedLevel = spdlog::level::level_enum;
}


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