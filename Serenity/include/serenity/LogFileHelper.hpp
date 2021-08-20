#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include "serenity/WIP/Interfaces/IObserver.hpp"

namespace serenity
{
	namespace file_helper = std::filesystem;

	// Would Much Rather Be Able To Relatively Place Things If Able

	class LogFileHelper : protected IFileHelper
	{
	      public:
		LogFileHelper( );
		LogFileHelper( file_helper::path& pathToFileDir, file_helper::path& fileName );
		~LogFileHelper( );
		/*
			Really Only Works To Rename the singular file referenced by m_logName and
			not on arbitrary file renaming
		*/
		void RenameLogFile( std::string fileNewName );
		void ChangeDir( file_helper::path destDir );
		void SetDir( file_helper::path oldPathDir, file_helper::path destDirPath );
		void StorePathComponents( file_helper::path& pathToStore );
		void NotifyLogger( ) override;
		void UpdateFileInfo( file_helper::path pathToFile ) override;
		static bool fileInfoChanged;

		// void SetRelativePath( );
	      public:
		file_helper::path GetFileName( );
		file_helper::path GetFilePath( );
		file_helper::path GetLogDirPath( );
		file_helper::path GetCurrentDir( );


		// Testing Functions
		std::string PathComponents_Str( file_helper::path& path );

	      private:
		file_helper::path m_cachePath;
		file_helper::path m_cacheDir;

		std::string m_logDir;
		std::string m_logName;
		file_helper::path m_currentDir   = file_helper::current_path( );
		file_helper::path m_relativePath = m_currentDir.relative_path( );
		file_helper::path m_logDirPath   = m_relativePath /= m_logDir;
		file_helper::path m_filePath     = m_logDirPath /= m_logName;
		file_helper::path m_rootPath     = m_currentDir.root_path( );
		file_helper::path m_rootDir      = m_currentDir.root_directory( );
		file_helper::path m_rootName     = m_currentDir.root_name( );
		file_helper::path m_parentPath   = m_currentDir.parent_path( );
		file_helper::path m_pathStem     = m_currentDir.stem( );
		file_helper::path m_fileName     = m_filePath.filename( );

	      private:
		// Different From ChangeDir(); SetDir() assigns the paths to member variables
		void ForceUpdate( );
	};
}  // namespace serenity


namespace serenity
{
	namespace file_utils
	{
		bool ValidateFileName( std::string fileName );
		void RenameFile( std::string oldFile, std::string newFile );
	}  // namespace file_utils
}  // namespace serenity