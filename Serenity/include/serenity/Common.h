#pragma once

#include <serenity/Defines.h>
#include <serenity/Version.h>

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

	namespace sinks
	{
		using basic_sv = fmt::basic_string_view<char>;
		namespace se_colors
		{
#ifdef PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

			struct win_colors
			{
				// Directly Pulled From spdlog
				const WORD BOLD   = FOREGROUND_INTENSITY;
				const WORD RED    = FOREGROUND_RED;
				const WORD GREEN  = FOREGROUND_GREEN;
				const WORD CYAN   = FOREGROUND_GREEN | FOREGROUND_BLUE;
				const WORD WHITE  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				const WORD YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;

				const WORD BG_RED = BACKGROUND_RED;
			};

			// Directly Pulled From spdlog
			struct ansi_colors
			{  // Formatting codes
				const basic_sv reset      = "\033[m";
				const basic_sv bold       = "\033[1m";
				const basic_sv dark       = "\033[2m";
				const basic_sv underline  = "\033[4m";
				const basic_sv blink      = "\033[5m";
				const basic_sv reverse    = "\033[7m";
				const basic_sv concealed  = "\033[8m";
				const basic_sv clear_line = "\033[K";

				// Foreground colors
				const basic_sv black   = "\033[30m";
				const basic_sv red     = "\033[31m";
				const basic_sv green   = "\033[32m";
				const basic_sv yellow  = "\033[33m";
				const basic_sv blue    = "\033[34m";
				const basic_sv magenta = "\033[35m";
				const basic_sv cyan    = "\033[36m";
				const basic_sv white   = "\033[37m";

				/// Background colors
				const basic_sv on_black   = "\033[40m";
				const basic_sv on_red     = "\033[41m";
				const basic_sv on_green   = "\033[42m";
				const basic_sv on_yellow  = "\033[43m";
				const basic_sv on_blue    = "\033[44m";
				const basic_sv on_magenta = "\033[45m";
				const basic_sv on_cyan    = "\033[46m";
				const basic_sv on_white   = "\033[47m";

				/// Bold colors
				const basic_sv yellow_bold = "\033[33m\033[1m";
				const basic_sv red_bold    = "\033[31m\033[1m";
				const basic_sv bold_on_red = "\033[1m\033[41m";
			};

		}  // namespace se_colors
	}          // namespace sinks
}  // namespace serenity
