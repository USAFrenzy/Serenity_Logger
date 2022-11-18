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
//! NOTE [17Nov22] This is slightly messy but the whole process of checking what fields are currently needing to be formatted sped the process back up by ~6% again
//! NOTE [18Nov22] - Shifted the first branch around in favor of negation checking and having the default case as the first called if sourc/thread aren't present
template<typename T>
requires utf_utils::utf_constraints::IsSupportedUContainer<std::remove_cvref_t<T>>
inline constexpr void serenity::targets::TargetBase::FormatLogMessage(T&& cont) {
	using enum serenity::msg_details::SeFmtFuncFlags;
	namespace cf = serenity::msg_details::custom_flags;

	using v      = std::string_view;
	const auto& fmtr { *msgPattern };
	switch( msgPattern->FmtFunctionFlag() ) {
			case Base: [[fallthrough]];
			default:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg));
				}
			case Time_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_time);
				}
			case Src_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()));
				}
			case Thread_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()));
				}
			case Src_Thread_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()),
					                  cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()));
				}
			case Time_Src_Thread_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()),
					                  cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()), fmtRefs.m_time);
				}
			case Localized_Time_Src_Thread_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                    v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()),
					                    cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()), fmtRefs.m_time);
				}
			case Localized_Time_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src,
					                    fmtRefs.m_time);
				}
			case Time_Src_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()), fmtRefs.m_time);
				}
			case Localized_Time_Src_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                    v(cf::Format_Source_Loc { fmtRefs.m_src, fmtr.SourceFmtFlag() }.FormatUserPattern()), fmtRefs.m_time);
				}
			case Time_Thread_Base:
				{
					return VFORMAT_TO(std::forward<T>(cont), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                  cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()), fmtRefs.m_time);
				}
			case Localized_Time_Thread_Base:
				{
					return VFORMAT_TO_L(std::forward<T>(cont), fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg),
					                    cf::Format_Thread_ID { fmtRefs.m_thread }.FormatUserPattern(fmtr.ThreadFmtLength()), fmtRefs.m_time);
				}
		}
}

// TODO: Given these are the top level functions called for logging, take care of encoding the string type message to utf-8 here and track it down the call stack.
//                Since the container used internally here is just a simple std::string, I can streamline these calls specifically by keeping the utf-8 encoding for
//                the
//                 message itself, and if string types are present in the argument, encoding those to utf-8 into the buffer used for the target type.
template<typename... Args> void serenity::targets::TargetBase::Trace(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			msgDetails->SetMsgLevel(LoggerLevel::info);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			msgDetails->SetMsgLevel(LoggerLevel::error);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			if( auto flag { msgPattern->FmtFunctionFlag() }; flag == (flag & serenity::msg_details::SeFmtFuncFlags::Src) ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}
