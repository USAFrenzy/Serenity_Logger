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
	msgPattern->FormatMessageArgs(msg, std::forward<Args>(args)...);
	PrintMessage(msgPattern->GetFormatters().FormatUserPattern());
	PolicyFlushOn();
}

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
