#include <serenity/Targets/Target.h>

namespace serenity::targets {
	constexpr const char* DEFAULT_PATTERN = "|%l| %x %n %T [%N]: %+";

	TargetBase::TargetBase(): logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN) {
		msgDetails = std::make_unique<msg_details::Message_Info>("Base Logger", msgLevel, message_time_mode::local);
		msgPattern = std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get());
	}

	TargetBase::TargetBase(std::string_view name): logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN) {
		msgDetails = std::make_unique<msg_details::Message_Info>("Base Logger", msgLevel, message_time_mode::local);
		msgPattern = std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get());
	}

	TargetBase::TargetBase(std::string_view name, std::string_view fmtPattern)
		: logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(fmtPattern) {
		msgDetails = std::make_unique<msg_details::Message_Info>("Base Logger", msgLevel, message_time_mode::local);
		msgPattern = std::make_unique<msg_details::Message_Formatter>(pattern, msgDetails.get());
	}

	void TargetBase::SetPattern(std::string_view pattern) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		msgPattern->SetPattern(std::string { pattern.data(), pattern.size() });
	}

	void TargetBase::SetFlushPolicy(const serenity::experimental::Flush_Policy& pPolicy) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		baseHelper.Policy() = pPolicy;
	}

	const std::string TargetBase::LoggerName() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return msgDetails->Name();
	}

	void TargetBase::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( loc != MsgInfo()->GetLocale() ) {
				MsgInfo()->SetLocale(loc);
		}
	}

	const std::locale TargetBase::GetLocale() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return MsgInfo()->GetLocale();
	}

	// Leaving empty for derived classes to implement
	void TargetBase::PolicyFlushOn() { }

	msg_details::Message_Formatter* TargetBase::MsgFmt() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return msgPattern.get();
	}

	msg_details::Message_Info* TargetBase::MsgInfo() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return msgDetails.get();
	}

	void TargetBase::ResetPatternToDefault() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		msgPattern->SetPattern(DEFAULT_PATTERN);
	}

	void TargetBase::SetLogLevel(LoggerLevel level) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		logLevel = level;
	}

	const LoggerLevel TargetBase::Level() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return logLevel;
	}

	void TargetBase::SetLoggerName(std::string_view name) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		msgDetails->SetName(name);
	}

	void TargetBase::EnableMultiThreadingSupport(bool enable) {
		baseHelper.EnableMultiThreadingSupport(enable);
	}

}    // namespace serenity::targets
