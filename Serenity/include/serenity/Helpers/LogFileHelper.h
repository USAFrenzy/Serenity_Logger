#pragma once


#include <filesystem>
#include <spdlog/details/file_helper.h>
#include <serenity/Interfaces/IObserver.h>

namespace serenity
{
	class LogFileHelper : protected IFileHelper
	{
	      public:
		LogFileHelper( ) = delete;
		LogFileHelper( file_helper::directory_entry &pathToFileDir, std::string &fileName );
		~LogFileHelper( );

		// void ChangeDir( file_helper::path destDir );
		void SetDir( file_helper::path oldPathDir, file_helper::path destDirPath );
		void virtual SetLogDirPath( file_helper::path logDirPath );
		void SetLogFilePath( file_helper::path logPath );
		void StorePathComponents( file_helper::path &pathToStore );
		// Just Learned Of The Existence Of CVs, Looks Hella Promising For Notifying A System Of Changes And Will Definitely
		// Have To Look Into It More As I Feel it Fits In Here Quite Well, Especially Since The Default Of This Library Is To
		// Be Multi-threaded
		void NotifyLogger( ) override;
		void UpdateFileInfo( file_helper::path pathToFile ) override;

		file_helper::path const FileName( );
		file_helper::path const LogFilePath( );
		file_helper::path virtual const LogDirPath( );
		file_helper::path virtual const CurrentDir( );


		// Testing Functions
		std::string const PathComponents_Str( file_helper::path path );

	      private:
		file_helper::path m_cachePath;
		file_helper::path m_cacheDir;

		file_helper::path m_currentDir   = file_helper::current_path( );
		file_helper::path m_relativePath = m_currentDir.relative_path( );
		file_helper::path m_logDirPath;
		file_helper::path m_filePath   = m_logDirPath;
		file_helper::path m_rootPath   = m_currentDir.root_path( );
		file_helper::path m_rootDir    = m_currentDir.root_directory( );
		file_helper::path m_rootName   = m_currentDir.root_name( );
		file_helper::path m_parentPath = m_currentDir.parent_path( );
		file_helper::path m_pathStem   = m_currentDir.stem( );
		file_helper::path m_fileName   = m_filePath.filename( );


	      private:
		void                         ForceUpdate( );
		spdlog::details::file_helper spdLogFilehandle;

	      public:
		static bool fileInfoChanged;
	};
}  // namespace serenity