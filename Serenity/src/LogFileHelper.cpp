#include "serenity/LogFileHelper.hpp"
#include "serenity/Defines.hpp"
#include <regex>

// Just For Simple Test func()
#include <iostream>
// Unfortunate Really, But To Use The Logger Type Functions, This Is Required
#include <serenity/Logger.hpp>


namespace serenity
{
	bool LogFileHelper::fileInfoChanged = false;
	// clang-format off
	/*
		For The Constuctors, would be helpful to take in a path, search for the root, find the whole path
		from root to end of input path, then break into component paths and set the member vairables
		respective of those paths
		
		- Also Would Like To Just Have The Constructors Look Like:
		
		LogFileHelper::LogFileHelper()
		: m_logDir("Logs"), m_logName("Log.txt")
		{
			StorePathComponents();
		}
	*/
	// clang-format on

	LogFileHelper::LogFileHelper( ) : m_logDir( "Logs" ), m_logName( "Log.txt" )
	{
		auto defaultPath     = file_helper::current_path( );
		auto defaultFilePath = defaultPath /= ( m_logDir + "\\" + m_logName );
		StorePathComponents( defaultPath );
		ChangeDir( defaultFilePath );
	}
	LogFileHelper::LogFileHelper( file_helper::path &logDir, file_helper::path &fileName )
	  : m_logDir( logDir.string( ) ), m_logName( fileName.string( ) )
	{
		auto defaultPath     = file_helper::current_path( );
		auto defaultFilePath = logDir /= fileName;
		StorePathComponents( defaultPath );
		ChangeDir( defaultFilePath );
	}


	void LogFileHelper::UpdateFileInfo( file_helper::path pathToFile )
	{
		StorePathComponents( pathToFile );
		NotifyLogger( );
	}
	LogFileHelper::~LogFileHelper( )
	{
		m_logName.clear( );
		m_logDirPath.clear( );
		m_filePath.clear( );
		UpdateFileInfo( m_filePath );
	}

	void LogFileHelper::NotifyLogger( )
	{
		fileInfoChanged = true;
	}

	// #######################################################################################
	// # (TODO): Look For A Better Way To Filter The File Name And Extension                 #
	// # -> Creating A Temp Variable And All The Extra Stuff Is Really Hacky At The Moment   #
	// #######################################################################################
	bool LogFileHelper::RenameLogFile( std::string fileNewName )
	{
		std::string newExtension;
		std::string newFileName;
		std::string tmp = fileNewName;
		// Possibly Redundant Checks (Really should just parameterize this to take a path to a target log file to rename)
		if( ( !( m_filePath.filename( ).has_extension( ) ) && ( m_filePath.filename( ) == m_pathStem ) ) ) {
			SE_INTERNAL_WARN( "WARNING: Couldn't Rename File. Current Path Is Pointing To Directory: {} And Not A File.",
					  m_filePath.filename( ) );
			return false;
		}
		if( file_utils::ValidateFileName( fileNewName ) && file_utils::ValidateExtension( fileNewName ) ) {
			if( file_utils::CompareExtensions( m_filePath.filename( ).string( ), fileNewName ) ) {
				m_filePath.replace_filename( fileNewName );
				UpdateFileInfo( m_filePath );
			}
			else {
				auto extBegin = tmp.find_last_of( "." );
				auto exEnd    = ( tmp.find_last_not_of( "." ) + 1 );
				if( tmp.begin( ) != tmp.end( ) ) {
					tmp.erase( extBegin, exEnd - extBegin );
					newFileName = tmp;
					SE_INTERNAL_DEBUG( "newFileName: {}", newFileName );
				}
				auto begin = fileNewName.find_first_not_of( "." );
				auto end   = fileNewName.find_last_of( "." );
				if( std::string::npos != begin && std::string::npos != end && begin <= end ) {
					fileNewName.erase( begin, end - begin );
					newExtension.assign( fileNewName );
					SE_INTERNAL_DEBUG( "newExtension: {}", newExtension );
				}
				SE_INTERNAL_DEBUG( "Full New File Name: {}{}", newFileName, newExtension );
				m_filePath.replace_filename( newFileName.append( newExtension ) );
				UpdateFileInfo( m_filePath );
			}
		}
		m_cachePath = m_filePath;
		m_cacheDir  = m_cachePath.stem( );
		return true;
	}

	void LogFileHelper::ChangeDir( file_helper::path destDir )
	{
		std::error_code ec;
		try {
			file_helper::current_path( destDir );
		}
		catch( const std::exception &e ) {
			SE_INTERNAL_FATAL( "EXCEPTION CAUGHT:\n{}", e.what( ) );
		}
		UpdateFileInfo( destDir );
		m_cachePath = destDir;
		m_cacheDir  = m_cachePath.stem( );
	}

	// Helper functions
	void LogFileHelper::ForceUpdate( )
	{
		UpdateFileInfo( m_cachePath );
	}
	file_helper::path LogFileHelper::GetFileName( )
	{
		if( m_fileName.has_extension( ) )  // i.e Not A Directory
		{
			return m_fileName;
		}
		else {
			return "";
		}
	}
	file_helper::path LogFileHelper::GetFilePath( )
	{
		ForceUpdate( );
		return m_filePath;
	}
	file_helper::path LogFileHelper::GetLogDirPath( )
	{
		return m_logDirPath;
	}
	file_helper::path LogFileHelper::GetCurrentDir( )
	{
		ForceUpdate( );
		return m_currentDir;
	}
	void LogFileHelper::SetDir( file_helper::path oldPathDir, file_helper::path destDirPath )
	{
		oldPathDir  = destDirPath;
		m_cachePath = destDirPath;
		m_cacheDir  = m_cachePath.stem( );
	}

	void LogFileHelper::StorePathComponents( file_helper::path &pathToStore )
	{
		m_filePath     = pathToStore;
		m_currentDir   = file_helper::current_path( );
		m_cachePath    = m_currentDir;
		m_rootPath     = pathToStore.root_path( );
		m_rootDir      = pathToStore.root_directory( );
		m_rootName     = pathToStore.root_name( );
		m_parentPath   = pathToStore.parent_path( );
		m_relativePath = pathToStore.relative_path( );
		m_pathStem     = pathToStore.stem( );
		m_cacheDir     = m_pathStem;
		m_fileName     = pathToStore.filename( );
		m_logName      = m_fileName.string( );  // Honestly Pretty Redundant
		m_logDirPath   = m_filePath.string( );
		m_logDir       = m_relativePath.string( );
	}
	// clang-format off
	// ############################################################################### Testing Functions  ###############################################################################
	// clang-format on

	std::string LogFileHelper::PathComponents_Str( file_helper::path &path )
	{
		auto pPath        = path.string( );
		auto rootPath     = path.root_path( ).string( );
		auto rootDir      = path.root_directory( ).string( );
		auto rootName     = path.root_name( ).string( );
		auto parentPath   = path.parent_path( ).string( );
		auto relativePath = path.relative_path( ).string( );
		auto pathStem     = path.stem( ).string( );
		return "\nFor The Path: " + pPath + "\n\t#################### Path Components ####################\n\tRoot Path:\t" +
		       rootPath + "\n\tRoot Name:\t" + rootName + "\n\tRoot Dir:\t" + rootDir + "\n\tRelative Path:\t" + relativePath +
		       "\n\tParent Path:\t" + parentPath + "\n\tPath Stem:\t" + pathStem;
	}
}  // namespace serenity


// MISC Functions
namespace serenity
{
	std::string GetSerenityVerStr( )
	{
		auto version = VERSION_NUMBER( SERENITY_MAJOR, SERENITY_MINOR, SERENITY_REV );
		return version;
	}

	namespace file_utils
	{
		/*
		This Regex Pattern will allow any filename other than those that contain these in their name:
		-/	-|	-<	->	-:	-\	-?	-*	-" // Invalid Windows File Name
	   Characters -CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3,
	   LPT4, LPT5, LPT6, LPT7, LPT8, LPT9 // Windows Reserved File Names

		Otherwise The Only Banned Characters Are:
		-\	-NUL	-:   -/                      // Only Restrictions On Linux/Unix And Forcing ':' And '/'
	   To Be Restricted For MacOS And Sanity Respectively
	*/
		bool ValidateFileName( std::string fileName )
		{
			try {
				std::smatch match;
#if WIN32
				std::regex validateFile(
				  "^[\\/:\"*?<>|]|CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|"
				  "COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9+$" );
#else
				std::regex validateFile( "^[\\/:]|NUL+$" );
#endif
				if( ( std::regex_search( fileName, match, validateFile ) ) ) {
					SE_INTERNAL_WARN(
					  "WARNING:\t"
					  "File Name [ {} ] Contains Invalid Characters.",
					  fileName );
					return false;
				}
				else {
					return true;
				}
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN ValidateFileName():\n{}", e.what( ) );
			}
		}

		bool ValidateExtension( std::string fileName )
		{
			try {
				std::smatch match;
				std::regex  validExtension( "^.[a-zA-Z]{7}$" );  // making 7 a thing here due to files like .config
				if( ( std::regex_search( fileName, match, validExtension ) ) ) {
					SE_INTERNAL_WARN(
					  "WARNING:\t"
					  "File Name [ {} ] Extension Is Invalid.",
					  fileName );
					return false;
				}
				else {
					return true;
				}
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN ValidateExtension():\n{}", e.what( ) );
			}
		}


		bool CompareExtensions( std::string oldFile, std::string newFile )
		{
			try {
				std::string oldExtension, newExtension;
				// Find The Respective Extensions
				if( !( oldFile.find_last_of( "." ) != std::string::npos ) ) {
					oldExtension = static_cast<char>( oldFile.substr( ).find_last_of( "." ) + 1 );
				}
				if( !( newFile.find_last_of( "." ) != std::string::npos ) ) {
					newExtension = static_cast<char>( newFile.substr( ).find_last_of( "." ) + 1 );
				}
				if( !( newExtension == oldExtension ) ) {
					SE_INTERNAL_WARN(
					  "WARNING:\t"
					  "New File Name: [ {} ] Does Not Have The Same Extension As Old File: [ {} ]\n",
					  newFile, oldFile );
					return false;
				}
				else {
					return true;
				}
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN CompareExtensions():\n{}", e.what( ) );
			}
		}

		void Rename( std::string oldFile, std::string newFile )
		{
			std::string extension;
			/*
				Check to see if fileNewName is a valid rename string and if so, perform a simple
			   extension check. Could add more complete checks here, but for now, if it's a valid
			   rename string, then just rename the file to the new file name.
			*/
			if( file_utils::ValidateFileName( newFile ) ) {
				file_utils::CompareExtensions( oldFile, newFile );
				oldFile = newFile;
			}
			else {
				SE_INTERNAL_ERROR( "Could Not Rename {} To {}\tReason: Not A Valid Rename String", oldFile, newFile );
			}
		}

		std::vector<std::filesystem::directory_entry> RetrieveDirEntries( std::filesystem::path &path, bool recursiveSearch )
		{
			std::vector<std::filesystem::directory_entry> dirEntries;
			std::size_t                                   pathSize = path.string( ).size( );
			dirEntries.clear( );

			if( path.has_extension( ) ) {
				SE_INTERNAL_WARN(
				  "WARNING: Path To Retrieve Directory Entries Points To A File Instead Of A Directory\nPath: {}", path );
				return dirEntries;
			}
			if( recursiveSearch ) {
				for( const std::filesystem::directory_entry &entry :
				     std::filesystem::recursive_directory_iterator( path ) ) {
					dirEntries.emplace_back( entry.path( ).string( ) );
				}
			}
			else {
				for( const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator( path ) ) {
					dirEntries.emplace_back( entry.path( ).string( ) );
				}
			}
			return dirEntries;
		}

		std::tuple<bool, std::vector<std::filesystem::directory_entry>>
		  SearchDirEntries( std::vector<std::filesystem::directory_entry> dirEntries, std::string searchString )
		{
			std::vector<std::filesystem::directory_entry> matchedResults;
			bool                                          containsMatch { false };


			for( const auto &entry : dirEntries ) {
				auto entryStr = entry.path( ).filename( ).string( );
				// compare returns 0 if they're equal
				if( entryStr.compare( searchString ) == 0 ) {
					matchedResults.emplace_back( entry );
					containsMatch = true;
				}
			}
			return std::make_tuple( containsMatch, matchedResults );
		}

		// Return The Object Specified (Not Yet Implemented)
		std::filesystem::directory_entry RetrieveDirObject( std::filesystem::directory_entry &entry )
		{
			std::filesystem::directory_entry temp;
			return temp;
		}

	}  // namespace file_utils
}  // namespace serenity