#pragma once

#include "Target.h"

#include <fstream>
#include <thread>
#include <atomic>

namespace serenity::expiremental::targets
{
	class FileTarget : public TargetBase
	{
	  public:
		// Wanted To Abstract some of the variables away as the list was growing pretty large
		struct BackgroundThread
		{
			std::atomic<bool>  cleanUpThreads { false };
			std::atomic<bool>  flushThreadEnabled { false };
			mutable std::mutex readWriteMutex;
			std::thread        flushThread;
		};
		struct FileSettings
		{
			std::filesystem::path filePath;
			std::vector<char>     fileBuffer;
			size_t                bufferSize { DEFAULT_BUFFER_SIZE };
			size_t                fileBufOccupied { 0 };
		};

		FileTarget( );  // default that will just write to a "Generic_Log.txt"
		explicit FileTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit FileTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
							 bool replaceIfExists = false );
		FileTarget( const FileTarget & ) = delete;
		FileTarget &operator=( const FileTarget & ) = delete;
		~FileTarget( );

		const std::string FilePath( );
		const std::string FileName( );
		void              EraseContents( );
		virtual bool      RenameFile( std::string_view newFileName );
		bool              OpenFile( bool truncate = false );
		bool              CloseFile( );
		void              Flush( );

	  private:
		LoggerLevel   logLevel;
		Flush_Policy &policy;

	  protected:
		std::ofstream    fileHandle;
		FileSettings     fileOptions;
		BackgroundThread flushWorker;
		void             PolicyFlushOn( ) override;
		void             PrintMessage( std::string_view formatted ) override;
	};
}  // namespace serenity::expiremental::targets