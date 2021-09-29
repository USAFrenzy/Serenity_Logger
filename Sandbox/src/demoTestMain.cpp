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


	logger_info initInfo     = { };
	initInfo.loggerName      = "Filesystem Logger";
	initInfo.logName         = "File_System.txt";
	initInfo.logDir          = logDir;
	base_sink_info sink_info = { };
	sink_info.base_info      = &initInfo;
	sink_info.sinks.emplace_back( SinkType::basic_file_mt );
	sink_info.sinks.emplace_back( SinkType::stdout_color_mt );


	InternalLibLogger::EnableInternalLogging( );
	// Works As Intended [X]
	SetGlobalLevel( LoggerLevel::trace );
	Logger logTwo( sink_info );

	logTwo.se_trace( "Message Before Changing Shit In Internal Logger" );

	// Recent Changes Mean That The Internal Logger From This Section No Longer Has A File Handle When Recreated.
	// When Recreating The Logger, base_info Seemed To Remain null (RED FLAG) -> Look Into This
	se_internal::internal_logger_info changeOptions = logTwo.InternalLogger( )->internal_info( );
	changeOptions.sink_info.sinks.emplace_back( SinkType::basic_file_mt );
	changeOptions.sink_info.truncateFile = true;
	logTwo.ChangeInternalLoggerOptions( *&changeOptions );

	SetGlobalLevel( LoggerLevel::warning );  // Subsequent Calls Will Set Logger && Global Levels

	logTwo.SetLogLevel( LoggerLevel::trace );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "/RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::info );
	logTwo.RenameLog( spdLogDest, false );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::trace );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	logTwo.se_info( "Testing Swapping To New Log Instead Of Renaming..." );
	logTwo.WriteToNewLog( "/WriteToNewLog/CreateNewLog.txt" );
	logTwo.se_debug( "File Path: [{}]", logTwo.FileHelperHandle( )->LogFilePath( ) );
	logTwo.se_debug( "Relative File Path: [{}]", logTwo.FileHelperHandle( )->RelativePathToLog( ) );
	logTwo.se_debug( "Log Directory: [{}]", logTwo.FileHelperHandle( )->LogDir( ).path( ) );
	logTwo.se_debug( "File Name: [{}]", logTwo.FileHelperHandle( )->LogName( ) );

	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}


void PrintReminder( )
{
	auto day   = "28";
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