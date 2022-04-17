#include <serenity/MessageDetails/Message_Time.h>

#include <charconv>

#define C20_CHRONO_TEST 1
#if C20_CHRONO_TEST
	#include <iostream>
#endif
namespace serenity::msg_details {

	void Message_Time::UpdateTimeZoneInfoThread() {
		std::unique_lock<std::mutex> lock(timeMutex);
		auto stopToken   = tzUpdateThread.get_stop_token();
		auto stopWaiting = [ &, this ]() {
			return ((stopToken.stop_requested()) || (cleanUpThread.load()));
		};
		// TODO: Figure out how to handle any error that may be returned by reload_tzdb()
		while( !cleanUpThread.load() ) {
				cv.wait_until(lock, sysCache.end, stopWaiting);
				if( (stopToken.stop_requested() || cleanUpThread.load()) ) return;
				std::chrono::reload_tzdb();
				sysCache          = timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
				isDaylightSavings = (sysCache.save != std::chrono::minutes(0));
			}
	}

	Message_Time::Message_Time(message_time_mode mode): m_mode(mode), timezoneDB(std::chrono::get_tzdb()) {
		auto currentTimePoint { std::chrono::system_clock::now() };
		UpdateTimeDate(currentTimePoint);
		auto yr { m_cache.tm_year + 1900 };
		(yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0)) ? leapYear = true : leapYear = false;
		sysCache          = timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
		isDaylightSavings = (sysCache.save != std::chrono::minutes(0));
		tzUpdateThread    = std::jthread { &Message_Time::UpdateTimeZoneInfoThread, this };
	}

	Message_Time::~Message_Time() {
		cleanUpThread.store(true);
		cleanUpThread.notify_one();
		tzUpdateThread.request_stop();
		cv.notify_one();
	}

	const std::chrono::time_zone* Message_Time::GetTimeZone() {
		std::unique_lock<std::mutex> lock(timeMutex);
		return timezoneDB.current_zone();
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
		(m_mode == message_time_mode::local) ? LOCAL_TIME(m_cache, time) : GM_TIME(m_cache, time);
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

	// This much faster implementation than iterating naively in a loop was found at the following:
	// Ultra-fast-Algorithms-for-Working-with-Leap-Years blog by Ted Nguyen
	// https://stackoverflow.com/questions/4587513/how-to-calculate-number-of-leap-years-between-two-years-in-c-sharp Victor Haydin's answer
	int Message_Time::LeapYearsSinceEpoch(int yearIndex) {
		return (yearIndex / 4) - (yearIndex / 100) + (yearIndex / 400);
	}

	bool Message_Time::isLeapYear() const {
		return leapYear;
	}

	bool Message_Time::IsDaylightSavings() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return isDaylightSavings;
	}

	std::chrono::minutes Message_Time::DaylightSavingsOffsetMin() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return sysCache.save;
	}

	std::chrono::seconds Message_Time::UtcOffset() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return sysCache.offset;
	}
}    // namespace serenity::msg_details
