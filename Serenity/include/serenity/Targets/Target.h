#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>
#include <serenity/MessageDetails/Message_Formatter.h>
#include <serenity/MessageDetails/Message_Info.h>
#include <serenity/MessageDetails/Message_Time.h>
#include <serenity/Utilities/TargetHelper.h>

#include <chrono>

namespace serenity::targets {
	class TargetBase
	{
	      public:
		TargetBase();
		TargetBase(std::string_view name);
		TargetBase(std::string_view name, std::string_view msgPattern);
		~TargetBase() = default;
		void SetFlushPolicy(const serenity::experimental::Flush_Policy& pPolicy);
		const std::string LoggerName();
		void SetPattern(std::string_view pattern);
		void ResetPatternToDefault();
		void SetLogLevel(LoggerLevel level);
		const LoggerLevel Level();
		void SetLoggerName(std::string_view name);
		template<typename... Args> void Trace(std::string_view msg, Args&&... args);
		template<typename... Args> void Info(std::string_view msg, Args&&... args);
		template<typename... Args> void Debug(std::string_view msg, Args&&... args);
		template<typename... Args> void Warn(std::string_view msg, Args&&... args);
		template<typename... Args> void Error(std::string_view msg, Args&&... args);
		template<typename... Args> void Fatal(std::string_view msg, Args&&... args);
		void EnableMultiThreadingSupport(bool enableMultiThreading = true);
		virtual void SetLocale(const std::locale& loc);
		const std::locale GetLocale();

	      protected:
		msg_details::Message_Formatter* MsgFmt();
		msg_details::Message_Info* MsgInfo();
		virtual void PrintMessage(std::string_view formatted) = 0;
		virtual void PolicyFlushOn();
		helpers::BaseTargetHelper baseHelper;

	      private:
		LoggerLevel logLevel;
		LoggerLevel msgLevel;
		std::string pattern;
		mutable std::mutex baseMutex;
		std::unique_ptr<msg_details::Message_Info> msgDetails;
		std::unique_ptr<msg_details::Message_Formatter> msgPattern;
	};

#include "Target-impl.h"
}    // namespace serenity::targets
