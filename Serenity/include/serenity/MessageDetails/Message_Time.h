#pragma once

#include <serenity/Common.h>

#include <string_view>
#include <chrono>

namespace serenity::msg_details {
	class Message_Time
	{
	      public:
		Message_Time() = delete;
		explicit Message_Time(message_time_mode mode);
		Message_Time(const Message_Time&)            = delete;
		Message_Time& operator=(const Message_Time&) = delete;
		~Message_Time()                              = default;

		void CalculateCurrentYear(int yearOffset);
		std::string_view GetCurrentYearSV(bool shortened) const;
		void UpdateTimeDate(std::chrono::system_clock::time_point timePoint);
		void UpdateCache(std::chrono::system_clock::time_point timePoint);
		std::tm& Cache();
		message_time_mode& Mode();
		void SetTimeMode(message_time_mode mode);
		std::chrono::seconds& LastLogPoint();
		const bool isLeapYear();

	      private:
		message_time_mode m_mode;
		std::tm m_cache;
		std::chrono::seconds secsSinceLastLog;
		bool leapYear;
		int currentYear;
		std::string cachedShortYear;
		std::string cachedLongYear;
	};
}    // namespace serenity::msg_details