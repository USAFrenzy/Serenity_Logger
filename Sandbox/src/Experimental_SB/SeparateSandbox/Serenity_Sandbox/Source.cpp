#include <iostream>
#include <format>
#include <vector>
#include <chrono>
#include <atomic>
#include <filesystem>

#include <time.h>

#include "Utilities.h"

// clang-format off

namespace SE_LUTS{

	static constexpr std::array<const char*, 22> allValidFlags = {
		"%a", "%b", "%d", "%l", "%n", "%t", "%w", "%x", "%y", "%A", "%B",
		"%D", "%F", "%H", "%L", "%M", "%N", "%S", "%T", "%X", "%Y", "%+" 
	};
	
	static constexpr  std::array<const char*, 7> short_weekdays = 
	{
		"Sun", "Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat" 
	};
	
	static constexpr  std::array<const char*, 7> long_weekdays = 
	{
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	 };
	
	static  constexpr std::array<const char*, 12> short_months = 
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
	};
	
	static constexpr  std::array<const char*, 12> long_months = 
	{
		"January", "February", "March", "April", "May", "June", 
		"July", "August", "September", "October", "November", "December" 
	};
	
	static constexpr   std::array<const char*, 100> numberStr = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", 
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", 
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", 
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", 
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", 
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", 
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
	};

} // namespace SE_LUTS

// clang-format on

enum class message_time_mode
{
	local,
	utc
};
class Message_Time
{
  public:
	explicit Message_Time( message_time_mode mode )
	{
		m_mode = mode;
		UpdateTimeDate( std::chrono::system_clock::now( ) );
	}

	std::string_view GetCurrentYearSV( int yearOffset, bool shortened = false )
	{
		if( !shortened ) {
			auto year { 1900 + yearOffset };
			// could be clever here in order to use LUT instead of vformat - is it worth? Probs not
			return std::move( std::vformat( "{}", std::make_format_args( year ) ) );
		}
		else {
			auto year { yearOffset - 100 };
			return SE_LUTS::numberStr[ year ];
		}
	}

	void UpdateTimeDate( std::chrono::system_clock::time_point timePoint )
	{
		auto time { std::chrono::system_clock::to_time_t( timePoint ) };
		secsSinceLastLog = std::chrono::duration_cast<std::chrono::seconds>( timePoint.time_since_epoch( ) );
		( m_mode == message_time_mode::local ) ? localtime_s( &m_cache, &time ) : gmtime_s( &m_cache, &time );
	}

	void UpdateCache( std::chrono::system_clock::time_point timePoint )
	{
		UpdateTimeDate( timePoint );
	}

	std::tm &Cache( )
	{
		return m_cache;
	}

	const message_time_mode &Mode( )
	{
		return m_mode;
	}

	void SetTimeMode( message_time_mode mode )
	{
		m_mode = mode;
	}

	std::chrono::seconds &LastLogPoint( )
	{
		return secsSinceLastLog;
	}

  private:
	message_time_mode    m_mode;
	std::tm              m_cache;
	std::chrono::seconds secsSinceLastLog;
};

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

static std::string_view MsgLevelToShortString( LoggerLevel &level )
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

class Message_Info
{
  public:
	Message_Info( ) : m_name( ), m_msgLevel( LoggerLevel::trace ), m_msgTime( message_time_mode::local ) { }
	Message_Info( std::string_view name, LoggerLevel level, message_time_mode mode )
	  : m_name( name ), m_msgLevel( level ), m_msgTime( mode )
	{
	}

	Message_Info &operator=( const Message_Info &t )
	{
		m_message  = t.m_message;
		m_msgLevel = t.m_msgLevel;
		m_msgTime  = t.m_msgTime;
		m_name     = t.m_name;
		return *this;
	}
	// ############### New Function Added #################
	std::string &MessageBuffer( )
	{
		return m_msgStrBuf;
	}
	// ####################################################
	LoggerLevel &MsgLevel( )
	{
		return m_msgLevel;
	}
	std::string &Name( )
	{
		return m_name;
	}
	Message_Time &TimeDetails( )
	{
		return m_msgTime;
	}
	void SetName( const std::string name )
	{
		m_name = name;
	}

	void SetMsgLevel( const LoggerLevel level )
	{
		m_msgLevel = level;
	}

	std::chrono::system_clock::time_point MessageTimePoint( )
	{
		return std::chrono::system_clock::now( );
	}
	void SetTimeMode( const message_time_mode mode )
	{
		m_msgTime.SetTimeMode( mode );
	}

	message_time_mode TimeMode( )
	{
		return m_msgTime.Mode( );
	}

	std::tm &TimeInfo( )
	{
		return m_msgTime.Cache( );
	}
	std::string &Message( )
	{
		return m_message;
	}

	template <typename... Args> void SetMessage( const std::string_view message, Args &&...args )
	{
		m_message.clear( );
		using iterator = std::back_insert_iterator<std::basic_string<char>>;
		using context  = std::basic_format_context<iterator, char>;
		std::vformat_to( std::back_inserter( m_message ), message, std::make_format_args<context>( std::forward<Args>( args )... ) );
		m_message.append( "\n" );
	}

  private:
	std::string  m_name;
	LoggerLevel  m_msgLevel;
	std::string  m_message;
	Message_Time m_msgTime;
	std::string  m_msgStrBuf;
};

// ################################################################################################################################################
//                                                          Message_Formmater Wrapper Class
// ################################################################################################################################################
class Message_Formatter
{
  public:
	Message_Formatter( Message_Info *details ) : msgInfo( *&details ) { }

	struct Formatter
	{
		virtual std::string_view Format( )             = 0;
		virtual std::string      UpdateInternalView( ) = 0;
	};

	struct Format_Arg_a : Formatter
	{
		Format_Arg_a( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
		{
			hour = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastHour = cacheRef.tm_hour;
			auto hr { lastHour };
			// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
			std::string_view paddedHour { ( hr > 12 ) ? SE_LUTS::numberStr[ static_cast<int64_t>( hr ) - 12 ]
													  : SE_LUTS::numberStr[ hr ] };
			return std::string { paddedHour.data( ), paddedHour.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_hour != lastHour ) ? hour = std::move( UpdateInternalView( ) ) : hour;
		}

	  private:
		const std::tm &cacheRef;
		int            lastHour;
		std::string    hour;
	};

	struct Format_Arg_b : Formatter
	{
		Format_Arg_b( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastMonth( cacheRef.tm_mon )
		{
			month = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastMonth = cacheRef.tm_mon;
			std::string_view paddedMonth { SE_LUTS::numberStr[ lastMonth ] };
			return std::string { paddedMonth.data( ), paddedMonth.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mon != lastMonth ) ? month = std::move( UpdateInternalView( ) ) : month;
		}

	  private:
		const std::tm &cacheRef;
		int            lastMonth;
		std::string    month;
	};

	struct Format_Arg_d : Formatter
	{
		Format_Arg_d( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastDay( cacheRef.tm_mday )
		{
			day = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastDay = cacheRef.tm_mday;
			std::string_view paddedDay { SE_LUTS::numberStr[ lastDay ] };
			return std::string { paddedDay.data( ), paddedDay.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mday != lastDay ) ? day = std::move( UpdateInternalView( ) ) : day;
		}

	  private:
		const std::tm &cacheRef;
		int            lastDay;
		std::string    day;
	};

	struct Format_Arg_l : Formatter
	{
		Format_Arg_l( Message_Info &info ) : levelRef( info.MsgLevel( ) )
		{
			levelStr = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastLevel = levelRef;
			auto lvl { MsgLevelToShortString( lastLevel ) };
			return std::string { lvl.data( ), lvl.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( levelRef != lastLevel ) ? levelStr = std::move( UpdateInternalView( ) ) : levelStr;
		}

	  private:
		LoggerLevel &levelRef;
		LoggerLevel  lastLevel;
		std::string  levelStr;
	};

	struct Format_Arg_n : Formatter
	{
		Format_Arg_n( Message_Info &info ) : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
		{
			ddmmyy = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastDay = cacheRef.tm_mday;
			std::string result;
			auto        day { SE_LUTS::numberStr[ cacheRef.tm_mday ] };
			auto        month { SE_LUTS::short_months[ cacheRef.tm_mon ] };
			auto        year { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
			return result.append( day ).append( month ).append( year );
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mday != lastDay ) ? ddmmyy = std::move( UpdateInternalView( ) ) : ddmmyy;
		}

	  private:
		Message_Time & timeRef;
		const std::tm &cacheRef;
		int            lastDay;
		std::string    ddmmyy;
	};

	struct Format_Arg_t : Formatter
	{
		Format_Arg_t( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( info.TimeDetails( ).Cache( ).tm_min )
		{
			hmStr = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastMin = cacheRef.tm_min;
			auto hr { cacheRef.tm_hour };  // just to avoid another lookup, make local copy
			// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
			auto hour { ( hr > 12 ) ? SE_LUTS::numberStr[ static_cast<int64_t>( hr ) - 12 ] : SE_LUTS::numberStr[ hr ] };
			auto min { SE_LUTS::numberStr[ lastMin ] };
			std::string result;
			return result.append( hour ).append( ":" ).append( min );
		}

		std::string_view Format( ) override
		{
			auto sec = SE_LUTS::numberStr[ cacheRef.tm_sec ];
			if( cacheRef.tm_min != lastMin ) {
				auto result { hmStr = std::move( UpdateInternalView( ) ) };
				return result.append( ":" ).append( sec );
			}
			else {
				auto result { hmStr };
				return result.append( ":" ).append( sec );
			}
		}

	  private:
		const std::tm &cacheRef;
		int            lastMin;
		std::string    hmStr;
	};

	struct Format_Arg_w : Formatter
	{
		Format_Arg_w( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) )
		{
			lastDecDay = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastDay = cacheRef.tm_wday;
			return std::move( std::vformat( "{}", std::make_format_args( lastDay ) ) );
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_wday != lastDay ) ? lastDecDay = std::move( UpdateInternalView( ) ) : lastDecDay;
		}

	  private:
		const std::tm &cacheRef;
		int            lastDay { 0 };
		std::string    lastDecDay;
	};

	struct Format_Arg_x : Formatter
	{
		Format_Arg_x( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastWkday( cacheRef.tm_wday )
		{
			wkday = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastWkday = cacheRef.tm_wday;
			std::string_view sWkday { SE_LUTS::short_weekdays[ lastWkday ] };
			return std::string { sWkday.data( ), sWkday.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( lastWkday != cacheRef.tm_wday ) ? wkday = std::move( UpdateInternalView( ) ) : wkday;
		}

	  private:
		const std::tm &cacheRef;
		std::string    wkday;
		int            lastWkday { 0 };
	};

	struct Format_Arg_y : Formatter
	{
		Format_Arg_y( Message_Info &info ) : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastYear( cacheRef.tm_year )
		{
			year = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastYear = cacheRef.tm_year;
			auto yr { timeRef.GetCurrentYearSV( lastYear, true ) };
			return std::string { yr.data( ), yr.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_year != lastYear ) ? year = std::move( UpdateInternalView( ) ) : year;
		}

	  private:
		const std::tm &cacheRef;
		Message_Time & timeRef;
		int            lastYear;
		std::string    year;
	};

	struct Format_Arg_A : Formatter
	{
		Format_Arg_A( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
		{
			dayHalf = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastHour = cacheRef.tm_hour;
			std::string_view dHalf { ( lastHour >= 12 ) ? "PM" : "AM" };
			return std::string { dHalf.data( ), dHalf.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_hour != lastHour ) ? dayHalf = std::move( UpdateInternalView( ) ) : dayHalf;
		}

	  private:
		const std::tm &cacheRef;
		int            lastHour;
		std::string    dayHalf;
	};

	struct Format_Arg_B : Formatter
	{
		Format_Arg_B( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastMonth( cacheRef.tm_mon )
		{
			month = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastMonth = cacheRef.tm_mon;
			std::string_view lMonth { SE_LUTS::long_months[ lastMonth ] };
			return std::string { lMonth.data( ), lMonth.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mon != lastMonth ) ? month = std::move( UpdateInternalView( ) ) : month;
		}

	  private:
		const std::tm &cacheRef;
		int            lastMonth;
		std::string    month;
	};

	struct Format_Arg_D : Formatter
	{
		Format_Arg_D( Message_Info &info ) : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
		{
			mmddyy = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			std::string result;
			lastDay = cacheRef.tm_mday;
			// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
			auto m { SE_LUTS::numberStr[ static_cast<int64_t>( cacheRef.tm_mon ) + 1 ] };
			auto d { SE_LUTS::numberStr[ lastDay ] };
			auto y { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
			return result.append( m ).append( "/" ).append( d ).append( "/" ).append( y );
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mday != lastDay ) ? mmddyy = std::move( UpdateInternalView( ) ) : mmddyy;
		}

	  private:
		Message_Time & timeRef;
		const std::tm &cacheRef;
		int            lastDay;
		std::string    mmddyy;
	};

	struct Format_Arg_F : Formatter
	{
		Format_Arg_F( Message_Info &info ) : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
		{
			yymmdd = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			std::string result;
			lastDay = cacheRef.tm_mday;
			auto y { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
			// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
			auto m { SE_LUTS::numberStr[ static_cast<int64_t>( cacheRef.tm_mon ) + 1 ] };
			auto d { SE_LUTS::numberStr[ lastDay ] };
			return result.append( y ).append( "-" ).append( m ).append( "-" ).append( d );
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_mday != lastDay ) ? yymmdd = std::move( UpdateInternalView( ) ) : yymmdd;
		}

	  private:
		Message_Time & timeRef;
		const std::tm &cacheRef;
		int            lastDay;
		std::string    yymmdd;
	};

	struct Format_Arg_H : Formatter
	{
		Format_Arg_H( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
		{
			hour = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastHour = cacheRef.tm_hour;
			std::string_view hr { SE_LUTS::numberStr[ lastHour ] };
			return std::string { hr.data( ), hr.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_hour != lastHour ) ? hour = std::move( UpdateInternalView( ) ) : hour;
		}

	  private:
		const std::tm &cacheRef;
		int            lastHour;
		std::string    hour;
	};

	struct Format_Arg_L : Formatter
	{
		Format_Arg_L( Message_Info &info ) : levelRef( info.MsgLevel( ) )
		{
			levelStr = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastLevel = levelRef;
			auto lvl { MsgLevelToString( lastLevel ) };
			return std::string { lvl.data( ), lvl.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( levelRef != lastLevel ) ? levelStr = std::move( UpdateInternalView( ) ) : levelStr;
		}

	  private:
		LoggerLevel &levelRef;
		LoggerLevel  lastLevel;
		std::string  levelStr;
	};

	struct Format_Arg_M : Formatter
	{
		Format_Arg_M( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( cacheRef.tm_min )
		{
			min = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastMin = cacheRef.tm_min;
			std::string_view minute { SE_LUTS::numberStr[ lastMin ] };
			return std::string { minute.data( ), minute.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_min != lastMin ) ? min = std::move( UpdateInternalView( ) ) : min;
		}

	  private:
		const std::tm &cacheRef;
		int            lastMin;
		std::string    min;
	};

	struct Format_Arg_N : Formatter
	{
		Format_Arg_N( Message_Info &info ) : name( info.Name( ) ) { }

		std::string UpdateInternalView( ) override
		{
			// No Need to implement as it's not used
			return "";
		}

		std::string_view Format( ) override
		{
			return name;
		}

	  private:
		std::string &name;
	};

	struct Format_Arg_S : Formatter
	{
		Format_Arg_S( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastSec( cacheRef.tm_sec )
		{
			sec = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastSec = cacheRef.tm_sec;
			std::string_view second { SE_LUTS::numberStr[ lastSec ] };
			return std::string { second.data( ), second.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_sec != lastSec ) ? sec = std::move( UpdateInternalView( ) ) : sec;
		}

	  private:
		const std::tm &cacheRef;
		int            lastSec;
		std::string    sec;
	};

	struct Format_Arg_T : Formatter
	{
		Format_Arg_T( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( cacheRef.tm_min )
		{
			hmStr = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastMin = cacheRef.tm_min;
			auto        hour { SE_LUTS::numberStr[ cacheRef.tm_hour ] };
			auto        min { SE_LUTS::numberStr[ lastMin ] };
			std::string result;
			return result.append( hour ).append( ":" ).append( min );
		}

		std::string_view Format( ) override
		{
			auto sec = SE_LUTS::numberStr[ cacheRef.tm_sec ];
			if( cacheRef.tm_min != lastMin ) {
				auto result = hmStr = std::move( UpdateInternalView( ) );
				return result.append( ":" ).append( sec );
			}
			else {
				auto result = hmStr;
				return result.append( ":" ).append( sec );
			}
		}

	  private:
		const std::tm &cacheRef;
		int            lastMin;
		std::string    hmStr;
	};

	struct Format_Arg_X : Formatter
	{
		Format_Arg_X( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) ), lastWkday( cacheRef.tm_wday )
		{
			wkday = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastWkday = cacheRef.tm_wday;
			std::string_view lWkday { SE_LUTS::long_weekdays[ lastWkday ] };
			return std::string { lWkday.data( ), lWkday.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_wday != lastWkday ) ? wkday = std::move( UpdateInternalView( ) ) : wkday;
		}

	  private:
		const std::tm &cacheRef;
		int            lastWkday;
		std::string    wkday;
	};

	struct Format_Arg_Y : Formatter
	{
		Format_Arg_Y( Message_Info &info ) : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastYear( cacheRef.tm_year )
		{
			year = std::move( UpdateInternalView( ) );
		}

		std::string UpdateInternalView( ) override
		{
			lastYear = cacheRef.tm_year;
			auto yr { timeRef.GetCurrentYearSV( lastYear ) };
			return std::string { yr.data( ), yr.size( ) };
		}

		std::string_view Format( ) override
		{
			return ( cacheRef.tm_year != lastYear ) ? year = std::move( UpdateInternalView( ) ) : year;
		}

	  private:
		Message_Time & timeRef;
		const std::tm &cacheRef;
		int            lastYear;
		std::string    year;
	};

	struct Format_Arg_Message : Formatter
	{
		Format_Arg_Message( Message_Info &info ) : message( info.Message( ) ) { }

		std::string UpdateInternalView( ) override
		{
			// No Need to implement as it's not used
			return "";
		}

		std::string_view Format( ) override
		{
			return message;
		}

	  private:
		std::string &message;
	};

	struct Format_Arg_Char : Formatter
	{
		Format_Arg_Char( std::string_view ch ) : m_char( ch.data( ), ch.size( ) ) { }

		std::string UpdateInternalView( ) override
		{
			// No Need to implement as it's not used
			return "";
		}

		std::string_view Format( ) override
		{
			return m_char;
		}

	  private:
		std::string m_char;
	};

	class Formatters
	{
	  public:
		Formatters( std::vector<std::shared_ptr<Formatter>> &&container ) : m_Formatter( std::move( container ) )
		{
			// Reserve an estimated amount based off arg sizes
			localBuffer.reserve( m_Formatter.size( ) * 32 );
		}
		Formatters( )
		{
			localBuffer.reserve( 512 );
		}

		void Emplace_Back( std::unique_ptr<Formatter> &&formatter )
		{
			m_Formatter.emplace_back( std::move( formatter ) );
			// Reserve an estimated amount based off arg sizes
			localBuffer.reserve( m_Formatter.size( ) * 32 );
		}

		std::string_view Format( )
		{
			localBuffer.clear( );
			for( auto &formatter : m_Formatter ) {
				auto formatted { formatter->Format( ) };
				localBuffer.append( std::move( formatted.data( ) ), formatted.size( ) );
			}
			return localBuffer;
		}

	  private:
		std::string                             localBuffer;
		std::vector<std::shared_ptr<Formatter>> m_Formatter;
	};

	void FlagFormatter( size_t flag )
	{
		switch( flag ) {
			case 0: formatter.Emplace_Back( std::make_unique<Format_Arg_a>( *msgInfo ) ); break;
			case 1: formatter.Emplace_Back( std::make_unique<Format_Arg_b>( *msgInfo ) ); break;
			case 2: formatter.Emplace_Back( std::make_unique<Format_Arg_d>( *msgInfo ) ); break;
			case 3: formatter.Emplace_Back( std::make_unique<Format_Arg_l>( *msgInfo ) ); break;
			case 4: formatter.Emplace_Back( std::make_unique<Format_Arg_n>( *msgInfo ) ); break;
			case 5: formatter.Emplace_Back( std::make_unique<Format_Arg_t>( *msgInfo ) ); break;
			case 6: formatter.Emplace_Back( std::make_unique<Format_Arg_w>( *msgInfo ) ); break;
			case 7: formatter.Emplace_Back( std::make_unique<Format_Arg_x>( *msgInfo ) ); break;
			case 8: formatter.Emplace_Back( std::make_unique<Format_Arg_y>( *msgInfo ) ); break;
			case 9: formatter.Emplace_Back( std::make_unique<Format_Arg_A>( *msgInfo ) ); break;
			case 10: formatter.Emplace_Back( std::make_unique<Format_Arg_B>( *msgInfo ) ); break;
			case 11: formatter.Emplace_Back( std::make_unique<Format_Arg_D>( *msgInfo ) ); break;
			case 12: formatter.Emplace_Back( std::make_unique<Format_Arg_F>( *msgInfo ) ); break;
			case 13: formatter.Emplace_Back( std::make_unique<Format_Arg_H>( *msgInfo ) ); break;
			case 14: formatter.Emplace_Back( std::make_unique<Format_Arg_L>( *msgInfo ) ); break;
			case 15: formatter.Emplace_Back( std::make_unique<Format_Arg_M>( *msgInfo ) ); break;
			case 16: formatter.Emplace_Back( std::make_unique<Format_Arg_N>( *msgInfo ) ); break;
			case 17: formatter.Emplace_Back( std::make_unique<Format_Arg_S>( *msgInfo ) ); break;
			case 18: formatter.Emplace_Back( std::make_unique<Format_Arg_T>( *msgInfo ) ); break;
			case 19: formatter.Emplace_Back( std::make_unique<Format_Arg_X>( *msgInfo ) ); break;
			case 20: formatter.Emplace_Back( std::make_unique<Format_Arg_Y>( *msgInfo ) ); break;
			case 21:
				formatter.Emplace_Back( std::make_unique<Format_Arg_Message>( *msgInfo ) );
				break;
				// if arg after "%" isn't a flag handled here, do nothing
			default: break;
		}
	}

	void SetPattern( std::string pattern )
	{
		fmtPattern = std::move( pattern );
	}
	Formatters &GetFormatters( )
	{
		return formatter;
	}

	void StoreFormat( )
	{
		std::string fmt { fmtPattern };
		std::string flag;
		size_t      index { 150 };

		while( !fmt.empty( ) ) {
			if( fmt.front( ) == '%' ) {
				flag.clear( );
				flag.append( fmt.substr( 0, 2 ) );
				auto position = std::find( SE_LUTS::allValidFlags.begin( ), SE_LUTS::allValidFlags.end( ), flag );
				if( position != SE_LUTS::allValidFlags.end( ) ) {
					index = std::distance( SE_LUTS::allValidFlags.begin( ), position );
				}
				FlagFormatter( index );
				fmt.erase( 0, flag.size( ) );
				if( fmt.empty( ) ) {
					break;
				}
			}
			else {
				formatter.Emplace_Back( std::make_unique<Format_Arg_Char>( fmt.substr( 0, 1 ) ) );
				fmt.erase( 0, 1 );
				if( fmt.empty( ) ) {
					break;
				}
			}
		}
	}

	Message_Info *MessageDetails( )
	{
		return msgInfo;
	}

  private:
	Formatters    formatter;
	std::string   fmtPattern;
	Message_Info *msgInfo;
};

// ################################################################################################################################################
//                                                        FORMATTER SPECIALIZATIONS
// ################################################################################################################################################

// Other than the test cases, not using these due to slower than builtin functions (even with vtable lookups). Keeping these around
// though for the MSVC planned updates for <format> and if i can figure out if there's something I'm missing here...
/*
#############################################################################################################################
*	                                        WITH NEW FOUND REVELATIONS!!!!                                                 *
#############################################################################################################################
*  I might be able to use:
*      using iterator = std::back_insert_iterator<containerType>;
*      using context  = std::basic_format_context<iterator, char>;
*      std::vformat_to( std::back_inserter( container ), fmtStr, std::make_format_args<context>( args ) );
*  Like I did in SetMessage() to make these all much, much faster and possibly even fast enough to just simply call
*  std::format(Formatters) in an application (benches being a mock case)
#############################################################################################################################
*/

template <> struct std::formatter<Message_Formatter::Format_Arg_Message> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_Message fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_a> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_a fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_b> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_b fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_d> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_d fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_l> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_l fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_n> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_n fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_t> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_t fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_w> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_w fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_x> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_x fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_y> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_y fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_A> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_A fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_B> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_B fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_D> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_D fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_F> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_F fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_H> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_H fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_L> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_L fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_M> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_M fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_N> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_N fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_S> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_S fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_T> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_T fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_X> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_X fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_Y> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_Y fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Formatters> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Formatters fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

template <> struct std::formatter<Message_Formatter::Format_Arg_Char> : std::formatter<std::string_view>
{
	template <class FormatContext> auto format( Message_Formatter::Format_Arg_Char fmt, FormatContext &context )
	{
		return std::formatter<std::string_view>::format( fmt.Format( ), context );
	}
};

// ################################################################################################################################################
//                                                             Functionality Test
// ################################################################################################################################################
static void TestEnv( )
{
	auto                    now = std::chrono::system_clock::now( );
	std::chrono::zoned_time time { std::chrono::current_zone( ), now };
	auto localTimeSeconds = std::chrono::duration_cast<std::chrono::seconds>( time.get_local_time( ).time_since_epoch( ) );
	std::chrono::local_time<std::chrono::system_clock::duration> localTime;
	std::chrono::sys_time<std::chrono::system_clock::duration>   gmTime;

	std::cout << "#####################################\n";
	std::cout << "#         SysTime Version:          #\n";
	std::cout << "#####################################\n\n";
	gmTime     = time.get_sys_time( );
	auto gmSec = std::chrono::duration_cast<std::chrono::seconds>( gmTime.time_since_epoch( ) );

	std::cout << std::format( "HH:MM:SS System Time: {:%H:%M:%S}", gmSec ) << "\n";
	std::cout << std::format( "%T System Time: {:%T}", gmSec ) << "\n";
	std::cout << std::format( "%H System Time: {:%H}", gmTime ) << "\n";
	std::cout << std::format( "%M System Time: {:%M}", gmTime ) << "\n";
	std::cout << std::format( "%S System Time: {:%S}", gmSec ) << "\n";
	std::cout << std::format( "%r System Time: {:%r}", gmSec ) << "\n";
	std::cout << std::format( "%b System Time: {:%b}", gmTime ) << "\n";
	std::cout << std::format( "%y System Time: {:%y}", gmTime ) << "\n";
	std::cout << std::format( "%Y System Time: {:%Y}", gmTime ) << "\n";

	std::cout << "#####################################\n";
	std::cout << "#       Local Time Version:         #\n";
	std::cout << "#####################################\n\n";
	localTime     = time.get_local_time( );
	auto localSec = std::chrono::duration_cast<std::chrono::seconds>( localTime.time_since_epoch( ) );

	std::cout << std::format( "HH:MM:SS LocalTime : {:%H:%M:%S}", localSec ) << "\n";
	std::cout << std::format( "%T LocalTime: {:%T}", localSec ) << "\n";
	std::cout << std::format( "%H LocalTime: {:%H}", localTime ) << "\n";
	std::cout << std::format( "%M LocalTime: {:%M}", localTime ) << "\n";
	std::cout << std::format( "%S LocalTime: {:%S}", localSec ) << "\n";
	std::cout << std::format( "%r LocalTime: {:%r}", localSec ) << "\n";
	std::cout << std::format( "%b LocalTime: {:%b}", localTime ) << "\n";
	std::cout << std::format( "%y LocalTime: {:%y}", localTime ) << "\n";
	std::cout << std::format( "%Y LocalTime: {:%Y}", localTime ) << "\n";
}

static void TestFunctions( )
{
	Message_Info testInfo;
	testInfo.SetName( "[Function_Test_Name]" );
	testInfo.SetMessage( "Functionality Test Message" );

	Message_Formatter::Format_Arg_a       a( testInfo );
	Message_Formatter::Format_Arg_b       b( testInfo );
	Message_Formatter::Format_Arg_d       d( testInfo );
	Message_Formatter::Format_Arg_l       l( testInfo );
	Message_Formatter::Format_Arg_n       n( testInfo );
	Message_Formatter::Format_Arg_t       t( testInfo );
	Message_Formatter::Format_Arg_w       w( testInfo );
	Message_Formatter::Format_Arg_x       x( testInfo );
	Message_Formatter::Format_Arg_y       y( testInfo );
	Message_Formatter::Format_Arg_A       A( testInfo );
	Message_Formatter::Format_Arg_B       B( testInfo );
	Message_Formatter::Format_Arg_D       D( testInfo );
	Message_Formatter::Format_Arg_F       F( testInfo );
	Message_Formatter::Format_Arg_H       H( testInfo );
	Message_Formatter::Format_Arg_L       L( testInfo );
	Message_Formatter::Format_Arg_M       M( testInfo );
	Message_Formatter::Format_Arg_N       N( testInfo );
	Message_Formatter::Format_Arg_S       S( testInfo );
	Message_Formatter::Format_Arg_T       T( testInfo );
	Message_Formatter::Format_Arg_X       X( testInfo );
	Message_Formatter::Format_Arg_Y       Y( testInfo );
	Message_Formatter::Format_Arg_Message message( testInfo );
	Message_Formatter::Format_Arg_Char    space( " " );
	std::cout << "####################################################################################################################"
				 "############################\n"
				 "#                                                            Functionality Test                                     "
				 "                           #\n"
				 "####################################################################################################################"
				 "############################\n";
	// This works perfectly
	std::cout << std::format( "12-Hour Clock Hour: {}", a ) << "\n";
	std::cout << std::format( "Short Month: {}", b ) << "\n";
	std::cout << std::format( "Padded Day: {}", d ) << "\n";
	std::cout << std::format( "Short Level: {}", l ) << "\n";
	std::cout << std::format( "DDMMMYY: {}", n ) << "\n";
	std::cout << std::format( "12-Hour Time T: {}", t ) << "\n";
	std::cout << std::format( "Decimal Weekday: {}", w ) << "\n";
	std::cout << std::format( "Short Weekday: {}", x ) << "\n";
	std::cout << std::format( "Short Year: {}", y ) << "\n";
	std::cout << std::format( "AM/PM Specifier: {}", A ) << "\n";
	std::cout << std::format( "Long Month: {}", B ) << "\n";
	std::cout << std::format( "MM/DD/YY: {}", D ) << "\n";
	std::cout << std::format( "YYYY-MM-DD: {}", F ) << "\n";
	std::cout << std::format( "24-Hour Clock Hour: {}", H ) << "\n";
	std::cout << std::format( "Long Level: {}", L ) << "\n";
	std::cout << std::format( "Padded Minute: {}", M ) << "\n";
	std::cout << std::format( "Name: {}", N ) << "\n";
	std::cout << std::format( "Padded Second: {}", S ) << "\n";
	std::cout << std::format( "24-Hour Time T: {}", T ) << "\n";
	std::cout << std::format( "Short Weekday: {}", X ) << "\n";
	std::cout << std::format( "Long Year: {}", Y ) << "\n";
	std::cout << std::format( "Message: {}", message ) << "\n";
}

#define KB 1024
#define MB KB * 1024
#define GB MB * 1024
// ################################################################################################################################################
//                                                                "BENCHES"
// ################################################################################################################################################
static void BenchOne( )
{
	Message_Info info;
	info.SetName( "Testing" );
	std::string mockArg;
	for( int i = 0; i < 399; i++ ) {
		mockArg += "a";
	}
	info.SetTimeMode( message_time_mode::local );
	Message_Formatter format( &info );
	format.SetPattern( "|%l| %x %n %T [%N]: %+" );
	format.StoreFormat( );

	std::cout << "####################################################################################################################"
				 "############################\n"
				 "#                                                                       Benches                                     "
				 "                           #\n"
				 "####################################################################################################################"
				 "############################\n\n";

	unsigned long int                  i { 0 }, iterations { 1000000 };
	serenity::se_utils::Instrumentator timer;

	timer.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		auto now      = std::chrono::system_clock::now( );
		auto pointSec = std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch( ) );
		if( info.TimeDetails( ).LastLogPoint( ) != pointSec ) {
			info.TimeDetails( ).UpdateCache( now );
		}
		info.SetMessage( "{}", mockArg );
		info.SetMsgLevel( std::move( LoggerLevel::trace ) );
		std::cout << format.GetFormatters( ).Format( );
	}
	timer.StopWatch_Stop( );

	auto elapsed = timer.Elapsed_In( serenity::se_utils::time_mode::us );
	std::cout << "\nElapsed Time For Format Args: l, x, n, T, N, & Message Specializations: " << elapsed / 1000000.0 << " us\n";
	format.MessageDetails( )->MessageBuffer( ).append( std::move( format.GetFormatters( ).Format( ) ) );
	std::cout << "Result:\n" << format.MessageDetails( )->MessageBuffer( ) << "\n";
	format.MessageDetails( )->MessageBuffer( ).clear( );
}

static void BenchTwo( )
{
	Message_Info info;
	info.SetName( "Testing" );
	std::string mockArg;
	for( int i = 0; i < 399; i++ ) {
		mockArg += "a";
	}
	info.SetTimeMode( message_time_mode::local );
	info.TimeDetails( ).UpdateCache( std::chrono::system_clock::now( ) );
	Message_Formatter format( &info );
	format.SetPattern( "|%l| %x %n %T [%N]: %+" );
	format.StoreFormat( );

	unsigned int i { 0 }, iterations { 1000000 };
	FILE *       file { };
	// Changing the buffer size is neccessary for better performance here:
	// with default buffer size times are a little over 1us.
	// bufferSize = 8KB  - times are ~0.87us
	// bufferSize = 16Kb - times are ~0.74us
	// bufferSize = 32KB - times are ~0.71us
	// bufferSize = 64KB - times are ~0.67us
	// bufferSize > 64KB - times are basically the same as 64KB
	// Based on the above, sweet spot is somewhere between 32KB and 64KB, however,
	// a good compromise can be made somewhere between 8KB - 16KB for mem usage and speed
	const size_t      bufferSize = static_cast<size_t>( 64 ) * KB;
	std::vector<char> fileBuf;
	fileBuf.reserve( bufferSize );

	const std::filesystem::path            logPath { std::filesystem::current_path( ) /= "Logs" };
	const std::filesystem::directory_entry logDirPath { logPath };
	if( !std::filesystem::exists( logDirPath ) ) {
		std::filesystem::create_directories( logDirPath );
	}
	auto filePath = logPath;
	filePath /= "Test.txt";
	filePath.make_preferred( );

	file = std::fopen( filePath.string( ).c_str( ), "wb" );
	std::setvbuf( file, fileBuf.data( ), _IOFBF, bufferSize );

	serenity::se_utils::Instrumentator timer;

	timer.StopWatch_Reset( );
	for( i; i < iterations; ++i ) {
		/* This one for testing order */
		// auto message = std::move( std::to_string( i ).append( " {}" ) );
		/* This one by default */
		auto message { "{}" };

		auto now      = std::chrono::system_clock::now( );
		auto pointSec = std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch( ) );
		if( info.TimeDetails( ).LastLogPoint( ) != pointSec ) {
			info.TimeDetails( ).UpdateCache( now );
		}
		info.SetMsgLevel( LoggerLevel::trace );
		info.SetMessage( message, mockArg );

		// So far, my manual way is muuuucccchhhh faster at almost 2x speed
		/* ~0.66us */
		auto formatted { format.GetFormatters( ).Format( ) };

		/* consistently ~3.02us */
		// std::format_to( std::back_inserter( format.MessageDetails( )->MessageBuffer( ) ), "{}", format.GetFormatters( ) );
		// auto &formatted { format.MessageDetails( )->MessageBuffer( ) };

		std::fwrite( formatted.data( ), sizeof( char ), formatted.size( ), file );

		// formatted.clear( );
	}
	timer.StopWatch_Stop( );
	std::fflush( file );
	std::fclose( file );
	auto elapsed = timer.Elapsed_In( serenity::se_utils::time_mode::us );

	std::cout << "\nElapsed Time For Format Args: l, x, n, T, N, & Message Specializations For File Writes: " << elapsed / 1000000.0
			  << " us\n";
	info.SetMessage( "{}", mockArg );
	// using the std::formatter specialization here just as proof of concept that it does work, but in loop above, is ~2x slower than
	// manual calls atm
	std::format_to( std::back_inserter( format.MessageDetails( )->MessageBuffer( ) ), "{}", format.GetFormatters( ) );
	std::cout << "Result:\n" << format.MessageDetails( )->MessageBuffer( ) << "\n";
	format.MessageDetails( )->MessageBuffer( ).clear( );
}
// clang-format off
// ################################################################################################################################################
//                                                                       MAIN
// ################################################################################################################################################
// So far, this is a promising start: ~60us vs ~74us for terminal ~4.9 vs ~7.4us for file writes & flush on laptop
// EDIT: The async writing thread (although synchronous due to just copying the buffer and writing it) brought times down to
// ~4.4us-4.5us EDIT 2: Side-stepping write()/flush() calls with sputn()/pubsync() calls & increased file internal buffer size reduced
// times to ~3.9us - 4.1us EDIT 3: No longer need async formatting thread, sped up the formatting functions enough that the async
// branching is now slower than just
//         calling the manual formatter->Format() style functions. (times remain as with EDIT 2, 3.9us-4.1us) Not only that, but now,
//         even without the async writing thread, times aren't that much slower: if doing direct format to write on every iteration,
//         times are about 4.4us-4.6us.
// TODO: Work on implementing vector formatting - that should speed this up much faster I think. Mainly given that in the current Format() call
// TODO: it iterates over all the formatters calling their respective manual Format() functions and returns the overall result from a
// TODO: buffer. It may not make a huge difference in functionality, but, since std::format lib uses byte conversions it may be much,
// TODO: much faster.
// AN IDEA: look into std::variant and see if that helps with the above vector formatting issue since current issue is that it defaults
// to base class Other than that, this is looking great -> ~57us for terminal & ~4.0us for file with flushes (~3.5us-3.7us without
// stringifying the iteration number into the message)
// EDIT 4: Lots of changes, with formatting conditional checks, how SetMessage() formats and stores the message and a few others.
//         Times Are now 48us-49us for terminal and 1.93us-1.95us for File =D
// clang-format on

int main( )
{
	// TestFunctions( );
	// TestEnv( );
	BenchOne( );
	BenchTwo( );
}
