#pragma once

#include "Target.h"

#include <fstream>
#include <shared_mutex>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			class FileTarget : public TargetBase
			{
			  public:
				FileTarget( );  // default that will just write to a "GenericLog.txt"
				FileTarget( std::string_view filePath, bool replaceIfExists = false );
				FileTarget( const FileTarget &t ) = delete;
				FileTarget &operator=( const FileTarget &t ) = delete;
				~FileTarget( );

				std::string FilePath( );
				void        EraseContents( );
				bool        RenameFile( std::string_view newFileName );
				bool        OpenFile( bool truncate = false );
				bool        CloseFile( );
				void        Flush( );
				void        AsyncWriteMessage( std::string formatted ) override final;

			  private:
				std::ofstream         fileHandle;
				LoggerLevel           logLevel;
				std::filesystem::path filePath;

				// ------------------- WIP -------------------
				std::vector<std::future<void>> m_futures;
				std::atomic<bool>              ableToFlush { true };
				std::atomic<bool>              cleanUpThreads { false };
				std::shared_mutex              readWriteMutex;
				// ------------------- WIP -------------------

			  private:
				void PolicyFlushOn( Flush_Policy& ) final override;
				void PrintMessage( ) final override;
			};
		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity