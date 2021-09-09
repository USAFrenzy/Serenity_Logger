#pragma once

#include <spdlog/spdlog.h>
#include <filesystem>

#include "serenity/Defines.h"

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
		bool is_empty( )
		{
			if( loggerName.empty( ) && logName.empty( ) && ( level == LoggerLevel::off ) && logDir.path( ).empty( ) ) {
				return true;
			}
			else {
				return false;
			}
		}

		std::string                  loggerName = "Logger";
		std::string                  logName    = "Log.txt";
		LoggerLevel                  level      = LoggerLevel::trace;
		file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs" };
	};

}  // namespace serenity