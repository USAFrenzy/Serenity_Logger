#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>
#include <serenity/MessageDetails/Message_Formatter.h>
#include <serenity/MessageDetails/Message_Info.h>
#include <serenity/MessageDetails/Message_Time.h>
#include <serenity/Utilities/FormatBackend.h>
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
		std::string LoggerName() const;
		void SetPattern(std::string_view pattern);
		void ResetPatternToDefault();
		void SetLogLevel(LoggerLevel level);
		LoggerLevel Level() const;
		void SetLoggerName(std::string_view name);
		template<typename... Args> void Trace(MsgWithLoc msg, Args&&... args);
		template<typename... Args> void Info(MsgWithLoc msg, Args&&... args);
		template<typename... Args> void Debug(MsgWithLoc msg, Args&&... args);
		template<typename... Args> void Warn(MsgWithLoc msg, Args&&... args);
		template<typename... Args> void Error(MsgWithLoc msg, Args&&... args);
		template<typename... Args> void Fatal(MsgWithLoc msg, Args&&... args);
		void EnableMultiThreadingSupport(bool enableMultiThreading = true);
		virtual void SetLocale(const std::locale& loc);
		std::locale GetLocale() const;

	  protected:
		std::shared_ptr<helpers::BaseTargetHelper>& TargetHelper();
		const std::unique_ptr<msg_details::Message_Formatter>& MsgFmt() const;
		const std::unique_ptr<msg_details::Message_Info>& MsgInfo() const;
		virtual void PrintMessage();
		virtual void PolicyFlushOn();

	  private:
		LoggerLevel logLevel;
		LoggerLevel msgLevel;
		std::string pattern;
		mutable std::mutex baseMutex;
		std::unique_ptr<msg_details::Message_Info> msgDetails;
		std::unique_ptr<msg_details::Message_Formatter> msgPattern;
		std::shared_ptr<helpers::BaseTargetHelper> baseHelper;
		template<typename... Args> void LogMessage(std::string_view msg, Args&&... args);
	};

#include "Target-impl.h"
}    // namespace serenity::targets
