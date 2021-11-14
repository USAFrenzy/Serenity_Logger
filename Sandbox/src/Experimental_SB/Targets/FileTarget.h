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
				FileTarget( );  // default that will just write to a "GenericLog.txt"
				FileTarget( std::string fileName, bool truncateIfExists = false );
				// --------------------------------------------------------------------------------------------------------
				/*
					Debating on if I want this behavior or not - It'd be nice to copy the file handle over, but at
				   the same time, once multi-threading is introduced I would absolutely NEED either atomics or mutexes
				   to keep this thread-safe during writes and prevent data races from occuring
				 */
				FileTarget( const FileTarget &t );
				FileTarget operator=( const FileTarget &t );
				// --------------------------------------------------------------------------------------------------------
				~FileTarget( );

				bool OpenFile( bool truncate );
				bool CloseFile( );
				bool RenameFile( );
				bool WriteToFile( );
				bool Flush( );

			      private:
				std::ofstream fileHandle;
			};
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity