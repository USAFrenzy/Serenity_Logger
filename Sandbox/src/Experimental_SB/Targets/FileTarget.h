#pragma once

#include "Target.h"

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
				FileTarget( const FileTarget & ) = delete;
				FileTarget &operator=( const FileTarget & ) = delete;
				~FileTarget( );

				std::string FilePath( );
				void        EraseContents( );
				bool        RenameFile( std::string_view newFileName );
				bool        OpenFile( bool truncate = false );
				bool        CloseFile( );
				void        Flush( );

			  private:
				FILE *                fileHandle;
				LoggerLevel           logLevel;
				std::filesystem::path filePath;
				std::vector<char>     fileBuffer;
				size_t                bufferSize;
				// ------------------- WIP -------------------
				std::atomic<bool> ableToFlush { true };
				std::atomic<bool> cleanUpThreads { false };
				// ------------------- WIP -------------------

			  private:
				void PolicyFlushOn( Flush_Policy & ) override;
				void PrintMessage( std::string_view formatted ) override;
			};
		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity