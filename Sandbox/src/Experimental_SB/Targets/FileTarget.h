#pragma once


#include "Target.h"
#include <fstream>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			class FileTarget : public TargetBase
			{
			      public:
				FileTarget( ); // default that will just write to a "GenericLog.txt"
				FileTarget( std::string_view filePath, bool replaceIfExists = false );
				FileTarget( const FileTarget &t ) = delete;
				FileTarget &operator=( const FileTarget &t ) = delete;
				~FileTarget( );

				bool OpenFile( bool truncate = false );
				bool CloseFile( );
				bool EraseContents( );
				bool RenameFile( std::string_view newFileName);
				void PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args ) override;
				bool Flush( );

			      private:
				LoggerLevel   logLevel;
				std::ofstream fileHandle;
				std::string filePath;

			};
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity