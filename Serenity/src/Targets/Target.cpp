#include <serenity/Targets/Target.h>

#if defined BUILT_IN_FORMATTING_ENABLED && !defined DISABLE_CFMT_WARN
	#ifdef WINDOWS_PLATFORM
		#pragma message(                                                                                                                                            \
		"\tBuilt-in Argument Formatting Is Enabled.\n\tTo Use 'libfmt' Or <format>, Please Define Either 'USE_STD_FORMAT' or 'USE_FMTLIB' Instead.\n\tTo Disable This Message, Please Define 'DISABLE_CFMT_WARN'")
	#else
		#warning                                                                                                                                                    \
		"\tBuilt-in Argument Formatting Is Enabled.\n\tTo Use 'libfmt' Or <format>,  Please Define Either 'USE_STD_FORMAT' or 'USE_FMTLIB' Instead.\n\tTo Disable This Message, Please Define 'DISABLE_CFMT_WARN'"
	#endif    // WINDOWS_PLATFORM
#endif        // BUILT_IN_FORMATTING_ENABLED

namespace serenity::targets {
	constexpr const char* DEFAULT_PATTERN = "|%l| %a %d%b%y %T [%N]: %+";

	TargetBase::TargetBase()
		: logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN),
		  msgDetails(std::make_unique<msg_details::Message_Info>("Base Logger", msgLevel, message_time_mode::local)),
		  msgPattern(std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get())), baseHelper(std::make_unique<helpers::BaseTargetHelper>()) { }

	TargetBase::TargetBase(std::string_view name)
		: logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN),
		  msgDetails(std::make_unique<msg_details::Message_Info>(name, msgLevel, message_time_mode::local)),
		  msgPattern(std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get())), baseHelper(std::make_unique<helpers::BaseTargetHelper>()) { }

	TargetBase::TargetBase(std::string_view name, std::string_view fmtPattern)
		: logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(fmtPattern),
		  msgDetails(std::make_unique<msg_details::Message_Info>(name, msgLevel, message_time_mode::local)),
		  msgPattern(std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get())), baseHelper(std::make_unique<helpers::BaseTargetHelper>()) { }

	void TargetBase::SetPattern(std::string_view pattern) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		msgPattern->SetPattern(std::string { pattern.data(), pattern.size() });
	}

	void TargetBase::SetFlushPolicy(const serenity::experimental::Flush_Policy& pPolicy) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}

		baseHelper->SetFlushPolicy(pPolicy);
	}

	std::string TargetBase::LoggerName() const {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		return msgDetails->Name();
	}

	void TargetBase::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		if( loc != MsgFmt()->Locale() ) {
				MsgFmt()->SetLocaleReference(loc);
		}
	}

	std::locale TargetBase::GetLocale() const {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		return MsgFmt()->Locale();
	}

	std::shared_ptr<helpers::BaseTargetHelper>& TargetBase::TargetHelper() {
		return baseHelper;
	}

	void TargetBase::PrintMessage() { }

	// Leaving empty for derived classes to implement
	void TargetBase::PolicyFlushOn() { }

	const std::unique_ptr<msg_details::Message_Formatter>& TargetBase::MsgFmt() const {
		return msgPattern;
	}

	const std::unique_ptr<msg_details::Message_Info>& TargetBase::MsgInfo() const {
		return msgDetails;
	}

	void TargetBase::ResetPatternToDefault() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		msgPattern->SetPattern(DEFAULT_PATTERN);
	}

	void TargetBase::SetLogLevel(LoggerLevel level) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		logLevel = level;
	}

	LoggerLevel TargetBase::Level() const {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		return logLevel;
	}

	void TargetBase::SetLoggerName(std::string_view name) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		msgDetails->SetName(name);
	}

	void TargetBase::EnableMultiThreadingSupport(bool enable) {
		baseHelper->EnableMultiThreadingSupport(enable);
	}

}    // namespace serenity::targets
