#pragma once

#include <string_view>
#include <array>

namespace serenity
{
	namespace expiremental
	{
		static constexpr std::array<std::string_view, 7> short_weekdays = { "Sun",   "Mon", "Tues", "Wed",
										    "Thurs", "Fri", "Sat" };

		static constexpr std::array<std::string_view, 7> long_weekdays = { "Sunday",   "Monday", "Tuesday", "Wednesday",
										   "Thursday", "Friday", "Saturday" };

		static constexpr std::array<std::string_view, 12> short_months = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
										   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		static constexpr std::array<std::string_view, 12> long_months = { "January",   "February", "March",    "April",
										  "May",       "June",     "July",     "August",
										  "September", "October",  "November", "December" };

		static std::string svToString( std::string_view s )
		{
			return std::string( s.data( ), s.size( ) );
		}

		enum class LoggerLevel
		{
			trace   = 0,
			info    = 1,
			debug   = 2,
			warning = 3,
			error   = 4,
			fatal   = 5,
			off     = 6,
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
	#define DB_PRINT( msg, ... ) ( std::cout << std::format( msg, __VA_ARGS__ ) << "\n" )
#else
	#define DB_PRINT( msg, ... )
#endif  // !NDEBUG