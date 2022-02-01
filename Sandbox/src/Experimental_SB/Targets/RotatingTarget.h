#include "FileTarget.h"

namespace serenity::expiremental::targets
{
	/// @brief This class is in charge of logging to any basic file type and handling the rotation of files up to a maximum
	/// number of files set. This class inherits from the FileTarget class and, therefore, the TargetBase class for common
	/// logging functions and logging settings.
	///
	/// @details For all Rotating Target Constructors: \n
	/// - if either the directories or the file don't exist yet, the  *constructor will create the neccessary directories  \n
	///   as well as the file needed to write to. \n
	/// - For the constructors that don't take a file path variable, the logs will be written to a "Logs" directory in the \n
	///   location that the app is being run from. \n
	class RotatingTarget : public FileTarget
	{
	  public:
		/// @brief Default constructor that will set the logger name to "Rotating_Logger". All sets all other values to their
		/// defaults.
		/// @details The default constructor will also create a file named "Rotating_Log.txt" upon creation. This base name
		/// and extension will be cached internally and the file will be renamed as Rotating_Log_01.txt and follow rotation
		/// settings thereafter in the "Logs" directory of where the app is running from
		RotatingTarget( );
		/// @brief Constructor that sets the logger name, the file name, file path, and log directory off the @p filePath
		/// variable, and will truncate the file if it already exists depending on the value of @p replaceIfExists
		/// @details This constructor will cache the file name, the file path, the file extension, and the log directory off
		/// of the @p filePath variable. The file will then be renamed for rotation - if the file already exists, it will
		/// delete the oldest file with the base file name and rename the file using that file's index number.
		/// @param name: the name that the logger itself will use and be identified by
		/// 	@param filePath: the full path to the file to write to
		/// 	@param replaceIfExists: this value will determine if the file is truncated upon being opened the first time
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		/// @brief Constructor that sets the logger name, the format pattern to use, the file name, file path, and log
		/// directory off the @p filePath variable, and will truncate the file if it already exists depending on the value of
		/// @p replaceIfExists
		/// @details This constructor will cache the file name, the file path, the file extension, and the log directory off
		/// of the @p filePath variable. The file will then be renamed for rotation - if the file already exists, it will
		/// delete the oldest file with the base file name and rename the file using that file's index number.
		/// @param name: the name that the logger itself will use and be identified by
		/// @param formatPattern:  the format pattern that determines how the prepended text will be displayed before the log
		/// message
		/// 	@param filePath: the full path to the file to write to
		/// 	@param replaceIfExists: this value will determine if the file is truncated upon being opened the first time
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
								 bool replaceIfExists = false );
		///  @brief Deleted
		RotatingTarget( const RotatingTarget & ) = delete;
		///  @brief Deleted
		RotatingTarget &operator=( const RotatingTarget & ) = delete;
		/// @brief Cleans up any background resources used and closes the file context currently held
		/// @details When the deconstructor is called, will clean up background flush thread if enabled, flush the contents
		/// of the file handle to the file (if messages were written to buffer, will now write contents of buffer to file),
		/// and then close the file context
		~RotatingTarget( );
		/// @brief This function takes in a boolean value that determines whether or not the file context currently held
		/// should rotate when file size limit option has been reached
		void ShouldRotateFile( bool shouldRotate = true );
		/// @brief Sets the overall rotation settings for the logger in regards to the filecontext.
		/// @details Current options revolve aroundfile size settings only and can be found in the RotateSettings class.
		/// @param settings: controls the following: file size limit, number of files to rotate through, and whether or
		/// not the logger should rotate through files up to the max number of files set
		void SetRotateSettings( RotateSettings settings );
		/// @brief Sets up the base file given in constructor, or the base file after being renamed, for rotation.
		/// @details This function will cache the file's name, extension, and path, as well as the log directory for the file
		/// before renaming the current file to the first iteration of the log file to rotate through. (Example:
		/// Rotate_Log.txt becomes Rotate_Log_01.txt). If this rotation-ready file already exists, will open the file by
		/// overwriting its contents and truncating its size to 0.
		void RenameFileForRotation( );
		/// @brief This function controls how the file is rotated. If the logger should rotate, the file will be closed and
		/// the next file in iteration up to the max file limit will be opened for writing.
		/// @details If the logger should rotate, will close the current file, increment the file count up to max number of
		/// files set, and try to open the next file in iteration. If file already exists, will remove the oldest file with
		/// the given file name base before opening the next file in iteration. If the setting @p rotateFile is set to false,
		/// this function will return and do nothing else.
		void RotateFileOnSize( );
		/// @brief Renames the current file to the name passed in via @p newFileName.
		/// @details Will close the current file being written to and replace the old file name with the new file name given.
		/// Previous files are unaffected. However, if cycling through rotation, future files will have this new name as
		/// their base as well.
		bool RenameFile( std::string_view newFileName ) override;
		/// @brief Writes the message to the currently held file context unless writing to the buffer was enabled - in which
		/// case, this will write to the buffer instead.
		/// @details Checks if background flush thread is active, if it is - will lock access to the file for writing. If
		/// rotate setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to
		/// the file. If the file size would exceed the limit, closes the current file and rotates to next file before
		/// writing the message. This function will then follow any settings active in the flush policy
		/// @param formatted: The actual message in its entirety to send to the output destination.
		void PrintMessage( std::string_view formatted ) override;

	  private:
		bool           rotateFile;
		RotateSettings rotateSettings;
	};

}  // namespace serenity::expiremental::targets