#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>

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
	undefined
};

static std::string_view msgLevelToStr( LoggerLevel level )
{
	switch( level ) {
		case LoggerLevel::info: return "[Info]"; break;
		case LoggerLevel::trace: return "[Trace]"; break;
		case LoggerLevel::debug: return "[Debug]"; break;
		case LoggerLevel::warning: return "[Warning]"; break;
		case LoggerLevel::error: return "[Error]"; break;
		case LoggerLevel::fatal: return "[Fatal]"; break;
		case LoggerLevel::undefined: return "[Undefined]"; break;
		default: return "";
	}
}
// ***************************************************************************
struct Message_Time
{
};

struct Message_Info
{
	Message_Info( ) = default;
	Message_Info( LoggerLevel messageLevel, std::string_view formatPattern, std::string_view message, void *messageTime )
	  : msgLevel( messageLevel ), fmtPattern( formatPattern ), msg( message ), msgTime( messageTime )
	{
	}
	LoggerLevel      msgLevel { LoggerLevel::trace };
	std::string_view msg;
	std::string_view fmtPattern;
	void *           msgTime /* std::chrono time stuff here */;
};

// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the project I'll
// be using this in (< win 8.1 EOL).
class ConsoleColors
{
      public:
	ConsoleColors( )
	{
		flush_level    = LoggerLevel::trace;
		msgLevelColors = {
		  { LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
		  { LoggerLevel::info, se_colors::bright_colors::foreground::green },
		  { LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
		  { LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
		  { LoggerLevel::error, se_colors::basic_colors::foreground::red },
		  { LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
		  { LoggerLevel::undefined, se_colors::basic_colors::combos::white::on_black },
		};
	}

	void SetMsgColor( const char *color )
	{
		msgLevelColors[ message.msgLevel ] = color;
	}
	const char *GetMsgColor( )
	{
		return msgLevelColors[ message.msgLevel ];
	}

	bool ShouldLog( )
	{
		return ( message.msgLevel <= flush_level ) ? true : false;
	}
	void PrintWithColor( const std::string_view msg )
	{
		std::cout << GetMsgColor( ) << msgLevelToStr( message.msgLevel ) << " [" << loggerName << "]: " << toString( msg )
			  << Reset( ) << "\n";
	}

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
	// This Setup influenced by the hasher that spdlog uses to set colors for msg lvl
	std::unordered_map<LoggerLevel, const char *> msgLevelColors;
};  // struct ConsoleColors


int main( )
{
	using namespace se_colors;

	ConsoleColors C;

	// Trace Is Deafult Color
	C.SetMsgColor( bright_colors::combos::white::on_black );
	C.PrintWithColor( "Trace Will Be Bright White" );
	// Info Is Light Green
	C.SetMsgColor( bright_colors::foreground::green );
	C.PrintWithColor( "Info Will Be Bright Green" );
	// Debug Is Light Cyan
	C.SetMsgColor( bright_colors::foreground::cyan );
	C.PrintWithColor( "Debug Will Be Bright Cyan" );
	// Warning Is Light Yellow
	C.SetMsgColor( bright_colors::foreground::yellow );
	C.PrintWithColor( "Warning Will Be Bright Yellow" );
	// Error Is Dark Red
	C.SetMsgColor( basic_colors::foreground::red );
	C.PrintWithColor( "Error Will Be Red" );
	// Fatal Is Light Yellow On Dark Red
	C.SetMsgColor( bright_colors::combos::yellow::on_red );
	C.PrintWithColor( "Fatal Will Be Bright Yellow On Red" );

	// Above Are The Settings For Logging Message Colors (Influenced by spdlog message level colors)
	// - "fatal" changed to something I personally thought was more visually appealing as a default.
	// (spdlog does offer changes to default colors via set_color() either way)

	// Currently 48 bytes
	// std::cout << "Size of Message_Info: " << sizeof( Message_Info );

#if ALLOC_TEST

	std::cout << "\n\nTotal Memory Allocated: [ " << total_bytes << " bytes] "
		  << "OR [" << total_bytes / 1000.0 << " KB]\n\n";
#endif
}
