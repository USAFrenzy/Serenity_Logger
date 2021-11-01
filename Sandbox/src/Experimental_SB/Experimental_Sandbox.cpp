#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>
#include <format>
#include <chrono>

#include <ctime>
#include <time.h>

#include <serenity/Utilities/Utilities.h>

#define INSTRUMENT 0
#define ALLOC_TEST 0

#ifndef NDEBUG
	#define DB_PRINT( msg, ... ) ( std::cout << std::format( msg, __VA_ARGS__ ) << "\n" )
#else
	#define DB_PRINT( msg, ... )
#endif  // !NDEBUG

#if INSTRUMENT
	#define INSTRUMENTATION_ENABLED
#endif


#if ALLOC_TEST  // Testing Allocations
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

static std::string svToString( std::string_view s )
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

constexpr std::array<std::string_view, 7> short_weekdays = { "Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat" };

constexpr std::array<std::string_view, 7> long_weekdays = { "Sunday",   "Monday", "Tuesday", "Wednesday",
							    "Thursday", "Friday", "Saturday" };

constexpr std::array<std::string_view, 12> short_months = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
							    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

constexpr std::array<std::string_view, 12> long_months = { "January", "February", "March",     "April",   "May",      "June",
							   "July",    "August",   "September", "October", "November", "December" };

/***********************************************************************************************************
*	                                            29/10/21 NOTE:	                                            *
*					 How I am approaching the flag parsing and setting is as follows                     *
************************************************************************************************************
*      For The Initial Caching Of Those Pattern Variables So That It's Inexpensive To Parse The Format     *
************************************************************************************************************
* 0)  Copy Message_Info pattern string passed in at construction into Message_Time pattern string variable
* 1)  Using validFlags array, find each instance a valid flag appears in pattern format
* 2a) When flag is found, capture the flag character and forward to a flag handler function
* 2b) Map the flag character to the Flags enum bitfield value and set that field
* 2c) Repeat this until end of pattern is reached
* 3)  In a caching function that evaulates which bit fields are set, set the appropriate member tm struct
*     variables.
//! Note: The Above Requires That A Pattern Is Present For Message_Time And That The Current Caching Function
//! Is Altered To Reflect The Thoughts Above
//! Step 0: Subject To Change Based On How Logging Targets Are Created And How These Fields Get Passed Around
************************************************************************************************************
*                   For The Actual Printing/Logging Using The Prepended Pattern:                           *
************************************************************************************************************
* 1)  Similar to above, using validFlags array, find each instance a valid flag appears in pattern format
* 2a) When flag is found, append all characters found up until that flag to an internal buffer
* 2b) In a separate flag handler function that deals with cached variables from above, pass in the flag
      captured, return the value and append to buffer
* 2c) Continue this until end of pattern string is reached and return the pattern buffer
* 3)  In the printing function, append the message to the buffer that was returned and print message
*************************************************************************************************************/

/************************************************************************************************************
									custom flags
						************************************
	- %N (Name)				- %L (Full Message Level)		- %x (Short Weekday String)
	- %l (Short Message Level)	- %n (DD/MMM/YY Date)			- %X (Long Weekday String)

							The rest are strftime equivalents
						******************************************
	- %d (Day Of Month)			- %T (HH:MM:SS Time format)		- %S (Seconds)
	- %D (MM/DD/YY Date)			- %w (weekday as decimal 0-6)	- %Y (Year XXXX)
	- %b (Abbrev Month Name)		- %F (YYYY-MM-DD Date)			- %M (Minute)
	- %B (Full Month Name)			- %H (24hr Hour format)		- %y (year XX Format)
************************************************************************************************************/
constexpr std::array<const char *, 18> validFlags = { "%N", "%n", "%L", "%l", "%b", "%B", "%d", "%D", "%F",
						      "%H", "%M", "%S", "%T", "%w", "%y", "%Y", "%x", "%X" };
// This Might Also NOT Be The Way To Aid Formatting.. This Is Just An Idea
// *************************************************************************
_Enum_is_bitflag_ enum class Flags : uint32_t {
	name         = 0,        // Mapping To %N
	f_msg_lvl    = 1 << 1,   // Mapping To %L
	s_msg_lvl    = 1 << 2,   // Mapping To %l
	d_wkday      = 1 << 3,   // Mapping To %w
	l_month      = 1 << 4,   // Mapping To %B
	s_month      = 1 << 5,   // Mapping To %b
	m_day        = 1 << 6,   // Mapping To %d
	l_year       = 1 << 7,   // Mapping To %Y
	s_year       = 1 << 8,   // Mapping To %y
	mdy_date     = 1 << 9,   // Mapping To %D
	ymd_date     = 1 << 10,  // Mapping To %F
	full_time    = 1 << 11,  // Mapping To %T
	mil_hour     = 1 << 12,  // Mapping To %H
	min          = 1 << 13,  // Mapping To %M
	sec          = 1 << 14,  // Mapping To %S
	l_wkday      = 1 << 15,
	s_wkday      = 1 << 16,
	ddmmmyy_date = 1 << 17,
	// More to possibly come..(obviously would have to change inherited type attr if additions made)
};


inline Flags operator|( Flags a, Flags b )
{
	return static_cast<Flags>( static_cast<uint16_t>( a ) | static_cast<uint16_t>( b ) );
}

static const std::unordered_map<std::string_view, Flags> flagMapper = {
  { "%N", Flags::name },     { "%L", Flags::f_msg_lvl }, { "%l", Flags::s_msg_lvl }, { "%w", Flags::d_wkday },
  { "%x", Flags::s_wkday },  { "%X", Flags::l_wkday },   { "%B", Flags::l_month },   { "%b", Flags::s_month },
  { "%d", Flags::m_day },    { "%Y", Flags::l_year },    { "%y", Flags::s_year },    { "%D", Flags::mdy_date },
  { "%F", Flags::ymd_date }, { "%T", Flags::full_time }, { "%H", Flags::mil_hour },  { "%M", Flags::min },
  { "%S", Flags::sec },
};


struct Cached_Date_Time
{
	std::string_view long_weekday;   // Long Weekday Name String representation mapped from DayString()
	std::string_view short_weekday;  // Short Weekday Name String representation mapped from DayString()
	int              dec_wkday;
	int              hour;
	int              min;
	int              sec;
	int              long_year { 0 };   // Full Year representation mapped from GetCurrentYear();
	int              short_year { 0 };  // Short Year representation mapped from GetCurrentYear();
	std::string_view long_month;        // Long Month Name String representation mapped from MonthString()
	std::string_view short_month;       // Short Month Name String representation mapped from MonthString()
	int              dec_month;         // zero-padded string representation of decimal month
	int              day;
	bool             initialized { false };
};

class Message_Time
{
      public:
	Message_Time( message_time_mode mode ) : m_mode( mode )
	{
		UpdateTimeInfo( );
	}
	std::string_view WeekdayString( int weekdayIndex, bool shortened = false )
	{
		if( !shortened ) {
			return long_weekdays.at( weekdayIndex );
		}
		else {
			return short_weekdays.at( weekdayIndex );
		}
	}
	int GetCurrentYear( int yearOffset, bool shortened = false )
	{
		if( !shortened ) {
			return 1900 + yearOffset;  //  Format XXXX, same as %Y strftime
		}
		else {
			return yearOffset - 100;  //  Format XX, Same as %y strftime
		}
	}
	std::string_view MonthString( int monthIndex, bool shortened = false )
	{
		if( !shortened ) {
			return long_months.at( monthIndex );
		}
		else {
			return short_months.at( monthIndex );
		}
	}

	std::string ZeroPadDecimal( int dec )
	{
		return ( dec >= 10 ) ? std::to_string( dec ) : "0" + std::to_string( dec );
	}

	void InitializeCache( std::tm *t )
	{
		m_cache.long_year     = std::move( GetCurrentYear( t->tm_year ) );
		m_cache.short_year    = std::move( GetCurrentYear( t->tm_year, true ) );
		m_cache.long_month    = std::move( MonthString( t->tm_mon ) );
		m_cache.dec_month     = std::move( ( t->tm_mon + 1 ) );
		m_cache.short_month   = std::move( MonthString( t->tm_mon, true ) );
		m_cache.long_weekday  = std::move( WeekdayString( t->tm_wday ) );
		m_cache.short_weekday = std::move( WeekdayString( t->tm_wday, true ) );
		m_cache.dec_wkday     = t->tm_wday;
		m_cache.day           = t->tm_mday;
		m_cache.hour          = t->tm_hour;
		m_cache.min           = t->tm_min;
		m_cache.sec           = t->tm_sec;
		m_cache.initialized   = true;
	}


	Cached_Date_Time UpdateCacheTime( )
	{
		time( &m_time );
		t_struct     = std::localtime( &m_time );
		m_cache.sec  = t_struct->tm_sec;
		m_cache.hour = t_struct->tm_hour;
		m_cache.min  = t_struct->tm_min;
		return m_cache;
	}

	void UpdateTimeInfo( )
	{
		// For both modes, check if the cache has been initialized, if so, then just update time variables. If not, then
		// populate t_struct with time date variables and initialize cache
		if( m_cache.initialized ) {
			UpdateCacheTime( );
		}
		else {
			m_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now( ) );
			if( m_mode == message_time_mode::local ) {
				t_struct = std::localtime( &m_time );
				InitializeCache( t_struct );
			}
			else {
				t_struct = std::gmtime( &m_time );
				InitializeCache( t_struct );
			}
		}
	}


	inline const message_time_mode Mode( )
	{
		return m_mode;
	}

	const Cached_Date_Time Cache( )
	{
		return m_cache;
	}

      private:
	message_time_mode m_mode;
	std::time_t       m_time;
	std::tm *         t_struct = { };
	Cached_Date_Time  m_cache  = { };
};

class Message_Info
{
      public:
	Message_Info( std::string name ) : m_name( name ), msgLevel( LoggerLevel::trace ), msg( ), msgTime( message_time_mode::local )
	{
	}

	Message_Info( ) : m_name( "Default Logger" ), msgLevel( LoggerLevel::trace ), msg( ), msgTime( message_time_mode::local ) { }

	Message_Info( std::string name, LoggerLevel messageLevel, std::string_view message, Message_Time messageTime )
	  : m_name( name ), msgLevel( messageLevel ), msg( message ), msgTime( messageTime )
	{
	}
	Message_Info &operator=( const Message_Info &t )
	{
		msg      = t.msg;
		msgLevel = t.msgLevel;
		msgTime  = t.msgTime;
		m_name   = t.m_name;
		return *this;
	}
	LoggerLevel MsgLevel( )
	{
		return msgLevel;
	}
	void SetName( std::string name )
	{
		m_name = name;
	}
	std::string Name( )
	{
		return m_name;
	}

	Message_Time TimeDetails( )
	{
		return msgTime;
	}

	void SetMessageLevel( LoggerLevel messageLvl )
	{
		msgLevel = messageLvl;
	}


      private:
	std::string      m_name;
	LoggerLevel      msgLevel;
	std::string_view msg;
	Message_Time     msgTime;
};

// *************************************************************************
class Message_Pattern
{
      public:
	// Weekday Day Month Year HH::MM::SS -UTC Offset Default If No User Pattern Set
	Message_Pattern( ) : fmtPattern( "[DEFAULT PATTERN PLACEHOLDER] " ), msgInfo( ) { }
	Message_Pattern( std::string formatPattern, Message_Info *msgDetails ) : fmtPattern( formatPattern ), msgInfo( msgDetails ) { }
	Message_Pattern &operator=( const Message_Pattern &t )
	{
		buffer     = t.buffer;
		fmtPattern = t.fmtPattern;
		msgInfo    = t.msgInfo;
		return *this;
	}
	std::string GetMsgFmt( )
	{
		return fmtPattern;
	}

	// Quarter Way There!! FInds The Flag, Handles The Flag, And Appends Formatted Message To Pre Mesage String
	// However, Drops All Other Characters After Returning From Flag Handle
	// TODO: Finish Working On This And Test For Flags As Well As Fix Dropping Characters After Flag Argument Returns Value
	template <typename... Args> std::string_view FormatMessage( std::string message, Args &&...args )
	{
		buffer.clear( );
		size_t it { 0 }, position { 0 };
		auto   fmt = fmtPattern;
		//! DELETE THIS PRINT STATEMENT WHEN DONE
		DB_PRINT( "\n" );
		while( it != std::string::npos ) {
			if( ( position != std::string::npos ) && ( !fmt.empty( ) ) ) {
				if( fmt.front( ) == '%' ) {
					// std::cout << "Searching For Flags\n";
					size_t      pos { 0 };
					std::string potentialFlag;
					if( ( pos = fmt.find( "%" ) ) != std::string::npos ) {
						if( fmt.front( ) == '%' ) {
							potentialFlag = fmt.substr( pos, pos + 2 );
						}
						else {
							potentialFlag = fmt.substr( pos, pos + 1 );
						}
						//! DELETE THIS PRINT STATEMENT WHEN DONE
						DB_PRINT( std::format( "Potential Flag Found: \"{}\"", potentialFlag ) );

						if( std::any_of( validFlags.begin( ), validFlags.end( ), [ = ]( const char *m ) {
#ifndef NDEBUG
							    //! DELETE THIS CHECK WHEN DONE -> JUST RETURN THE BOOLEAN CHECK DIRECTLY
							    if( potentialFlag == m ) {
								    //! DELETE THIS PRINT STATEMENT WHEN DONE
								    DB_PRINT( "Flag \"{}\" Matches \"{}\" - Valid Flag", potentialFlag, m );
								    return true;
							    }
							    else {
								    //! DELETE THIS PRINT STATEMENT WHEN DONE
								    DB_PRINT( "Flag \"{}\" Does Not Match \"{}\"", potentialFlag, m );
								    return false;
							    }
#else
							      return potentialFlag == m;
#endif  // !NDEBUG
						    } ) )
						{
							std::string token;
							// while searching for flag positions, append each char to buffer
							if( ( ( position = fmt.find( "%" ) ) != std::string::npos ) ) {
								token = fmt.substr( 0, position );
								//! DELETE THIS PRINT STATEMENT WHEN DONE
								DB_PRINT( "Token SubString Before Flag Reached: {}", token );
								buffer.append( token );
								// after appending everything up until the "%", erase that subsection
								// (+1 for % length), handle the flag, and continue iterating until the
								// end of format pattern is reached
								fmt.erase( 0, position + 1 );
								//! DELETE THIS PRINT STATEMENT WHEN DONE
								DB_PRINT( "String Before Flag Token Deleted: {}", fmt );
								buffer.append( FlagFormatter( fmt.front( ) ) );
								fmt.erase( 0, position + 1 );  // Erase the Flag Token
											       //! DELETE THIS PRINT STATEMENT WHEN DONE
								DB_PRINT( "String After Flag Token Deleted: {}", fmt );
							}
						}
					}
				}
				else {
					DB_PRINT( "Format Char To Append: {}", fmt.front( ) );
					buffer += fmt.at( 0 );
					DB_PRINT( "Buffer String So Far: {}", buffer );
					fmt.erase( fmt.begin( ), fmt.begin( ) + 1 );
				}
			}
			else {
				break;  // Reached End Of Format String And No More Flags Found
			}
		}
		if( fmtPattern.size( ) == 0 ) {
			//! DELETE THIS PRINT STATEMENT WHEN DONE
			DB_PRINT( "Found No Flags" );
			return buffer;
		}
		else {
			//! DELETE THIS PRINT STATEMENT WHEN DONE
			DB_PRINT( "Found No More Flags\n" );
			return buffer.append( fmt + std::move( std::format( message, std::forward<Args>( args )... ) ) );
		}
	}


      private:
	std::string FlagFormatter( char flag )
	{
		switch( flag ) {
			case 'b': return Format_Arg_b( ); break;
			case 'B': return Format_Arg_B( ); break;
			case 'd': return Format_Arg_d( ); break;
			case 'D': return Format_Arg_D( ); break;
			case 'F': return Format_Arg_F( ); break;
			case 'H': return Format_Arg_H( ); break;
			case 'L': return Format_Arg_L( ); break;
			case 'l': return Format_Arg_l( ); break;
			case 'M': return Format_Arg_M( ); break;
			case 'n': return Format_Arg_n( ); break;
			case 'N': return Format_Arg_N( ); break;
			case 'S': return Format_Arg_S( ); break;
			case 'T': return Format_Arg_T( ); break;
			case 'w': return Format_Arg_w( ); break;
			case 'x': return Format_Arg_x( ); break;
			case 'X': return Format_Arg_X( ); break;
			case 'y': return Format_Arg_y( ); break;
			case 'Y': return Format_Arg_Y( ); break;
			default: return ""; break;
		}
	}

	// TODO: Finish Flag Argument Formatters
	std::string Format_Arg_b( )
	{
		return svToString( msgInfo->TimeDetails( ).Cache( ).short_month );
	}

	std::string Format_Arg_B( )
	{
		return svToString( msgInfo->TimeDetails( ).Cache( ).long_month );
	}

	std::string Format_Arg_D( )
	{
		// MM/DD/YY
		std::string date;
		date.append( Format_Arg_d( ) + "/" );
		date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).day ) + "/" );
		return date.append( Format_Arg_y( ) );
	}

	std::string Format_Arg_F( )
	{
		// YYYY-MM-DD
		std::string date;
		date.append( Format_Arg_Y( ) + "-" );
		date.append( Format_Arg_d( ) + "-" );
		return date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).day ) );
	}

	std::string Format_Arg_M( )
	{
		return svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateCacheTime( ).min ) );
	}

	std::string Format_Arg_S( )
	{
		return svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateCacheTime( ).sec ) );
	}

	// Seconds Conter Not Incrementing
	std::string Format_Arg_T( )
	{
		auto cache = msgInfo->TimeDetails( ).UpdateCacheTime( );
		// HH:MM:SS
		std::string time;
		time += svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) ) + ":";
		time += svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) ) + ":";
		time += svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
		return time;
	}

	std::string Format_Arg_w( )
	{
		return std::to_string( msgInfo->TimeDetails( ).Cache( ).dec_wkday );
	}

	std::string Format_Arg_y( )
	{
		return std::to_string( msgInfo->TimeDetails( ).Cache( ).short_year );
	}

	std::string Format_Arg_Y( )
	{
		return std::to_string( msgInfo->TimeDetails( ).Cache( ).long_year );
	}

	std::string Format_Arg_N( )
	{
		return msgInfo->Name( );
	}

	std::string Format_Arg_l( )
	{
		return svToString( MsgLevelToShortString( msgInfo->MsgLevel( ) ) );
	}

	std::string Format_Arg_L( )
	{
		return svToString( MsgLevelToString( msgInfo->MsgLevel( ) ) );
	}

	std::string Format_Arg_d( )
	{
		return svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).Cache( ).day ) );
	}

	std::string Format_Arg_H( )
	{
		return svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateCacheTime( ).hour ) );
	}


	std::string Format_Arg_x( )
	{
		return svToString( msgInfo->TimeDetails( ).Cache( ).short_weekday );
	}

	std::string Format_Arg_X( )
	{
		return svToString( msgInfo->TimeDetails( ).Cache( ).long_weekday );
	}

	std::string Format_Arg_n( )
	{
		std::string date;
		date.append( svToString( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).Cache( ).day ) ) );
		date.append( svToString( msgInfo->TimeDetails( ).Cache( ).short_month ) );
		date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).short_year ) );
		return date;
	}

      private:
	std::string   fmtPattern;
	Message_Info *msgInfo;
	std::string   buffer;
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
	ColorConsole( )
	{
		msgDetails.SetName( loggerName );
		msgPattern     = { pattern, &msgDetails };
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
	ColorConsole( std::string name )
	{
		loggerName = std::move( name );
		msgDetails.SetName( loggerName );
		msgPattern     = { pattern, &msgDetails };
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

	void SetMsgColor( LoggerLevel level, std::string_view color )
	{
		msgLevelColors.at( level ) = color;
	}

	std::string_view GetMsgColor( LoggerLevel level )
	{
		return msgLevelColors.at( level );
	}

	bool ColorizeOutput( bool colorize )
	{
		coloredOutput = colorize;
	}

	bool ShouldLog( LoggerLevel level )
	{
		return ( logLevel <= msgDetails.MsgLevel( ) ) ? true : false;
	}

	template <typename... Args> void PrintMessage( LoggerLevel level, const std::string msg, Args... args )
	{
		msgDetails.SetMessageLevel( level );

		std::string_view msgColor;
		if( coloredOutput ) {
			msgColor = GetMsgColor( level );
		}
		std::cout << msgColor << msgPattern.FormatMessage( msg, std::forward<Args>( args )... ) << Reset( ) << "\n";
	}

	// Not Apart Of This Class - Just Here For Testing (Part Of Logger Class)
	// ##########################################################################
	template <typename... Args> void trace( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::trace ) ) {
			PrintMessage( LoggerLevel::trace, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void info( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::info ) ) {
			PrintMessage( LoggerLevel::info, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void debug( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::debug ) ) {
			PrintMessage( LoggerLevel::debug, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void warn( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::warning ) ) {
			PrintMessage( LoggerLevel::warning, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void error( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::error ) ) {
			PrintMessage( LoggerLevel::error, s, std::forward<Args>( args )... );
		}
	}
	template <typename... Args> void fatal( std::string s, Args &&...args )
	{
		if( ShouldLog( LoggerLevel::fatal ) ) {
			PrintMessage( LoggerLevel::fatal, s, std::forward<Args>( args )... );
		}
	}
	// ##########################################################################

	std::string svToString( const std::string_view s )
	{
		return std::string( s.data( ), s.size( ) );
	}

      private:
	std::string Reset( )
	{
		return se_colors::formats::reset;
	}


      private:
	std::string                                       loggerName = "Console_Logger";
	std::string                                       pattern    = "|%l| %x %n %T [%N]: ";
	Message_Info                                      msgDetails = { loggerName };
	Message_Pattern                                   msgPattern = { pattern, &msgDetails };
	bool                                              coloredOutput { true };
	LoggerLevel                                       logLevel { LoggerLevel::trace };
	std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
};  // class ColorConsole

int main( )
{
	using namespace se_colors;
	using namespace serenity::se_utils;
	ColorConsole C;
	// Formatting Works With Latest Standard Flag
	auto s = "White";

	const char *   test;
	Instrumentator macroTester;
#ifdef INSTRUMENTATION_ENABLED
	macroTester.StopWatch_Reset( );

	// test string
	std::string temp;
	for( int i = 0; i < 399; i++ ) {
		temp += "a";
	}  // 400 chars = 400 bytes
	test = temp.c_str( );
	int i { 0 }, iterations { 1000000 };
	for( i; i < iterations; i++ ) {
#else
	test = "Trace";
#endif  // INSTRUMENTATION_ENABLED
	// Trace Is Default Color
		C.trace( "{}", test );
#ifndef INSTRUMENTATION_ENABLED
		// Info Is Light Green
		C.info( "Info" );
		// Debug Is Light Cyan
		C.debug( "Debug" );
		// Warning Is Light Yellow
		C.warn( "Warning" );
		// Error Is Dark Red
		C.error( "Error" );
		// Fatal Is Light Yellow On Dark Red
		C.fatal( "Fatal" );
#endif  // !INSTRUMENTATION_ENABLED

#ifdef INSTRUMENTATION_ENABLED
	}
	macroTester.StopWatch_Stop( );

	// Currently avereages ~ 0.14ms for a C-Style String Of 400 Bytes Over 1,000,000 iterations -> Not a bad start =D
	// (Granted Not Apples to apples given this is testing a console target that has nowhere near as many features and
	// safety nets in place, but as a reference, spdlog's null sink single thread C-Style String of 400 bytes benches at
	// ~40ms taken from their GH repo bench stats
	std::cout << Tag::Yellow(
	  "\n\n******************************************************************\n******************** Instrumentation Data: "
	  "***********************\n******************************************************************\n" );
	std::cout << Tag::Bright_Yellow( "Averaged Total Elapsed Time (Averaged Over 1,000,000 Iterations):\n" )
		  << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );
#endif  // INSTRUMENTATION_ENABLED

#if ALLOC_TEST
	auto mem_used = macroTester.mem_tracker.Memory_Usage( );
	// setting up for the current usage and allocations so no variation in conversions
	std::cout << Tag::Bright_Yellow( "Total Memory Used:\n" ) << Tag::Bright_Cyan( "\t- In Bytes:\t\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( mem_used ) + " bytes]\n" )
		  << Tag::Bright_Cyan( "\t- In Kilobytes:\t\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( mem_used / 1000.0 ) + " KB]\n" );
#endif  // ALLOC_TEST

#ifdef INSTRUMENTATION_ENABLED
	std::cout << Tag::Bright_Yellow( "Size of ColorConsole Class:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( ColorConsole ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Info Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( Message_Info ) ) + " bytes]\n" );


	std::cout << Tag::Bright_Yellow( "Size of Message_Pattern Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( Message_Pattern ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Time Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( Message_Time ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Cached_Date_Time Struct:\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( Cached_Date_Time ) ) + " bytes]\n" );

	std::cout << Tag::Yellow( "******************************************************************\n" );
	std::cout << Tag::Yellow( "******************************************************************\n" );
	std::cout << Tag::Yellow( "******************************************************************\n\n" );

#endif  // INSTRUMENTATION_ENABLED
}
