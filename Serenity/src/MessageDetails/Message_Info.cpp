#include <serenity/MessageDetails/Message_Info.h>

namespace serenity::msg_details {
	Message_Info::Message_Info(std::string_view name, LoggerLevel level, message_time_mode mode)
		: m_name(name), m_msgLevel(level), m_msgTime(mode)
	{
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
	}

	LoggerLevel &Message_Info::MsgLevel()
	{
		return m_msgLevel;
	}

	std::string &Message_Info::Name()
	{
		return m_name;
	}

	Message_Time &Message_Info::TimeDetails()
	{
		return m_msgTime;
	}

	void Message_Info::SetName(const std::string_view name)
	{
		m_name = name;
	}

	void Message_Info::SetMsgLevel(const LoggerLevel level)
	{
		m_msgLevel = level;
	}

	std::chrono::system_clock::time_point Message_Info::MessageTimePoint()
	{
		return std::chrono::system_clock::now();
	}

	void Message_Info::SetTimeMode(const message_time_mode mode)
	{
		m_msgTime.SetTimeMode(mode);
	}

	message_time_mode Message_Info::TimeMode()
	{
		return m_msgTime.Mode();
	}

	std::tm &Message_Info::TimeInfo()
	{
		return m_msgTime.Cache();
	}

	std::string &Message_Info::Message()
	{
		return m_message;
	}

	const size_t Message_Info::MessageSize()
	{
		return m_message.size();
	}
}    // namespace serenity::msg_details