
#include "serenity/Logger.hpp"
#include "serenity/LogFileHelper.hpp"

#define LOGGER_MESSAGES_SB   0
#define TEST_ASSERT          0
#define LOGGER_FILESYSTEM_SB 1
#define FILE_RENAME_TEST     1
#define TEST_FILE_RETRIEVAL  1
#define TEST_FILE_SEARCH     1
// This One Is Just For Me
#define WANT_TO_SEE_MSGS 0


int main( )
{
#if LOGGER_MESSAGES_SB

	serenity::file_helper::path logName1    = "MacroLog.txt";
	auto                        logDirPath1 = serenity::file_helper::current_path( ) /= "Logs";
	serenity::LogFileHelper     logInfo1( logDirPath1, logName1 );
	Logger                      logOne( "Macro Test Logger", &logInfo1, LoggerLevel::trace );

	SE_INTERNAL_INFO( "Hello From The True Library!\n- Serenity Library Info: \n\t- Version: {}", serenity::GetSerenityVerStr( ) );
	SE_INTERNAL_TRACE( "This Is A General Trace Message - Time To Iterate Through The Log Levels!" );
	std::string A = "Hello A";
	SE_INTERNAL_DEBUG( "Oh Hey, A Debug Message! Variable Substitution for 'A' should be 'Hello A': {}", A );
	SE_INTERNAL_WARN( "What Do Ya Know? It's A Warning Message! Best Take Care With These!" );
	SE_INTERNAL_ERROR( "Say Hello To The Darkness - This Is An Error Message. You Done Fucked Up" );
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

	Logger logTwo( "Filesystem Logger", "File_System.txt", LoggerLevel::trace );

	SE_INFO( "Now Just Testing That The Logger Initialized Name Got Passed Through To The Logs" );
	SE_TRACE( "Working With File Systems Now\n\n" );

	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	SE_TRACE( "Now Testing Swapping Log Names:" );

	logTwo.RenameLogFile( "New_Log_Test.txt" );
	SE_INFO( "New File Name: {}\n", logTwo.GetFileName( ) );

	auto originalPath = "C:\\Users\\mccul\\Desktop\\Logging Project\\build\\Sandbox";

	SE_INFO( "Previous Working Dir: {}", logTwo.GetCurrentDir( ) )
	serenity::file_helper::path pathToChangeToOne = "C:\\Users\\mccul\\Desktop\\Logging Project\\build";
	serenity::file_helper::path pathToChangeToTwo = "C:\\Users\\mccul\\Desktop";

	logTwo.ChangeDir( pathToChangeToOne );
	SE_INFO( "New Working Dir: {}\n", logTwo.GetCurrentDir( ) );

	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	logTwo.ChangeDir( pathToChangeToTwo );
	SE_INFO( "New Working Dir: {}\n", logTwo.GetCurrentDir( ) );
	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	logTwo.ChangeDir( originalPath );
	SE_INFO( "Just Visual Representation Of Path Components:{}\n", logTwo.PathComponents_Str( pathToChangeToOne ) );

	#if FILE_RENAME_TEST
	SE_TRACE( "The Following Are Some Test Cases Of FileName Validations In RenameLogFile():\n" )
	SE_INFO( "\nORIGINAL FILE NAME: {}\nFile Path: {}", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( "lol.jpg.txt" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( "Samlple File.txt" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( "Samlple_File.txt" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( "Logger.conf" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( "Logger.ini" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( ".config" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	logTwo.RenameLogFile( ".hiddenFile.jpg" );
	SE_INFO( "\nFile Name:{}\nFile Path: {}\n", logTwo.GetFileName( ), logTwo.GetFilePath( ) );

	// This One Currently Does NOT Write To A Log On That Path.. (Need file stream objects)
	/*logTwo.RenameLogFile( "Test_Log.txt" );
	auto newTestPath = originalPath / logTwo.GetFileName( );
	SE_INFO( "Should Be Writing To A Log Under: {}\n", logTwo.GetLogDirPath( ) );*/


	serenity::file_helper::path pathToRootDir = "C:\\Users\\mccul\\Desktop\\Logging Project";
	logTwo.ChangeDir( pathToRootDir );
		#if TEST_FILE_RETRIEVAL
	SE_TRACE( "Quick Test Of The Whole File Retrieval Deal With RetrieveDirEntries():" );
	SE_TRACE( "Listing Files Under: {}\n", pathToRootDir );
	auto dirEntries = serenity::file_utils::RetrieveDirEntries( pathToRootDir, true );
			#if WANT_TO_SEE_MSGS
	for( const auto &file : dirEntries ) {
		SE_INFO( "File Retrieved:\tPath: {}", file.path( ).relative_path( ) );
	}
			#endif
			#if TEST_FILE_SEARCH
	SE_TRACE( "Quick Test Of Searching The Retrieved Files For A Match:" );
	auto dirFiles  = serenity::file_utils::SearchDirEntries( dirEntries, "Manually_placed_file.txt" );
	auto fileFound = std::get<0>( dirFiles );
	auto files     = std::get<1>( dirFiles );  // tuples are weird -> Might try to find a way to abstract this if possible
	if( fileFound ) {
		for( const auto &file : files ) {
			SE_INFO( "Search Found: {}\nPath: {}", file.path( ).filename( ), file );
		}
	}
			#endif

		#endif


	#endif

#endif
}
/* clang-format off
######################################################################################################################################################
#                                                               General !es
######################################################################################################################################################
- Currently, even if changing directories (or at least setting what should be a change in directories), no directories are actually created or checked
  - Possibly need to actually use filesystem::create_directories function for this but checks for if that directory already exists would then need to
    occur. I believe it's probably the same thing for files? I may need to explicitly create a file with a filesystem function using a passed in name.
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