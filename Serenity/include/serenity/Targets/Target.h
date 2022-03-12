#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>
#include <serenity/MessageDetails/Message_Formatter.h>
#include <serenity/MessageDetails/Message_Info.h>
#include <serenity/MessageDetails/Message_Time.h>

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
		const serenity::experimental::Flush_Policy& Policy();
		const std::string LoggerName();
		void SetPattern(std::string_view pattern);
		void ResetPatternToDefault();
		void SetLogLevel(LoggerLevel level);
		void EnableMultiThreadingSupport(bool enableMultiThreading = true);
		bool isMTSupportEnabled();
		const LoggerLevel Level();
		void SetLoggerName(std::string_view name);
		template<typename... Args> void Trace(std::string_view msg, Args&&... args);
		template<typename... Args> void Info(std::string_view msg, Args&&... args);
		template<typename... Args> void Debug(std::string_view msg, Args&&... args);
		template<typename... Args> void Warn(std::string_view msg, Args&&... args);
		template<typename... Args> void Error(std::string_view msg, Args&&... args);
		template<typename... Args> void Fatal(std::string_view msg, Args&&... args);
		virtual void SetLocale(const std::locale& loc);
		const std::locale GetLocale();

	protected:
		msg_details::Message_Formatter* MsgFmt();
		msg_details::Message_Info* MsgInfo();
		void WriteToBaseBuffer(bool fmtToBuf = true);
		const bool isWriteToBuf();
		std::string* const Buffer();
		virtual void PrintMessage(std::string_view formatted) = 0;
		virtual void PolicyFlushOn();

	private:
		bool toBuffer;
		LoggerLevel logLevel;
		LoggerLevel msgLevel;
		std::string pattern;
		msg_details::Message_Info msgDetails;
		msg_details::Message_Formatter msgPattern;
		std::string internalBuffer;
		bool multiThreadSupport;
		std::mutex baseMutex;

	protected:
		serenity::experimental::Flush_Policy policy;
	};

#include "Target-impl.h"
}    // namespace serenity::targets
