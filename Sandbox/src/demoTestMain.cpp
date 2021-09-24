#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>


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


	// Works As Intended [X]
	SetGlobalLevel( LoggerLevel::trace );
	Logger logTwo( initInfo );
	SetGlobalLevel( LoggerLevel::warning );                             // Subsequent Calls Will Set Logger && Global Levels
	logTwo.SetLogLevel( LoggerLevel::trace, LoggerInterface::client );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );

	logTwo.RenameLog( spdLogDest, false );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}
