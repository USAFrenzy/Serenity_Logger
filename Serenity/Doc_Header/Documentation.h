/// @file Documentation.h
/// @author Ryan McCullough
/// @brief This file contains the documentation for the Serenity Logger Library.
/// @version 0.2
/// @date 2022-02-01
/// @copyright MIT License

/*****************************************************************************************************************************
 * This file contains all of Serenity Logger's documentation for the available interfaces including the classes, structs,
 * enums, look-up tables used, namespaces, and intended usage. Once the library is feature complete, this file will contain
 * examples and tips on how to use this library effectively, as well as how to extend the library's functionality.
 ****************************************************************************************************************************/

/***************************************************************************************************************************
 *                                                Common.h Header Documentation                                            *
 **************************************************************************************************************************/
/// @file Common.h
/// @brief This file contains some common functions, enums, and look-up tables used for most logging targets.
///
/// @def ISATTY
/// @brief This macro is defined based on platform detection
/// @details This macro is used to determine if the console output is a terminal device or not. If WINDOWS_PLATFORM is
/// enabled, this will map to _isatty, otherwise, this will map to isatty.
///
/// @def FILENO
/// @brief This macro is defined based on platform detection
/// @details This macro is used in tandem with the ISATTY macro to determine if console output is a terminal device or not.
/// If WINDOWS_PLATFORM is enabled, this will map to _fileno, otherwise this will map to fileno.
///
/// @def LINUX_PLATFORM
/// @brief If WINDOWS_PLATFORM and MAC_PLATFORM are not defined, then this macro is defined.
/// @details If this macro is defined, includes the unistd.h header and defines ISATTY to isatty and FILENO to fileno
///
/// @def ENABLE_VIRTUAL_TERMINAL_PROCESSING
/// @brief This macro is defined by this library only if WINDOWS_PLATFORM is defined and this macro isn't defined by the
/// include of the Windows.h header. Used to support the ansi color codes for the ColorConsoleTarget class.
///
/// @def KB
/// @brief This macro defines a kilobyte as 1024 bytes
///
/// @def MB
/// @brief This macro defines a megabyte as 1024 * 1024 bytes (1024 * KB)
///
/// @def GB
/// @brief This macro defines a gigabyte as 1024 v* 1024 * 1024 bytes (1024 * MB)
///
/// @def DEFAULT_BUFFER_SIZE( 64 * KB )
/// @brief The size used for the file handle's buffer in file targets. The default size is 64KB.
/// @details Between 32KB and 64KB seemed to be the sweet spot range on the machine this library was built on.
/// This value can be changed to better fit performance for the user to avoid frequent OS flush calls.
///
/// @enum serenity::LineEnd
/// @brief The platform-dependant line ending that should be used.
/// @details These values are used by the line_ending unorderd map to map the enum value to the corresponding end of line to
/// append to each log message. Both this enum and the line_ending map are used automatically via the platform detection
/// macros when formatting the log message.
/// @var serenity::LineEnd::linux
/// @brief Used to map to line_ending value "\n"
/// @var serenity::LineEnd::windows
/// @brief Used to map to line_ending value "\r\n"
/// @var serenity::LineEnd::mac
/// @brief Used to map to line_ending value "\r"
///
/// @enum serenity::LoggerLevel
/// @brief This enum encapsulates the logging level features and the actual logger's level
/// @details This enum is used in formatting, on checking if the logger should log, whether or not the logger should flush
/// based on the level set, setting the message level, etc...
///
/// @fn serenity::MsgLevelToShortString(LoggerLevel level)
/// @brief Used to return the short string representation of the current message level
/// @details \n
/// |     Logger Level      |   Return Value   |
/// |  :----:               |      :----:      |
/// |  LoggerLevel::info    |       "I"        |
/// |  LoggerLevel::trace   |       "T"        |
/// |  LoggerLevel::debug   |       "D"        |
/// |  LoggerLevel::warning |       "W"        |
/// |  LoggerLevel::error   |       "E"        |
/// |  LoggerLevel::fatal   |       "F"        |
/// \n Unknown values result in an empty string returned
///
/// @fn serenity::MsgLevelToString(LoggerLevel level)
/// @brief  Used to return the full string representation of the current message level
/// @details \n
/// |     Logger Level      |   Return Value   |
/// |  :----:               |      :----:      |
/// |  LoggerLevel::info    |      "Info"      |
/// |  LoggerLevel::trace   |     "Trace"      |
/// |  LoggerLevel::debug   |     "Debug"      |
/// |  LoggerLevel::warning |      "Warn"      |
/// |  LoggerLevel::error   |     "Error"      |
/// |  LoggerLevel::fatal   |     "Fatal"      |
/// \n Unknown values result in an empty string returned
///
///
/// @enum serenity::message_time_mode
/// @brief The time mode that should be used for the logger target.
/// @details This controls how the time is displayed when formatting the prepended text.
///
/// @namespace serenity
/// @brief This is the namespace that the entire library falls under.
///
/// @namespace serenity::experimental
/// @brief This namespace holds functions or features still being worked on.
///
/// @namespace serenity::SERENITY_LUTS
/// @brief This namespace holds the look-ups used in some of the formatting structs.
///
/// @namespace serenity::msg_details
/// @brief This namespace holds the classes that comprise mostly everything about a log message.
///
/// @namespace serenity::targets
/// @brief This namespace holds the classes in charge of where to output the log message to.
///
/// @var serenity::SERENITY_LUTS::allValidFlags
/// @brief This look-up array contains all the internal flags that are built-in.
/// @details The flags correspond to: \n
/// |   Flag   |                        What The Flag Represents                             |
/// |   :----: |                               :----:                                        |
/// |     a    | the string representation of a padded 12-hour clock's hour value            |
/// |     b    | the string representation of a padded numerical month value                 |
/// |     d    | the string representation of a padded numerical day value                   |
/// |     l    | the string representation of the one character log level                    |
/// |     n    | the string representation of the date in DDMMMYY format                     |
/// |     t    | the string representation of the wall clock time in a 12-hour format        |
/// |     w    | the unpadded string representation of the numerical weekday value           |
/// |     x    | the string representation of the shortened weekday name                     |
/// |     y    | the string representation of the shortened year in YY format                |
/// |     A    | the string representation of "AM" or "PM" dependant on the hour             |
/// |     B    | the string representation of the whole month name                           |
/// |     D    | the string representation of the date in MM/DD/YY format                    |
/// |     F    | the string representation of the date in YY-MM-DD format                    |
/// |     H    | the string representation of a padded hour on a 24-hour clock               |
/// |     L    | the string representation of the full log level                             |
/// |     M    | the string representation of a padded minute                                |
/// |     N    | the string representation of the Logger's name                              |
/// |     S    | the string representation of a padded second                                |
/// |     T    | the string representation of the 24-hour wall clock in HH:MM:SS format      |
/// |     X    | the string representation of the full Weekday name                          |
/// |     Y    | the string representation of the year in YYYY format format                 |
/// |     +    | the formatted message with platform-dependant end of line applied           |
///
/// @var serenity::SERENITY_LUTS::short_weekdays
/// @brief The short weekday names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_wkday variable
///
/// @var serenity::SERENITY_LUTS::long_weekdays
/// @brief The full weekday names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_wkday variable
///
/// @var serenity::SERENITY_LUTS::short_months
/// @brief The short month names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_mon variable
///
/// @var serenity::SERENITY_LUTS::long_months
/// @brief The full month names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_mon variable
///
/// @var serenity::SERENITY_LUTS::numberStr
/// @brief Padded numbers up to "99"
/// @details Primarily used in clock times, numerical days, months, and year values. Offers padding for "00" -  "09" by
/// default for formatting arguments that dictate numerical padding.
///
/// @var serenity::SERENITY_LUTS::line_ending
/// @brief This maps the platform detected to the end-of-line value that should be appended to the end of each log message.
/// @details This is still a work in progress in the sense of adding user-specified platform support, however, this supports
/// windows, linux, and mac by default.
///
/// @struct serenity::BackgroundThread
/// @brief Struct that holds the specific variables in charge of the background flush thread in file targets
/// @details This struct is in charge of initializing and cleaning up the flush thread background worker, protecting the
/// file during reads/writes, and providing a naive guard against always taking a lock on the file
/// @var serenity::BackgroundThread::cleanUpThreads
/// @brief Cleans up the flush thread when interval based flushing is disabled or when the program ends
/// @var serenity::BackgroundThread::flushThreadEnabled
/// @brief naive guard to protect from always taking the mutex lock when the flush thread isn't active
/// @var serenity::BackgroundThread::readWriteMutex
/// @brief protects the file during reads to write to disk and writing to the file handle
/// @var serenity::BackgroundThread::flushThread
/// @brief Involved in initializing and running the background thread
///
/// @struct serenity::FileSettings
/// @brief Struct that holds the basic file settings for file targets
/// @details This struct is in charge of caching the file path, file name, log directory, holding the buffer used for the
/// file handle, and the size of that file buffer
/// @var serenity::FileSettings::filePath
/// @brief used to hold the full path to the log file and is used for path caching and base file name caching in
/// regards to the RotatingTarget class.
/// @var serenity::FileSettings::fileBuffer
/// @brief a simple char buffer used by the file handle.
/// @var serenity::FileSettings::bufferSize
/// @brief used to set the buffer used by the file handle and its default size is equivalent to the
/// DEFAULT_BUFFER_SIZE macro.
///
/// @struct serenity::experimental::RotateSettings
/// @brief Struct that holds and controls different rotation settings specific to the RotatingTarget class.
/// @details This struct is in charge of the rotation settings for the RotateTarget class. Currently, this only involves
/// file size based rotation settings
/// @var serenity::experimental::RotateSettings::rotateOnFileSize
/// @brief Determines if file size rotation is enabled/disabled
/// @var serenity::experimental::RotateSettings::maxNumberOfFiles
/// @brief This can be any arbitrary number - determines how many files to create and rotate through.
/// @var serenity::experimental::RotateSettings::fileSizeLimit
/// @brief The upper limit of what the size each file should be before rotating to a new file
/// @fn serenity::experimental::RotateSettings::SetOriginalSettings(const std::filesystem::path &filePath)
/// @brief This function is in charge of caching the file path, its extension, its name, and the log directory.
/// @details If no user log directory is used, the log directory will be the directory where the file should be located. \n
/// i.e. If the path was users/desktop/Log.txt, then the log "directory" here would be the desktop location
/// @param filePath
/// @brief The full path to the file. This path can be an absolute or relative path.
/// @fn serenity::experimental::RotateSettings::SetCurrentFileSize(size_t currentSize)
/// @brief Sets the variable in charge of tracking current file size of the file context held.
/// @fn serenity::experimental::RotateSettings::OriginalPath()
/// @brief Returns a reference to the original full file path string
/// @fn serenity::experimental::RotateSettings::OriginalDirectory()
/// @brief Returns a reference to the original log directory path string
/// @fn serenity::experimental::RotateSettings::OriginalName()
/// @brief Returns the original base name of the file
/// @fn serenity::experimental::RotateSettings::OriginalExtension()
/// @brief Returns the originnal extension string
/// @fn serenity::experimental::RotateSettings::FileSize()
/// @brief Returns the current size of the file context via the manual tracking method (less expensive).
///
///
/// @def DB_PRINT( msg, ... )
/// @brief Used for internal debug printing and only enabled when not in a release build.
///
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                    ColorConsoleTarget.h Header Documentation                                            *
 **************************************************************************************************************************/
/// @file ColorConsoleTarget.h
/// @brief This file holds the class in charge of writing to the standard outputs and is terminal/pipe-aware.
///
/// @enum serenity::targets::console_interface
/// @brief This enum encapsulates values that mirror the standard outputs
/// @var serenity::targets::console_interface::std_out
/// @brief Mirrors std::out
/// @var serenity::targets::console_interface::std_err
/// @brief Mirrors std::cerr
/// @var serenity::targets::console_interface::std_log
/// @brief Mirrors std::clog
///
/// @class serenity::targets::ColorConsole
/// @brief This class is in charge of logging to the terminal and supports color logging and the ability to log without
/// color. \n This class inherits from the TargetBase class for common logging functions and logging settings.
/// @details For all Console Target Constructors: \n
/// - will set the console mode to "console_interface::std_out" \n
/// - Initializes the default colors to use for each message level \n
/// - If output is a terminal and hasn't been redirected and if the output handle is valid, enables color output \n
/// - Sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag if on Windows Platform \n
///
/// @fn serenity::targets::ColorConsole::ColorConsole()
/// @brief Default constructor that will set the logger name to "Console_Logger". All other settings will be set to
/// their defaults
///
/// @fn serenity::targets::ColorConsole::ColorConsole( std::string_view name )
/// @brief Constructor that will set the logger name to the parameter @p name passed in. All other settings will be
/// set to their defaults
/// @param name: the name that the logger itself will use and be identified by
///
/// @fn serenity::targets::ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
/// @brief Constructor that will set the logger name to the parameter @p name passed in and set the format pattern
/// via the parameter @p msgPattern passed in. All other settings will be set to their defaults
/// @param name: the name that the logger itself will use and be identified by
/// @param msgPattern: the format pattern that determines how the prepended text will be displayed before the log
/// message
///
/// @fn serenity::targets::ColorConsole::~ColorConsole( )
/// @brief If the output wasn't directed to a terminal, will flush the contents to disk.
/// @details If on Windows platform, this function will also reset the console mode to default in order to to clear
/// "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag.
///
/// @fn serenity::targets::ColorConsole::GetMsgColor( LoggerLevel level )
/// @brief Returns the log level based color for the current message to use
/// @param level: the level used to recieve the color code for that level. Logger levels are trace, info, debug,
/// warning, error, and fatal.
/// @returns The ansi color code for the @p level passed in
///
/// @fn serenity::targets::ColorConsole::SetMsgColor( LoggerLevel level, std::string_view color )
/// @brief Sets color specified for the log level specified
/// @param level: The logger level to bind the color code to from @p color variable
/// @param color: The color code to bind  to the @p level passed in. This color code is expected to be an ansi code.
///
/// @fn serenity::targets::ColorConsole::SetConsoleInterface( console_interface mode )
/// @brief Sets console mode. Console modes mirror standard outputs.
/// @details For Windows, sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING". (If not defined, a macro is used to define this
/// value). For any other platfrom, sets the standard output to use. \n
/// The console_interface values (std_out, std_err, std_log) represent std::out, std::err, and std::clog respectively.
/// @param mode: Can be one of the following: console_interface::std_out, console_interface::std_err, or
/// console_interface::std_log
///
/// @fn serenity::targets::ColorConsole::ConsoleInterface( )
/// @brief Returns the current console mode being used
///
/// @fn serenity::targets::ColorConsole::ColorizeOutput( bool colorize )
/// @brief Enables/Disables colored text output
///
/// @fn serenity::targets::ColorConsole::SetOriginalColors( )
/// @brief Initializes the default colors to use for the log levels
/// @details The default colors used are as follows: \n
/// | Logger Level |            Color Used            |
/// |    :----:    |             :----:               |
/// |     info     |      Bright Green On Black       |
/// |     trace    |     Bright White On Black        |
/// |     debug    |     Bright Cyan On Black         |
/// |     warning  |      Bright Yellow On Black      |
/// |     error    |     Basic Red On Black           |
/// |     fatal    |     Bright Yellow On Red         |
/// |      off     | Reset To Default Terminal Colors |
///
/// @fn serenity::targets::ColorConsole::IsTerminalType( )
/// @brief Checks to see if output handle is referring to a terminal type device or not
/// @returns True if output is terminal, false otherwise
///
/// @fn serenity::targets::ColorConsole::IsValidHandle( )
/// @brief Checks to see if output handle is valid and not empty
/// @returns True if handle to output isn't a nullptr, or if on Windows, that the output handle isn't INVALID_HANDLE_VALUE.
/// Returns false otherwise.
///
/// @fn serenity::targets::ColorConsole::PrintMessage( std::string_view formatted )
/// @brief Outputs the message (@p formatted) to the destination output device.
/// @details If output handle is valid, will write the message to the output. If color is enabled, will write the
/// message in the color specified for the log level used if the output is to a terminal. \n
/// For windows, uses WriteConsole( ) unless the output is not a terminal (in which case, uses WriteFile( ) instead ).
/// @param formatted: The actual message in its entirety to send to the output destination.
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                         FileTarget.h Header Documentation                                               *
 **************************************************************************************************************************/
/// @file FileTarget.h
/// @brief This file contains the FileTarget class which is in charge of all log operations to basic files.
///
/// @class serenity::targets::FileTarget
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
///
/// @fn serenity::targets::FileTarget::FileTarget()
/// @brief Default constructor that sets the logger name to "File_Target" and sets all other values to their defaults
///
/// @fn serenity::targets::FileTarget::FileTarget( std::string_view fileName, bool replaceIfExists = false )
/// @brief Constructor that will set the file name and will truncate the file if it already exists and if
/// "replaceIfExists" is set to true. Sets all other values to their defaults.
/// @details While the file name will be changed to the parameter @p fileName, the file itself will be created under
/// the default log directory.
/// @param fileName: The name of the file to create and/or use for logging
/// @param replaceIfExists: This value denotes whether or not to truncate the file if it exists already
///
/// @fn serenity::targets::FileTarget::FileTarget( std::string_view name, std::string_view filePath, bool
///  replaceIfExists = false )
/// @brief Constructor that will set the logger name, file path, log directory, and whether or not to truncate this
/// file if it exists or not. Sets all other values to their defaults.
/// @details The name of the logger will be set based on the @p name variable. The file path should include the log
/// directory and the actual file name in the @p filePath variable as this is how it's intended to be parsed. If the
/// file already exists and the @p replaceIFExists variable is set to true, then the file will be truncated before
/// opening for use in logging
/// @param name: The name the logger will be identified by.
/// @param filePath: the full path to the file. This value should include any directories as well as the file name
/// itself.
/// @param replaceIfExists: This value denotes whether or not to truncate the file if it exists already
///
/// @fn serenity::targets::FileTarget::FileTarget( std::string_view name, std::string_view formatPattern,
/// std::string_view filePath, bool replaceIfExists = false )
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
/// @param replaceIfExists: This value denotes whether or not to truncate the file if it exists already
///
/// @fn serenity::targets::FileTarget::~FileTarget( )
/// @brief Cleans up any background threads, flushes contents to disk, and closes the file context being used
/// @details Cleans up background flush thread if enabled by joining the thread and then flushing the contents of the
/// file handle to the file (if messages were written to buffer, will now write contents of buffer to file).
/// Afterwards, the file context being used is closed.
///
/// @fn serenity::targets::FileTarget::FilePath( )
/// @brief Returns the file path of the file context currently being held
///
/// @fn serenity::targets::FileTarget::FileName( )
/// @brief Returns the file name of the file context currently being held
///
/// @fn serenity::targets::FileTarget::EraseContents( )
///  @brief Closes the file context currently being held and re-opens the same file context, truncating the file size
///  to 0
///
/// @fn serenity::targets::FileTarget::RenameFile( std::string_view newFileName )
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
///
/// @fn serenity::targets::FileTarget::OpenFile( bool truncate = false )
///  @brief Opens the file held by the file handle
/// @details Opens the file context currently being held and sets the file buffer size using "DEFAULT_BUFFER_SIZE"
/// macro. If the file doessn't already exist, this function will create the file first.
/// @returns If successful, returns true. If unsuccessful, will return false with error message
///
/// @fn serenity::targets::FileTarget::CloseFile( )
/// @brief Joins any currently running flush background thread (if enabled) and if the file handle's file context is
/// currently open, will flush contents to disk and close the file.
/// @returns If successful, returns true. If unsuccessful, will return false with error message
///
/// @fn serenity::targets::FileTarget::Flush( )
/// @brief If contents were written to buffer, this will now write contents of buffer to file and
/// flush contents to disk, otherwise, just flushes contents to disk
///
/// @fn serenity::targets::FileTarget::PolicyFlushOn( )
/// @brief Executes the currently set flush policy
/// @details Compares current flush setting and executes that policy if active. Current policies are: always flush,
/// never flush, LogLevel-based flushing and time-based flushing. Time based flushing uses a background thread worker
/// which will intermittenly lock the file when elapsed time is reached from "flushEvery" setting, flush the contents
/// to disk, and then unlock the file for further writes
///
/// @fn serenity::targets::FileTarget::PrintMessage( std::string_view formatted )
/// @brief Writes the message passed in by the  @p formatted variable to the file context.
/// @details Checks if background flush thread is active, if it is - will lock access to the file for writing. If
/// rotate setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to
/// the file. If the file size would exceed the limit, closes the current file and rotates to next file before
/// writing the message. Will then follow any settings active in the flush policy
/// @param formatted: The actual message in its entirety to send to the output destination.
///
/// @var serenity::targets::FileTarget::fileHandle
/// @brief Protected file handle that holds the file context.
/// @details This variable is protected to avoid duplication if using RotatingTarget class due to it inheriting from this
/// class.
///
/// @var serenity::targets::FileTarget::fileOptions
/// @brief The structure this class and the derived class ( RotatingTarget ) use to keep track of basic file settings
/// @see FileSettings
///
/// @var serenity::targets::FileTarget::flushWorker
/// @brief The structure this class and the derived class ( RotatingTarget ) use to flush contents to disk on an interval
/// basis.
/// @see BackgroundThread
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                         FlushPolicy.h Header Documentation                                               *
 **************************************************************************************************************************/
///
/// @file FlushPolicy.h
/// @brief This file holds the enums Flush and PeriodicOptions, a struct PeriodicSettings, and the Flush_Policy class itself.
///
/// @enum serenity::experimental::Flush
/// @brief This enum encapsulates the values for how flushing should be done.
/// @var serenity::experimental::Flush::always
/// @brief This value is used to always flush contents after a log message is made
/// @var serenity::experimental::Flush::periodically
/// @brief This value is used to flush contents after a log message is made based on a threshold
/// @var serenity::experimental::Flush::never
/// @brief This value is used to never flush when a log message is made. This option will defer flushing until the handle's
/// buffer is full or when the destructor is called.
///
/// @enum serenity::experimental::PeriodicOptions
/// @brief This enum encapsulates the values for when flushing should be done if the serenity::experimental::Flush value of
/// periodically has been set.
/// @var serenity::experimental::PeriodicOptions::timeBased
/// @brief This value enables flushing on a set wall-clock interval (default value is 500ms).
/// @var serenity::experimental::PeriodicOptions::logLevelBased
/// @brief This value enables flushing once a message level threshold has been reached. (default value is
/// LoggerLevel::trace).
///
/// @struct serenity::experimental::PeriodicSettings
/// @brief This struct holds the actual values used for when the serenity::experimental::Flush value periodically is set and
/// is based on what the serenity::experimental::PeriodicOptions value enabled is.
/// @var serenity::experimental::PeriodicSettings::flushEvery
/// @brief This value is used to determine how often contents should be flushed if
/// serenity::experimental::PeriodicOption::timeBased is enabled. (Default Value is 500ms).
/// @var serenity::experimental::PeriodicSettings::flushOn
/// @brief This value is used to determine the message threshold that should be reached before contents are flushed if
/// serenity::experimental::PeriodicOptions::logLevelBased is enabled. (Default value is LoggerLevel::trace).
///
/// @class serenity::experimental::Flush_Policy
/// @brief This class controls when and how logging target contents should be flushed.
///
/// @fn serenity::experimental::Flush_Policy::Flush_Policy( Flush primaryOpt )
/// @brief This constructor will set the primary mode of flushing to use, however, all other settings are initialized to
/// their defaults.
/// @details Initializes to the following: \n
/// |           Option           |                 Value                |
/// |            :----:          |                 :----:               |
/// |      Primary Option        |            @p primaryOpt             |
/// |      Secondary Option      | PeriodicOptions::undef               |
/// |      Secondary Settings    | flushOn = LoggerLevel::trace         |
/// |                            | flushEvery = 500ms                   |
///
///
/// @fn serenity::experimental::Flush_Policy::Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt, PeriodicSettings
///  settings )
/// @brief This constructor will set all the relevant options and offers full control on how the flush policy is setup.
/// @details Initializes to the following: \n
/// |           Option           |                 Value                |
/// |            :----:          |                 :----:               |
/// |      Primary Option        |            @p primaryOpt             |
/// |      Secondary Option      |            @p secondaryOpt           |
/// |      Secondary Settings    | flushOn =  @p settings.flushOn       |
/// |                            | timeBased = @p settings.flushEvery   |
///
/// @fn serenity::experimental::Flush_Policy::Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt )
/// @brief This constructor sets up the primary and secondary flush options but leaves the periodic settings as their
/// defaults.
/// @details Initializes to the following: \n
/// |           Option           |                 Value                |
/// |            :----:          |                 :----:               |
/// |      Primary Option        |            @p primaryOpt             |
/// |      Secondary Option      |           @p secondaryOpt            |
/// |      Secondary Settings    | flushOn = LoggerLevel::trace         |
/// |                            | flushEvery = 500ms                   |
///
/// @fn serenity::experimental::Flush_Policy::Flush_Policy( PeriodicOptions secondaryOpt, PeriodicSettings settings )
/// @brief This constructoy will set both the secondary option and the periodic settings to use for that option.
/// @details Initializes to the following: \n
/// |           Option           |                 Value                       |
/// |            :----:          |                 :----:                      |
/// |      Primary Option        | Default Initialized to Flush::periodically  |
/// |      Secondary Option      |           @p secondaryOpt                   |
/// |      Secondary Settings    | flushOn =  @p settings.flushOn              |
/// |                            | timeBased = @p settings.flushEvery          |
///
/// @fn serenity::experimental::Flush_Policy::operator=( const Flush_Policy &p )
/// @brief copy assignment operator used in the copy constructor. Copies the Primary mode, Secondary mode, and
/// periodic settings from @p p.
///
/// @fn serenity::experimental::Flush_Policy::Flush_Policy( const Flush_Policy &p )
/// @brief Copy constructor that will intialize a Flush_Policy instance by copy-intializing options and settings from @p p.
///
/// @fn serenity::experimental::Flush_Policy::SetPrimaryMode( Flush primary )
/// @brief Changes the primary mode of the policy to @p primary.
///
/// @fn serenity::experimental::Flush_Policy:: SetSecondaryMode( PeriodicOptions secondary )
/// @brief Changes the secondary mode to @p secondary
///
/// @fn serenity::experimental::Flush_Policy::SetSecondarySettings( PeriodicSettings subSettings )
/// @brief Changes the setting values used to the values of @p subSettings
///
/// @fn serenity::experimental::Flush_Policy::PrimarySetting()
/// @brief Returns the primary mode in effect
///
/// @fn serenity::experimental::Flush_Policy::SubSetting()
/// @brief Returns the secondary setting in effect
///
/// @fn serenity::experimental::Flush_Policy::Policy()
/// @brief Returns the Flush_Policy instance being used
///
/// @fn serenity::experimental::Flush_Policy:: SecondarySettings()
/// @brief Returns the periodic settings in effect
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                         Message_Formatter.h Header Documentation                                        *
 **************************************************************************************************************************/
/// @file Message_Formatter.h
/// @brief This file holds functions related to how particular arguments and flags are formatted when logging.
///
/// @class serenity::msg_details::Message_Formatter
/// @brief Controls how the prepended string of text before the actual log message will be displayed and how each
/// flag/argument is formatted.
///
/// @fn serenity::msg_details::Message_Formatter::Message_Formatter( std::string_view pattern, Message_Info
/// *details )
/// @brief Constructor that takes in a format pattern paramater and a Message_Info class pointer.
/// @details The format pattern is set and internally stored by its individual flag arguments to iterate over when a
/// message is logged. The Message_Info class is used to populate the arguments or references when storing and using
/// the individual flag components
///
// *************************************** These are deleted or default ***********************************************
/// @fn serenity::msg_details::Message_Formatter::~Message_Formatter()
/// @fn serenity::msg_details::Message_Formatter::Message_Formatter()
/// @fn serenity::msg_details::Message_Formatter::Message_Formatter( const Message_Formatter & )
/// @fn serenity::msg_details::Message_Formatter::operator=( const Message_Info & )
// ********************************************************************************************************************
///
/// @struct serenity::msg_details::Message_Formatter::Formatter
/// @brief Virtual Base Struct that all formatter structs inherit from and implement
///
/// @fn serenity::msg_details::Message_Formatter::Formatter::Format()
/// @brief Each derived class or struct must implement this funtion. This function determines how an argument
/// will be formatted
///
/// @fn serenity::msg_details::Message_Formatter::Formatter::UpdateInternalView()
/// @brief A derived class or struct may implement this function. This function determines if an argument needs
/// to be updated if a derived class or struct implements some form of caching with argument values
///
/// @class serenity::msg_details::Message_Formatter::Formatters
/// @brief This class is the top-level formatting class that holds and controls each flag arguments specific
/// formatting abilities
///
/// @fn serenity::msg_details::Message_Formatter::Formatters::Formatters(
/// std::vector<std::unique_ptr<Formatter>> &&container )
/// @brief Constructor that takes ownership of the vector of Formatter pointers passed in.
///
// ****************************************** Default ***************************************
/// @fn serenity::msg_details::Message_Formatter::Formatters::Formatters()
// ******************************************************************************************
///
/// @fn serenity::msg_details::Message_Formatter::Formatters::Emplace_Back( std::unique_ptr<Formatter>
/// &&formatter )
/// @brief Takes ownership of the Formatter pointer passed in and stores this pointer in the internal Formatters
/// container.
///
/// @fn serenity::msg_details::Message_Formatter::Formatters::Format()
/// @brief Calls each Formatter pointer's specific Format() implementation that is stored in the internal
/// Formatter container in the order they are stored and returns the whole formatted string as a view
///
/// @fn serenity::msg_details::Message_Formatter::Formatters::Clear()
/// @brief Clears the internal Formatter pointers container
///
/// @fn serenity::msg_details::Message_Formatter::FlagFormatter( size_t flag )
/// @brief Intializes and stores the Formatter struct at the index provided into the Formatters container
///
/// @fn serenity::msg_details::Message_Formatter::SetPattern( std::string_view pattern )
/// @brief Sets the format pattern variable and then parses the format string to store each flag as its own
/// individual Formmater struct that will be in charge of how each flag is formatted
///
/// @fn serenity::msg_details::Message_Formatter::GetFormatters()
/// @brief Returns the Message_Formatter's instance of the Formatters container which holds the individual Formatter
/// pointers. Can be called to manually call the Format() function for all arguments stored
///
/// @fn serenity::msg_details::Message_Formatter::StoreFormat()
/// @brief Store the format pattern internally to be used more efficiently when formatting log messages
/// @details SetPattern() calls this function internally. Parses the internal format pattern string stored from
/// SetPattern() or from a constructor that took in a format pattern argument and for each flag found by the
/// delimiter "%", will try to match the potential flag to the index of the valid flags array. If a match is found,
/// initializes and stores the respective Formatter struct for that flag, otherwise, stores this value as well as any
/// other char, by passing in the value to initialize and store a Format_Arg_Char struct instead
///
/// @fn serenity::msg_details::Message_Formatter::MessageDetails()
/// @brief Returns a pointer to the Message_Info instance used by Message_Formatter
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                            Message_Info.h Header Documentation                                          *
 **************************************************************************************************************************/
/// @file Message_Info.h
/// @brief This file holds generic information about the log message and log target via the Message_Info class instance.
///
/// @class serenity::msg_details::Message_Info
/// @brief This class is primarily in charge of the message formatting and updating the time-related cache values, however,
/// this class also provides some utility functions involved with the message settings and some target related settings as
/// well.
///
/// @fn serenity::msg_details::Message_Info::Message_Info( std::string_view name, LoggerLevel level,
/// message_time_mode mode )
/// @brief Constructor that sets the internal representation to the target's name, level, and time mode used.
/// @details The constructor will also dictate via platform detection macros on what the line ending for each message should
/// be. \n
/// @see serenity::experimental::LineEnd, @see serenity::experimental::SERENITY_LUTS::line_ending
///
// ******************************************* Either Default or Deleted ***************************************
/// @fn serenity::msg_details::Message_Info::operator=( const Message_Info &t )
/// @fn serenity::msg_details::Message_Info::Message_Info( const Message_Info & )
/// @fn serenity::msg_details::Message_Info::Message_Info()
/// @fn serenity::msg_details::Message_Info::~Message_Info()
// *************************************************************************************************************
///
/// @fn serenity::msg_details::Message_Info::MsgLevel( )
/// @brief Returns a reference of the current message's level setting
/// @note This is different from the target's level setting. The message level setting is determined by the logging function
/// called.
///
/// @fn serenity::msg_details::Message_Info::Name()
/// @brief Returns a reference to the name value held internally.
///
/// @fn serenity::msg_details::Message_Info::TimeDetails()
/// @brief Returns a reference to the Message_Time class instance used.
///
/// @fn serenity::msg_details::Message_Info::SetName( const std::string_view name )
/// @brief Changes the internal view of the name value and is usually called by a target's SetName/Rename function
///
/// @fn serenity::msg_details::Message_Info::SetMsgLevel( const LoggerLevel level )
/// @brief Sets the message level of the log message and is generally called within the logging specific functions
///
/// @fn serenity::msg_details::Message_Info::MessageTimePoint()
/// @brief Returns the current message time point in terms of the system clock
///
/// @fn serenity::msg_details::Message_Info::SetTimeMode( const message_time_mode mode )
/// @brief Sets the time mode used and how the time-related cache variables are both intialized and updated.
///
/// @fn serenity::msg_details::Message_Info::TimeMode()
/// @brief Returns the time mode currently being used (whether local time or utc time).
///
/// @fn serenity::msg_details::Message_Info::TimeInfo()
/// @brief Returns the structure being used to cache the time-related values
///
/// @fn serenity::msg_details::Message_Info::Message()
/// @brief Returns the message with the platform-dependant line already applied, but without the prepended formatted text
/// (if any was present).
///
/// @fn serenity::msg_details::Message_Info::SetMessage( const std::string_view message, Args &&...args )
/// @brief Formats and then stores the message with the given arguments.
/// @details This function is generally called via the logging-specific functions from targets. \n
/// This function also is the one in charge of applying the platform-dependant end of line to each log message.
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                            Message_Time.h Header Documentation                                          *
 **************************************************************************************************************************/
///
/// @file Message_Time.h
/// @brief This file contains the Message_Time class and time-related functions
///
/// @class serenity::msg_details::Message_Time
/// @brief This class is in charge of caching the time-related values used in formatting the prepended text for a log
/// message.
///
// ********************************************* Default Or Deleted ***********************************
/// @fn serenity::msg_details::Message_Time::Message_Time()
///@fn serenity::msg_details::Message_Time::Message_Time( const Message_Time & )
/// @fn serenity::msg_details::Message_Time::operator=( const Message_Time & )
/// @fn serenity::msg_details::Message_Time::~Message_Time()
// ****************************************************************************************************
///
/// @fn serenity::msg_details::Message_Time::Message_Time( message_time_mode mode )
/// @brief Sets the time mode used in caching and other time-related functions from the @p mode variable and initializes the
/// cache.
///
/// @fn serenity::msg_details::Message_Time:: GetCurrentYearSV( int yearOffset, bool shortened = false )
/// @brief Takes the year offset value and returns either the year in YYYY format if @p shortened is set to false or in YY
/// format if @p shortened is set to true.
///
/// @fn serenity::msg_details::Message_Time::UpdateTimeDate( std::chrono::system_clock::time_point timePoint )
/// @brief Takes the current time point and returns the time-related values for that time point
///
/// @fn serenity::msg_details::Message_Time::UpdateCache( std::chrono::system_clock::time_point timePoint )
/// @brief Calls UpdateTimeDate() internally and uses the values returned to update the cache values
///
/// @fn serenity::msg_details::Message_Time::Cache()
/// @brief Returns the cache holding the time-related values
///
/// @fn serenity::msg_details::Message_Time::Mode()
/// @brief Returns the time mode currently being used to populate and update the cache and other time-related functions
///
/// @fn serenity::msg_details::Message_Time::SetTimeMode( message_time_mode mode )
/// @brief Sets the time mode to use in intializing/updating the cache and for other time-related functions (local time or
/// utc time).
///
/// @fn serenity::msg_details::Message_Time::LastLogPoint()
/// @brief Returns the cached time point in terms of seconds for when the last log message was made.
/// @details This function is used to determine if the cache should be updated based on a second time lapse
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                            RotatingTarget.h Header Documentation                                        *
 **************************************************************************************************************************/
/// @file RotatingTarget.h
/// @brief This file holds the RotatingTarget Class and rotation based functions used in logging to files.
///
/// @class serenity::experimental::targets::RotatingTarget
/// @brief This class is in charge of logging to any basic file type and handling the rotation of files up to a maximum
/// number of files set. This class inherits from the FileTarget class and, therefore, the TargetBase class for common
/// logging functions and logging settings.
///
/// @details For all Rotating Target Constructors: \n
/// - if either the directories or the file don't exist yet, the  *constructor will create the neccessary directories  \n
///   as well as the file needed to write to. \n
/// - For the constructors that don't take a file path variable, the logs will be written to a "Logs" directory in the \n
///   location that the app is being run from. \n
///
/// @fn serenity::experimental::targets::RotatingTarget::RotatingTarget()
/// @brief Default constructor that will set the logger name to "Rotating_Logger". All sets all other values to their
/// defaults.
/// @details The default constructor will also create a file named "Rotating_Log.txt" upon creation. This base name
/// and extension will be cached internally and the file will be renamed as Rotating_Log_01.txt and follow rotation
/// settings thereafter in the "Logs" directory of where the app is running from
///
/// @fn serenity::experimental::targets::RotatingTarget::RotatingTarget( std::string_view name, std::string_view filePath,
/// bool replaceIfExists = false )
/// @brief Constructor that sets the logger name, the file name, file path, and log directory off the @p filePath
/// variable, and will truncate the file if it already exists depending on the value of @p replaceIfExists
/// @details This constructor will cache the file name, the file path, the file extension, and the log directory off
/// of the @p filePath variable. The file will then be renamed for rotation - if the file already exists, it will
/// delete the oldest file with the base file name and rename the file using that file's index number.
/// @param name: the name that the logger itself will use and be identified by
/// @param filePath: the full path to the file to write to
/// @param replaceIfExists: this value will determine if the file is truncated upon being opened the first time
///
/// @fn serenity::experimental::targets::RotatingTarget::RotatingTarget( std::string_view name, std::string_view
///  formatPattern, std::string_view filePath, bool replaceIfExists = false )
/// @brief Constructor that sets the logger name, the format pattern to use, the file name, file path, and log
/// directory off the @p filePath variable, and will truncate the file if it already exists depending on the value of
/// @p replaceIfExists
/// @details This constructor will cache the file name, the file path, the file extension, and the log directory off
/// of the @p filePath variable. The file will then be renamed for rotation - if the file already exists, it will
/// delete the oldest file with the base file name and rename the file using that file's index number.
/// @param name: the name that the logger itself will use and be identified by
/// @param formatPattern:  the format pattern that determines how the prepended text will be displayed before the log
/// message
/// @param filePath: the full path to the file to write to
/// @param replaceIfExists: this value will determine if the file is truncated upon being opened the first time
///
// *********************************************** Deleted ******************************************
///  @fn serenity::experimental::targets::RotatingTarget::RotatingTarget( const RotatingTarget & )
///  @fn serenity::experimental::targets::RotatingTarget::operator=( const RotatingTarget & )
// **************************************************************************************************
///
/// @fn serenity::experimental::targets::RotatingTarget::~RotatingTarget( )
/// @brief Cleans up any background resources used and closes the file context currently held
/// @details When the deconstructor is called, will clean up background flush thread if enabled, flush the contents
/// of the file handle to the file (if messages were written to buffer, will now write contents of buffer to file),
/// and then close the file context
///
/// @fn serenity::experimental::targets::RotatingTarget::ShouldRotateFile( bool shouldRotate = true )
/// @brief This function takes in a boolean value that determines whether or not the file context currently held
/// should rotate when file size limit option has been reached
///
/// @fn serenity::experimental::targets::RotatingTarget::SetRotateSettings( RotateSettings settings )
/// @brief Sets the overall rotation settings for the logger in regards to the filecontext.
/// @details Current options revolve aroundfile size settings only and can be found in the RotateSettings class.
/// @param settings: controls the following: file size limit, number of files to rotate through, and whether or
/// not the logger should rotate through files up to the max number of files set
///
/// @fn serenity::experimental::targets::RotatingTarget::RenameFileForRotation()
/// @brief Sets up the base file given in constructor, or the base file after being renamed, for rotation.
/// @details This function will cache the file's name, extension, and path, as well as the log directory for the file
/// before renaming the current file to the first iteration of the log file to rotate through. (Example:
/// Rotate_Log.txt becomes Rotate_Log_01.txt). If this rotation-ready file already exists, will open the file by
/// overwriting its contents and truncating its size to 0.
///
/// @fn serenity::experimental::targets::RotatingTarget::RotateFileOnSize()
/// @brief This function controls how the file is rotated. If the logger should rotate, the file will be closed and
/// the next file in iteration up to the max file limit will be opened for writing.
/// @details If the logger should rotate, will close the current file, increment the file count up to max number of
/// files set, and try to open the next file in iteration. If file already exists, will remove the oldest file with
/// the given file name base before opening the next file in iteration. If the setting @p rotateFile is set to false,
/// this function will return and do nothing else.
///
/// @fn serenity::experimental::targets::RotatingTarget::RenameFile( std::string_view newFileName )
/// @brief Renames the current file to the name passed in via @p newFileName.
/// @details Will close the current file being written to and replace the old file name with the new file name given.
/// Previous files are unaffected. However, if cycling through rotation, future files will have this new name as
/// their base as well.
///
/// @fn serenity::experimental::targets::RotatingTarget::PrintMessage( std::string_view formatted )
/// @brief Writes the message to the currently held file context unless writing to the buffer was enabled - in which
/// case, this will write to the buffer instead.
/// @details Checks if background flush thread is active, if it is - will lock access to the file for writing. If
/// rotate setting is enabled, will check that the file size doesn't exceed file size limit and writes the message to
/// the file. If the file size would exceed the limit, closes the current file and rotates to next file before
/// writing the message. This function will then follow any settings active in the flush policy
/// @param formatted: The actual message in its entirety to send to the output destination.
//**************************************************************************************************************************

/***************************************************************************************************************************
 *                                               Target.h Header Documentation                                             *
 **************************************************************************************************************************/
///
/// @file Target.h
/// @brief This file contains the TargetBase class - the class all built-in targets inherit from for common shared functions.
///
/// @class serenity::targets::TargetBase
/// @brief The base class that derived target classes inherit from. Contains common functions between all targets for
/// message logging and logging settings.
/// @details For All TargetBase Constructors: \n
/// The following default values are set regardless of which constructor is used: \n
/// - Flush Policy is set to "never". \n
/// - Log Level is set to "LoggerLevel::trace". \n
/// - Default pattern is set to "|%l| %x %n %T [%N]: %+" which translates to the form of: \n
///   "|T| Sat 29Jan22 [Base_Logger]: The message to log". \n
/// - Time mode used is set to "time_mode::local".\n
///
/// @fn serenity::targets::TargetBase::TargetBase()
/// @brief Default constructor that sets the logger name to "Base_Target" and sets all other values to their defaults
///
/// @fn serenity::targets::TargetBase::TargetBase( std::string_view name )
/// @brief Constructor that will set the logger name to the name paramater passed in and all other values to their
/// defaults.
/// @param name: the name that the logger itself will use and be identified by
///
/// @fn serenity::targets::TargetBase::TargetBase( std::string_view name, std::string_view msgPattern )
/// @brief Constructor that will set the logger name to the name paramater passed in as well as the format pattern to
/// the msgPattern passed in. All other values will be assigned their default values.
/// @param name: the name that the logger itself will use and be identified by
/// @param msgPattern: the format pattern that determines how the prepended text will be displayed before the log
/// message
///
/// @fn serenity::targets::TargetBase::SetFlushPolicy( Flush_Policy pPolicy )
/// @brief Sets the current policy in use to defer to the policy passed in from "pPolicy".
/// @param pPolicy: refers to any settings that are added or changed by the user, including whether to flush always,
/// never, or periodically.
/// @details On top of having options to never flush, always flush, and periodically flush, the Flush_Policy class
/// includes the settings for whether flushing should occur based on a time-interval or log level if the periodical
/// flushing option is enabled.
///
/// @fn serenity::targets::TargetBase::Policy()
/// @brief Returns the current policy in use
///
/// @fn serenity::targets::TargetBase::LoggerName()
/// @brief Returns the logger's name
///
/// @fn serenity::targets::TargetBase::SetPattern( std::string_view pattern )
/// @brief Calls the handle to the Message_Formatter's SetPattern( ) function.
/// @details 	Sets the format pattern variable and parses the format string for internal storage and usage of this
/// pattern by initializing and moving the initialized formatter struct in charge of the respective flag to a
/// Formatters container
/// @param pattern - the format pattern to store. This pattern is what determines how the prepended text will be
/// displayed before the log message
///
/// @fn serenity::targets::TargetBase::ResetPatternToDefault()
/// @brief Resets the current format pattern in use to the default format pattern
///
/// @fn serenity::targets::TargetBase::SetLogLevel( LoggerLevel level )
/// @brief Sets the log level that messages should be logged at.
/// @details For example, if "SetLogLevel(LoggerLevel::Error);"
/// is used, then no messages below LoggerLevel::Error will be logged, however once a Fatal or Error message is made,
/// then they would be logged to the output destination
/// @param level - the logger level threshold that will determine if a message should be logged or not
///
/// @fn serenity::targets::TargetBase::WriteToBaseBuffer( bool fmtToBuf = true )
/// @brief Enables/Disables writing to a buffer.
/// @details When writing to the buffer, messages will be appended with the
/// platform-specific end of line before being added to the buffer. When Flush( ) is called, if writing to the buffer
/// was enabled, the buffer contents will now be written to the file and then flushed to disk. Disabled by default.
/// @param fmtToBuf: the value that controls whether or not buffer writes are enabled/disabled
///
/// @fn serenity::targets::TargetBase::isWriteToBuf()
/// @brief Returns true if buffer writes are enabled and false if they are disabled
///
/// @fn serenity::targets::TargetBase::Buffer()
/// @brief Returns a pointer to the buffer container
///
/// @fn serenity::targets::TargetBase::Level()
/// @brief Returns the current log level setting (the threshold of whether to log a message or not).
///
/// @fn serenity::targets::TargetBase::SetLoggerName( std::string_view name )
/// @brief Sets the name of the logger
/// @param name: the name that the logger itself will use and be identified by
///
/// @fn serenity::targets::TargetBase::Trace( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::trace setting
/// @details Checks if the message should be logged via the log level threshold setting. If it isn't, immediately
/// returns. If the message should be logged, this function will then perform a quick check on whether or not writes
/// to the buffer were enabled and will write to the buffer if they were. If writes to the buffers weren't enabled,
/// then PrintMessage( ) will be called and the derived target handles how this is implemented. After writing the
/// message, follows the derived target's PolicyFlushOn( ) implementation
/// @tparam msg: The message being passed in. Follows C++20's format library and libfmt's substitution model using
/// "{}" to replace any arguments from the \p args parameter.
/// @tparam args: Variadic placeholder for any number of and any type of arguments to use in substituion.
///
/// @fn serenity::targets::TargetBase::Info( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::info setting
/// @details @copydetails serenity::targets::TargetBase::Trace()
///
/// @fn serenity::targets::TargetBase::Debug( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::debug setting
/// @details @copydetails serenity::targets::TargetBase::Trace()
///
/// @fn serenity::targets::TargetBase::Warn( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::warning setting
/// @details @copydetails serenity::targets::TargetBase::Trace()
///
/// @fn serenity::targets::TargetBase::Error( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::error setting
/// @details @copydetails serenity::targets::TargetBase::Trace()
///
/// @fn serenity::targets::TargetBase::Fatal( std::string_view msg, Args &&...args )
/// @brief Logs a message giving the message a LoggerLevel::fatal setting
/// @details @copydetails serenity::targets::TargetBase::Trace()
///
/// @fn serenity::targets::TargetBase::PrintMessage( std::string_view formatted )
/// @brief Pure virtual function that all derived classes must implement and is used to write the formatted
///  message to a specific target.
/// @param formatted: The actual message in its entirety to send to the output destination.
/// @details For built in targets, the message is formatted before reaching this point by calling Message_Info's
/// SetMessage( ) function. PrintMessage( )'s intended usage is the last stop call before a log message is sent to
/// its destination
///
/// @fn serenity::targets::TargetBase::PolicyFlushOn()
/// @brief Virtual function that can be omitted if derived class has no need to implement and is used to handle
/// how and when the derived target should flush its contents to disk.
///
/// @fn serenity::targets::TargetBase::MsgFmt()
/// @brief Returns a pointer to the handle for the Message_Formatter class instance
///
/// @fn serenity::targets::TargetBase::MsgInfo()
/// @brief Returns a pointer to the handle for the Message_Info class instance
//**************************************************************************************************************************
