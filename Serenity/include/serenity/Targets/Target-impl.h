#pragma once
#include <serenity/Targets/Target.h>

#include <chrono>
#include <string>

template<typename... Args> void serenity::targets::TargetBase::LogMessage(std::string_view msg, Args&&... args) {
	using namespace std::chrono;
	if( msgPattern->FmtFunctionFlag() >= serenity::msg_details::SeFmtFuncFlags::Time ) {
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
#ifdef USE_FMT_LIB
	// make local copies because fmt doesn't like const arguments
	auto lvl { fmtRefs.m_lvl };
	auto timeStruct { fmtRefs.m_time };
#else
	const auto& lvl { fmtRefs.m_lvl };
	const auto& timeStruct { fmtRefs.m_time };
#endif    // USE_FMT_LIB

	using sv = std::string_view;
	const auto& fmtr { *msgPattern };
	switch( fmtr.FmtFunctionFlag() ) {
			case Time_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), timeStruct);
				}
			case Localized_Time_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), timeStruct);
				}
			case Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg));
				}
			case Src_Base:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src);
				}
			case Time_Src_Base:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src, timeStruct);
				}
			case Localized_Time_Src_Base:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src, timeStruct);
				}
			case Thread_Base:
				{
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), thread);
				}
			case Time_Thread_Base:
				{
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), thread, timeStruct);
				}
			case Localized_Time_Thread_Base:
				{
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), thread, timeStruct);
				}
			case Src_Thread_Base:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src, thread);
				}
			case Time_Src_Thread_Base:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO(std::forward<T>(cont), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src, thread, timeStruct);
				}
			case Localized_Time_Src_Thread_Base: [[fallthrough]];
			default:
				{
					sv src { std::move(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()) };
					sv thread { std::move(cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength())) };
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), sv(fmtr.Pattern()), lvl, sv(fmtRefs.m_name), sv(fmtRefs.m_msg), src, thread, timeStruct);
				}
		}
}

template<typename... Args> void serenity::targets::TargetBase::Trace(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			msgDetails->SetMsgLevel(LoggerLevel::info);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			msgDetails->SetMsgLevel(LoggerLevel::error);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(std::move(s.input), std::forward<Args>(args)...);
	}
}
