#pragma once

#include <string_view>
#include <array>

#ifdef _WIN32
	#ifdef _WIN64
		#define WINDOWS_PLATFORM
		#define WIN32_LEAN_AND_MEAN
		#include <Windows.h>
	#else
		#error "x86 Is Currently Unsupported"
	#endif
#elif defined( __APPLE__ ) || ( __MACH__ )
#define APPLE_PLATFORM
	#include <TargetConditionals.h>
#else
#define LINUX_PLATFORM
	#error "Unable To Detect Platform. Currently Only Supports Windows"
#endif

namespace serenity
{
// Messing with buffer sizes
#define KB ( 1024 )
#define MB ( 1024 * KB )
#define GB ( 1024 * MB )

	namespace expiremental
	{
		namespace SE_LUTS
		{
			static constexpr std::array<std::string_view, 22> allValidFlags = { "%a", "%b", "%d", "%l", "%n", "%t", "%w", "%x",
																				"%y", "%A", "%B", "%D", "%F", "%H", "%L", "%M",
																				"%N", "%S", "%T", "%X", "%Y", "%+" };

			static constexpr std::array<std::string_view, 7> short_weekdays = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

			static constexpr std::array<std::string_view, 7> long_weekdays = {
			"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

			static constexpr std::array<std::string_view, 12> short_months = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

			static constexpr std::array<const char *, 12> long_months = {
			"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

			static constexpr std::array<std::string_view, 100> numberStr = {
			"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
			"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
			"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
			"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
			"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99" };

		}  // namespace SE_LUTS

		// TODO: Get Rid of 'test' once done
		enum class LoggerLevel
		{
			trace   = 0,
			info    = 1,
			debug   = 2,
			warning = 3,
			error   = 4,
			fatal   = 5,
			off     = 6,
			test    = 20,
		};

		static std::string_view MsgLevelToShortString( LoggerLevel level )
		{
			switch( level ) {
				case LoggerLevel::info: return "I"; break;
				case LoggerLevel::trace: return "T"; break;
				case LoggerLevel::debug: return "D"; break;
				case LoggerLevel::warning: return "W"; break;
				case LoggerLevel::error: return "E"; break;
				case LoggerLevel::fatal: return "F"; break;
				case LoggerLevel::test: return "T"; break;

				default: return ""; break;
			}
		}

		static std::string_view MsgLevelToString( LoggerLevel level )
		{
			switch( level ) {
				case LoggerLevel::info: return "Info"; break;
				case LoggerLevel::trace: return "Trace"; break;
				case LoggerLevel::debug: return "Debug"; break;
				case LoggerLevel::warning: return "Warn"; break;
				case LoggerLevel::error: return "Error"; break;
				case LoggerLevel::fatal: return "Fatal"; break;
				case LoggerLevel::test: return "Test"; break;

				default: return ""; break;
			}
		}

		enum class message_time_mode
		{
			local,
			utc
		};
	}  // namespace expiremental
}  // namespace serenity

#ifndef NDEBUG
	#define DB_PRINT( msg, ... ) ( printf( "%s\n", std::format( msg, __VA_ARGS__ ).c_str( ) ) )
#else
	#define DB_PRINT( msg, ... )
#endif  // !NDEBUG