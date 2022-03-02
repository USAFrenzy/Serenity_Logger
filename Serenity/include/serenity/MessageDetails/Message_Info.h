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
			std::chrono::system_clock::time_point MessageTimePoint();
			void SetTimeMode(const message_time_mode mode);
			message_time_mode TimeMode();
			std::tm& TimeInfo();
			std::string& Message();
			const size_t MessageSize();
			void SetLocale(const std::locale& loc);
			const std::locale GetLocale();

			template<typename... Args> void SetMessage(const std::string_view message, Args&&... args) {
				m_message.clear();

				m_locale != globals::default_locale ? VFORMAT_TO_LOC(m_message, m_locale, message, std::forward<Args>(args)...)
								    : VFORMAT_TO(m_message, message, std::forward<Args>(args)...);

				m_message.append(SERENITY_LUTS::line_ending.at(platformEOL));
			}

		private:
			LineEnd platformEOL;
			std::string m_name;
			LoggerLevel m_msgLevel;
			std::string m_message;
			Message_Time m_msgTime;
			std::locale m_locale;
	};
}    // namespace serenity::msg_details