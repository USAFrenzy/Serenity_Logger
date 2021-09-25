#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>


void Test( );  // quick and dirty prototype for simple test

int main( )
{
	using namespace serenity;
	printf( "Library Version: %s\n", GetSerenityVerStr( ).c_str( ) );
	file_helper::path const originalPath = file_helper::current_path( );

	auto logDirPath = originalPath;
	logDirPath /= "Logs";
	file_helper::directory_entry logDir { logDirPath };
	logger_info                  initInfo = { };
	initInfo.loggerName                   = "Filesystem Logger";
	initInfo.logName                      = "File_System.txt";
	initInfo.logDir                       = logDir;
	initInfo.level                        = LoggerLevel::trace;
	initInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
	initInfo.sink_info.sinks.emplace_back( SinkType::basic_file_mt );

	InternalLibLogger::EnableInternalLogging( );
	// Works As Intended [X]
	SetGlobalLevel( LoggerLevel::trace );
	Logger logTwo( initInfo );

	// This Section Also Seems To Work As Intended, However, There Is The Same Double Printing Issue From A While Back When Working
	// On RenameLog() - Probably A Handle That Needs To Be Reset Like In RenameLog()
	se_internal::internal_logger_info changeOptions = { };
	changeOptions.sink_info.sinks.emplace_back( SinkType::basic_file_mt );
	logTwo.ChangeInternalLoggerOptions( changeOptions );

	Test( );  // Disable/Enable Internal Logging

	SetGlobalLevel( LoggerLevel::warning );    // Subsequent Calls Will Set Logger && Global Levels
	logTwo.SetLogLevel( LoggerLevel::trace );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );

	logTwo.RenameLog( spdLogDest, false );
	SetGlobalLevel( LoggerLevel::trace );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}

void Test( )
{
	serenity::InternalLibLogger::EnableInternalLogging( );
	serenity::se_internal::internal_logger_info tmpInfo = { };
	tmpInfo.loggerName                                  = "Testing Logger";
	printf( "\n" );
	serenity::InternalLibLogger tmp( tmpInfo );
	tmp.trace( "Testing That The Disable Setting Works..." );
	serenity::InternalLibLogger::DisableInternalLogging( );
	tmp.trace( "Internal Logging Disabled..." );
	serenity::InternalLibLogger::EnableInternalLogging( );
	tmp.trace( "Internal Logging Re-enabled..." );
	spdlog::drop( "Testing Logger" );
	printf( "\n" );
}