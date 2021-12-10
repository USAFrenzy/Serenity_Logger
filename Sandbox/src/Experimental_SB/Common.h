#pragma once

#include <string_view>
#include <array>

namespace serenity
{
	namespace expiremental
	{
		static constexpr std::array<const char *, 7> short_weekdays = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

		static constexpr std::array<const char *, 7> long_weekdays = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

		static constexpr std::array<const char *, 12> short_months = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		static constexpr std::array<const char *, 12> long_months = {
		"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

		static std::string svToString( std::string_view s )
		{
			return std::string( s.data( ), s.size( ) );
		}

		constexpr std::string concatToStr( std::string_view s, std::string_view t )
		{
			std::string str;
			str.reserve( s.size( ) + t.size( ) );
			return std::move( str.append( s.data( ) ).append( t.data( ) ) );
		}

		struct InternalFormat
		{
			std::string partitionUpToSpecifier;
			std::string timeDatePartition;
			std::string remainingPartition;
			std::string wholeFormatString;
		};

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
				case LoggerLevel::test: return "Test"; break;

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