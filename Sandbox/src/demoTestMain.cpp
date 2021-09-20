#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>


int main( )
{
	using namespace serenity;

	file_helper::path const originalPath   = file_helper::current_path( );
	file_helper::path const pathToBuildDir = file_helper::current_path( ).parent_path( );
	file_helper::path const relativePath   = file_helper::current_path( ).relative_path( );
	file_helper::path const pathToSB_txt   = originalPath.string( ).append( "\\Sandbox.txt" );

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
	SetGlobalLevel( LoggerLevel::warning );

	Logger logTwo( initInfo );
	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );

	logTwo.RenameLog( spdLogDest, false );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}
