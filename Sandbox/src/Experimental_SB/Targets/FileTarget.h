#pragma once

#include "Target.h"

#include <fstream>

namespace serenity::expiremental::targets
{
	/// @brief This class is in charge of logging to any basic file type and inherits from the TargetBase class for common
	/// logging functions and logging settings.
	/// @details For all FileTarget Constructors: \n
	/// - If either the directories or the file don't exist yet, the constructor will create the neccessary \n
	///   directories as well as the file needed to write to. \n
	/// - Default initializes the TargetBase inherited values \n
	///   - These being the flush policy, the log level, and time mode. \n
	///   - For the constructors that don't take in a format pattern, this also includes the default pattern \n
	/// \n
	///                      For the constructors that don't take a file path argument: \n
	/// - The log directory and log itself is created in the location that the application being run is located. \n
	/// - The default log directory is named "Logs". \n
	/// - The default file name is "Generic_Log.txt" and will be created under the default log directory. \n
	class FileTarget : public TargetBase
	{
	  public:
		/// @brief Default constructor that sets the logger name to "File_Target" and sets all other values to their defaults
		FileTarget( );
		/// @brief Constructor that will set the file name and will truncate the file if it already exists and if
		/// "replaceIfExists" is set to true. Sets all other values to their defaults.
		/// @details While the file name will be changed to the parameter @p fileName, the file itself will be created under
		/// the default log directory.
		/// @param fileName: The name of the file to create and/or use for logging
		/// @param replaceIfExist: This value denotes whether or not to truncate the file if it exists already
		explicit FileTarget( std::string_view fileName, bool replaceIfExists = false );
		/// @brief Constructor that will set the logger name, file path, log directory, and whether or not to truncate this
		/// file if it exists or not. Sets all other values to their defaults.
		/// @details The name of the logger will be set based on the @p name variable. The file path should include the log
		/// directory and the actual file name in the @p filePath variable as this is how it's intended to be parsed. If the
		/// file already exists and the @p replaceIFExists variable is set to true, then the file will be truncated before
		/// opening for use in logging
		/// @param name: The name the logger will be identified by.
		/// @param filePath: the full path to the file. This value should include any directories as well as the file name
		/// itself.
		/// @param replaceIfExist: This value denotes whether or not to truncate the file if it exists already
		explicit FileTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		/// @brief Constructor that will set the logger name, format pattern to use, file path, log directory, and whether or
		/// not to truncate this file if it exists or not. Sets all other values to their defaults.
		/// @details The name of the logger will be set based on the @p name variable. The format pattern used can be empty
		/// if no prepended text is wanted and can include any of, none of, or all of the flags if desired. The file path
		/// should include the log directory and the actual file name in the @p filePath variable as this is how it's
		/// intended to be parsed. If the file already exists and the @p replaceIFExists variable is set to true, then the
		/// file will be truncated before opening for use in logging
		/// @param name: The name the logger will be identified by.
		/// @param formatPattern: The pattern that is used to determine how the prepended text is displayed before the log
		/// message.
		/// @param filePath: The full path to the file. This value should include any directories as well as the file name
		/// itself.
		/// @param replaceIfExist: This value denotes whether or not to truncate the file if it exists already
		explicit FileTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
							 bool replaceIfExists = false );
		/// @brief Deleted
		FileTarget( const FileTarget & ) = delete;
		///  @brief Deleted
		FileTarget &operator=( const FileTarget & ) = delete;
		/// @brief Cleans up any background threads, flushes contents to disk, and closes the file context being used
		/// @details Cleans up background flush thread if enabled by joining the thread and then flushing the contents of the
		/// file handle to the file (if messages were written to buffer, will now write contents of buffer to file).
		/// Afterwards, the file context being used is closed.
		~FileTarget( );
		///  @brief Returns the file path of the file context currently being held
		const std::string FilePath( );
		///  @brief Returns the file name of the file context currently being held
		const std::string FileName( );
		///  @brief Closes the file context currently being held and re-opens the same file context, truncating the file size
		///  to 0
		void EraseContents( );
		/// @brief Closes the file (if open) and trys to rename current file context. If rename is succesful, will re-open
		/// file context under the new file name.
		/// @details Virtual function that can be overriden in derived classes. The intended usage is to close the file, try
		/// to rename the current file, and then open the file again. This implementation trys to catch any errors in the
		/// renaming process and opening process by returning an error message.
		/// @attention This function, while it doesn't neccessarily throw, does not handle any exceptions that may be thrown
		/// when executed. It simply returns an error message to help diagnose issues; error handling is expected to be
		/// implemented by the user in this case.
		///  @returns If successful, returns true. If unsuccessful, will return false with error
		/// message
		virtual bool RenameFile( std::string_view newFileName );
		///  @brief Opens the file held by the file handle
		/// @details Opens the file context currently being held and sets the file buffer size using "DEFAULT_BUFFER_SIZE"
		/// macro. If the file doessn't already exist, this function will create the file first.
		/// @returns If successful, returns true. If unsuccessful, will return false with error message
		bool OpenFile( bool truncate = false );
		/// @brief Joins any currently running flush background thread (if enabled) and if the file handle's file context is
		/// currently open, will flush contents to disk and close the file.
		/// @returns If successful, returns true. If unsuccessful, will return false with error message
		bool CloseFile( );
		/// @brief If contents were written to buffer, this will now write contents of buffer to file and
		/// flush contents to disk, otherwise, just flushes contents to disk
		void Flush( );

	  private:
		LoggerLevel   logLevel;
		Flush_Policy &policy;

	  protected:
		std::ofstream    fileHandle;
		FileSettings     fileOptions;
		BackgroundThread flushWorker;
		/// @brief Executes the currently set flush policy
		/// @details Compares current flush setting and executes that policy if active. Current policies are: always flush,
		/// never flush, LogLevel-based flushing and time-based flushing. Time based flushing uses a background thread worker
		/// which will intermittenly lock the file when elapsed time is reached from "flushEvery" setting, flush the contents
		/// to disk, and then unlock the file for further writes
		void PolicyFlushOn( ) override;
		/// @brief Writes the message passed in by the  @p formatted variable to the file context.
		/// @details Checks if background flush thread is active, if it is - will lock access to the file for writing. If
		/// rotate setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to
		/// the file. If the file size would exceed the limit, closes the current file and rotates to next file before
		/// writing the message. Will then follow any settings active in the flush policy
		/// @param formatted: The actual message in its entirety to send to the output destination.
		void PrintMessage( std::string_view formatted ) override;
	};
}  // namespace serenity::expiremental::targets