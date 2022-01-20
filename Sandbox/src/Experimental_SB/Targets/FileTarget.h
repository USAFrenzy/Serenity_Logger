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
		struct RotateSettings
		{
			// Will add an interval based setting later
			// (something like a weekly basis on specified day and a daily setting)
			bool   rotateOnFileSize { false };
			size_t maxNumberOfFiles { 5 };
			size_t fileSize { 512 * KB };

			std::filesystem::path &OriginalFileName( )
			{
				return originalFileName;
			}

		  private:
			std::filesystem::path originalFileName;
		};
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
			bool                  rotateFile { false };
			size_t                bufferSize { DEFAULT_BUFFER_SIZE };
			size_t                fileBufOccupied { 0 };
			RotateSettings *      rotateFileSettings { nullptr };
		};

		FileTarget( );  // default that will just write to a "Generic_Log.txt"
		FileTarget( std::string_view filePath, bool replaceIfExists = false );
		FileTarget( const FileTarget & ) = delete;
		FileTarget &operator=( const FileTarget & ) = delete;
		~FileTarget( );

		std::string FilePath( );
		void        EraseContents( );
		bool        RenameFile( std::string_view newFileName );
		bool        OpenFile( bool truncate = false );
		bool        CloseFile( );
		void        Flush( );
		// ------------------- WIP -------------------
		void ShouldRotateFile( bool shouldRotate = true );
		void SetRotateSettings( RotateSettings settings );
		void RotateFileOnSize( );
		// ------------------- WIP -------------------

	  private:
		Flush_Policy &   policy;
		BackgroundThread flushWorker;
		FileSettings     fileOptions;
		std::ofstream    fileHandle;
		LoggerLevel      logLevel;

	  protected:
		void PolicyFlushOn( ) override;
		void PrintMessage( std::string_view formatted ) override;
		void RenameFileForRotation( );
	};
}  // namespace serenity::expiremental::targets