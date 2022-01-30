#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <fstream>

namespace serenity::expiremental::targets
{
	/// Mirrors the standard outputs: std::out, std::err, std::clog
	enum class console_interface
	{
		std_out,
		std_err,
		std_log
	};
	/// @brief This class is in charge of logging to the terminal and supports color logging and the ability to log without
	/// color. This class inherits from the TargetBase class for common logging functions and logging settings.
	class ColorConsole : public TargetBase
	{
	  public:
		// clang-format off
		/*************************************************************************************************************//**
	    *                                For all Console Target Constructors:
		* - will set the console mode to "console_interface::stdout"
		* - Initializes the default colors to use for each message level
		* - If output is a terminal and hasn't been redirected and if the output handle is valid, enables color output
		* - Sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag if on Windows Platform
        ******************************************************************************************************************/
		// clang-format on

		ColorConsole( );
		explicit ColorConsole( std::string_view name );
		explicit ColorConsole( std::string_view name, std::string_view msgPattern );
		///  If the output wasn't directed to a terminal, will flush the contents.
		///  If Windows, also resets the console mode to default to clear "ENABLE_VIRTUAL_TERMINAL_PROCESSING" flag
		~ColorConsole( );
		///  Returns the log level based color for the current message to use
		std::string_view GetMsgColor( LoggerLevel level );
		///  Sets color specified for the log level specified
		void SetMsgColor( LoggerLevel level, std::string_view color );
		/// Sets console mode. Console modes mirror standard outputs.
		/// For Windows, sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING". (If not defined, a macro is used to define this value).
		/// Parameter "mode": Can be one of the following: console_interface::std_out, console_interface::std_err, or
		/// console_interface::std_log
		void SetConsoleInterface( console_interface mode );
		///  Returns the current console mode being used
		const console_interface ConsoleInterface( );
		///  Enables/Disables colored text output
		void ColorizeOutput( bool colorize );
		///  Initializes the default colors to use for log levels
		void SetOriginalColors( );
		///  Checks to see if output handle is referring to a terminal type device or not
		bool IsTerminalType( );
		///  Checks to see if output handle is valid and not empty
		bool IsValidHandle( );
		/// If output handle is valid, will write the message to the output. If color is enabled, will write the message in
		/// the color specified for the log level used if the output is to a terminal. For windows, uses WriteConsole( )
		/// unless the output is not a terminal (in which case, uses WriteFile( ) )
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
