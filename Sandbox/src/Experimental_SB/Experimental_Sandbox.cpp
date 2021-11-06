#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>
#include <format>


// Experimental
#include "Common.h"
#include "MessageDetails/Message_Info.h"
#include "MessageDetails/Message_Pattern.h"
#include "MessageDetails/Message_Time.h"


#include <serenity/Utilities/Utilities.h>
#include <serenity/Common.h>
#include "Targets/ColorConsoleTarget.h"

#define INSTRUMENT 1
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
// clang-format off
// #####################################################################################################################################################
// This will probably be used for something more along the lines of just checking if a new flag that isn't default supported is being parsed, in a very 
// similar fashion to how spdlog allows users to create custom flags and custom flag argument handlers
// #####################################################################################################################################################
static constexpr std::array<std::string_view, 18> validFlags = { "%N", "%n", "%L", "%l", "%b", "%B", "%d", "%D", "%F",
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
	l_wkday      = 1 << 15,  // Mapping To %X 
	s_wkday      = 1 << 16,  // Mapping To %x
	ddmmmyy_date = 1 << 17,  // Mapping To %n
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
// #####################################################################################################################################################
// clang-format on


int main( )
{
	using namespace se_colors;
	using namespace serenity::expiremental;
	using namespace serenity::se_utils;

	targets::ColorConsole C;
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
	unsigned long int i { 0 }, iterations { 1000000 };
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
	std::cout << Tag::Bright_Yellow( "Averaged Total Elapsed Time (Averaged Over " )
		  << Tag::Bright_Yellow( std::to_string( iterations ) ) << Tag::Bright_Yellow( " Iterations:\n " )
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
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::ColorConsole ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Info Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Info ) ) + " bytes]\n" );


	std::cout << Tag::Bright_Yellow( "Size of Message_Pattern Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Pattern ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Time Struct:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Time ) ) + " bytes]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Cached_Date_Time Struct:\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Cached_Date_Time ) ) + " bytes]\n" );

	std::cout << Tag::Yellow( "******************************************************************\n" );
	std::cout << Tag::Yellow( "******************************************************************\n" );
	std::cout << Tag::Yellow( "******************************************************************\n\n" );

#endif  // INSTRUMENTATION_ENABLED
}
