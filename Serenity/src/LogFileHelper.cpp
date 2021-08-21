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
	// clang-format off
		/*
			^([_[:alnum:][:space:]])+.[a-zA-Z]+$|^[.]([_[:alnum:][:space:]])+[.]([_[:alnum:][:space:]])+$|^[.]([_[:alnum:][:space:]])+$
			I'm positive there's a better way to do this, but for now, it works for what I've tested:
			- Accepts hidden files (.file), hidden files with extensions (.hidden.jpg), files with spaces in between (Spaced File.txt), and normal files (NormalFile.bmp)
				- Warning is now correctly thrown on files like log.jpg.txt 
				- In The future, could add test cases in case valid files like (Server.Data.Logs.txt) exist since right now, the warning would still be issued 
		*/
	// clang-format on


	void LogFileHelper::RenameLogFile( std::string fileNewName )
	{
		std::string       extension;
		file_helper::path oldFileName = m_filePath.filename( );
		/*
			Check to see if fileNewName is a valid rename string and if so, then check if it contains
		   it's own extension. If it does, throw away the stored extension and use the one include,
		   otherwise, retain the old extension format
		*/
		if( file_utils::ValidateFileName( fileNewName ) ) {
			if( file_utils::ValidateForSameExtension( m_filePath.filename( ).extension( ).string( ), fileNewName ) ) {
				extension = m_filePath.filename( ).extension( ).string( );
			}
			else {
				if( fileNewName.find_last_of( "." ) != std::string::npos ) {
					extension = static_cast<char>( fileNewName.find_last_of( "." ) + 1 );
				}
			}
			m_filePath.replace_filename( fileNewName );
			m_filePath.replace_extension( extension );
			UpdateFileInfo( m_filePath );
			m_logName   = m_filePath.filename( ).string( );
			m_cachePath = m_filePath;
			m_cacheDir  = m_cachePath.stem( );
		}
		else {
			SE_ERROR( "ERROR: Could Not Rename {} To {}\tReason: Not A Valid Rename String", m_filePath.filename( ),
				  fileNewName );
			m_fileName = oldFileName;  // Just Ensuring That The Old File Name Remains
		}
	}

	void LogFileHelper::ChangeDir( file_helper::path destDir )
	{
		std::error_code ec;
		try {
			file_helper::current_path( destDir );
		}
		catch( const std::exception &e ) {
			SE_FATAL( "EXCEPTION CAUGHT:\n{}", e.what( ) );
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
		// Second Check Is A Hack Around Files That Contain No Leading String
		// (ex: .config, .conf, .gitignore)
		if( ( m_fileName.has_extension( ) ) || ( m_fileName.string( ).find_first_of( "." ) != std::string::npos ) ) {
			return m_fileName;
		}
		else {
			SE_WARN( "Path: [ {} ] Doesn't Point To A File", m_currentDir );
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
		SetDir( m_logDir, m_cacheDir );
		ForceUpdate( );
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
		m_logDir       = ( m_relativePath /= m_fileName ).string( );
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
		// specifically filters out files like sample.invalid.file.txt
		/*	(NOTE):I know this file naming scheme is valid and with the introduction
			of ValidateForSameExtension() And replace_extension() in RenameLogFile(),
			I should probably add that scheme back into the regex match for valid names.
			Once RenameFile() in file_utils namespace has implemented more checks - will fix
		*/
		bool ValidateFileName( std::string fileName )
		{
			std::smatch match;
			std::regex  validateFile(
                          "^([_[:alnum:][:space:]])+.[a-zA-Z]+$|^[.]([_[:alnum:][:space:]])+[.]([_[:alnum:][:"
                          "space:]])+$|^"
                          "[.]([_[:alnum:][:space:]])+$" );
			if( !( std::regex_search( fileName, match, validateFile ) ) ) {
				SE_INTERNAL_WARN(
				  "WARNING:\t"
				  "File Name [ {} ] May Contain Invalid Characters Or Extension May Be Invalid.",
				  fileName );
				return false;
			}
			else {
				return true;
			}
		}

		bool ValidateForSameExtension( std::string oldFile, std::string newFile )
		{
			std::string oldExtension, newExtension;
			// Find The Respective Extensions
			if( !( oldFile.find_last_of( "." ) != std::string::npos ) ) {
				oldExtension = static_cast<char>( oldFile.substr( ).find_last_of( "." ) + 1 );
			}
			if( !( newFile.find_last_of( "." ) != std::string::npos ) ) {
				newExtension = static_cast<char>( newFile.substr( ).find_last_of( "." ) + 1 );
			}
			if( !( newExtension == oldExtension ) ) {
				SE_WARN(
				  "WARNING:\t"
				  "New File Name: [ {} ] Does Not Have The Same Extension As Old File: [ {} ]",
				  oldFile, newFile );
				return false;
			}
			else {
				return true;
			}
		}

		void RenameFile( std::string oldFile, std::string newFile )
		{
			std::string extension;
			/*
				Check to see if fileNewName is a valid rename string and if so, perform a simple
			   extension check. Could add more complete checks here, but for now, if it's a valid
			   rename string, then just rename the file to the new file name.
			*/
			if( file_utils::ValidateFileName( newFile ) ) {
				file_utils::ValidateForSameExtension( oldFile, newFile );
				oldFile = newFile;
			}
			else {
				SE_ERROR( "Could Not Rename {} To {}\tReason: Not A Valid Rename String", oldFile, newFile );
			}
		}

		std::vector<std::filesystem::directory_entry> RetrieveDirEntries( std::filesystem::path &path )
		{
			std::vector<std::filesystem::directory_entry> dirEntries;
			std::size_t                                   pathSize = path.string( ).size( );  // Just To Simplify Reading

			for( const std::filesystem::directory_entry &entry :
			     // Or directory_iterator(path) if recursion isn't needed
			     std::filesystem::recursive_directory_iterator( path ) )
			{
				dirEntries.emplace_back( entry.path( ).string( ).substr( pathSize ) );
			}
			return dirEntries;
		}


		// Both SearchDirEntries() and RetrieveDirObject() should utilize the bool dirEntryExists

		// Return Path To Dir Object If Search String Exists (Not Yet Implemented)
		std::filesystem::path SearchDirEntries( std::string &searchString )
		{
			std::filesystem::path temp;
			return temp;
		}

		// Return The Object Specified (Not Yet Implemented)
		std::filesystem::directory_entry RetrieveDirObject( std::filesystem::directory_entry &entry )
		{
			std::filesystem::directory_entry temp;
			return temp;
		}

	}  // namespace file_utils
}  // namespace serenity