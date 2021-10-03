#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>


void PrintReminder( );

int main( )
{
	PrintReminder( );
	using namespace serenity;


	file_helper::path const originalPath = file_helper::current_path( );

	auto logDirPath = originalPath;
	logDirPath /= "Logs";
	file_helper::directory_entry logDir { logDirPath };


	sinks::logger_info initInfo = { };
	initInfo.loggerName         = "Filesystem Logger";
	initInfo.logName            = "File_System.txt";
	initInfo.logDir             = logDir;

	std::vector<sinks::SinkType> dist_sink_handles;
	dist_sink_handles.emplace_back( sinks::SinkType::basic_file_mt );
	dist_sink_handles.emplace_back( sinks::SinkType::stdout_color_mt );
	sinks::dist_sink_info dist_sink( dist_sink_handles );

	sinks::base_sink_info sink_info = { };
	sink_info.base_info             = initInfo;
	sink_info.sinks.emplace_back( sinks::SinkType::dist_sink_mt );
	sink_info.dist_sink = &dist_sink;


	InternalLibLogger::EnableInternalLogging( );
	// Works As Intended [X]
	se_globals::SetGlobalLevel( LoggerLevel::trace );
	Logger logTwo( sink_info );

	logTwo.se_trace( "Message Before Changing Stuff In Internal Logger" );

	sinks::internal_logger_info changeOptions = { };
	changeOptions.sink_info.sinks.emplace_back( sinks::SinkType::basic_file_mt );
	changeOptions.sink_info.truncateFile = true;
	logTwo.ChangeInternalLoggerOptions( changeOptions );

	// se_globals::SetGlobalLevel( LoggerLevel::warning );  // Subsequent Calls Will Set Logger && Global Levels

	logTwo.SetLogLevel( LoggerLevel::trace );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "/RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::info );
	logTwo.RenameLog( spdLogDest, false );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::trace );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	logTwo.se_info( "Testing Swapping To New Log Instead Of Renaming...\n" );
	logTwo.WriteToNewLog( "\\WriteToNewLog/CreateNewLog.txt" );
	logTwo.se_debug( "File Path: [{}]", logTwo.FileHelperHandle( )->LogFilePath( ) );
	logTwo.se_debug( "Relative File Path: [{}]", logTwo.FileHelperHandle( )->RelativePathToLog( ) );
	logTwo.se_debug( "Log Directory: [{}]", logTwo.FileHelperHandle( )->LogDir( ).path( ) );
	logTwo.se_debug( "File Name: [{}]\n", logTwo.FileHelperHandle( )->LogName( ) );


	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}


void PrintReminder( )
{
	auto day   = "02";
	auto month = "OCT";
	auto year  = "21";
	printf( "\n\t\t\t#############################\n" );
	printf( "\t\t\t#  Library Version: %s   #\n", serenity::se_utils::GetSerenityVerStr( ).c_str( ) );
	printf( "\t\t\t#  Date: %s %s %s          #\n", day, month, year );
	printf( "\t\t\t#############################\n\n" );
}