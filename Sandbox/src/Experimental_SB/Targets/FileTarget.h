#pragma once

#include "Target.h"

#include <filesystem>
#include <fstream>

// Messing with buffer sizes
#define KB          ( 1024 )
#define MB          ( 1024 * KB )
#define GB          ( 1024 * MB )
#define BUFFER_SIZE static_cast<size_t>( 512 * KB )

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

				bool        OpenFile( bool truncate = false );
				bool        CloseFile( );
				std::string FilePath( );
				void        EraseContents( );
				bool        RenameFile( std::string_view newFileName );
				void        PrintMessage( msg_details::Message_Info msgInfo, const std::string_view msg,
							  std::format_args &&args ) final override;
				void        Flush( );

			      private:
				std::vector<std::string> buffer;  // faster than string buffer
				FILE *                   fileHandle;
				LoggerLevel              logLevel;
				std::filesystem::path    filePath;

			      private:
				void PolicyFlushOn( Flush_Policy &policy, std::string_view msg ) final override;
			};
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity