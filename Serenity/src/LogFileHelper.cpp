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
	LogFileHelper::LogFileHelper( file_helper::path& logDir, file_helper::path& fileName )
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
		file_utils::ValidateFileName( fileNewName );
		std::string extension;
		/*
			Check to see if fileNewName contains it's own extension. If so, throw away the stored
		    extension and use the one include, otherwise, retain the old extension format
		*/
		if( !( fileNewName.find_last_of( "." ) != std::string::npos ) ) {
			extension = m_filePath.filename( ).extension( ).string( );
		}
		m_filePath.replace_filename( fileNewName + extension );
		UpdateFileInfo( m_filePath );
		m_logName   = m_filePath.filename( ).string( );
		m_cachePath = m_filePath;
		m_cacheDir  = m_cachePath.stem( );
	}

	void LogFileHelper::ChangeDir( file_helper::path destDir )
	{
		std::error_code ec;
		try {
			file_helper::current_path( destDir );
		}
		catch( const std::exception& e ) {
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
		if( ( m_fileName.has_extension( ) ) ||
		    ( m_fileName.string( ).find_first_of( "." ) != std::string::npos ) ) {
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

	void LogFileHelper::StorePathComponents( file_helper::path& pathToStore )
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

	std::string LogFileHelper::PathComponents_Str( file_helper::path& path )
	{
		auto pPath        = path.string( );
		auto rootPath     = path.root_path( ).string( );
		auto rootDir      = path.root_directory( ).string( );
		auto rootName     = path.root_name( ).string( );
		auto parentPath   = path.parent_path( ).string( );
		auto relativePath = path.relative_path( ).string( );
		auto pathStem     = path.stem( ).string( );
		return "\nFor The Path: " + pPath +
		       "\n\t#################### Path Components ####################\n\tRoot Path:\t" + rootPath +
		       "\n\tRoot Name:\t" + rootName + "\n\tRoot Dir:\t" + rootDir + "\n\tRelative Path:\t" +
		       relativePath + "\n\tParent Path:\t" + parentPath + "\n\tPath Stem:\t" + pathStem;
	}
}  // namespace serenity


// MISC Functions
namespace serenity
{
	namespace file_utils
	{
		bool ValidateFileName( std::string fileName )
		{
			std::smatch match;
			std::regex validateFile(
			  "^([_[:alnum:][:space:]])+.[a-zA-Z]+$|^[.]([_[:alnum:][:space:]])+[.]([_[:alnum:][:"
			  "space:]])+$|^"
			  "[.]([_[:alnum:][:space:]])+$" );
			if( !( std::regex_search( fileName, match, validateFile ) ) ) {
				SE_INTERNAL_WARN(
				  "WARNING:\t"
				  "File Name [ {} ] May Contain Invalid Characters Or Extension May Be Invalid."
				  "\n\t\t\t\t"
				  "If This Change Was Intended, Then You May Ignore This Warning.",
				  fileName );
				return false;
			}
			else {
				return true;
			}
		}

		void RenameFile( std::string oldFile, std::string newFile ) { }

	}  // namespace file_utils
}  // namespace serenity