#pragma once
#include <serenity/Targets/Target.h>

#include <chrono>
#include <string>

template<typename... Args> void serenity::targets::TargetBase::LogMessage(std::string_view msg, Args&&... args) {
	using namespace std::chrono;
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Time) == SeFmtFuncFlags::Time ) {
			msgDetails->SetTimePoint();
			auto now = msgDetails->MessageTimePoint();
			if( duration_cast<seconds>(now.time_since_epoch()) != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
	}
	auto& msgHandle { msgDetails->Message() };
	msgHandle.clear();
	VFORMAT_TO_L(msgHandle, msgPattern->Locale(), msg, std::forward<Args>(args)...);
	PrintMessage();
	PolicyFlushOn();
}
template<typename T>
requires utf_utils::utf_constraints::IsSupportedUContainer<std::remove_cvref_t<T>>
inline constexpr auto serenity::targets::TargetBase::FormatLogMessage(T&& cont) {
	using enum serenity::msg_details::SeFmtFuncFlags;
	using namespace serenity::msg_details::custom_flags;
	const auto& info { *msgDetails };
	const auto& fmt { *msgPattern };

	switch( fmt.FmtFunctionFlag() ) {
			case Time_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), info.TimeInfo());
				}
			case Localized_Time_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmt.Locale(), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), info.TimeInfo());
				}
			case Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message());
				}
			case Src_Base:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src);
				}
			case Time_Src_Base:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src, info.TimeInfo());
				}
			case Localized_Time_Src_Base:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmt.Locale(), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src, info.TimeInfo());
				}
			case Thread_Base:
				{
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), thread);
				}
			case Time_Thread_Base:
				{
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), thread, info.TimeInfo());
				}
			case Localized_Time_Thread_Base:
				{
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmt.Locale(), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), thread, info.TimeInfo());
				}
			case Src_Thread_Base:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src, thread);
				}
			case Time_Src_Thread_Base:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO(std::forward<T>(cont), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src, thread, info.TimeInfo());
				}
			case Localized_Time_Src_Thread_Base: [[fallthrough]];
			default:
				{
					auto src { std::string_view(std::move(Format_Source_Loc { info.SourceLocation(), fmt.SourceFmtFlag() }.FormatUserPattern())) };
					auto thread { std::string_view(std::move(Format_Thread_ID { info.ThisThreadID() }.FormatUserPattern(fmt.ThreadFmtLength()))) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmt.Locale(), fmt.Pattern(), info.MsgLevel(), info.Name(), info.Message(), src, thread, info.TimeInfo());
				}
		}
}

template<typename... Args> void serenity::targets::TargetBase::Trace(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::trace ) {
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::info ) {
			msgDetails->SetMsgLevel(LoggerLevel::info);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::debug ) {
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::warning ) {
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::error ) {
			msgDetails->SetMsgLevel(LoggerLevel::error);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(MsgWithLoc s, Args&&... args) {
	using SeFmtFuncFlags = serenity::msg_details::SeFmtFuncFlags;
	if( logLevel <= LoggerLevel::fatal ) {
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			if( (msgPattern->FmtFunctionFlag() & SeFmtFuncFlags::Src) == SeFmtFuncFlags::Src ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}
