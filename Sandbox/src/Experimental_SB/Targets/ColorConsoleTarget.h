#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <fstream>

namespace serenity::expiremental::targets
{
	///
	/// @brief Mirrors the standard outputs: std::out, std::err, std::clog
	///
	enum class console_interface
	{
		std_out,
		std_err,
		std_log,
	};
	/// @brief This class is in charge of logging to the terminal and supports color logging and the ability to log without
	/// color. This class inherits from the TargetBase class for common logging functions and logging settings.
	/// @details For all Console Target Constructors: \n
	/// - will set the console mode to "console_interface::stdout" \n
	/// - Initializes the default colors to use for each message level \n
	/// - If output is a terminal and hasn't been redirected and if the output handle is valid, enables color output \n
	/// - Sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag if on Windows Platform \n
	class ColorConsole : public TargetBase
	{
	  public:
		/// @brief Default constructor that will set the logger name to "Console_Logger". All other settings to their
		/// defaults
		ColorConsole( );
		/// @brief Constructor that will set the logger name to the parameter @p name passed in. All other settings to their
		/// defaults
		/// @param name: the name that the logger itself will use and be identified by
		explicit ColorConsole( std::string_view name );
		/// @brief Constructor that will set the logger name to the parameter @p name passed in and set the format pattern
		/// via the parameter @p msgPattern passed in. All other settings to their defaults
		/// @param name: the name that the logger itself will use and be identified by
		/// @param msgPattern: the format pattern that determines how the prepended text will be displayed before the log
		/// message
		explicit ColorConsole( std::string_view name, std::string_view msgPattern );
		///  @brief If the output wasn't directed to a terminal, will flush the contents.
		/// @details If on Windows platform, this function will also reset the console mode to default in order to to clear
		/// "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag.
		~ColorConsole( );
		/// @brief Returns the log level based color for the current message to use
		/// @param level: the level used to recieve the color code for that level. Logger levels are trace, info, debug,
		/// warning, error, and fatal.
		/// @returns The ansi color code for the @p level passed in
		std::string_view GetMsgColor( LoggerLevel level );
		/// @brief Sets color specified for the log level specified
		/// @param level: The logger level to bind the color code to from @p color variable
		/// @param color: The color code to bind  to the @p level passed in. This color code is expected to be an ansi code.
		void SetMsgColor( LoggerLevel level, std::string_view color );
		/// @brief Sets console mode. Console modes mirror standard outputs.
		/// @details For Windows, sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING". (If not defined, a macro is used to define this
		/// value). For any other platfrom, sets the standard output to use. The console_interface values (std_out, std_err,
		/// std_log) represent std::out, std::err, and std::clog respectively.
		/// @param mode: Can be one of the following: console_interface::std_out, console_interface::std_err, or
		/// console_interface::std_log
		void SetConsoleInterface( console_interface mode );
		///
		///  @brief Returns the current console mode being used
		///
		const console_interface ConsoleInterface( );
		///
		///  @brief Enables/Disables colored text output
		///
		void ColorizeOutput( bool colorize );
		///
		///  @brief Initializes the default colors to use for log levels
		///
		void SetOriginalColors( );
		///
		///  @brief Checks to see if output handle is referring to a terminal type device or not
		///
		bool IsTerminalType( );
		///
		///  @brief Checks to see if output handle is valid and not empty
		///
		bool IsValidHandle( );
		/// @brief Outputs the message (@p formatted) to the destination output device.
		/// @details If output handle is valid, will write the message to the output. If color is enabled, will write the
		/// message in the color specified for the log level used if the output is to a terminal. For windows, uses
		/// WriteConsole( ) unless the output is not a terminal (in which case, uses WriteFile( ) )
		/// @param formatted: The actual message in its entirety to send to the output destination.
		void PrintMessage( std::string_view formatted ) override;

	  private:
		bool                                              coloredOutput;
		console_interface                                 consoleMode;
		std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
#ifdef WINDOWS_PLATFORM
		HANDLE outputHandle;
#else
		FILE *outputHandle;
#endif  // WINDOWS_PLATFORM

	};  // class ColorConsole

}  // namespace serenity::expiremental::targets
