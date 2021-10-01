#pragma once

#include <serenity/Defines.h>
// #include <serenity/Sinks/Sinks.h>
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

	namespace se_utils
	{
		static LoggerLevel ToLogLevel( MappedLevel level )
		{
			std::unordered_map<MappedLevel, LoggerLevel> levelMap = {
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
			std::unordered_map<LoggerLevel, MappedLevel> levelMap = {
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
	}  // namespace se_utils
}  // namespace serenity