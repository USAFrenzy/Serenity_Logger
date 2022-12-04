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

	class ConsoleTarget: public TargetBase
	{
	  public:
		ConsoleTarget();
		explicit ConsoleTarget(utf_utils::InputSource name);
		explicit ConsoleTarget(utf_utils::InputSource name, utf_utils::InputSource msgPattern);
		~ConsoleTarget();

		std::string_view GetMsgColor(LoggerLevel level);
		void SetMsgColor(LoggerLevel level, utf_utils::InputSource color);
		void SetConsoleInterface(console_interface mode);
		const console_interface ConsoleInterface();
		void ColorizeOutput(bool colorize);
		void SetOriginalColors();
		bool IsTerminalType();
		bool IsValidHandle();
		void SetLocale(const std::locale& loc) override;

	  protected:
		void PrintMessage() override;

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
	};    // class ConsoleTarget

}    // namespace serenity::targets
