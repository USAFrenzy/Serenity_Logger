#include <iostream>

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

// clang-format off
// TODO: ####################################################################################################################################
// TODO: Remove Internal Dependancy On Utilities File - I'll work on that as a separate project from this (as it honestly should have been)
// TODO: ####################################################################################################################################
// clang-format on

#define INSTRUMENT 1

#if INSTRUMENT
	#define INSTRUMENTATION_ENABLED
#endif

// TODO: Finish adding strftime equivalent flags and the last two original flags
// TODO: and then figure a way to add user defined flags and formatting callbacks
/************************************************************************************************************
									   custom flags
							************************************
	- %N (Name)                   - %L (Full Message Level)      - %x (Short Weekday String)
	- %l (Short Message Level)	    - %n (DD/MMM/YY Date)          - %X (Long Weekday String)

							  The rest are strftime equivalents
						 ******************************************
	- %d (Day Of Month)          - %T (HH:MM:SS Time format)     - %S (Seconds)
	- %D (MM/DD/YY Date)         - %w (weekday as decimal 0-6)   - %Y (Year XXXX)
	- %b (Abbrev Month Name)     - %F (YYYY-MM-DD Date)          - %M (Minute)
	- %B (Full Month Name)	   - %H (24hr Hour format)	         - %y (year XX Format)
************************************************************************************************************/

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
// Want to keep this utility file separate from actual project as it is itself a WIP project anyways
#include <serenity/Utilities/Utilities.h>

// This is just used to write all logs in the same directory as the serenity targets
std::string SpdlogPath( bool rotate = false )
{
	if( !rotate ) {
		auto filePath { std::filesystem::current_path( ) };
		filePath /= "Logs/Spdlog_File.txt";
		return filePath.make_preferred( ).string( );
	} else {
		auto filePath { std::filesystem::current_path( ) };
		filePath /= "Logs/Spdlog_Rotating.txt";
		return filePath.make_preferred( ).string( );
	}
}

// This is from
// (https://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values/16606128)
// Just using this to set throughput precision
template <typename T> std::string SetPrecision( const T value, const int precision = 3 )
{
	std::ostringstream temp;
	temp.precision( precision );
	temp << std::fixed << value;
	return temp.str( );
}

int main( )
{
	std::vector<spdlog::sink_ptr> sinks;

	auto stdoutSink { std::make_shared<spdlog::sinks::stdout_color_sink_st>( ) };
	sinks.emplace_back( stdoutSink );
	auto spdlogConsoleLogger = std::make_shared<spdlog::logger>( "Console Logger", begin( sinks ), end( sinks ) );
	spdlog::register_logger( spdlogConsoleLogger );
	spdlogConsoleLogger->set_pattern( "%^|%L| %a %d%b%C %T [%n]: %v%$" );  // equivalent to Target's Default Pattern

	bool truncate = true;
	auto fileSink { std::make_shared<spdlog::sinks::basic_file_sink_st>( SpdlogPath( ), truncate ) };
	sinks.clear( );
	sinks.emplace_back( fileSink );
	auto spdlogFileLogger = std::make_shared<spdlog::logger>( "File Logger", begin( sinks ), end( sinks ) );
	spdlog::register_logger( spdlogFileLogger );
	spdlogFileLogger->set_pattern( "%^|%L| %a %d%b%C %T [%n]: %v%$" );  // equivalent to Target's Default Pattern

	auto fileSize { 512 * KB };
	auto numberOfFiles { 5 };
	auto rotateOnOpen { false };
	// above settings equivalent to default RotatingTarget Settings
	bool isRotateFilePath { true };
	auto rotatingSink { std::make_shared<spdlog::sinks::rotating_file_sink_st>( SpdlogPath( isRotateFilePath ),
																				fileSize,
																				numberOfFiles,
																				rotateOnOpen ) };
	sinks.clear( );
	sinks.emplace_back( rotatingSink );
	auto spdlogRotatingLogger { std::make_shared<spdlog::logger>( "Rotating_Logger", begin( sinks ), end( sinks ) ) };
	spdlog::register_logger( spdlogRotatingLogger );
	spdlogRotatingLogger->set_pattern( "%^|%L| %a %d%b%C %T [%n]: %v%$" );  // equivalent to Target's Default Pattern

	using namespace se_colors;
	using namespace serenity::expiremental;
	using namespace serenity::se_utils;

	targets::ColorConsole   C;
	targets::FileTarget     testFile;
	targets::RotatingTarget rotatingFile;

	// TODO: Fix this to work as expected
	// PeriodicSettings settings;
	// settings.flushEvery = std::chrono::milliseconds( 500 );
	// Flush_Policy policy( PeriodicOptions::timeBased, settings );
	// testFile.SetFlushPolicy( policy );

#ifndef INSTRUMENTATION_ENABLED
	std::cout << "####################################################################\n";
	std::cout << "# This Will Be The Default Pattern Format And Message Level Colors #\n";
	std::cout << "####################################################################\n";
	// Trace Is Default Color
	C.Trace( "Trace" );
	// Info Is Light Green
	C.Info( "Info" );
	// Debug Is Light Cyan
	C.Debug( "Debug" );
	// Warning Is Light Yellow
	C.Warn( "Warning" );
	// Error Is Dark Red
	C.Error( "Error" );
	// Fatal Is Light Yellow On Dark Red
	C.Fatal( "Fatal" );
	std::cout << "####################################################################\n\n";
	std::cout << "########################################################################\n";
	std::cout << "# Testing Some Basic Functions To Make Sure All Is Working As I Expect #\n";
	std::cout << "########################################################################\n";
	C.SetPattern( "%T [%N]: %+" );
	C.Info( "Pattern String Has Been Changed To \"%T [%N]: %+\"" );
	C.ColorizeOutput( false );
	C.Info( "Colorized Output Set To False, Therefore, This Output Should Be The Default Color" );
	C.Error( "This Line Should Also Still Lack Color" );
	C.ColorizeOutput( true );
	C.Info( "Colorized Output Re-enabled, therefore this line should be colored" );
	C.Info( "Log Level Will Be Set To \"warning\"" );
	C.SetLogLevel( LoggerLevel::warning );
	C.Info( "Log Level Has Been Set To Warning, So This Line Should Not Print" );
	C.Warn( "However, This Line Should Print" );
	C.Fatal( "And This Line Should Also Print" );
	C.SetLogLevel( LoggerLevel::trace );
	C.Info( "Log Level Set Back To \"trace\"" );
	C.ResetPatternToDefault( );
	C.Info( "Pattern String Should Have Been Changed Back To Default Pattern" );
	C.SetMsgColor( LoggerLevel::warning, se_colors::bright_colors::combos::cyan::on_grey );
	C.Trace( "Warning Level Color Has Been Changed To Cyan On Grey" );
	C.Warn( "So This Line Should Now Be Cyan On Grey" );
	C.SetOriginalColors( );
	C.Warn( "Colors Should Have Been Reset, So This Should Be Back To Bright Yellow" );

	// This Is Now Fully Working As Well
	// TODO: Add interval based rotation settings
	serenity::expiremental::RotateSettings settings;
	settings.fileSizeLimit    = 256 * KB;
	settings.maxNumberOfFiles = 5;
	settings.rotateOnFileSize = true;
	rotatingFile.SetRotateSettings( settings );

	testFile.Trace( "This Is A Trace Message To The File" );
	testFile.Info( "This Is An Info Message To The File" );
	testFile.Debug( "This Is A Debug Message To The File" );
	testFile.Warn( "This Is A Warning Message To The File" );
	testFile.Error( "This Is An Error Message To The File" );
	testFile.Fatal( "This Is A Fatal Message To The File" );
	testFile.RenameFile( "Renamed_File.txt" );
	testFile.Trace( "File Should Have Been Renamed To \"Renamed_File.txt\"" );
	testFile.Flush( );

	// Found out why this was so slow.. the std::filesystem::file_size() call is apparently extremely expensive, opted for
	// manual tracking of size
	auto start { std::chrono::steady_clock::now( ) };
	for( int i = 0; i < 4'000'000; ++i ) {
		rotatingFile.Trace( "This is a test loop for rotating the file. Iteration {}", i );
	}
	auto end { std::chrono::steady_clock::now( ) };
	auto elapsed = ( end - start );
	auto time { std::chrono::duration_cast<std::chrono::microseconds>( elapsed ) };
	auto averaged { time / 4'000'000.0f };
	std::cout << "\nTime Taken For Rotation Loop (Averaged): " << averaged << "\n";
	// Next Step Is To Benchmark And Flesh Out The FileTarget Class And Then Start Working On An HTML/XML Shredder
	// And Finally Link All The Targets Together Using A Singular Logging Class
#endif  // !INSTRUMENTATION_ENABLED

#ifdef INSTRUMENTATION_ENABLED
	// Really don't need this many instrumentators but just being lazy with reuse and adding more for clarity anyways
	Instrumentator macroTester;
	Instrumentator macroTesterFile;
	Instrumentator spdlogConsoleTester;
	Instrumentator spdlogFileTester;
	Instrumentator spdlogRotateTester;
	Instrumentator rotateTester;

	const char *test = nullptr;
	// test string
	std::string temp;
	for( int h = 0; h < 399; h++ ) {
		temp += "a";
	}  // 400 chars = 400 bytes
	test = temp.c_str( );

	unsigned long int       i { 0 };
	const unsigned long int iterations { 1'000'000 };
	macroTester.StopWatch_Reset( );
	std::cout << "Benching Color Console Target...\n\n";
	for( i; i < iterations; i++ ) {
		C.Info( "{}", test );
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
	std::cout << "\nSpdlog Color Console Sink Bench Finished.  Benching File Target...\n";

	i = 0;  // reset
	macroTesterFile.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		testFile.Info( "{}", test );
	}
	macroTesterFile.StopWatch_Stop( );
	testFile.Flush( );
	auto totalFileTime = macroTesterFile.Elapsed_In( time_mode::ms );
	std::cout << "\nFile Target Bench Finished. Benching Spdlog Basic File Sink...\n";

	i = 0;  // reset
	spdlogFileTester.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		spdlogFileLogger->info( "{}", test );
	}
	spdlogFileTester.StopWatch_Stop( );
	spdlogFileLogger->flush( );
	auto totalSpdFileTime = spdlogFileTester.Elapsed_In( time_mode::ms );
	std::cout << "\nSpdlog Basic File Sink Bench Finished. Benching Rotating Target...\n";

	i = 0;  // reset
	rotateTester.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		rotatingFile.Info( "{}", test );
	}
	rotateTester.StopWatch_Stop( );
	rotatingFile.Flush( );
	auto totalRotateTime = rotateTester.Elapsed_In( time_mode::ms );
	std::cout << "\nRotating Target Bench Finished. Benching Spdlog Rotating Sink...\n";

	i = 0;  // reset
	spdlogRotateTester.StopWatch_Reset( );
	for( i; i < iterations; i++ ) {
		spdlogRotatingLogger->info( "{}", test );
	}
	spdlogRotateTester.StopWatch_Stop( );
	spdlogRotatingLogger->flush( );
	auto totalSpdRotateTime = spdlogRotateTester.Elapsed_In( time_mode::ms );
	std::cout << "\nSpdlog Rotating File Sink Bench Finished.\n";

	auto        percentConsole = ( ( totalColorTime - totalspdColorTime ) / totalspdColorTime ) * 100;
	std::string consolePercent;
	if( percentConsole > 0 ) {
		consolePercent = "- " + std::to_string( std::abs( percentConsole ) );
	} else {
		consolePercent = "+ " + std::to_string( std::abs( percentConsole ) );
	}

	auto        percentFile = ( ( totalFileTime - totalSpdFileTime ) / totalSpdFileTime ) * 100;
	std::string filePercent;
	if( percentFile > 0 ) {
		filePercent = "-" + std::to_string( std::abs( percentFile ) );
	} else {
		filePercent = "+" + std::to_string( std::abs( percentFile ) );
	}

	auto        percentRotating = ( ( totalRotateTime - totalSpdRotateTime ) / totalSpdRotateTime ) * 100;
	std::string rotatePercent;
	if( percentRotating > 0 ) {
		rotatePercent = "-" + std::to_string( std::abs( percentRotating ) );
	} else {
		rotatePercent = "+" + std::to_string( std::abs( percentRotating ) );
	}

	auto testStrInMB { ( temp.length( ) ) / static_cast<float>( MB ) };
	auto FileThroughput { ( testStrInMB * iterations ) / macroTesterFile.Elapsed_In( time_mode::sec ) };
	auto SpdlogFileThroughput { ( testStrInMB * iterations ) / spdlogFileTester.Elapsed_In( time_mode::sec ) };
	auto ConsoleThroughput { ( testStrInMB * iterations ) / macroTester.Elapsed_In( time_mode::sec ) };
	auto SpdlogConsoleThroughput { ( testStrInMB * iterations ) / spdlogConsoleTester.Elapsed_In( time_mode::sec ) };
	auto rotatingThroughput { ( testStrInMB * iterations ) / rotateTester.Elapsed_In( time_mode::sec ) };
	auto SpdlogRotatingThrouput { ( testStrInMB * iterations ) / spdlogRotateTester.Elapsed_In( time_mode::sec ) };

	std::cout << Tag::Yellow( "\n\n***************************************************************************************\n"
							  "*************** Instrumentation Data (Averaged Over " )
			  << Tag::Yellow( std::to_string( iterations ) + " Iterations: " )
			  << Tag::Yellow( "***************\n"
							  "***************************************************************************************\n" );
	std::cout << Tag::Bright_Yellow( "Color Console Target (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
			  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
			  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
			  << Tag::Bright_Green( std::to_string( macroTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );

	std::cout << Tag::Bright_Yellow( "Spdlog Color Console Sink (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::us ) / iterations ) + " us"
																													 "\n" )
			  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms"
																													 "\n" )
			  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogConsoleTester.Elapsed_In( time_mode::sec ) / iterations ) + " s"
																													  "\n" );

	std::cout << Tag::Bright_Magenta( "Color Console Target Is " + consolePercent +
									  " Percent Of Spdlog's Color Console Sink Speed\n" );

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

	std::cout << Tag::Bright_Magenta( "File Target Is " + filePercent + " Percent Of Spdlog's File Sink Speed\n" );

	std::cout << Tag::Bright_Yellow( "Roating Target (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( rotateTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
			  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( rotateTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
			  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
			  << Tag::Bright_Green( std::to_string( rotateTester.Elapsed_In( time_mode::sec ) / iterations ) + " s\n" );

	std::cout << Tag::Bright_Yellow( "Spdlog Rotating File Sink (ST)\n" ) << Tag::Bright_Cyan( "\t- In Microseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogRotateTester.Elapsed_In( time_mode::us ) / iterations ) + " us\n" )
			  << Tag::Bright_Cyan( "\t- In Milliseconds:\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogRotateTester.Elapsed_In( time_mode::ms ) / iterations ) + " ms\n" )
			  << Tag::Bright_Cyan( "\t- In Seconds:\t\t\t" )
			  << Tag::Bright_Green( std::to_string( spdlogRotateTester.Elapsed_In( time_mode::sec ) / iterations ) + " s"
																													 "\n" );

	std::cout << Tag::Bright_Magenta( "Rotating Target Is " + rotatePercent + " Percent Of Spdlog's File Sink Speed\n" );

	std::cout << "\n";
	std::cout << Tag::Bright_Yellow( "Program Throughput :\n" );
	std::cout << Tag::Bright_Cyan( "Color Console Target Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( ConsoleThroughput ) + " MB/s\n" );
	std::cout << Tag::Bright_Cyan( "spdlog Color Sink Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( SpdlogConsoleThroughput ) + " MB/s\n" );
	std::cout << Tag::Bright_Cyan( "File Target Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( FileThroughput ) + " MB/s\n" );
	std::cout << Tag::Bright_Cyan( "spdlog File Sink Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( SpdlogFileThroughput ) + " MB/s\n" );
	std::cout << Tag::Bright_Cyan( "Rotating Target Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( rotatingThroughput ) + " MB/s\n" );
	std::cout << Tag::Bright_Cyan( "spdlog Rotating File Sink Throughput:" ) << "\n  "
			  << Tag::Bright_Green( SetPrecision( SpdlogRotatingThrouput ) + " MB/s\n" );
	std::cout << "\n";

	std::cout << Tag::Bright_Yellow( "Size of Base Target Class:\t\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::TargetBase ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of ColorConsole Target Class:\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::ColorConsole ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of File Target Class:\t\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::FileTarget ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Rotating Target Class:\t\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( targets::RotatingTarget ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Info Class:\t\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Info ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Formatter Class:\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Formatter ) ) + "\tbytes ]\n" );

	std::cout << Tag::Bright_Yellow( "Size of Message_Time Class:\t\t" )
			  << Tag::Bright_Green( "[ " + std::to_string( sizeof( msg_details::Message_Time ) ) + "\tbytes ]\n" );

	std::cout << Tag::Yellow( "***************************************************************************************"
							  "\n" );
	std::cout << Tag::Yellow( "***************************************************************************************"
							  "\n" );
	std::cout << Tag::Yellow( "***************************************************************************************"
							  "\n\n" );

#endif  // INSTRUMENTATION_ENABLED
}
