#pragma once

#include <serenity/Defines.h>
#include <serenity/Sinks/Sinks.h>
#pragma warning( push, 0 )
#include <spdlog/spdlog.h>
#pragma warning( pop )
#include <filesystem>
#include <map>

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

	struct rotating_sink_info
	{
		int  maxFileSize { 1024 * 1024 * 5 };
		int  maxFileNum { 5 };
		bool rotateWhenOpened { false };
	};
	struct daily_sink_info
	{
		int      hour { 0 };
		int      min { 0 };
		bool     truncate { false };
		uint16_t maxFiles { 0 };
	};

	enum class SinkType;

	struct logger_info
	{
		std::string                  loggerName = "Logger";
		std::string                  logName    = "Log.txt";
		LoggerLevel                  level      = LoggerLevel::trace;
		file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs" };
		// These Fields Are Default nullptr Unless The Corresponding Struct Is Passed In ( For Use With CreateLogger() )
		/*  logger_info a = {};
		 *  rotating_sink_info b = {};
		 *  a.rotate_sink = &b;
		 *  auto sink = SinkType::rotating_file_sink;
		 *  CreateLogger(sink, a); -> creates <spdlog::sinks::rotating_file_sink>( a.loggerName, a.logName, a.b->maxFileSize,
		 *  a.b->maxFileNum, a.b->rotateWhenOpened);
		 */
		rotating_sink_info *rotate_sink = nullptr;
		daily_sink_info *   daily_sink  = nullptr;
		base_sink_info      sink_info   = { };
	};

	struct internal_logger_info
	{
		std::string                  loggerName = "SERENITY";
		std::string                  logName    = "Internal_Log.txt";
		LoggerLevel                  level      = LoggerLevel::trace;
		file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs" };
		base_sink_info               sink_info = { };
	};
}  // namespace serenity
