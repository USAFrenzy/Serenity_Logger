#include <serenity/Targets/Target.h>

namespace serenity::targets {
	constexpr const char* DEFAULT_PATTERN = "|%l| %x %n %T [%N]: %+";

	TargetBase::TargetBase()
		: toBuffer(false),
		  logLevel(LoggerLevel::trace),
		  msgLevel(LoggerLevel::trace),
		  pattern(DEFAULT_PATTERN),
		  msgDetails("Base Logger", msgLevel, message_time_mode::local),
		  msgPattern(pattern, &msgDetails),
		  multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	TargetBase::TargetBase(std::string_view name)
		: toBuffer(false),
		  logLevel(LoggerLevel::trace),
		  msgLevel(LoggerLevel::trace),
		  pattern(DEFAULT_PATTERN),
		  msgDetails(name, msgLevel, message_time_mode::local),
		  msgPattern(pattern, &msgDetails),
		  multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	TargetBase::TargetBase(std::string_view name, std::string_view fmtPattern)
		: toBuffer(false),
		  logLevel(LoggerLevel::trace),
		  msgLevel(LoggerLevel::trace),
		  pattern(fmtPattern),
		  msgDetails(name, msgLevel, message_time_mode::local),
		  msgPattern(pattern, &msgDetails),
		  multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	void TargetBase::WriteToBaseBuffer(bool fmtToBuf) {
		toBuffer = fmtToBuf;
	}

	const bool TargetBase::isWriteToBuf() {
		return toBuffer;
	}

	std::string* const TargetBase::Buffer() {
		return &internalBuffer;
	}

	void TargetBase::SetPattern(std::string_view pattern) {
		msgPattern.SetPattern(std::string { pattern.data(), pattern.size() });
	}

	void TargetBase::SetFlushPolicy(const serenity::experimental::Flush_Policy& pPolicy) {
		policy = pPolicy;
	}

	const serenity::experimental::Flush_Policy& TargetBase::Policy() {
		return policy;
	}

	const std::string TargetBase::LoggerName() {
		return msgDetails.Name();
	}

	void TargetBase::SetLocale(const std::locale& loc) {
		if( loc != MsgInfo()->GetLocale() ) {
				MsgInfo()->SetLocale(loc);
		}
	}

	const std::locale TargetBase::GetLocale() {
		return MsgInfo()->GetLocale();
	}

	// Leaving empty for derived classes to implement
	void TargetBase::PolicyFlushOn() { }

	msg_details::Message_Formatter* TargetBase::MsgFmt() {
		return &msgPattern;
	}

	msg_details::Message_Info* TargetBase::MsgInfo() {
		return &msgDetails;
	}

	void TargetBase::ResetPatternToDefault() {
		msgPattern.SetPattern(DEFAULT_PATTERN);
	}

	void TargetBase::SetLogLevel(LoggerLevel level) {
		logLevel = level;
	}

	const LoggerLevel TargetBase::Level() {
		return logLevel;
	}

	void TargetBase::SetLoggerName(std::string_view name) {
		msgDetails.SetName(name);
	}

	void TargetBase::EnableMultiThreadingSupport(bool enableMultiThreading) {
		multiThreadSupport = enableMultiThreading;
	}

	bool TargetBase::isMTSupportEnabled() {
		return multiThreadSupport;
	}
}    // namespace serenity::targets
