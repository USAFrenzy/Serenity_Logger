#include <serenity/Targets/ColorConsoleTarget.h>

#if defined BUILT_IN_FORMATTING_ENABLED && !defined DISABLE_CFMT_WARN
	#ifdef WINDOWS_PLATFORM
		#pragma message(                                                                                                                                            \
		"\tBuilt-in Argument Formatting Is Enabled.\n\tFor Custom Formatting, Please Define Either 'USE_STD_FORMAT' or 'USE_FMTLIB' Instead.\n\tTo Disable This Message, Please Define 'DISABLE_CFMT_WARN'")
	#else
		#warning                                                                                                                                                    \
		"\tBuilt-in Argument Formatting Is Enabled.\n\tFor Custom Formatting, Please Define Either 'USE_STD_FORMAT' or 'USE_FMTLIB' Instead.\n\tTo Disable This Message, Please Define 'DISABLE_CFMT_WARN'"
	#endif    // WINDOWS_PLATFORM
#endif        // BUILT_IN_FORMATTING_ENABLED

#include <iostream>

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

	// TODO: Add a function that checks if locale needs to be swapped as well
	// TODO: as reset the old mode to its default if mode is changed in SetConsoleMode()
	void ColorConsole::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(consoleMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		using enum console_interface;
		switch( consoleMode ) {
				case std_out:
					if( std::cout.getloc() != loc ) {
							std::cout.imbue(loc);
							std::wcout.imbue(loc);
					}
					break;
				case std_err:
					if( std::cerr.getloc() != loc ) {
							std::cerr.imbue(loc);
							std::wcerr.imbue(loc);
					}
					break;
				case std_log:
					if( std::clog.getloc() != loc ) {
							std::clog.imbue(loc);
							std::wclog.imbue(loc);
					}
					break;
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
