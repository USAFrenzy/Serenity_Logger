#pragma once


#include <serenity/Common.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Helpers/LibLogger.h>

#include <filesystem>

namespace serenity
{
	class LogFileHelper : protected IFileHelper
	{
	      public:
		explicit LogFileHelper( const file_helper::path pathToFile );
		LogFileHelper( )                        = delete;
		LogFileHelper( const LogFileHelper & )  = delete;
		LogFileHelper( const LogFileHelper && ) = delete;
		LogFileHelper &operator=( const LogFileHelper & ) = delete;
		~LogFileHelper( )                                 = default;

		const file_helper::path            LogFilePath( );
		const file_helper::path            RelativePathToLog( );
		const std::string                  LogName( );
		const file_helper::directory_entry LogDir( );
		void                               SetLogFilePath( const file_helper::path logPath );
		void                               SetLogDirPath( const file_helper::path logDirPath );
		void                               StorePathComponents( const file_helper::path &pathToStore );
		void                               NotifyLogger( ) override;
		void                               UpdateFileInfo( const file_helper::path pathToFile ) override;
		bool                               OpenFile( const file_helper::path filePath, bool truncate = false );
		void                               Flush( );
		bool                               CloseFile( const file_helper::path filePath );

	      private:
		static std::shared_ptr<InternalLibLogger> internalLogger;
		file_helper::path                         m_logDirPath;
		file_helper::path                         m_filePath;
		file_helper::path                         m_fileName;

	      public:
		std::atomic<bool> fileInfoChanged;
	};
}  // namespace serenity