#include <serenity/Targets/ColorConsoleTarget.h>

namespace serenity::targets {
	ColorConsole::ColorConsole(): TargetBase("Console Logger"), consoleMode(console_interface::std_out), coloredOutput(false) {
		TargetHelper()->Policy()->SetPrimaryMode(serenity::experimental::FlushSetting::always);
		SetConsoleInterface(consoleMode);
		SetOriginalColors();
		if( IsValidHandle() && IsTerminalType() ) {
				coloredOutput = true;
		}
	}

	ColorConsole::ColorConsole(std::string_view name): TargetBase(name), consoleMode(console_interface::std_out), coloredOutput(false) {
		TargetHelper()->Policy()->SetPrimaryMode(serenity::experimental::FlushSetting::always);
		SetConsoleInterface(consoleMode);
		SetOriginalColors();
		if( IsValidHandle() && IsTerminalType() ) {
				coloredOutput = true;
		}
	}

	ColorConsole::ColorConsole(std::string_view name, std::string_view msgPattern)
		: TargetBase(name, msgPattern), consoleMode(console_interface::std_out), coloredOutput(false) {
		TargetHelper()->Policy()->SetPrimaryMode(serenity::experimental::FlushSetting::always);
		SetConsoleInterface(consoleMode);
		SetOriginalColors();
		if( IsValidHandle() && IsTerminalType() ) {
				coloredOutput = true;
		}
	}

	ColorConsole::~ColorConsole() {
		// If console was redirected, flush output to destination
#ifdef WINDOWS_PLATFORM
		if( !IsTerminalType() ) {
				FlushFileBuffers(outputHandle);
		}
		DWORD opMode { 0 };
		if( !GetConsoleMode(outputHandle, &opMode) ) {
				exit(GetLastError());
		}
		if( !SetConsoleMode(outputHandle, opMode) ) {
				exit(GetLastError());
		}
#else
		if( !IsTerminalType() ) {
				fflush(outputHandle);
		}
#endif    // WINDOWS_PLATFORM
	}     // ~ColorConsole

	bool ColorConsole::IsValidHandle() {
#ifdef WINDOWS_PLATFORM
		return outputHandle != INVALID_HANDLE_VALUE;
#else
		return outputHandle != nullptr;
#endif    // WINDOWS_PLATFORM
	}

	void ColorConsole::SetMsgColor(LoggerLevel level, std::string_view color) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		msgLevelColors.at(level) = color;
	}

	std::string_view ColorConsole::GetMsgColor(LoggerLevel level) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		return msgLevelColors.at(level);
	}

	void ColorConsole::ColorizeOutput(bool colorize) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		coloredOutput = colorize;
	}

	// Other than some wierd color trailing on the right-hand side for some of the
	// differently colored lines, This now ACTUALLY works as intended (color
	// trailing might be result of known windows cell issue)
	void ColorConsole::SetConsoleInterface(console_interface mode) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		consoleMode = mode;
#ifdef WINDOWS_PLATFORM
		if( mode == console_interface::std_out ) {
				outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		} else {
				outputHandle = GetStdHandle(STD_ERROR_HANDLE);
			}
		DWORD opMode { 0 };
		if( !GetConsoleMode(outputHandle, &opMode) ) {
				exit(GetLastError());
		}
		opMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if( !SetConsoleMode(outputHandle, opMode) ) {
				exit(GetLastError());
		}
#else
		if( mode == console_interface::std_out ) {
				outputHandle = stdout;
		} else {
				outputHandle = stderr;
			}
#endif    // WINDOWS_PLATFORM
	}

	const console_interface ColorConsole::ConsoleInterface() {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		return consoleMode;
	}

	bool ColorConsole::IsTerminalType() {
		auto consoleType { (consoleMode == console_interface::std_out) ? stdout : stderr };
		return (ISATTY(FILENO(consoleType))) ? true : false;
	}

	void ColorConsole::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		message.clear();
		std::string_view msgColor { "" }, reset { "" };
		if( IsValidHandle() ) {
				if( IsTerminalType() && coloredOutput ) {
						msgColor = msgLevelColors.at(MsgInfo()->MsgLevel());
						reset    = se_colors::formats::reset;
				}
				// message.reserve(formatted.size() + msgColor.size() + reset.size());
				message.append(msgColor).append(formatted).append(reset);
#ifdef WINDOWS_PLATFORM
				if( IsTerminalType() ) {
						WriteConsole(outputHandle, message.data(), static_cast<DWORD>(message.size()), NULL, NULL);
						return;
				} else {
						WriteFile(outputHandle, message.data(), static_cast<DWORD>(message.size()), NULL, NULL);
						return;
					}
#else
				fwrite(message.data(), 1, message.size(), outputHandle);
				return;
#endif       // WINDOWS_PLATFORM
		}    // IsValidHandle() Check
	}        // PrintMessage()

	void ColorConsole::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		TargetBase::SetLocale(loc);
	}

	void ColorConsole::SetOriginalColors() {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		msgLevelColors = {
			{ LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
			{ LoggerLevel::info, se_colors::bright_colors::foreground::green },
			{ LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
			{ LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
			{ LoggerLevel::error, se_colors::basic_colors::foreground::red },
			{ LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
			{ LoggerLevel::off, se_colors::formats::reset },
		};
	}

}    // namespace serenity::targets
