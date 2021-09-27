#pragma once


#include <serenity/Common.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Helpers/LibLogger.h>

#include <filesystem>


/*
			Restructuring And Rescoping The LogFileHelper Class
	--------------------------------------------------------------------
	-> Focus Should Be On Abstracting The Paths From logger To Here
	-> This Class Should Have Meaningful And Useful Functions
	   -> Initially Wanting To Abstract file_utils Functions Here
	      -> Ex: OpenFile, CloseFile, Validation Functions For File Name
	-> Logger Class That Holds A Handle To This One Should Then Just Wrap
	   The Utility Provided By This Class As A Wrapper For Those file_utils
	   Functions
	   -> Possibly Inefficient Given Its A Wrapper->Wrapper->Implementation
	      Scope But I Assume It Shouldn't Have A Meaningful Perf Impact Once
	      Compiled And Resolved
*/

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
		const file_helper::directory_entry LogDir( );
		void                               SetLogFilePath( const file_helper::path logPath );
		void                               SetLogDirPath( const file_helper::path logDirPath );
		void                               StorePathComponents( const file_helper::path &pathToStore );
		void                               NotifyLogger( ) override;
		void                               UpdateFileInfo( const file_helper::path pathToFile ) override;
		bool                               OpenFile( const file_helper::path filePath );
		bool                               CloseFile( const file_helper::path filePath );

	      private:
		static std::shared_ptr<InternalLibLogger> internalLogger;
		file_helper::path                         m_logDirPath;
		file_helper::path                         m_filePath = m_logDirPath;
		file_helper::path                         m_fileName;

	      public:
		std::atomic<bool> fileInfoChanged;
	};
}  // namespace serenity