#pragma once

#include "Target.h"

#include <fstream>

namespace serenity::expiremental::targets
{
	class FileTarget : public TargetBase
	{
	  public:
		// clang-format off
		/*******************************************************************************************************//**
	    *     For all File Target Constructors, if either the directories or the file don't exist yet, the 
		*     constructor will create the neccessary directories as well as the file needed to write to.
		*     - For the constructors that don't take a file path variable, the logs will be written to 
		*       a "Logs" directory in the location that the app is being run from.
        **********************************************************************************************************/
		// clang-format on

		/// Default constructor will write to Generic_Log.txt
		FileTarget( );
		explicit FileTarget( std::string_view fileName, bool replaceIfExists = false );
		explicit FileTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit FileTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
							 bool replaceIfExists = false );
		///  Deleted
		FileTarget( const FileTarget & ) = delete;
		///  Deleted
		FileTarget &operator=( const FileTarget & ) = delete;
		/// Cleans up background flush thread if enabled, flush the contents of the file handle to the file (if messages were
		/// written to buffer, will now write contents of buffer to file), and then close the file context
		~FileTarget( );
		///  Returns the file path of the file context currently being held
		const std::string FilePath( );
		///  Returns the file name of the file context currently being held
		const std::string FileName( );
		///  Closes the file context currently being held and re-opens the same file context, truncating the file size to 0
		void EraseContents( );
		/// Closes the file (if open) and trys to rename current file context. If rename is succesful, will re-open file
		/// context under the new file name If successful, returns true. If unsuccessful, will return false with error
		/// message
		virtual bool RenameFile( std::string_view newFileName );
		///  Opens the file held by the file handle and sets the file buffer size using "DEFAULT_BUFFER_SIZE" macro.
		///  If successful, returns true. If unsuccessful, will return false with error message
		bool OpenFile( bool truncate = false );
		/// Joins any currently running flush background thread (if enabled) and if the file handle's file context is
		/// currently open, will flush contents to disk and close the file.
		/// If successful, returns true. If unsuccessful, will return false with error message
		bool CloseFile( );
		/// If contents were written to buffer with isWriteToBuf() set to true, will now write contents of buffer to file and
		/// flush contents to disk, otherwise, just flushes contents to disk
		void Flush( );

	  private:
		LoggerLevel   logLevel;
		Flush_Policy &policy;

	  protected:
		std::ofstream    fileHandle;
		FileSettings     fileOptions;
		BackgroundThread flushWorker;
		/// Compares current flush setting and executes that policy if active. Current policies are: always flush, never
		/// flush, LogLevel-based flushing and time-based flushing. Time based flushing uses a background thread worker which
		/// will intermittenly lock the file when elapsed time is reached from "flushEvery" setting, flush the contents to
		/// disk, and then unlock the file for further writes
		void PolicyFlushOn( ) override;
		/// Checks if background flush thread is active, if it is - will lock access to the file for writing. If rotate
		/// setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to the
		/// file. If the file size would exceed the limit, closes the current file and rotates to next file before writing
		/// the message. Will then follow any settings active in the flush policy
		void PrintMessage( std::string_view formatted ) override;
	};
}  // namespace serenity::expiremental::targets