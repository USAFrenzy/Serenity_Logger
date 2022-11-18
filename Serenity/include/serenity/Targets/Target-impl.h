#pragma once
#include <serenity/Common.h>
#include <serenity/Targets/Target.h>
#include <chrono>
#include <string>

template<typename... Args> void serenity::targets::TargetBase::LogMessage(std::string_view msg, Args&&... args) {
	using namespace std::chrono;
	if( msgPattern->HasTimeField() ) {
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
template<typename T>
requires utf_utils::utf_constraints::IsSupportedUContainer<std::remove_cvref_t<T>>
inline constexpr void serenity::targets::TargetBase::FormatLogMessage(T& cont) {
	using v = std::string_view;
	if( const auto& fmtr { *msgPattern }; fmtr.HasTimeField() ) {
			// we have a time field so check for source and thread
			if( fmtr.HasSourceField() && fmtr.HasThreadField() ) {
					// we have both source and thread fields so format based on if time is localized
					return !(fmtr.HasLocalizedTime())
					     ? VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src, fmtRefs.m_thread, fmtRefs.m_time)
					     : VFORMAT_TO_L(cont, fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src, fmtRefs.m_thread,
					                    fmtRefs.m_time);
			} else if( fmtr.HasSourceField() ) {
					// we don't have a thread field, but do have a source field, now fmt based on localized time flags
					return !(fmtr.HasLocalizedTime())
					     ? VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src, fmtRefs.m_time)
					     : VFORMAT_TO_L(cont, fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src, fmtRefs.m_time);
			} else if( fmtr.HasThreadField() ) {
					// we don't have a source field, but do have a thread field, now fmt based on localized time flags
					return !(fmtr.HasLocalizedTime())
					     ? VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_thread, fmtRefs.m_time)
					     : VFORMAT_TO_L(cont, fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_thread, fmtRefs.m_time);
			} else {
					// we don't have either a source or a thread field so format based on if time is localized
					return !(fmtr.HasLocalizedTime())
					     ? VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_time)
					     : VFORMAT_TO_L(cont, fmtr.Locale(), v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_time);
				}
	} else if( fmtr.HasSourceField() ) {
			// we don't have a time field, but we do have a source field, format based on whether we have a thread field present or not
			return fmtr.HasThreadField() ? VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src, fmtRefs.m_thread)
			                             : VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_src);
	} else if( fmtr.HasThreadField() ) {
			// we don't have a time field or source field present, but do have a thread field, so format based off that
			return VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg), fmtRefs.m_thread);
	} else {
			// we don't have a time field, source field, or thread field so do the basic formatting call
			return VFORMAT_TO(cont, v(fmtr.Pattern()), fmtRefs.m_lvl, v(fmtRefs.m_name), v(fmtRefs.m_msg));
		}
}

// TODO: Given these are the top level functions called for logging, take care of encoding the string type message to utf-8 here and track it down the call stack.
//                Since the container used internally here is just a simple std::string, I can streamline these calls specifically by keeping the utf-8 encoding for
//                the
//                 message itself, and if string types are present in the argument, encoding those to utf-8 into the buffer used for the target type.
template<typename... Args> void serenity::targets::TargetBase::Trace(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			msgDetails->SetMsgLevel(LoggerLevel::info);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			msgDetails->SetMsgLevel(LoggerLevel::error);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			if( msgPattern->HasSourceField() ) {
					msgDetails->SourceLocation() = s.source;
			}
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}
