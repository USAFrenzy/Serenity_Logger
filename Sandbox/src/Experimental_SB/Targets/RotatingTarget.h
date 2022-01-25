#include "FileTarget.h"

namespace serenity::expiremental::targets
{
	class RotatingTarget : public FileTarget
	{
	  public:
		/// <summary>
		/// Default constructor will write to Rotating_Log.txt which will be renamed
		/// as Rotating_Log_01.txt and follow rotation settings thereafter
		/// </summary>
		RotatingTarget( );
		/// <summary>
		/// Constructor that takes in a logger name, a file path to the file to log
		/// to, and an optional boolean that, when set to true, will truncate the
		/// file given in the file path variable when opened (Default value is
		/// false).
		/// </summary>
		/// <param name="filePath:">The path to the file to log
		/// to. If either the directories or the file don't exist yet, this will
		/// create the neccessary directories as well as the file</param>
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		/// <summary>
		/// Constructor that takes in a logger name, a format pattern to use for
		/// messages logged, a file path to the file to log to, and an optional
		/// boolean that, when set to true, will truncate the file given in the file
		/// path variable when opened (Default value is false).
		/// </summary>
		/// <param name="filePath:">The path to the file to log
		/// to. If either the directories or the file don't exist yet, this will
		/// create the neccessary directories as well as the file</param>
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
								 bool replaceIfExists = false );
		/// <summary>
		/// Deleted
		/// </summary>
		RotatingTarget( const RotatingTarget & ) = delete;
		/// <summary>
		/// Deleted
		/// </summary>
		RotatingTarget &operator=( const RotatingTarget & ) = delete;
		/// <summary>
		/// When the deconstructor is called, will clean up background flush thread
		/// if enabled, flush the contents of the file handle to the file (if
		/// messages were written to buffer, will now write contents of buffer to
		/// file), and then close the file context
		/// </summary>
		~RotatingTarget( );
		/// <summary>
		/// This function takes in a boolean value that determines whether or not the
		/// file context currently held should rotate when file size limit option has
		/// been reached
		/// </summary>
		void ShouldRotateFile( bool shouldRotate = true );
		/// <summary>
		/// Sets the overall rotation settings for the logger in regards to the file
		/// context. Current options revolve around file size settings only.
		/// </summary>
		/// <param name="settings:"> This parameter controls the following: file size
		/// limit, number of files to rotate through, and whether or not the logger
		/// should rotate through files up to the max number of files set</param>
		void SetRotateSettings( RotateSettings settings );
		/// <summary>
		/// This function will cache the file path given in the constructor, the file
		/// name base (Example: for Rotate_Log.txt, Rotate_Log is stored), the
		/// extension of the file, and the log directory before renaming the current
		/// file to the first iteration of the log file to rotate through. (Example:
		/// Rotate_Log.txt becomes Rotate_Log_01.txt)
		/// </summary>
		void RenameFileForRotation( );
		/// <summary>
		/// If the logger should rotate, will close the current file, increment the
		/// file count up to max number of files set, and try to open the next file
		/// in iteration. If file already exists, will remove the oldest file with
		/// the given file name base before opening the next file in iteration
		/// </summary>
		void RotateFileOnSize( );
		/// <summary>
		/// Will close the current file being written to, replace the old file name
		/// with the new file name given, and then will re-cache file path, file
		/// name, extension, and log directory before opening the renamed file.
		/// Previous files are unaffected, however, if cycling through rotation,
		/// future files will have this new name as their base as well
		/// </summary>
		bool RenameFile( std::string_view newFileName ) override;
		/// <summary>
		/// Checks if background flush thread is active, if it is - will lock access
		/// to the file for writing. If rotate setting is enabled, will check that
		/// the file size doesn't exceed file size limit and writes the message to
		/// the file. If the file size would exceed the limit, closes the current
		/// file and rotates to next file before writing the message. Will then
		/// follow any settings active in the flush policy
		/// </summary>
		void PrintMessage( std::string_view formatted ) override;

	  private:
		bool           rotateFile;
		RotateSettings rotateSettings;
	};

}  // namespace serenity::expiremental::targets