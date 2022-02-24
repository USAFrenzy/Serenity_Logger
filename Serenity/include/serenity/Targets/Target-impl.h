#pragma once

#include <serenity/Common.h>
#include <chrono>
#include <string>

template<typename... Args> void TargetBase::Trace(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::trace ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::trace);
			msgDetails.SetMessage(s, std::forward<Args>(args)...);
			std::string formatted;
			if( msgDetails.GetLocale().name() == globals::defaultLocale.name() ) {
					formatted = std::move(MsgFmt()->GetFormatters().Format());
			} else {
					localeSupport.str(std::string());
					localeSupport.clear();
					localeSupport << MsgFmt()->GetFormatters().Format();
					formatted = std::move(localeSupport.str());
				}
			if( isWriteToBuf() ) {
					internalBuffer.append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(localeSupport->str());
					PolicyFlushOn();
				}
	}
}

template<typename... Args> void TargetBase::Info(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::info ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::info);
		msgDetails.SetMessage(s, std::forward<Args>(args)...			std::string formatted;
		if (msgDetails.GetLocale().name() == globals::defaultLocale.name()) {
				formatted = std::move(MsgFmt()->GetFormatters().Format());
		}
		else {
				localeSupport.str(std::string());
				localeSupport.clear();
				localeSupport << MsgFmt()->GetFormatters().Format();
				formatted = std::move(localeSupport.str());
		}
		if (isWriteToBuf()) {
				internalBuffer.append(formatted.data(), formatted.size());
				PolicyFlushOn();
		}
		else {
				PrintMessage(formatted);
				PolicyFlushOn();
		}
	}
}

template<typename... Args> void TargetBase::Debug(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::debug ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::debug);
			msgDetails.SetMessage(s, std::forward<Args>(args)...);
			std::string formatted;
			if( msgDetails.GetLocale().name() == globals::defaultLocale.name() ) {
					formatted = std::move(MsgFmt()->GetFormatters().Format());
			} else {
					localeSupport.str(std::string());
					localeSupport.clear();
					localeSupport << MsgFmt()->GetFormatters().Format();
					formatted = std::move(localeSupport.str());
				}
			if( isWriteToBuf() ) {
					internalBuffer.append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
	}
}

template<typename... Args> void TargetBase::Warn(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::warning ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::warning);
			msgDetails.SetMessage(s, std::forward<Args>(args)...);
			std::string formatted;
			if( msgDetails.GetLocale().name() == globals::defaultLocale.name() ) {
					formatted = std::move(MsgFmt()->GetFormatters().Format());
			} else {
					localeSupport.str(std::string());
					localeSupport.clear();
					localeSupport << MsgFmt()->GetFormatters().Format();
					formatted = std::move(localeSupport.str());
				}
			if( isWriteToBuf() ) {
					internalBuffer.append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
	}
}

template<typename... Args> void TargetBase::Error(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::error ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::error);
			msgDetails.SetMessage(s, std::forward<Args>(args)...);
			std::string formatted;
			if( msgDetails.GetLocale().name() == globals::defaultLocale.name() ) {
					formatted = std::move(MsgFmt()->GetFormatters().Format());
			} else {
					localeSupport.str(std::string());
					localeSupport.clear();
					localeSupport << MsgFmt()->GetFormatters().Format();
					formatted = std::move(localeSupport.str());
				}
			if( isWriteToBuf() ) {
					internalBuffer.append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
	}
}

template<typename... Args> void TargetBase::Fatal(std::string_view s, Args&&... args) {
	if( logLevel <= LoggerLevel::fatal ) {
			using namespace std::chrono;
			auto now                 = msgDetails.MessageTimePoint();
			seconds messageTimePoint = duration_cast<seconds>(now.time_since_epoch());
			if( messageTimePoint != msgDetails.TimeDetails().LastLogPoint() ) {
					msgDetails.TimeDetails().UpdateCache(now);
			}
			msgDetails.SetMsgLevel(LoggerLevel::fatal);
			msgDetails.SetMessage(s, std::forward<Args>(args)...);
			std::string formatted;
			if( msgDetails.GetLocale().name() == globals::defaultLocale.name() ) {
					formatted = std::move(MsgFmt()->GetFormatters().Format());
			} else {
					localeSupport.str(std::string());
					localeSupport.clear();
					localeSupport << MsgFmt()->GetFormatters().Format();
					formatted = std::move(localeSupport.str());
				}
			if( isWriteToBuf() ) {
					internalBuffer.append(formatted.data(), formatted.size());
					PolicyFlushOn();
			} else {
					PrintMessage(formatted);
					PolicyFlushOn();
				}
	}
}
