#pragma once

#include "Target.h"

#include <fstream>

namespace serenity::expiremental::targets
{
	class FileTarget : public TargetBase
	{
	  public:
		/// <summary>
		/// Default constructor will write to Generic_Log.txt in "Logs" directory of where the app is running from
		/// </summary>
		FileTarget( );
		/// <summary>
		/// Constructor that takes in a logger name and an optional boolean that, when set to true, will truncate the
		/// file given in the file path variable when opened (Default value is
		/// false). The log will be written to "Logs" directory where the app is being run
		/// </summary>
		/// <param name="filePath:">The path to the file to log
		/// to. If either the directories or the file don't exist yet, this will
		/// create the neccessary directories as well as the file</param>
		explicit FileTarget( std::string_view fileName, bool replaceIfExists = false );
		/// <summary>
		/// Constructor that takes in a logger name, a file path to the file to log
		/// to, and an optional boolean that, when set to true, will truncate the
		/// file given in the file path variable when opened (Default value is
		/// false).
		/// </summary>
		/// <param name="filePath:">The path to the file to log
		/// to. If either the directories or the file don't exist yet, this will
		/// create the neccessary directories as well as the file</param>
		explicit FileTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		/// <summary>
		/// Constructor that takes in a logger name, a format pattern to use for
		/// messages logged, a file path to the file to log to, and an optional
		/// boolean that, when set to true, will truncate the file given in the file
		/// path variable when opened (Default value is false).
		/// </summary>
		/// <param name="filePath:">The path to the file to log
		/// to. If either the directories or the file don't exist yet, this will
		/// create the neccessary directories as well as the file</param>
		explicit FileTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
							 bool replaceIfExists = false );
		/// <summary> Deleted </summary>
		FileTarget( const FileTarget & ) = delete;
		/// <summary> Deleted </summary>
		FileTarget &operator=( const FileTarget & ) = delete;
		/// <summary>
		/// When the deconstructor is called, will clean up background flush thread
		/// if enabled, flush the contents of the file handle to the file (if
		/// messages were written to buffer, will now write contents of buffer to
		/// file), and then close the file context
		/// </summary>
		~FileTarget( );
		/// <summary> Returns the file path of the file context currently being held </summary>
		const std::string FilePath( );
		/// <summary> Returns the file name of the file context currently being held </summary>
		const std::string FileName( );
		/// <summary> Closes the file context currently being held and re-opens the same file context, truncating the file size to 0
		/// </summary>
		void EraseContents( );
		/// <summary> Closes the file if open, trys to rename current file context. If rename is succesful, will re-open file context
		/// under new file name</summary> <returns> If successful, returns true. If unsuccessful, will return false with error message
		/// </returns>
		virtual bool RenameFile( std::string_view newFileName );
		/// <summary> Opens the file held by the file handle and sets the file buffer size using "DEFAULT_BUFFER_SIZE" macro.
		/// </summary> <returns>If successful, returns true. If unsuccessful, will return false with error message </returns>
		bool OpenFile( bool truncate = false );
		/// <summary>
		/// Joins any currently running flush background thread (if enabled) and if the file handle's file context is currently open,
		/// will flush contents to disk and close the file </summary>
		/// <returns> If successful, returns true. If unsuccessful, will return false with error message</returns>
		bool CloseFile( );
		/// <summary>
		/// If contents were written to buffer with isWriteToBuf() set to true, will now write contents of buffer to file and flush
		/// contents to disk, otherwise, just flushes contents to disk
		/// </summary>
		void Flush( );

	  private:
		LoggerLevel   logLevel;
		Flush_Policy &policy;

	  protected:
		std::ofstream    fileHandle;
		FileSettings     fileOptions;
		BackgroundThread flushWorker;
		/// <summary>
		/// Compares current flush setting and executes that policy if active. Current policies are: always flush, never flush,
		/// LogLevel-based flushing and time-based flushing. Time based flushing uses a background thread worker which will
		/// intermittenly lock the file when elapsed time is reached from "flushEvery" setting, flush the contents to disk, and then
		/// unlock the file for further writes
		/// </summary>
		void PolicyFlushOn( ) override;
		/// <summary>
		/// Checks if background flush thread is active, if it is - will lock access
		/// to the file for writing. If rotate setting is enabled, will check that
		/// the file size doesn't exceed file size limit and writes the message to
		/// the file. If the file size would exceed the limit, closes the current
		/// file and rotates to next file before writing the message. Will then
		/// follow any settings active in the flush policy
		/// </summary>
		void PrintMessage( std::string_view formatted ) override;
	};
}  // namespace serenity::expiremental::targets