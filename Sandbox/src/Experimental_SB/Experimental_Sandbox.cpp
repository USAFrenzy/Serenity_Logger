#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>
#include <format>
#include <chrono>

#define ALLOC_TEST 1

#if ALLOC_TEST  // Testing Allocations
size_t total_bytes = 0;
void * operator new( std::size_t n )
{
	// std::cout << "[Allocating " << n << " bytes]";
	total_bytes += n;
	return malloc( n );
}
void operator delete( void *p ) throw( )
{
	total_bytes -= sizeof( p );
	free( p );
}
#endif

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
// ***************************************************************************
/*
	Might Be Worth Seeing If It's Viable And Low Enough Overhead To Set Up All The Formatters In Each Respective
   Struct That's Applicable And Then Just Set Up The Formatted Pre Message String Based Off Of Each Individual
   Formatter Specification? Issue Though, Is I Have No Idea How To Tell Where A Particular Flag Would Rest In Relation
   To The Other Flags In The Top-level Format String Passed In...
*/

// originally had UTC here as well but not really implemented - going to flesh out what I would like to see first before adding
// something I may not even use
class Message_Time
{
      public:
	Message_Time( )
	{
		time = std::chrono::system_clock::now( );
	}
	// Leaving As LocalTimeCLock in case I add Other Clocks Later?
	auto LocalTimeClock( std::string fmt )
	{
		using namespace std::chrono;
		std::chrono::zoned_seconds m = { current_zone( ), floor<seconds>( system_clock::now( ) ) };

		std::string buffer;

		std::format_to( std::back_inserter( buffer ), fmt, m );
		return buffer;
	}

	auto UTCTimeClock( std::string_view fmt )
	{
		std::string buffer;
		std::format_to( std::back_inserter( buffer ), fmt, time );
		return buffer;
	}

      private:
	std::chrono::system_clock::time_point time;
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

	// More to possible come..(obviously would have to change type attr)
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
			pattern. From my basic understanding, std::formatter should pretty much have me covered here if i can figure
			out how to implement it

			EDIT: I May just need both std::formatter<std::chrono::zoned_time> and  std::formatter<std::chrono::utc_time>
			instead of just one formatter and then pass the basic_parse_context and time to the appropriate formatter?
		*/
	}

	std::string GetTimeFmt( )
	{
		return time_format;
	}

      private:
	// NOTE: probably wrong
	std::string                          time_format = ":%T";  // currently forcing
	std::formatter<std::chrono::seconds> timeStampFormatter;
};

class Message_Info
{
      public:
	Message_Info( ) { }
	Message_Info( LoggerLevel messageLevel, Message_Pattern formatPattern, std::string_view message, Message_Time messageTime )
	  : msgLevel( messageLevel ), formats( formatPattern ), msg( message ), msgTime( messageTime )
	{
	}

	std::string GetMsgTime( std::string fmt )
	{
		std::string pattern = "{" + fmt + "}";
		return msgTime.LocalTimeClock( pattern );
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
	Message_Time     msgTime;
};

// clang-format off
//! ############################################################# NOTE: Just So I Don't Lose The Pathing So Far #############################################################
/*
	For Time Formatting (Only Local Tested So Far):
	- GetMsgTime() Called With User Time Format Parameter (Currently Hard-coded)
	- Get The Time Format Pattern From The Parameter
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
		const char *msgColor;
		if( coloredOutput ) {
			msgColor = GetMsgColor( level );
		}
		else {
			msgColor = "";
		}
		std::cout << msgColor << MsgLevelToIcon( level ) << " "
			  << message.GetMsgTime( message.GetFormatPatterns( ).GetTimeFmt( ) ) << " "
			  << "[" << loggerName << "]:"
			  << " " << std::format( msg, std::forward<Args>( args )... ) << Reset( ) << "\n";
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
	Message_Info     message;
	LoggerLevel      flush_level;
	std::string_view loggerName = "Default_Console";
	bool             coloredOutput { true };
	// This Setup influenced by the hasher that spdlog uses to set colors for msg
	// lvl
	std::unordered_map<LoggerLevel, const char *> msgLevelColors;
};  // struct ColorConsole


int main( )
{
	using namespace se_colors;
	ColorConsole C( "Experimental Logger" );

	// Formatting Works With Latest Standard Flag
	auto s = "White";
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


	// Above Are The Settings For Logging Message Colors (Influenced by spdlog
	// message level colors)
	// - "fatal" changed to something I personally thought was more visually
	// appealing as a default. (spdlog does offer changes to default colors via
	// set_color() either way)

	// Currently 48 bytes
	// std::cout << "Size of Message_Info: " << sizeof( Message_Info );

#if ALLOC_TEST

	std::cout << "\n\nTotal Memory Allocated: [ " << total_bytes << " bytes] "
		  << "OR [" << total_bytes / 1000.0 << " KB]\n\n";
#endif
}
