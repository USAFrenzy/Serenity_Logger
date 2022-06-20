#pragma once
#include <serenity/Common.h>
#include <serenity/Targets/Target.h>
#include <chrono>
#include <string>

template<typename... Args> void serenity::targets::TargetBase::Trace(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}

template<typename... Args> void serenity::targets::TargetBase::Info(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::info);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}

template<typename... Args> void serenity::targets::TargetBase::Debug(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}

template<typename... Args> void serenity::targets::TargetBase::Warn(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}

template<typename... Args> void serenity::targets::TargetBase::Error(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::error);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}

template<typename... Args> void serenity::targets::TargetBase::Fatal(serenity::MsgWithLoc s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			using namespace std::chrono;
			msgDetails->SetTimePoint();
			auto now                 = msgDetails->MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails->TimeDetails().LastLogPoint() ) {
					msgDetails->TimeDetails().UpdateCache(now);
			}
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			msgDetails->SourceLocation() = s.source;

			msgPattern->FormatMessageArgs(s.msg, std::forward<Args>(args)...);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			PrintMessage(formatted);
			PolicyFlushOn();
	}
}
