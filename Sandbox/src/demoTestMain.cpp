#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>


void Test( std::string originalLoggerName );  // quick and dirty prototype for simple test
void PrintReminder( );

int main( )
{
	PrintReminder( );
	using namespace serenity;


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

	// This Section Also Seems To Work As Intended
	se_internal::internal_logger_info changeOptions = logTwo.InternalLogger( )->internal_info( );
	changeOptions.sink_info.sinks.emplace_back( SinkType::basic_file_mt );
	changeOptions.sink_info.truncateFile = true;
	logTwo.ChangeInternalLoggerOptions( changeOptions );

	Test( logTwo.InternalLogger( )->name( ) );  // Disable/Enable Internal Logging

	InternalLibLogger::EnableInternalLogging( );
	// SetGlobalLevel( LoggerLevel::warning );    // Subsequent Calls Will Set Logger && Global Levels
	logTwo.SetLogLevel( LoggerLevel::trace );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );

	logTwo.RenameLog( spdLogDest, false );
	// SetGlobalLevel( LoggerLevel::trace );
	logTwo.se_debug( "BACK IN MAIN!\n" );


	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}


void Test( std::string originalLoggerName )
{
	serenity::InternalLibLogger::EnableInternalLogging( );
	serenity::se_internal::internal_logger_info tmpInfo = { };

	tmpInfo.loggerName = "Testing Logger";
	printf( "\n" );
	serenity::InternalLibLogger tmp( tmpInfo );
	tmp.trace( "Testing That The Disable Setting Works..." );
	serenity::InternalLibLogger::DisableInternalLogging( );
	tmp.trace( "Internal Logging Disabled..." );
	serenity::InternalLibLogger::EnableInternalLogging( );
	tmp.trace( "Internal Logging Re-enabled..." );
	spdlog::drop( "Testing Logger" );
	serenity::InternalLibLogger::DisableInternalLogging( );
	// Reset For The Shared Pointer From Calling Scope
	tmpInfo                        = serenity::Logger::InternalLogger( )->internal_info( );
	tmpInfo.sink_info.truncateFile = false;
	tmp.SetLogLevel( serenity::LoggerLevel::off );
	tmp.CustomizeInternalLogger( tmpInfo );
	tmp.SetLogLevel( serenity::GetGlobalLevel( ) );
	printf( "\n" );
}

void PrintReminder( )
{
	auto day   = "26";
	auto month = "SEP";
	auto year  = "21";

	printf( "\n####################################################################################\n" );
	printf( "# Reminder To Myself To Start Incrementing Version Number Or Find Automated Method #\n" );
	printf( "####################################################################################" );
	printf( "\n\t\t\t#############################\n" );
	printf( "\t\t\t#  Library Version: %s   #\n", serenity::GetSerenityVerStr( ).c_str( ) );
	printf( "\t\t\t#  Date: %s%s%s            #\n", day, month, year );
	printf( "\t\t\t#############################\n\n" );
}