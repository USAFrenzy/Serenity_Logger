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
		/// Default constructor will write to Rotating_Log.txt which will be renamed as Rotating_Log_01.txt
		/// and follow rotation settings thereafter in the "Logs" directory of where the app is running from
		RotatingTarget( );
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
								 bool replaceIfExists = false );
		///  Deleted
		RotatingTarget( const RotatingTarget & ) = delete;
		///  Deleted
		RotatingTarget &operator=( const RotatingTarget & ) = delete;
		/// When the deconstructor is called, will clean up background flush thread if enabled, flush the contents of the
		/// file handle to the file (if messages were written to buffer, will now write contents of buffer to file), and then
		/// close the file context
		~RotatingTarget( );
		/// This function takes in a boolean value that determines whether or not the file context currently held should
		/// rotate when file size limit option has been reached
		void ShouldRotateFile( bool shouldRotate = true );
		/// Sets the overall rotation settings for the logger in regards to the filecontext. Current options revolve around
		/// file size settings only.
		/// Parameter "settings": controls the following: file size limit, number of files to rotate through, and whether or
		/// not the logger should rotate through files up to the max number of files set
		void SetRotateSettings( RotateSettings settings );
		/// This function will cache the file path given in the constructor, the file name base (Example: for Rotate_Log.txt,
		/// Rotate_Log is stored), the extension of the file, and the log directory before renaming the current file to the
		/// first iteration of the log file to rotate through. (Example: Rotate_Log.txt becomes Rotate_Log_01.txt)
		void RenameFileForRotation( );
		/// If the logger should rotate, will close the current file, increment the file count up to max number of files set,
		/// and try to open the next file in iteration. If file already exists, will remove the oldest file with the given
		/// file name base before opening the next file in iteration
		void RotateFileOnSize( );
		/// Will close the current file being written to and replace the old file name with the new file name given. Previous
		/// files are unaffected. However, if cycling through rotation, future files will have this new name as their base as
		/// well
		bool RenameFile( std::string_view newFileName ) override;
		/// Checks if background flush thread is active, if it is - will lock access to the file for writing. If rotate
		/// setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to the
		/// file. If the file size would exceed the limit, closes the current file and rotates to next file before writing
		/// the message. This function will then follow any settings active in the flush policy
		void PrintMessage( std::string_view formatted ) override;

	  private:
		bool           rotateFile;
		RotateSettings rotateSettings;
	};

}  // namespace serenity::expiremental::targets