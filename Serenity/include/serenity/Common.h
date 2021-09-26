#pragma once

#include <serenity/Defines.h>
#include <serenity/Sinks/Sinks.h>
#pragma warning( push, 0 )
#include <spdlog/spdlog.h>
#pragma warning( pop )
#include <filesystem>
#include <map>


namespace serenity
{
	using MappedLevel     = spdlog::level::level_enum;
	namespace file_helper = std::filesystem;

	enum class LoggerLevel
	{
		trace   = spdlog::level::trace,
		debug   = spdlog::level::debug,
		info    = spdlog::level::info,
		warning = spdlog::level::warn,
		error   = spdlog::level::err,
		fatal   = spdlog::level::critical,
		off     = spdlog::level::off,
	};

	static LoggerLevel ToLogLevel( MappedLevel level )
	{
		std::map<MappedLevel, LoggerLevel> levelMap = {
		  { MappedLevel::trace, LoggerLevel::trace }, { MappedLevel::info, LoggerLevel::info },
		  { MappedLevel::debug, LoggerLevel::debug }, { MappedLevel::warn, LoggerLevel::warning },
		  { MappedLevel::err, LoggerLevel::error },   { MappedLevel::critical, LoggerLevel::fatal } };
		LoggerLevel result   = LoggerLevel::off;
		auto        iterator = levelMap.find( level );
		if( iterator != levelMap.end( ) ) {
			result = iterator->second;
		}
		return result;
	}

	static MappedLevel ToMappedLevel( LoggerLevel level )
	{
		std::map<LoggerLevel, MappedLevel> levelMap = {
		  { LoggerLevel::trace, MappedLevel::trace }, { LoggerLevel::info, MappedLevel::info },
		  { LoggerLevel::debug, MappedLevel::debug }, { LoggerLevel::warning, MappedLevel::warn },
		  { LoggerLevel::error, MappedLevel::err },   { LoggerLevel::fatal, MappedLevel::critical } };
		MappedLevel result   = MappedLevel::off;
		auto        iterator = levelMap.find( level );
		if( iterator != levelMap.end( ) ) {
			result = iterator->second;
		}
		return result;
	}

	static std::string GetSerenityVerStr( )
	{
		auto version = VERSION_NUMBER( SERENITY_MAJOR, SERENITY_MINOR, SERENITY_REV );
		return version;
	}
	static LoggerLevel global_level { LoggerLevel::trace };

	// Honestly Kind Of Confused On How This Is Correctly Finding The Definition In Logger.cpp
	// And On How LibLogger.cpp Is Correctly Finding The Definition From Only Common.h ???
	// But I Mean, I Guess If It Works, It Works (Likely That This Might Be A Future Bug?)
	void SetGlobalLevel( LoggerLevel level );

	static LoggerLevel &GetGlobalLevel( )
	{
		return global_level;
	}
}  // namespace serenity


namespace serenity
{
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
		std::string                  loggerName = DEFAULT_LOGGER_NAME;
		std::string                  logName    = DEFAULT_LOG;
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
	namespace se_internal
	{
		struct internal_logger_info
		{
			std::string                  loggerName = INTERNAL_DEFAULT_NAME;
			std::string                  logName    = INTERNAL_DEFAULT_LOG;
			LoggerLevel                  level      = LoggerLevel::trace;
			file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs\\Internal" };
			base_sink_info               sink_info = { };
		};
		static logger_info tmp = { };

		static logger_info toLoggerInfo( internal_logger_info convertFrom )
		{
			tmp             = { };
			tmp.rotate_sink = nullptr;
			tmp.daily_sink  = nullptr;
			tmp.loggerName  = convertFrom.loggerName;
			tmp.loggerName  = convertFrom.loggerName;
			tmp.logName     = convertFrom.logName;
			tmp.logDir      = convertFrom.logDir;
			tmp.level       = convertFrom.level;
			tmp.sink_info   = convertFrom.sink_info;
			return tmp;
		}
	}  // namespace se_internal
}  // namespace serenity