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
		ColorConsole( );
		ColorConsole( std::string_view name );
		ColorConsole( std::string_view name, std::string_view msgPattern );
		~ColorConsole( );
		std::string_view  GetMsgColor( LoggerLevel level );
		void              SetMsgColor( LoggerLevel level, std::string_view color );
		void              SetConsoleInterface( console_interface mode );
		console_interface ConsoleInterface( );
		void              ColorizeOutput( bool colorize );
		void              SetOriginalColors( );
		// *************************************** WIP ***************************************
		bool IsTerminalType( );
		void InitTerminalOutputs( );
		void RedirectOutput( console_interface mode, std::string_view dest );
		void ResetOutputBuffer( console_interface mode );
		// *************************************** WIP ***************************************

	  protected:
		void PrintMessage( std::string_view formatted ) override;
		bool IsValidHandle( );

	  private:
		bool                                              coloredOutput;
		console_interface                                 consoleMode;
		std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
#ifdef WINDOWS_PLATFORM
		HANDLE outputHandle;
#else
		FILE *outputHandle;
#endif  // WINDOWS_PLATFORM
		std::ofstream                                               stdoutHandle;
		std::ofstream                                               stderrHandle;
		std::unordered_map<console_interface, std::streambuf *>     terminalBuff;
		std::vector<std::pair<console_interface, std::streambuf *>> redirectionBackup;
	};  // class ColorConsole
}  // namespace serenity::expiremental::targets
