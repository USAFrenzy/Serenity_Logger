#include <serenity/MessageDetails/Message_Info.h>

namespace serenity::msg_details {
	Message_Info::Message_Info(std::string_view name, LoggerLevel level, message_time_mode mode)
		: m_name(name), m_msgLevel(level), m_msgTime(mode), m_msgTimePoint(std::chrono::system_clock::now()) { }

	LoggerLevel& Message_Info::MsgLevel() {
		return m_msgLevel;
	}

	std::string& Message_Info::Name() {
		return m_name;
	}

	Message_Time& Message_Info::TimeDetails() {
		return m_msgTime;
	}

	void Message_Info::SetName(const std::string_view name) {
		m_name = name;
	}

	void Message_Info::SetMsgLevel(const LoggerLevel level) {
		m_msgLevel = level;
	}

	void Message_Info::SetTimePoint(std::chrono::system_clock::time_point tp) {
		m_msgTimePoint = tp;
	}

	std::chrono::system_clock::time_point Message_Info::MessageTimePoint() const {
		return m_msgTimePoint;
	}

	void Message_Info::SetTimeMode(const message_time_mode mode) {
		m_msgTime.SetTimeMode(mode);
	}

	message_time_mode Message_Info::TimeMode() {
		return m_msgTime.Mode();
	}

	const std::tm& Message_Info::TimeInfo() const {
		return m_msgTime.Cache();
	}

	std::string& Message_Info::Message() {
		return m_message;
	}

	size_t Message_Info::MessageSize() const {
		return m_message.size();
	}

	std::source_location& Message_Info::SourceLocation() {
		return source;
	}

}    // namespace serenity::msg_details