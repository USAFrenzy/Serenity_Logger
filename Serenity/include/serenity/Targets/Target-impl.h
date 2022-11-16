#pragma once
#include <serenity/Common.h>
#include <serenity/Targets/Target.h>
#include <chrono>
#include <string>

template<typename... Args> void serenity::targets::TargetBase::LogMessage(std::string_view msg, Args&&... args) {
	using namespace std::chrono;

	msgDetails->SetTimePoint();
	auto now = msgDetails->MessageTimePoint();
	if( duration_cast<seconds>(now.time_since_epoch()) != msgDetails->TimeDetails().LastLogPoint() ) {
			msgDetails->TimeDetails().UpdateCache(now);
	}
	auto& msgHandle { msgDetails->Message() };
	msgHandle.clear();
	VFORMAT_TO(msgHandle, msgPattern->Locale(), msg, std::forward<Args>(args)...);
	PrintMessage();
	PolicyFlushOn();
}

// TODO: Given these are the top level functions called for logging, take care of encoding the string type message to utf-8 here and track it down the call stack.
//                Since the container used internally here is just a simple std::string, I can streamline these calls specifically by keeping the utf-8 encoding for
//                the
//                 message itself, and if string types are present in the argument, encoding those to utf-8 into the buffer used for the target type.
template<typename... Args> void serenity::targets::TargetBase::Trace(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			msgDetails->SetMsgLevel(LoggerLevel::info);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			msgDetails->SetMsgLevel(LoggerLevel::error);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			msgDetails->SourceLocation() = s.source;
			LogMessage(s.msg, std::forward<Args>(args)...);
	}
}
