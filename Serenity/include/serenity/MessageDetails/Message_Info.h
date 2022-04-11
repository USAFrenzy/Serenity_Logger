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
		void SetMessage(std::string_view msg);
		void SetMsgLevel(const LoggerLevel level);
		void SetLocale(const std::locale& loc);
		void SetTimeMode(const message_time_mode mode);
		void SetTimePoint(std::chrono::system_clock::time_point tp = std::chrono::system_clock::now());
		std::chrono::system_clock::time_point MessageTimePoint() const;
		message_time_mode TimeMode();
		std::tm& TimeInfo();
		std::string& Message();
		size_t MessageSize() const;
		std::locale GetLocale() const;
		std::locale& GetLocale();

	      private:
		std::string m_name;
		LoggerLevel m_msgLevel;
		std::string m_message;
		Message_Time m_msgTime;
		std::locale m_locale;
		std::chrono::system_clock::time_point m_msgTimePoint;
	};
}    // namespace serenity::msg_details
