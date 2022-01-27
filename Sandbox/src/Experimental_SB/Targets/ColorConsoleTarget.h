#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <fstream>

namespace serenity::expiremental::targets
{
	enum class console_interface
	{
		std_out,
		std_err,
		std_log
	};

	class ColorConsole : public TargetBase
	{
	  public:
		/// <summary>
		/// Default Constructor that automatically sets logger name to "Console_Logger", the console mode to std::out by default, Sets
		/// the colors to use for log levels, and determines if colors should be used dependant on whether or not a valid output handle
		/// is set and the output is to a terminal type interface
		/// </summary>
		ColorConsole( );
		/// <summary>
		/// Constructor that takes in a logger name to use, automatically sets the console mode to std::out by default, Sets the colors
		/// to use for log levels, and determines if colors should be used dependant on whether or not a valid output handle is set and
		/// the output is to a terminal type interface
		/// </summary>
		explicit ColorConsole( std::string_view name );
		/// <summary>
		/// Constructor that takes in a logger name to use, a format pattern for how the messages should be formatted, automatically
		/// sets the console mode to std::out by default, Sets the colors to use for log levels, and determines if colors should be
		/// used dependant on whether or not a valid output handle is set and the output is to a terminal type interface
		/// </summary>
		explicit ColorConsole( std::string_view name, std::string_view msgPattern );
		/// <summary> If the output wasn't directed to a terminal, will flush the contents. If Windows, also resets the console mode to
		/// default to clear "ENABLE_VIRTUAL_TERMINAL_PROCESSING"  </summary>
		~ColorConsole( );
		/// <summary> Returns the log level based color for the current message to use </summary>
		std::string_view GetMsgColor( LoggerLevel level );
		/// <summary> Sets color specified for the log level specified</summary>
		void SetMsgColor( LoggerLevel level, std::string_view color );
		/// <summary>
		/// Sets console mode. Console modes mirror standard outputs. For Windows, sets "ENABLE_VIRTUAL_TERMINAL_PROCESSING".
		/// (If not defined, a macro is used to define this value).
		/// </summary>
		/// <param name="mode: "> Can be one of the following: console_interface::std_out, console_interface::std_err, or
		/// console_interface::std_log</param>
		void SetConsoleInterface( console_interface mode );
		/// <summary> Returns the current console mode being used </summary>
		const console_interface ConsoleInterface( );
		/// <summary> Enables/Diables colored text output </summary>
		void ColorizeOutput( bool colorize );
		/// <summary> Initializes the default colors to use for log levels </summary>
		void SetOriginalColors( );
		/// <summary> Checks to see if output handle is referring to a terminal type device or not </summary>
		bool IsTerminalType( );
		/// <summary> Checks to see if output handle is valid and not empty </summary>
		bool IsValidHandle( );
		/// <summary>
		/// If output handle is valid, will write the message to the output. If color is enabled, will write the message
		/// in the color specified for the log level used if the output is to a terminal. For windows, uses WriteConsole( ) unless the
		/// output is not a terminal (in which case, uses WriteFile( ) )
		/// </summary>
		/// <param name="formatted"></param>
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
