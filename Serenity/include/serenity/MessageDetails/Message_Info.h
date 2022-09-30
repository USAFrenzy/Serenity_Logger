#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Time.h>

namespace serenity::msg_details {

	class Message_Info
	{
	  public:
		Message_Info(std::string_view name, LoggerLevel level, message_time_mode mode);
		Message_Info& operator=(const Message_Info& t) = delete;
		Message_Info(const Message_Info&)              = delete;
		Message_Info()                                 = delete;
		~Message_Info()                                = default;

		LoggerLevel& MsgLevel();
		std::string& Name();
		Message_Time& TimeDetails();
		void SetName(const std::string_view name);
		void SetMsgLevel(const LoggerLevel level);
		void SetTimeMode(const message_time_mode mode);
		void SetTimePoint(std::chrono::system_clock::time_point tp = std::chrono::system_clock::now());
		std::chrono::system_clock::time_point MessageTimePoint() const;
		std::source_location& SourceLocation();
		message_time_mode TimeMode();
		const std::tm& TimeInfo() const;
		std::string& Message();
		size_t MessageSize() const;

	  private:
		std::string m_name;
		LoggerLevel m_msgLevel;
		Message_Time m_msgTime;
		std::chrono::system_clock::time_point m_msgTimePoint;
		std::source_location source;
		std::string m_message;
	};
}    // namespace serenity::msg_details
