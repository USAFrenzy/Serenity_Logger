#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>
#include <format>
#include <chrono>

#include <ctime>
#include <time.h>

#include <serenity/Utilities/Utilities.h>

#define INSTRUMENT
#define ALLOC_TEST 1


#ifdef INSTRUMENT
	#define INSTRUMENTATION_ENABLED
#endif


#if ALLOC_TEST  // Testing Allocations
	#ifdef INSTRUMENTATION_ENABLED
uint64_t total_allocated_bytes { 0 };

void *operator new( std::size_t n )
{
	total_allocated_bytes += n;
	serenity::se_utils::Instrumentator::mem_tracker.Allocated += n;
	return malloc( n );
}
void operator delete( void *p, size_t n ) throw( )
{
	total_allocated_bytes -= n;
	serenity::se_utils::Instrumentator::mem_tracker.Freed += n;
	free( p );
}
	#endif  // ALLOC_TEST
#endif          // INSTRUMENTATION_ENABLED

// Just throwing these here for testing this case
// ***************************************************************************
enum class LoggerLevel
{
	trace,
	info,
	debug,
	warning,
	error,
	fatal,
	off,
};

static std::string_view MsgLevelToIcon( LoggerLevel level )
{
	switch( level ) {
		case LoggerLevel::info: return "[I]"; break;
		case LoggerLevel::trace: return "[T]"; break;
		case LoggerLevel::debug: return "[D]"; break;
		case LoggerLevel::warning: return "[W]"; break;
		case LoggerLevel::error: return "[E]"; break;
		case LoggerLevel::fatal: return "[F]"; break;
		default: return ""; break;
	}
}

static std::string toString( std::string_view s )
{
	return std::string( s.data( ), s.size( ) );
}
// ***************************************************************************
/*
	Might Be Worth Seeing If It's Viable And Low Enough Overhead To Set Up All The Formatters In Each Respective
   Struct That's Applicable And Then Just Set Up The Formatted Pre Message String Based Off Of Each Individual
   Formatter Specification? Issue Though, Is I Have No Idea How To Tell Where A Particular Flag Would Rest In Relation
   To The Other Flags In The Top-level Format String Passed In...
*/


enum class message_time_mode
{
	local,
	utc
};

static std::array<const char *, 7>  weekdays = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static std::array<const char *, 12> Months   = { "January", "February", "March",     "April",   "May",      "Jun",
                                               "July",    "August",   "September", "October", "November", "December" };


class Message_Time
{
      public:
	Message_Time( message_time_mode mode ) : m_Mode( mode )
	{
		UpdateTimeInfo( mode );
	}

	void UpdateTimeInfo( message_time_mode mode )
	{
		time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now( ) );
		if( mode == message_time_mode::local ) {
			t_struct = std::localtime( &time );
		}
		else {
			t_struct = std::gmtime( &time );
		}
	}

	//  Pretty Much Putting This Here For The Time Being So I Can Sit On How I Want To Implement A Formatter
	/**********************************************************************************************************
	***********************************************************************************************************
		auto        weekday     = weekdays[ t_struct->tm_wday ];
		auto        hour        = t_struct->tm_hour;
		auto        min         = t_struct->tm_min;
		auto        sec         = t_struct->tm_sec;
		auto        year        = ( 1900 + t_struct->tm_year );
		auto        month       = Months[ t_struct->tm_mon ];
		auto day = t_struct->tm_mday;
		return std::format( "{} {}/{}/{} {}:{}:{}", weekday, month, day, year, hour, min, sec );
		***********************************************************************************************************
		**********************************************************************************************************/
	std::string FormatTime( std::string_view fmt, message_time_mode mode )
	{
		UpdateTimeInfo( mode );
		char buffer[ 80 ];
		auto formatted = strftime( buffer, sizeof( buffer ), toString( fmt ).c_str( ), t_struct );
		return std::string( buffer );
	}

	inline message_time_mode Mode( )
	{
		return m_Mode;
	}

      private:
	message_time_mode m_Mode;
	std::time_t       time;
	std::tm *         t_struct = { };
};

// This Might Also NOT Be The Way To Aid Formatting.. This Is Just An Idea
// *************************************************************************
_Enum_is_bitflag_ enum class Flags : uint8_t {
	none = 0,       // no flags set
	time = 1 << 0,  // Mapping To %H:%M:%S Time Format (Honestly Would Like A Way To Parallel strftime Substitution For This)
	name = 1 << 1,  // Mapping To Logger Name
	a    = 1 << 2,  // placeholder
	b    = 1 << 3,  // placeholder
	c    = 1 << 4,  // placeholder
	d    = 1 << 5,  // placeholder
	e    = 1 << 6,  // placeholder
	f    = 1 << 7,  // placeholder

	// More to possibly come..(obviously would have to change type attr)
};

inline Flags operator|( Flags a, Flags b )
{
	return static_cast<Flags>( static_cast<uint16_t>( a ) | static_cast<uint16_t>( b ) );
}

class Msg_Fmt_Flags
{
      public:
	Msg_Fmt_Flags( )
	{
		flagHasher = {
		  { "", Flags::none },
		  { "%T", Flags::time },
		  { "%N", Flags::name }, /* From What I Can Tell, %N isn't Used In Any Formatters So Far */
		};
	}

      private:
	std::unordered_map<const char *, Flags> flagHasher;
};
// *************************************************************************
class Message_Pattern
{
      public:
	Message_Pattern( ) { }                           // what to set if no pattern passed in
	Message_Pattern( std::string_view pattern ) { }  // use user's pattern

	void SetMsgPattern( std::string_view pattern )
	{
		std::basic_format_parse_context fmt { pattern };
		// I honestly have no idea about the std::formatter usage yet..
		/*
			Initial thoughts are to just roll a manual parser that checks for certain flags (yet to be implemented),
			substitute those explicit values in for those flags in a string or buffer, then format the values based on the
			pattern.
			EDIT: If I flesh out the Flags enum and Msg_Fmt_Flags class, I could possibly use these to separately format
		   and store each format unit (time, logger, date, message, color if console, etc) into a message pattern string that
		   would be passed to a print or log type function
		*/
	}

	std::string GetTimeFmt( )
	{
		return time_format;
	}

      private:
	// currently forcing
	std::string time_format = "%H:%M:%S";
	//! NOTE: probably wrong!!!
	std::formatter<std::chrono::seconds> timeStampFormatter;
};

class Message_Info
{
      public:
	Message_Info( ) = default;
	Message_Info( LoggerLevel messageLevel, Message_Pattern formatPattern, std::string_view message, Message_Time messageTime )
	  : msgLevel( messageLevel ), formats( formatPattern ), msg( message ), msgTime( messageTime )
	{
	}

	std::string GetMsgTime( )
	{
		return msgTime.FormatTime( formats.GetTimeFmt( ), msgTime.Mode( ) );
	}

	LoggerLevel MsgLevel( )
	{
		return msgLevel;
	}

	Message_Pattern GetFormatPatterns( )
	{
		return formats;
	}

      private:
	LoggerLevel      msgLevel { LoggerLevel::trace };
	std::string_view msg;
	Message_Pattern  formats { };
	Message_Time     msgTime { message_time_mode::local };
};

// clang-format off
//! ############################################################# NOTE: Just So I Don't Lose The Pathing So Far #############################################################
/*
	For Time Formatting (Only Local Tested So Far):
	- GetMsgTime() Called 
	- Get The Time Format From The User's Provided Format (Currently Hard-Coded The Time Format)
	- Get The Current Date Time
	- Format Current Date Time Into A Buffer Based On Time Format Pattern
	- Return Buffer String To GetMsgTime()
*/
//! #########################################################################################################################################################################
// clang-format on

// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the project I'll
// be using this in (< win 8.1 EOL).
class ColorConsole
{
      public:
	ColorConsole( std::string_view name )
	{
		loggerName     = std::move( name );
		msgLevelColors = {
		  { LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
		  { LoggerLevel::info, se_colors::bright_colors::foreground::green },
		  { LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
		  { LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
		  { LoggerLevel::error, se_colors::basic_colors::foreground::red },
		  { LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
		  { LoggerLevel::off, se_colors::formats::reset },
		};
	}

	void SetMsgColor( LoggerLevel level, const char *color )
	{
		msgLevelColors.at( level ) = color;
		msgLevelColors[ level ]    = color;
	}

	const char *GetMsgColor( LoggerLevel level )
	{
		return msgLevelColors[ level ];
	}

	bool ColorizeOutput( bool colorize )
	{
		coloredOutput = colorize;
	}

	bool ShouldLog( LoggerLevel level )
	{
		return ( message.MsgLevel( ) <= level ) ? true : false;
	}

	template <typename... Args> void PrintMessage( LoggerLevel level, const std::string_view msg, Args &&...args )
	{
#ifdef INSTRUMENTATION_ENABLED
		timer.StopWatch_Reset( );
#endif  // INSTRUMENTATION_ENABLED
		const char *msgColor;
		if( coloredOutput ) {
			msgColor = GetMsgColor( level );
		}
		else {
			msgColor = "";
		}
		std::cout << msgColor << MsgLevelToIcon( level ) << " " << message.GetMsgTime( ) << " "
			  << "[" << loggerName << "]:"
			  << " " << std::format( msg, std::forward<Args>( args )... ) << Reset( ) << "\n";
#ifdef INSTRUMENTATION_ENABLED
		timer.StopWatch_Stop( );
		std::cout << "Elapsed In: " << timer.Elapsed_In( serenity::se_utils::time_mode::us ) << "us\n";
#endif  // INSTRUMENTATION_ENABLED
	}

	// Not Apart Of This Class - Just Here For Testing (Part Of Logger Class)
	// ##########################################################################
	template <typename... Args> void trace( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::trace ) ) {
			PrintMessage( LoggerLevel::trace, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void info( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::info ) ) {
			PrintMessage( LoggerLevel::info, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void debug( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::debug ) ) {
			PrintMessage( LoggerLevel::debug, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void warn( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::warning ) ) {
			PrintMessage( LoggerLevel::warning, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void error( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::error ) ) {
			PrintMessage( LoggerLevel::error, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void fatal( std::string_view s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::fatal ) ) {
			PrintMessage( LoggerLevel::fatal, s, std::forward<Args>( args )... );
		}
	}
	// ##########################################################################

	std::string toString( const std::string_view s )
	{
		return std::string( s.data( ), s.size( ) );
	}

      private:
	std::string Reset( )
	{
		return se_colors::formats::reset;
	}

      private:
	Message_Info     message = { };
	LoggerLevel      flush_level;
	std::string_view loggerName = "Default_Console";
	bool             coloredOutput { true };
	// This Setup influenced by the hasher that spdlog uses to set colors for msg
	// lvl
	std::unordered_map<LoggerLevel, const char *> msgLevelColors;
	// for internal instrumentation
#ifdef INSTRUMENTATION_ENABLED
	serenity::se_utils::Instrumentator timer;
#endif  // INSTRUMENTAION_ENABLED
};      // struct ColorConsole

int main( )
{
	using namespace se_colors;
	using namespace serenity::se_utils;
	ColorConsole C( "Experimental Logger" );
	// Formatting Works With Latest Standard Flag
	auto s = "White";
#ifdef INSTRUMENTATION_ENABLED
	Instrumentator macroTester;
	/*
		First Iteration Takes Waaaaayyyy Too Long..
		My guess so far, without full instrumentation, is that it has to do with the initial setup of the local time zone data?
		I'm not even 50% sure if that's the case, but definitely want to flesh out the message details before mucking with
		timing optimizations. Good to know there's a slight optimization issue this early on though I guess.
	*/
	macroTester.StopWatch_Reset( );
#endif  // INSTRUMENTATION_ENABLED

	// Trace Is Deafult Color
	C.trace( "Trace Will Be Bright {}", s );
	// Info Is Light Green
	C.info( "Info Will Be Bright Green" );
	// Debug Is Light Cyan
	C.debug( "Debug Will Be Bright Cyan" );
	// Warning Is Light Yellow
	C.warn( "Warning Will Be Bright Yellow" );
	// Error Is Dark Red
	C.error( "Error Will Be Red" );
	// Fatal Is Light Yellow On Dark Red
	C.fatal( "Fatal Will Be Bright Yellow On Red" );

#ifdef INSTRUMENTATION_ENABLED
	macroTester.StopWatch_Stop( );

	std::cout << Tag::Yellow(
	  "\n\n***************************************************************\n******************** Instrumentation Data: "
	  "********************\n***************************************************************\n" );
	std::cout << Tag::Bright_Yellow( "Total Elapsed Time:\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::us ) ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::ms ) ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::sec ) ) + " s\n" );
#endif  // INSTRUMENTATION_ENABLED

#ifdef INSTRUMENTATION_ENABLED
	#if ALLOC_TEST
	// setting up for the current usage and allocations so no variation in conversions
	auto mem_used = macroTester.mem_tracker.Memory_Usage( );
	std::cout << Tag::Bright_Yellow( "Total Memory Used:\n" ) << Tag::Bright_Cyan( "\t- In Bytes:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( mem_used ) + " bytes]\n" ) << Tag::Bright_Cyan( "\t- In Kilobytes:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( mem_used / 1000.0 ) + " KB]\n" );
	#endif  // ALLOC_TEST
#endif          // INSTRUMENTATION_ENABLED

#ifdef INSTRUMENTATION_ENABLED
	// Currently 128 bytes
	std::cout << Tag::Bright_Yellow( "Size of Message_Info Struct:\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( Message_Info ) ) + " bytes]\n" );

	std::cout << Tag::Yellow( "***************************************************************\n" );
	std::cout << Tag::Yellow( "***************************************************************\n" );
	std::cout << Tag::Yellow( "***************************************************************\n\n" );

#endif  // INSTRUMENTATION_ENABLED
}
