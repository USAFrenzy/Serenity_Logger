
#include <serenity/Logger.hpp>
#include <serenity/Helpers/LogFileHelper.hpp>
#include <serenity/Utilities/Utilities.hpp>

#define LOGGER_MESSAGES_SB   0
#define TEST_ASSERT          0
#define LOGGER_FILESYSTEM_SB 1
#define TEST_FILE_RETRIEVAL  0
#define TEST_FILE_SEARCH     0
// This One Is Just For Me
#define WANT_TO_SEE_MSGS 0
#define TEST_NEW_RENAME  1

#include <fstream>  // tmp

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
	SE_DEBUG( "Current Dir: {}", logTwo.GetFileHelperHandle( )->GetCurrentDir( ) );

	const auto re_logDir = initInfo.logDir;
	SE_DEBUG( "initInfo.logDir: {}", initInfo.logDir.path( ).string( ) );
	SE_DEBUG( "re_logDir: {}", re_logDir.path( ).string( ) );
	SE_DEBUG( "GetLogDirPath(): {}", logTwo.GetFileHelperHandle( )->GetLogDirPath( ) );

	logTwo.GetFileHelperHandle( )->SetLogDirPath( re_logDir );
	// poorly named but meh, testing shit
	auto renamePath = logTwo.GetFileHelperHandle( )->GetLogDirPath( ).string( ) + "\\Rename_Test";
	file_utils::CreateDir( renamePath );

	const file_helper::path aPath = renamePath + "\\a.txt";
	SE_DEBUG( "aPath {}", aPath );
	const file_helper::path bPath = renamePath + "\\b.txt";
	SE_DEBUG( "bPath {}", bPath );
	const file_helper::path fPath = renamePath + "\\LastRename.txt";
	SE_DEBUG( "fPath {}", fPath );

	SE_DEBUG( "renamePath After string additions for other file paths: {}", renamePath );

	// logTwo.ChangeDir( rnPath);
	file_utils::ChangeDir( renamePath );
	SE_DEBUG( "Current Dir: {}", logTwo.GetFileHelperHandle( )->GetCurrentDir( ) );
	const auto cwd = logTwo.GetFileHelperHandle( )->GetCurrentDir( );

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
	// Now That Things Above Have Been Seemingly Fixed...
	// Time To Work On The Logger Portion

	// spdloglog Rename
	SE_DEBUG( "LogName: {}", logTwo.GetFileHelperHandle( )->GetLogFilePath( ) );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "\\RenamedSpdlogLog.txt" );
	SE_DEBUG( "spdDir: {}", spdLogDest );
	logTwo.RenameLog( spdLogDest );
	// update path and re-cash
	SE_DEBUG( "BACK IN MAIN!!!!" );
	SE_INTERNAL_INFO( "JUST PROVING THAT AFTER ALL IS SAID AND DONE, CONSOLE \"INTERNAL\" LOGGER IS WORKING AS WELL" );
#endif
}
/* clang-format off
######################################################################################################################################################
#                                                               General Notees
######################################################################################################################################################
- Currently, even if changing directories (or at least setting what should be a change in directories), no directories are actually created or checked
  - Possibly need to actually use filesystem::create_directories file_utilsnction for this but checks for if that directory already exists would then need to
    occur. I believe it's probably the same thing for files? I may need to explicitly create a file with a filesystem file_utilsnction using a passed in name.
	Although, checks for if that file already exists and how to write to it would then need to be in place.
- Also, Definitely NEEEEEEED more exception handling.. just the one in ChangerDir() caught so many once I took away the error code and logged what it 
  threw. I had missed those exceptions using the non-throwing version of std::filesystem::path(&path, error_code) before that.
- EDIT: Looking at cppreference, the above DOES seem the way to go:
- For creating a directory, looks like all that is needed is to ChangeDir() to where ever creation is wanted and then add the bit of code:
	- std::filesystem::create_directory("dirToCreate"); for one directory and std::filesystem::create_directories("dir1/dir2/dir3") for mulitple
	- Also looks like I can set the permissions on these directories using std::filesystem::permissions::enum and std::filesystem::perm_options::enum
	- Added bonus, looks like a recursive deletion can be used with std::filesystem::remove_all("rootDirToRemove") or just the singular form of
	  std::filesystem::remove("dirOrFile")
	- Looks like a symbolic link for shortcuts can be created as well, the reference states:
	  create_directory_symlink(const std::filesystem::path& target, const std::filesystem::path& link) with the optional 3rd param (error code param)
	  as the preferred way to do this.
	- For Debugging the symlinks or just being able to refer to them, the status can be read with:
	  std::filesystem::path read_symlink(const std::filesystem::path& p);
- For creating a file with the named parameter, it seems I have to resort to:  std::ofstream("dirName/fileName"); In Order To Create A Regular File
  - In Particular, I had a misunderstanding of where I was going wrong.. filesystem ONLY deals with *cough* the filesystem and its attributes.
    - ofstream, ifstream, iofstream objects are still needed to create files where directory entries can then refer to them, their paths, and their
	  attributes/status
######################################################################################################################################################
clang-format on */

/*
	ideas:






*/