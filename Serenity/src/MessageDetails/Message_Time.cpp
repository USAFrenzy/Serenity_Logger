#include <serenity/MessageDetails/Message_Time.h>

#include <charconv>

namespace serenity::msg_details {
	Message_Time::Message_Time(message_time_mode mode): m_mode(mode) {
		auto year { m_cache.tm_year };
		UpdateTimeDate(std::chrono::system_clock::now());
		if( (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 ) {
				leapYear = true;
		} else {
				leapYear = false;
			}
	}

	void serenity::msg_details::Message_Time::CalculateCurrentYear(int yearOffset) {
		auto longYear { 1900 + yearOffset };
		std::array<char, 4> buff {};
		std::to_chars(buff.data(), buff.data() + buff.size(), longYear);
		cachedLongYear.clear();
		cachedLongYear.append(buff.data(), buff.size());
		auto shortYear { yearOffset - 100 };
		cachedShortYear.clear();
		cachedShortYear.append(SERENITY_LUTS::numberStr[ shortYear ]);
	}

	std::string_view Message_Time::GetCurrentYearSV(bool shortened) const {
		return shortened ? cachedShortYear : cachedLongYear;
	}

	void Message_Time::UpdateTimeDate(std::chrono::system_clock::time_point timePoint) {
		auto time { std::chrono::system_clock::to_time_t(timePoint) };
		secsSinceLastLog = std::chrono::duration_cast<std::chrono::seconds>(timePoint.time_since_epoch());
		(m_mode == message_time_mode::local) ? localtime_s(&m_cache, &time) : gmtime_s(&m_cache, &time);
		currentYear = m_cache.tm_year;
		CalculateCurrentYear(currentYear);
	}

	void Message_Time::UpdateCache(std::chrono::system_clock::time_point timePoint) {
		UpdateTimeDate(timePoint);
	}

	std::tm& Message_Time::Cache() {
		return m_cache;
	}

	message_time_mode& Message_Time::Mode() {
		return m_mode;
	}

	void Message_Time::SetTimeMode(message_time_mode mode) {
		m_mode = mode;
	}

	std::chrono::seconds& Message_Time::LastLogPoint() {
		return secsSinceLastLog;
	}

	const bool Message_Time::isLeapYear() {
		return leapYear;
	}

}    // namespace serenity::msg_details
