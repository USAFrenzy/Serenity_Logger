#pragma once


#include <string>
#include <filesystem>
#include <optional>
// Currently For The Wrapper Funcs
#include <chrono>
#include <thread>
#include <future>
#include <type_traits>

#include <spdlog/details/file_helper.h>

#include "serenity/Interfaces/IObserver.hpp"
#include "serenity/Common.hpp"

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
		void        SetLogFilePath( file_helper::path logPath );
		void        StorePathComponents( file_helper::path &pathToStore );
		void        NotifyLogger( ) override;
		void        UpdateFileInfo( file_helper::path pathToFile ) override;
		static bool fileInfoChanged;

		// void SetRelativePath( );
	      public:
		file_helper::path const GetFileName( );
		file_helper::path const GetLogFilePath( );
		file_helper::path virtual const GetLogDirPath( );
		file_helper::path virtual const GetCurrentDir( );
		LogFileHelper *_instance( );


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
		LogFileHelper *   m_instance;  // probs uneccessary?


	      private:
		void                         ForceUpdate( );
		spdlog::details::file_helper spdLogFilehandle;
	};
}  // namespace serenity