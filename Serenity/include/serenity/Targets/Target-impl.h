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
	namespace cf = serenity::msg_details::custom_flags;
	using sv     = std::string_view;

	const auto& details { *msgDetails };
	const auto& fmtr { *msgPattern };

#ifdef USE_FMT_LIB
	// make local copies because fmt doesn't like const arguments
	auto lvl { details.MsgLevel() };
	auto timeStruct { details.TimeInfo() };
#else
	const auto& lvl { details.MsgLevel() };
	const auto& timeStruct { details.TimeInfo() };
#endif    // USE_FMT_LIB

	switch( fmtr.FmtFunctionFlag() ) {
			case Time_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), timeStruct);
				}
			case Localized_Time_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), timeStruct);
				}
			case Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()));
				}
			case Src_Base:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src));
				}
			case Time_Src_Base:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src), timeStruct);
				}
			case Localized_Time_Src_Base:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src), timeStruct);
				}
			case Thread_Base:
				{
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), thread);
				}
			case Time_Thread_Base:
				{
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(thread), timeStruct);
				}
			case Localized_Time_Thread_Base:
				{
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(thread),
					                    timeStruct);
				}
			case Src_Thread_Base:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src), thread);
				}
			case Time_Src_Thread_Base:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src), sv(thread), timeStruct);
				}
			case Localized_Time_Src_Thread_Base: [[fallthrough]];
			default:
				{
					auto src { std::move(cf::Format_Source_Loc { details.SourceLocation(), fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					auto thread { std::move(cf::Format_Thread_ID { details.ThisThreadID() }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(details.Name()), sv(details.Message()), sv(src),
					                    sv(thread), timeStruct);
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
