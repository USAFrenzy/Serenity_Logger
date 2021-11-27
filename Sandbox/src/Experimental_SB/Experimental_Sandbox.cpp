#include <iostream>

#include <serenity/Color/Color.h>

#include <unordered_map>
#include <format>

#define INSTRUMENT 1
#define ALLOC_TEST 0

// Experimental
#include "Common.h"
#include "MessageDetails/Message_Info.h"
#include "MessageDetails/Message_Formatter.h"
#include "MessageDetails/Message_Time.h"


#include <serenity/Utilities/Utilities.h>
#include <serenity/Common.h>
#include "Targets/ColorConsoleTarget.h"
#include "Targets/FileTarget.h"


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

/************************************************************************************************************
									custom flags
						************************************
	- %N (Name)					- %L (Full Message Level)		- %x (Short Weekday String)
	- %l (Short Message Level)	- %n (DD/MMM/YY Date)			- %X (Long Weekday String)

							The rest are strftime equivalents
						******************************************
	- %d (Day Of Month)				- %T (HH:MM:SS Time format)		- %S (Seconds)
	- %D (MM/DD/YY Date)			- %w (weekday as decimal 0-6)	- %Y (Year XXXX)
	- %b (Abbrev Month Name)		- %F (YYYY-MM-DD Date)			- %M (Minute)
	- %B (Full Month Name)			- %H (24hr Hour format)			- %y (year XX Format)
************************************************************************************************************/

// clang-format off
// #####################################################################################################################################################
// This will probably be used for something more along the lines of just checking if a new flag that isn't default supported is being parsed, in a very 
// similar fashion to how spdlog allows users to create custom flags and custom flag argument handlers
// #####################################################################################################################################################
static constexpr std::array<std::string_view, 20> validFlags = { "%N", "%n", "%L", "%l", "%b", "%B", "%d", "%D", "%F",
						      "%H", "%M", "%S", "%T", "%w", "%y", "%Y", "%x", "%X" , "%a", "%A"};
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

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main( )
{
	std::vector<spdlog::sink_ptr> sinks;

	auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>( );
	sinks.emplace_back( stdoutSink );
	auto spdlogConsoleLogger = std::make_shared<spdlog::logger>( "Console Logger", begin( sinks ), end( sinks ) );
	spdlog::register_logger( spdlogConsoleLogger );
	spdlogConsoleLogger->set_pattern( "%^|%L| %a %d%b%C %T [%n]: %v%$" );  // equivalent to Target's Default Pattern

	// Too lazy to set up paths correctly at the moment, hard-coding paths for desktop/laptop
	//std::string filePath = "C:/Users/mccul/OneDrive/Desktop/Serenity_Logger/build/Sandbox/Logs/Spdlog_File_Bench.txt";
	 std::string filePath = "C:/Users/mccul/Desktop/Logging Project/build/Sandbox/Logs/Spdlog_File_Bench.txt";

	bool truncate = true;
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>( filePath, truncate );
	sinks.clear( );
	sinks.emplace_back( fileSink );
	auto spdlogFileLogger = std::make_shared<spdlog::logger>( "File Logger", begin( sinks ), end( sinks ) );
	spdlog::register_logger( spdlogFileLogger );
	spdlogFileLogger->set_pattern( "%^|%L| %a %d%b%C %T [%n]: %v%$" );  // equivalent to Target's Default Pattern


	using namespace se_colors;
	using namespace serenity::expiremental;
	using namespace serenity::se_utils;

	targets::ColorConsole C;

	targets::FileTarget testFile;
	// Flush_Policy          policy( Flush_Policy::Flush::periodically, Flush_Policy::Periodic_Options::mem_usage );
	// testFile.SetFlushPolicy( policy );

	const char *   test;
	Instrumentator macroTester;
	Instrumentator macroTesterFile;
	Instrumentator spdlogConsoleTester;
	Instrumentator spdlogFileTester;


#ifdef INSTRUMENTATION_ENABLED
	macroTester.StopWatch_Reset( );

	// test string
	std::string temp;
	for( int h = 0; h < 399; h++ ) {
		temp += "a";
	}  // 400 chars = 400 bytes
	test = temp.c_str( );
	unsigned long int i { 0 }, iterations {1000000 };
	std::cout << "Benching Color Console Target...\n\n";
	for( i; i < iterations; i++ ) {
		C.info( "{}", test );


#endif  // INSTRUMENTATION_ENABLED

#ifndef INSTRUMENTATION_ENABLED
		printf( "####################################################################\n" );
		printf( "# This Will Be The Default Pattern Format And Message Level Colors #\n" );
		printf( "####################################################################\n" );
		// Trace Is Default Color
		C.trace( "Trace" );
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
		printf( "####################################################################\n\n" );
		printf( "########################################################################\n" );
		printf( "# Testing Some Basic Functions To Make Sure All Is Working As I Expect #\n" );
		printf( "########################################################################\n" );
		C.SetPattern( "%T [%N]: " );
		C.info( "Pattern String Has Been Changed To \"%T [%N]: \"" );
		C.ColorizeOutput( false );
		C.info( "Colorized Output Set To False, Therefore, This Output Should Be The Default Color" );
		C.error( "This Line Should Also Still Lack Color" );
		C.ColorizeOutput( true );
		C.info( "Colorized Output Re-enabled, therefore this line should be colored" );
		C.info( "Log Level Will Be Set To \"warning\"" );
		C.SetLogLevel( LoggerLevel::warning );
		C.info( "Log Level Has Been Set To Warning, So This Line Should Not Print" );
		C.warn( "However, This Line Should Print" );
		C.fatal( "And This Line Should Also Print" );
		C.SetLogLevel( LoggerLevel::trace );
		C.info( "Log Level Set Back To \"trace\"" );
		C.ResetPatternToDefault( );
		C.info( "Pattern String Should Have Been Changed Back To Default Pattern" );
		C.SetMsgColor( LoggerLevel::warning, se_colors::bright_colors::combos::cyan::on_grey );
		C.trace( "Warning Level Color Has Been Changed To Cyan On Grey" );
		C.warn( "So This Line Should Now Be Cyan On Grey" );
		C.warn( "As Should This Line - Testing For Any One-Off Weirdness Here" );
		C.SetOriginalColors( );
		C.warn( "Colors Should Have Been Reset, So This Should Be Back To Bright Yellow" );


		// This Is Now Fully Working As Well
		testFile.EraseContents( );
		testFile.trace( "This Is A Trace Message To The File" );
		testFile.info( "This Is An Info Message To The File" );
		testFile.debug( "This Is A Debug Message To The File" );
		testFile.warn( "This Is A Warning Message To The File" );
		testFile.error( "This Is An Error Message To The File" );
		testFile.fatal( "This Is A Fatal Message To The File" );

		// Next Step Is To Benchmark And Flesh Out The FileTarget Class And Then Start Working On An HTML/XML Shredder
		// And Finally Link All The Targets Together Using A Singular Logging Class
#endif  // !INSTRUMENTATION_ENABLED

#ifdef INSTRUMENTATION_ENABLED
	}
	macroTester.StopWatch_Stop( );
	auto totalColorTime = macroTester.Elapsed_In( time_mode::ms );
	std::cout << "\nColor Console Target Bench Finished. Benching Spdlog Color Console Sink...\n\n";

	spdlogConsoleTester.StopWatch_Reset( );
	i = 0;  // reset
	for( i; i < iterations; i++ ) {
		spdlogConsoleLogger->info( "{}", test );
	}
	spdlogConsoleTester.StopWatch_Stop( );
	auto totalspdColorTime = spdlogConsoleTester.Elapsed_In( time_mode::ms );
	std::cout << "\nSpdlog Color Console Sink Bench Finished. Benching File Target...\n";

	i = 0;  // reset
	macroTesterFile.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		testFile.trace( test );
	}
	macroTesterFile.StopWatch_Stop( );
	auto totalFileTime = macroTesterFile.Elapsed_In( time_mode::ms );
	std::cout << "\nFile Target Bench Finished. Benching Spdlog Basic File Sink...\n";

	i = 0;  // reset
	spdlogFileTester.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		spdlogFileLogger->info( "{}", test );
	}
	spdlogFileTester.StopWatch_Stop( );
	auto totalSpdFileTime = spdlogFileTester.Elapsed_In( time_mode::ms );
	std::cout << "\nSpdlog Basic File Sink Bench Finished.\n\n";

	// Currently avereages ~ 0.14ms for a C-Style String Of 400 Bytes Over 1,000,000 iterations -> Not a bad start =D
	// (Granted Not Apples to apples given this is testing a console target that has nowhere near as many features and
	// safety nets in place, but as a reference, spdlog's null sink single thread C-Style String of 400 bytes benches at
	// ~40ms taken from their GH repo bench stats
	std::cout << Tag::Yellow(
		       "\n\n***************************************************************************************\n"
		       "*************** Instrumentation Data (Averaged Over " )
		  << Tag::Yellow( std::to_string( iterations ) + " Iterations: " )
		  << Tag::Yellow(
		       "***************\n"
		       "***************************************************************************************\n" );
	std::cout << Tag::Bright_Yellow( "Color Console Target (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );

	std::cout << Tag::Bright_Yellow( "Spdlog Color Console Sink (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );


	std::cout << Tag::Bright_Yellow( "File Target (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTesterFile.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTesterFile.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
		  << Tag::Bright_Green( std::to_string( macroTesterFile.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );

	std::cout << Tag::Bright_Yellow( "Spdlog Basic File Sink (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogFileTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
		  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogFileTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
		  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
		  << Tag::Bright_Green( std::to_string( spdlogFileTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );


	std::cout << "\n";


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
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::ColorConsole ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of FileTarget Class:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::FileTarget ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Info Class:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Info ) ) + "\tbytes ]\n" );


	std::cout << Tag::Bright_Yellow( "Size of Message_Formatter Class:\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Formatter ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Time Class:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Time ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Cached_Date_Time Class:\t\t" )
		  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Cached_Date_Time ) ) + "\tbytes ]\n" );


	std::cout << Tag::Yellow( "***************************************************************************************\n" );
	std::cout << Tag::Yellow( "***************************************************************************************\n" );
	std::cout << Tag::Yellow( "***************************************************************************************\n\n" );

#endif  // INSTRUMENTATION_ENABLED
}
