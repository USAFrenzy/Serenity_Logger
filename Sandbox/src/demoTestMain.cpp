
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>
#include <fstream>

#define LOGGER_MESSAGES_SB   0
#define TEST_ASSERT          0
#define LOGGER_FILESYSTEM_SB 1
#define TEST_FILE_RETRIEVAL  0
#define TEST_FILE_SEARCH     0
// This One Is Just For Me
#define WANT_TO_SEE_MSGS 0
#define TEST_NEW_RENAME  1


int main( )
{
#if LOGGER_MESSAGES_SB
	// Not Testing ATM But Yeah, Logger Cnstr Changed - this doesnt reflect that change
	Logger logOne( "Macro Test Logger", "Macro_Test.txt", LoggerLevel::trace );

	SE_INTERNAL_INFO( "Hello From The True Library!\n- Serenity Library Info: \n\t- Version: {}", serenity::GetSerenityVerStr( ) );
	SE_INTERNAL_TRACE( "This Is A General Trace Message - Time To Iterate Through The Log Levels!" );
	std::string A = "Hello A";
	SE_INTERNAL_DEBUG( "Oh Hey, A Debug Message! Variable Substitution for 'A' should be 'Hello A': {}", A );
	SE_INTERNAL_WARN( "What Do Ya Know? It's A Warning Message! Best Take Care With These!" );
	SE_INTERNAL_ERROR( "Say Hello To The Darkness - This Is An Error Message. You Done file_utilscked Up" );
	SE_INTERNAL_FATAL(
	  "Last But ! Least, This Is A Fatal Message - Your Application Is Crashing My "
	  "Friend..." );
	// clang-format off
	#if TEST_ASSERT
		SE_INTERNAL_INFO("To Help With Those Pesky Fatal Messages, We Have The Assert Messages!\n- For Instance:\n");
		int a = 5;
		int b = 76;
		SE_INTERNAL_ASSERT((a > b), "ACTUAL VALUES:\na: {}\nb:{}", a, b);
	#endif
	// clang-format on
#endif

#if LOGGER_FILESYSTEM_SB

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
	initInfo.sink_info.sinks.emplace_back( SinkType::basic_file_mt );
	initInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );

	Logger logTwo( initInfo );

	#if TEST_FILE_RETRIEVAL
	SE_TRACE( "Quick Test Of The Whole File Retrieval Deal With RetrieveDirEntries():" );
	SE_TRACE( "Listing Files Under: {}\n", pathToDesktop );
	auto dirEntries = file_utils::RetrieveDirEntries( pathToDesktop, true );
		#if WANT_TO_SEE_MSGS
	for( const auto &file : dirEntries.retrievedItems ) {
		SE_INFO( "File Retrieved At: {}", file.path( ).relative_path( ) );
	}
		#endif
		#if TEST_FILE_SEARCH
	SE_TRACE( "Number Of Files Retrieved: {}", dirEntries.fileCount );
	SE_TRACE( "Time Taken To Complete: {}ms", dirEntries.elapsedTime );
	SE_TRACE( "Quick Test Of Searching The Retrieved Files For A Match:" );
	auto result = file_utils::SearchDirEntries( dirEntries.retrievedItems, "Manually_placed_file.txt" );
	if( result.fileFound ) {
		for( const auto &file : result.matchedResults ) {
			SE_INFO( "Search Found: {}\nPath: {}", file.path( ).filename( ), file );
		}
	}
	SE_TRACE( "Time Taken To Complete: {}ms", result.elapsedTime );
		#endif

	#endif


#endif
#if TEST_NEW_RENAME
	// clang-format off
	// PROBABLY JUST NEED TO "CONST" ALL THE THINGS!!!!
	// Man, Just Realizing by not const'ing, paths and shit are breaking..
	/*
		With The Above Note Taken Into Consideration, Would Really Like A Way To Fully 
		Ensure That Anyone Who Uses This In The Future (Myself Included) Doesn't End Up
		Breaking Their Path Consistencies Like I Was Before By Directly Appending Paths..
		i.e. How Do I Prevent The Bit Below From Affecting The Log File Path?
		std::filesystem::path somePath = GetLogFilePath();
		auto someOtherPath = somePath /= "someDir";
		std::filesystem::path anotherPath = GetLogFilePath();
		-> anotherPath Now Points To someOtherPath instead of GetLogFilePath()
	*/
	// clang-format on

	SE_TRACE( "Testing The file_utils::RenameFile()\n" );

	std::string const sbPath = "C:\\Users\\mccul\\Desktop\\Logging Project\\build\\Sandbox";
	file_utils::ChangeDir( sbPath );
	SE_DEBUG( "Current Dir: {}", logTwo.FileHelperHandle( )->CurrentDir( ) );

	const auto re_logDir = initInfo.logDir;
	SE_DEBUG( "initInfo.logDir: {}", initInfo.logDir.path( ).string( ) );
	SE_DEBUG( "re_logDir: {}", re_logDir.path( ).string( ) );
	SE_DEBUG( "GetLogDirPath(): {}", logTwo.FileHelperHandle( )->LogDirPath( ) );

	logTwo.FileHelperHandle( )->SetLogDirPath( re_logDir );
	auto renamePath = logTwo.FileHelperHandle( )->LogDirPath( ).string( ) + "\\Rename_Test";
	file_utils::CreateDir( renamePath );

	const file_helper::path aPath = renamePath + "\\a.txt";
	SE_DEBUG( "aPath {}\n", aPath );
	const file_helper::path bPath = renamePath + "\\b.txt";
	SE_DEBUG( "bPath {}\n", bPath );
	const file_helper::path fPath = renamePath + "\\LastRename.txt";
	SE_DEBUG( "fPath {}\n", fPath );

	SE_DEBUG( "renamePath After string additions for other file paths: {}", renamePath );

	file_utils::ChangeDir( renamePath );
	SE_DEBUG( "Current Dir: {}", logTwo.FileHelperHandle( )->CurrentDir( ) );
	const auto cwd = logTwo.FileHelperHandle( )->CurrentDir( );

	// Effectively Delete If Already Exists Just To Prove The Rename Process
	auto finalFile = cwd.string( ) + "\\LastRename.txt";
	file_utils::RemoveEntry( finalFile );

	std::ofstream file;
	// Note: opening with append won't append since the file is deleted for this test if it already existed
	file.open( aPath.filename( ), std::ios_base::app );

	while( file.is_open( ) ) {
		SE_DEBUG( "File is opened" );
		SE_DEBUG( "Attempting Write To File" );
		file << "This Is A Test String To Prove Size And Content Of File Remains Constant\n";
		SE_DEBUG( "Finished Write To File" );
		file.close( );
		SE_DEBUG( "File is closed" );
	}


	try {
		file_utils::RenameFile( aPath.filename( ), bPath.filename( ) );
		file_utils::RenameFile( bPath.filename( ), fPath.filename( ) );

		file_helper::path searchPath = logDirPath;
		auto              rnFiles    = file_utils::RetrieveDirEntries( searchPath, false );
		auto              found      = file_utils::SearchDirEntries( rnFiles.retrievedItems, "LastRename.txt" );
		if( found.fileFound ) {
			for( auto &file : found.matchedResults ) {
				SE_TRACE( "File: {} Was Successuly Found", file.path( ).filename( ) );
			}
		}
	}
	catch( const std::filesystem::filesystem_error &fs_err ) {
		SE_FATAL( "FS EXCEPTION CAUGHT:\n{}", fs_err.what( ) );
	}

	SE_DEBUG( "LogName: {}\n", logTwo.FileHelperHandle( )->LogFilePath( ) );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	SE_DEBUG( "spdDir: {}\n", spdLogDest );
	logTwo.RenameLog( spdLogDest );
	SE_DEBUG( "BACK IN MAIN!!!!\n" );
	SE_INTERNAL_INFO( "JUST PROVING THAT AFTER ALL IS SAID AND DONE, CONSOLE \"INTERNAL\" LOGGER IS WORKING AS WELL" );
#endif
}
