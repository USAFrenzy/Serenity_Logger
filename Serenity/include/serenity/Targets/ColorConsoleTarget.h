#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <fstream>

namespace serenity::targets {
	enum class console_interface
	{
		std_out,
		std_err,
		std_log,
	};

	class ColorConsole: public TargetBase
	{
	  public:
		ColorConsole();
		explicit ColorConsole(std::string_view name);
		explicit ColorConsole(std::string_view name, std::string_view msgPattern);
		~ColorConsole();

		std::string_view GetMsgColor(LoggerLevel level);
		void SetMsgColor(LoggerLevel level, std::string_view color);
		void SetConsoleInterface(console_interface mode);
		const console_interface ConsoleInterface();
		void ColorizeOutput(bool colorize);
		void SetOriginalColors();
		bool IsTerminalType();
		bool IsValidHandle();
		void SetLocale(const std::locale& loc) override;

	  protected:
		void PrintMessage(std::string_view formatted) override;

	  private:
		bool coloredOutput;
		console_interface consoleMode;
		std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
#ifdef WINDOWS_PLATFORM
		HANDLE outputHandle;
#else
		FILE* outputHandle;
#endif    // WINDOWS_PLATFORM
		std::mutex consoleMutex;
		std::string message;
	};    // class ColorConsole

}    // namespace serenity::targets
