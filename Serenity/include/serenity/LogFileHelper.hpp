#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include "serenity/WIP/Interfaces/IObserver.hpp"

namespace serenity {
	namespace file_helper = std::filesystem;

	// Would Much Rather Be Able To Relatively Place Things If Able

	class LogFileHelper : protected IFileHelper
	{
	      public:
		LogFileHelper( );
		LogFileHelper(file_helper::path& pathToFileDir, file_helper::path& fileName);
		~LogFileHelper( );
		/*
			Really Only Works To Rename the singular file referenced by m_logName and
			not on arbitrary file renaming
		*/
		void RenameFile(std::string fileNewName);
		void ChangeDir(file_helper::path destDir);
		// Best Way I Could Think Of For Selectively Updating Path Variables In An Update Funtion

		static bool fileInfoChanged;
		void NotifyLogger( ) override;
		using optPath                                      = std::optional<file_helper::path>;
		std::optional<std::filesystem::path> optCurrentDir = std::nullopt;
		std::optional<std::filesystem::path> optLogDir     = std::nullopt;
		std::optional<std::filesystem::path> optFilePath   = std::nullopt;
		std::optional<std::filesystem::path> optFileName   = std::nullopt;
		std::optional<file_helper::path> UpdateFileInfo(optPath optCurrentDir,
								optPath optLogDir,
								optPath optFilePath,
								optPath optFileName) override;

		// void SetRelativePath( );
	      public:
		file_helper::path GetFileName( );
		file_helper::path GetFilePath( );
		file_helper::path GetDirPath( );
		file_helper::path GetCurrentDir( );
		

		// Testing Functions
		std::string PathComponents_Str(file_helper::path& path);
		void StorePathComponents( );

	      private:
		file_helper::path m_currentDir = file_helper::current_path( );
		std::string m_logDir;
		file_helper::path m_logDirPath = m_currentDir /= m_logDir;
		std::string m_logName;
		file_helper::path m_filePath   = m_logDirPath /= m_logName;
		file_helper::path m_fileName   = m_filePath.filename( );

		// Gonna Test These Out In Hopes Of Replacing The Hard-Coded Variables Above
	      protected:
		/*file_helper::path m_currentDir;
		file_helper::path m_rootPath;
		file_helper::path m_rootName;
		file_helper::path m_rootDir;
		file_helper::path m_parentPath;
		file_helper::path m_relativePath;
		file_helper::path m_pathStem;
		file_helper::path m_fileName;*/
	};
} // namespace serenity