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
/// @enum serenity::expiremental::LineEnd
/// @brief The platform-dependant line ending that should be used.
/// @details These values are used by the line_ending unorderd map to map the enum value to the corresponding end of line to
/// append to each log message. Both this enum and the line_ending map are used automatically via the platform detection
/// macros when formatting the log message.
/// @var serenity::expiremental::LineEnd::linux
/// @brief Used to map to line_ending value "\n"
/// @var serenity::expiremental::LineEnd::windows
/// @brief Used to map to line_ending value "\r\n"
/// @var serenity::expiremental::LineEnd::mac
/// @brief Used to map to line_ending value "\r"
///
/// @enum serenity::expiremental::LoggerLevel
/// @brief This enum encapsulates the logging level features and the actual logger's level
/// @details This enum is used in formatting, on checking if the logger should log, whether or not the logger should flush
/// based on the level set, setting the message level, etc...
///
/// @fn serenity::expiremental::MsgLevelToShortString(LoggerLevel level)
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
/// @fn serenity::expiremental::MsgLevelToString(LoggerLevel level)
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
/// @enum serenity::expiremental::message_time_mode
/// @brief The time mode that should be used for the logger target.
/// @details This controls how the time is displayed when formatting the prepended text.
///
/// @namespace serenity
/// @brief This is the namespace that the entire library falls under.
/// @details This namespace includes: \n
/// - The experimental namespace, where features still being worked on live. \n
/// - The SERENITY_LUTS namespace, where all the look-up array tables live. \n
/// - The msg_details namespace, where Message_Formatter, Message_Time, and Message_Info live. \n
/// - The targets namespace, where the individual logging targets live. \n
///
/// @var serenity::expiremental::SERENITY_LUTS::allValidFlags
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
/// @var serenity::expiremental::SERENITY_LUTS::short_weekdays
/// @brief The short weekday names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_wkday variable
///
/// @var serenity::expiremental::SERENITY_LUTS::long_weekdays
/// @brief The full weekday names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_wkday variable
///
/// @var serenity::expiremental::SERENITY_LUTS::short_months
/// @brief The short month names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_mon variable
///
/// @var serenity::expiremental::SERENITY_LUTS::long_months
/// @brief The full month names used in formatting
/// @details Used by this library by indexing into this array via the std::tm* struct's tm_mon variable
///
/// @var serenity::expiremental::SERENITY_LUTS::numberStr
/// @brief Padded numbers up to "99"
/// @details Primarily used in clock times, numerical days, months, and year values. Offers padding for "00" -  "09" by
/// default for formatting arguments that dictate numerical padding.
///
/// @var serenity::expiremental::SERENITY_LUTS::line_ending
/// @brief This maps the platform detected to the end-of-line value that should be appended to the end of each log message.
/// @details This is still a work in progress in the sense of adding user-specified platform support, however, this supports
/// windows, linux, and mac by default.
///
/// @struct serenity::expiremental::BackgroundThread
/// @brief Struct that holds the specific variables in charge of the background flush thread in file targets
/// @details This struct is in charge of initializing and cleaning up the flush thread background worker, protecting the
/// file during reads/writes, and providing a naive guard against always taking a lock on the file
/// @var serenity::expiremental::BackgroundThread::cleanUpThreads
/// @brief Cleans up the flush thread when interval based flushing is disabled or when the program ends
/// @var serenity::expiremental::BackgroundThread::flushThreadEnabled
/// @brief naive guard to protect from always taking the mutex lock when the flush thread isn't active
/// @var serenity::expiremental::BackgroundThread::readWriteMutex
/// @brief protects the file during reads to write to disk and writing to the file handle
/// @var serenity::expiremental::BackgroundThread::flushThread
/// @brief Involved in initializing and running the background thread
///
/// @struct serenity::expiremental::FileSettings
/// @brief Struct that holds the basic file settings for file targets
/// @details This struct is in charge of caching the file path, file name, log directory, holding the buffer used for the
/// file handle, and the size of that file buffer
/// @var serenity::expiremental::FileSettings::filePath
/// @brief used to hold the full path to the log file and is used for path caching and base file name caching in
/// regards to the RotatingTarget class.
/// @var serenity::expiremental::FileSettings::fileBuffer
/// @brief a simple char buffer used by the file handle.
/// @var serenity::expiremental::FileSettings::bufferSize
/// @brief used to set the buffer used by the file handle and its default size is equivalent to the
/// DEFAULT_BUFFER_SIZE macro.
///
/// @struct serenity::expiremental::RotateSettings
/// @brief Struct that holds and controls different rotation settings specific to the RotatingTarget class.
/// @details This struct is in charge of the rotation settings for the RotateTarget class. Currently, this only involves
/// file size based rotation settings
/// @var serenity::expiremental::RotateSettings::rotateOnFileSize
/// @brief Determines if file size rotation is enabled/disabled
/// @var serenity::expiremental::RotateSettings::maxNumberOfFiles
/// @brief This can be any arbitrary number - determines how many files to create and rotate through.
/// @var serenity::expiremental::RotateSettings::fileSizeLimit
/// @brief The upper limit of what the size each file should be before rotating to a new file
/// @fn serenity::expiremental::RotateSettings::SetOriginalSettings(const std::filesystem::path &filePath)
/// @brief This function is in charge of caching the file path, its extension, its name, and the log directory.
/// @details If no user log directory is used, the log directory will be the directory where the file should be located. \n
/// i.e. If the path was users/desktop/Log.txt, then the log "directory" here would be the desktop location
/// @param filePath
/// @brief The full path to the file. This path can be an absolute or relative path.
/// @fn serenity::expiremental::RotateSettings::SetCurrentFileSize(size_t currentSize)
/// @brief Sets the variable in charge of tracking current file size of the file context held.
/// @fn serenity::expiremental::RotateSettings::OriginalPath()
/// @brief Returns a reference to the original full file path string
/// @fn serenity::expiremental::RotateSettings::OriginalDirectory()
/// @brief Returns a reference to the original log directory path string
/// @fn serenity::expiremental::RotateSettings::OriginalName()
/// @brief Returns the original base name of the file
/// @fn serenity::expiremental::RotateSettings::OriginalExtension()
/// @brief Returns the originnal extension string
/// @fn serenity::expiremental::RotateSettings::FileSize()
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
/// @enum serenity::expiremental::targets::console_interface
/// @brief This enum encapsulates values that mirror the standard outputs
/// @var serenity::expiremental::targets::console_interface::std_out
/// @brief Mirrors std::out
/// @var serenity::expiremental::targets::console_interface::std_err
/// @brief Mirrors std::cerr
/// @var serenity::expiremental::targets::console_interface::std_log
/// @brief Mirrors std::clog
///
/// @class serenity::expiremental::targets::ColorConsole
/// @brief This class is in charge of logging to the terminal and supports color logging and the ability to log without
/// color. \n This class inherits from the TargetBase class for common logging functions and logging settings.
/// @details For all Console Target Constructors: \n
/// - will set the console mode to "console_interface::std_out" \n
/// - Initializes the default colors to use for each message level \n
/// - If output is a terminal and hasn't been redirected and if the output handle is valid, enables color output \n
/// - Sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag if on Windows Platform \n
///
/// @fn serenity::expiremental::targets::ColorConsole::ColorConsole()
/// @brief Default constructor that will set the logger name to "Console_Logger". All other settings will be set to
/// their defaults
///
/// @fn serenity::expiremental::targets::ColorConsole::ColorConsole( std::string_view name )
/// @brief Constructor that will set the logger name to the parameter @p name passed in. All other settings will be
/// set to their defaults
/// @param name: the name that the logger itself will use and be identified by
///
/// @fn serenity::expiremental::targets::ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
/// @brief Constructor that will set the logger name to the parameter @p name passed in and set the format pattern
/// via the parameter @p msgPattern passed in. All other settings will be set to their defaults
/// @param name: the name that the logger itself will use and be identified by
/// @param msgPattern: the format pattern that determines how the prepended text will be displayed before the log
/// message
///
/// @fn serenity::expiremental::targets::ColorConsole::~ColorConsole( )
/// @brief If the output wasn't directed to a terminal, will flush the contents to disk.
/// @details If on Windows platform, this function will also reset the console mode to default in order to to clear
/// "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag.
///
/// @fn serenity::expiremental::targets::ColorConsole::GetMsgColor( LoggerLevel level )
/// @brief Returns the log level based color for the current message to use
/// @param level: the level used to recieve the color code for that level. Logger levels are trace, info, debug,
/// warning, error, and fatal.
/// @returns The ansi color code for the @p level passed in
///
/// @fn serenity::expiremental::targets::ColorConsole::SetMsgColor( LoggerLevel level, std::string_view color )
/// @brief Sets color specified for the log level specified
/// @param level: The logger level to bind the color code to from @p color variable
/// @param color: The color code to bind  to the @p level passed in. This color code is expected to be an ansi code.
///
/// @fn serenity::expiremental::targets::ColorConsole::SetConsoleInterface( console_interface mode )
/// @brief Sets console mode. Console modes mirror standard outputs.
/// @details For Windows, sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING". (If not defined, a macro is used to define this
/// value). For any other platfrom, sets the standard output to use. \n
/// The console_interface values (std_out, std_err, std_log) represent std::out, std::err, and std::clog respectively.
/// @param mode: Can be one of the following: console_interface::std_out, console_interface::std_err, or
/// console_interface::std_log
///
/// @fn serenity::expiremental::targets::ColorConsole::ConsoleInterface( )
/// @brief Returns the current console mode being used
///
/// @fn serenity::expiremental::targets::ColorConsole::ColorizeOutput( bool colorize )
/// @brief Enables/Disables colored text output
///
/// @fn serenity::expiremental::targets::ColorConsole::SetOriginalColors( )
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
/// @fn serenity::expiremental::targets::ColorConsole::IsTerminalType( )
/// @brief Checks to see if output handle is referring to a terminal type device or not
/// @returns True if output is terminal, false otherwise
///
/// @fn serenity::expiremental::targets::ColorConsole::IsValidHandle( )
/// @brief Checks to see if output handle is valid and not empty
/// @returns True if handle to output isn't a nullptr, or if on Windows, that the output handle isn't INVALID_HANDLE_VALUE.
/// Returns false otherwise.
///
/// @fn serenity::expiremental::targets::ColorConsole::PrintMessage( std::string_view formatted )
/// @brief Outputs the message (@p formatted) to the destination output device.
/// @details If output handle is valid, will write the message to the output. If color is enabled, will write the
/// message in the color specified for the log level used if the output is to a terminal. \n
/// For windows, uses WriteConsole( ) unless the output is not a terminal (in which case, uses WriteFile( ) instead ).
/// @param formatted: The actual message in its entirety to send to the output destination.
//**************************************************************************************************************************
