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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::trace);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::info);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::debug);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::warning);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::error);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
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
			msgPattern->FormatMessageArgs(s, std::forward<Args>(args)...);
			msgDetails->SetMsgLevel(LoggerLevel::fatal);
			auto formatted { msgPattern->GetFormatters().FormatUserPattern() };

			if( baseHelper->isWriteToBuf() ) {
					baseHelper->Buffer()->append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
	}
}
