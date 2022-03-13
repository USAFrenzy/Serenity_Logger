#include <serenity/Targets/Target.h>

namespace serenity::targets {
	constexpr const char* DEFAULT_PATTERN = "|%l| %x %n %T [%N]: %+";

	TargetBase::TargetBase()
		: toBuffer(false), logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN),
		  msgDetails("Base Logger", msgLevel, message_time_mode::local), msgPattern(pattern, &msgDetails), multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	TargetBase::TargetBase(std::string_view name)
		: toBuffer(false), logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(DEFAULT_PATTERN),
		  msgDetails(name, msgLevel, message_time_mode::local), msgPattern(pattern, &msgDetails), multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	TargetBase::TargetBase(std::string_view name, std::string_view fmtPattern)
		: toBuffer(false), logLevel(LoggerLevel::trace), msgLevel(LoggerLevel::trace), pattern(fmtPattern),
		  msgDetails(name, msgLevel, message_time_mode::local), msgPattern(pattern, &msgDetails), multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	void TargetBase::WriteToBaseBuffer(bool fmtToBuf) {
		toBuffer = fmtToBuf;
	}

	const bool TargetBase::isWriteToBuf() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return toBuffer;
	}

	std::string* const TargetBase::Buffer() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return &internalBuffer;
	}

	void TargetBase::SetPattern(std::string_view pattern) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		msgPattern.SetPattern(std::string { pattern.data(), pattern.size() });
	}

	void TargetBase::SetFlushPolicy(const serenity::experimental::Flush_Policy& pPolicy) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		policy = pPolicy;
	}

	const serenity::experimental::Flush_Policy& TargetBase::Policy() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return policy;
	}

	const std::string TargetBase::LoggerName() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return msgDetails.Name();
	}

	void TargetBase::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		if( loc != MsgInfo()->GetLocale() ) {
				MsgInfo()->SetLocale(loc);
		}
	}

	const std::locale TargetBase::GetLocale() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return MsgInfo()->GetLocale();
	}

	// Leaving empty for derived classes to implement
	void TargetBase::PolicyFlushOn() { }

	msg_details::Message_Formatter* TargetBase::MsgFmt() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return &msgPattern;
	}

	msg_details::Message_Info* TargetBase::MsgInfo() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return &msgDetails;
	}

	void TargetBase::ResetPatternToDefault() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		msgPattern.SetPattern(DEFAULT_PATTERN);
	}

	void TargetBase::SetLogLevel(LoggerLevel level) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		logLevel = level;
	}

	const LoggerLevel TargetBase::Level() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return logLevel;
	}

	void TargetBase::SetLoggerName(std::string_view name) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		msgDetails.SetName(name);
	}

	void TargetBase::EnableMultiThreadingSupport(bool enableMultiThreading) {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		multiThreadSupport = enableMultiThreading;
	}

	bool TargetBase::isMTSupportEnabled() {
		std::unique_lock<std::mutex> lock(baseMutex, std::defer_lock);
		if( multiThreadSupport ) {
				lock.lock();
		}
		return multiThreadSupport;
	}
}    // namespace serenity::targets
