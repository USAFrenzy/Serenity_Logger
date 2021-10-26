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
	undefined
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
		case LoggerLevel::undefined: return "[U]"; break;
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

enum class time_mode
{
	local,
	uct,
};

struct Message_Time
{
	Message_Time( time_mode tm )
	{
		using namespace std::chrono;
		mode = tm;
		const zoned_time currentTime { current_zone( ), floor<seconds>( system_clock::now( ) ) };
		current_time = currentTime;
		if( mode == time_mode::uct ) {
			current_time = currentTime.get_sys_time( );
		}
	}

	time_mode                  mode;
	std::chrono::zoned_seconds current_time;
};

struct Message_Pattern
{
	Message_Pattern( ) { }                           // what to set if no pattern passed in
	Message_Pattern( std::string_view pattern ) { }  // use user's pattern

	void SetMsgPattern( std::string_view pattern ) { }
	/* Pattern Parsing, Setting, Getting, Etc Done Here*/

	std::formatter<std::chrono::sys_time<std::chrono::seconds>> timeStampFormatter;
};

struct Message_Info
{
	Message_Info( ) { }
	Message_Info( LoggerLevel messageLevel, Message_Pattern formatPattern, std::string_view message, Message_Time messageTime )
	  : msgLevel( messageLevel ), fmtPattern( formatPattern ), msg( message ), msgTime( messageTime )
	{
	}

	auto GetMsgTime( )
	{
		return std::format( "{:%T}", msgTime.current_time );
	}

	LoggerLevel      msgLevel { LoggerLevel::trace };
	std::string_view msg;
	Message_Pattern  fmtPattern = { };
	Message_Time     msgTime    = { time_mode::local };
};

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
		  { LoggerLevel::undefined, "" },  // if undefined, do nothing (add no codes)
		};
	}

	void SetMsgColor( LoggerLevel level, const char *color )
	{
		msgLevelColors[ level ] = color;
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
		return ( message.msgLevel <= level ) ? true : false;
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
		std::cout << msgColor << MsgLevelToIcon( level ) << " " << message.GetMsgTime( ) << " "
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
