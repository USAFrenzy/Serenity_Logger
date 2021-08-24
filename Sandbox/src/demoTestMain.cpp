
#include <serenity/Logger.hpp>
#include <serenity/Helpers/LogFileHelper.hpp>
#include <serenity/Utilities/Utilities.hpp>

#define LOGGER_MESSAGES_SB   0
#define TEST_ASSERT          0
#define LOGGER_FILESYSTEM_SB 1
#define FILE_RENAME_TEST     0
#define TEST_FILE_RETRIEVAL  0
#define TEST_FILE_SEARCH     0
// This One Is Just For Me
#define WANT_TO_SEE_MSGS 0
#define TEST_NEW_RENAME  1

#include <fstream>  // tmp

int main( )
{
#if LOGGER_MESSAGES_SB

	Logger logOne( "Macro Test Logger", "Macro_Test.txt", LoggerLevel::trace );

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


	serenity::file_helper::path originalPath   = "C:\\Users\\mccul\\Desktop\\Logging Project\\build\\Sandbox";
	serenity::file_helper::path pathToSB_txt   = "C:\\Users\\mccul\\Desktop\\Logging Project\\build\\Sandbox\\Sandbox.txt";
	serenity::file_helper::path pathToBuildDir = "C:\\Users\\mccul\\Desktop\\Logging Project\\build";
	serenity::file_helper::path pathToRootDir  = "C:\\Users\\mccul\\Desktop\\Logging Project";
	serenity::file_helper::path pathToDesktop  = "C:\\Users\\mccul\\Desktop";

	Logger logTwo( "Filesystem Logger", "File_System.txt", LoggerLevel::trace );

	SE_INFO( "Now Just Testing That The Logger Initialized Name Got Passed Through To The Logs" );
	SE_TRACE( "Working With File Systems Now\n\n" );

	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	SE_TRACE( "Now Testing Swapping Log Names:" );
	auto originalFilePath = logTwo.GetFilePath( );
	serenity::file_utils::RenameFile( originalFilePath, "New_Log_Test.txt" );
	SE_INFO( "New File Name: {}\n", logTwo.GetFileName( ) );


	SE_INFO( "Previous Working Dir: {}", logTwo.GetCurrentDir( ) )

	logTwo.ChangeDir( pathToBuildDir );
	SE_INFO( "New Working Dir: {}\n", logTwo.GetCurrentDir( ) );

	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	logTwo.ChangeDir( pathToDesktop );
	SE_INFO( "New Working Dir: {}\n", logTwo.GetCurrentDir( ) );
	SE_INFO( "\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n", logTwo.GetCurrentDir( ), logTwo.GetLogDirPath( ),
		 logTwo.GetFilePath( ), logTwo.GetFileName( ) );

	logTwo.ChangeDir( originalPath );
	SE_INFO( "Just Visual Representation Of Path Components:{}\n", logTwo.PathComponents_Str( pathToBuildDir ) );

	logTwo.ChangeDir( pathToDesktop );
	#if TEST_FILE_RETRIEVAL
	SE_TRACE( "Quick Test Of The Whole File Retrieval Deal With RetrieveDirEntries():" );
	SE_TRACE( "Listing Files Under: {}\n", pathToDesktop );
	auto dirEntries = serenity::file_utils::RetrieveDirEntries( pathToDesktop, true );
		#if WANT_TO_SEE_MSGS
	for( const auto &file : dirEntries.retrievedItems ) {
		SE_INFO( "File Retrieved At: {}", file.path( ).relative_path( ) );
	}
		#endif
		#if TEST_FILE_SEARCH
	SE_TRACE( "Number Of Files Retrieved: {}", dirEntries.fileCount );
	SE_TRACE( "Time Taken To Complete: {}ms", dirEntries.elapsedTime );
	SE_TRACE( "Quick Test Of Searching The Retrieved Files For A Match:" );
	auto result = serenity::file_utils::SearchDirEntries( dirEntries.retrievedItems, "Manually_placed_file.txt" );
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
	/*
		I'm assuming what I'm running into is a race condition? Keep catching the exception:
		- "The process cannot access the file because it is being used by another process."
		When trying multiple renames in a row with std::filesystem::path objs but not with string paths
	*/
	namespace su = serenity::se_utils;
	namespace fu = serenity::file_utils;

	SE_TRACE( "Testing The file_utils::RenameFile()\n" );
	std::string sbPath = "C:\\Users\\mccul\\Desktop\\Logging Project\\build\\Sandbox";
	logTwo.ChangeDir( sbPath );
	SE_DEBUG( "Current Dir: {}", logTwo.GetCurrentDir( ) );
	auto cwd = logTwo.GetCurrentDir( );

	serenity::file_helper::path aPath = cwd /= "a.txt";
	SE_DEBUG( "aPath: {}", aPath );
	std::ofstream file;
	file.open( aPath.filename( ), std::ios_base::app );
	while( file.is_open( ) ) {
		SE_DEBUG( "file is opened" );
		file << "This Is A Test String To Prove Size And Content Of File Remains Constant";
		file.close( );
	}
	try {
		serenity::file_utils::RenameFile( cwd /= "a.txt", cwd /= "b.txt" );
		serenity::file_utils::RenameFile( cwd /= "b.txt", cwd /= "c.txt" );
		serenity::file_utils::RenameFile( cwd /= "c.txt", cwd /= "d.txt" );
		serenity::file_utils::RenameFile( cwd /= "d.txt", cwd /= "e.txt" );
		serenity::file_utils::RenameFile( cwd /= "e.txt", cwd /= "f.txt" );
		serenity::file_utils::RenameFile( cwd /= "f.txt", cwd /= "g.txt" );
		serenity::file_utils::RenameFile( cwd /= "g.txt", cwd /= "h.txt" );
		serenity::file_utils::RenameFile( cwd /= "h.txt", cwd /= "i.txt" );
		serenity::file_utils::RenameFile( cwd /= "i.txt", cwd /= "j.txt" );
		serenity::file_utils::RenameFile( cwd /= "j.txt", cwd /= "k.txt" );
		serenity::file_utils::RenameFile( cwd /= "k.txt", cwd /= "l.txt" );
		serenity::file_utils::RenameFile( cwd /= "l.txt", cwd /= "m.txt" );
		serenity::file_utils::RenameFile( cwd /= "m.txt", cwd /= "n.txt" );
		serenity::file_utils::RenameFile( cwd /= "n.txt", cwd /= "o.txt" );
		serenity::file_utils::RenameFile( cwd /= "o.txt", cwd /= "p.txt" );
		serenity::file_utils::RenameFile( cwd /= "p.txt", cwd /= "q.txt" );
		serenity::file_utils::RenameFile( cwd /= "q.txt", cwd /= "r.txt" );
		serenity::file_utils::RenameFile( cwd /= "r.txt", cwd /= "s.txt" );
		serenity::file_utils::RenameFile( cwd /= "s.txt", cwd /= "t.txt" );
		serenity::file_utils::RenameFile( cwd /= "t.txt", cwd /= "u.txt" );
		serenity::file_utils::RenameFile( cwd /= "u.txt", cwd /= "v.txt" );
		serenity::file_utils::RenameFile( cwd /= "v.txt", cwd /= "w.txt" );
		serenity::file_utils::RenameFile( cwd /= "w.txt", cwd /= "x.txt" );
		serenity::file_utils::RenameFile( cwd /= "x.txt", cwd /= "y.txt" );
		serenity::file_utils::RenameFile( cwd /= "y.txt", cwd /= "z.txt" );
		serenity::file_utils::RenameFile( cwd /= "z.txt", cwd /= "LastRename.txt" );
		serenity::file_helper::path searchPath = sbPath;
		auto                        rnFiles    = fu::RetrieveDirEntries( searchPath, false );
		auto                        found      = fu::SearchDirEntries( rnFiles.retrievedItems, "LastRename.txt" );
		if( found.fileFound ) {
			for( auto &file : found.matchedResults ) {
				SE_TRACE( "File: {} Was Successuly Found", file.path( ).filename( ) );
				SE_INFO( "Rename Test Was Successful" );
			}
		}
		serenity::file_utils::RenameFile( cwd /= "LastRename.txt", cwd /= "a.txt" );  // "reset"
		SE_TRACE( "\"LastRename.txt\" Reset To \"a.txt\" " );
	}
	catch( const std::filesystem::filesystem_error &fs_err ) {
		SE_FATAL( "FS EXCEPTION CAUGHT:\n{}", fs_err.what( ) );
	}
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