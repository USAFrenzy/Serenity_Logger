#include <serenity/MessageDetails/Message_Time.h>

#include <charconv>

#define C20_CHRONO_TEST 1
#if C20_CHRONO_TEST
	#include <iostream>
#endif
namespace serenity::msg_details {

	void Message_Time::UpdateTimeZoneInfoThread() {
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		auto stopToken = zoneHelper->tzUpdateThread.get_stop_token();
		const int maxAttempt { 3 };
		bool success { false };

		auto tryUpdating = [ &, this ]() {
			try {
					std::chrono::reload_tzdb();
					return std::make_tuple<bool, std::string>(true, std::string());
				}
			catch( const std::runtime_error& re ) {
					return std::make_tuple<bool, std::string>(false, re.what());
				}
		};

		while( !stopToken.stop_requested() ) {
				zoneHelper->cv.wait_until(lock, zoneHelper->sysCache.end, [ &, this ]() { return stopToken.stop_requested(); });
				for( int tries { 0 }; tries < maxAttempt; ++tries ) {
						// explicit lock/unlock calls only due to thread sleep call between each attempt
						if( !lock.owns_lock() ) lock.lock();
						// Check on each attempt if the thread is supposed to be wrapping up before retrying again
						if( stopToken.stop_requested() ) return;
						auto [ result, errStr ] = tryUpdating();
						success                 = result;
						if( success ) break;
						if( tries == maxAttempt ) {
								printf("%s\n", errStr.c_str());
								printf("Zone Update Thread Will Now Terminate Itself\n");
								break;
						}
						lock.unlock();
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
				if( !success ) return;
				zoneHelper->sysCache = zoneHelper->timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
				isDaylightSavings    = (zoneHelper->sysCache.save != std::chrono::minutes(0));
				success              = false;    // reset for next iteration
			}
	}

	// TODO: Probably should move some of the now migrated 'ZoneThread' aspects into 'ZoneThread' functions. Also should add
	// TODO: something similar to the file's 'BackgroundThread' for enabling/disabling the background thread here; that way
	// TODO: only the aspects of this library that need zone info will initiate the thread (obviously checking that the thread
	// TODO: hasn't already been launched first) like the 'RotatingTarget' on anything but file size and like the %z/%Z flags.
	Message_Time::Message_Time(message_time_mode mode): m_mode(mode), zoneHelper(std::make_unique<ZoneThread>(std::chrono::get_tzdb())) {
		auto currentTimePoint { std::chrono::system_clock::now() };
		UpdateTimeDate(currentTimePoint);
		auto yr { m_cache.tm_year + 1900 };
		(yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0)) ? leapYear = true : leapYear = false;
		zoneHelper->sysCache       = zoneHelper->timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
		isDaylightSavings          = (zoneHelper->sysCache.save != std::chrono::minutes(0));
		// With the above TODO statements, probably should abstract this to only launch when needed
		zoneHelper->tzUpdateThread = std::jthread { &Message_Time::UpdateTimeZoneInfoThread, this };
	}

	Message_Time::~Message_Time() {
		// TODO: Abstract this to something like a 'CleanUpZoneThread()'
		if( zoneHelper->tzUpdateThread.joinable() ) {
				zoneHelper->tzUpdateThread.request_stop();
				zoneHelper->cv.notify_one();
		}
	}

	const std::chrono::time_zone* Message_Time::GetTimeZone() {
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		return zoneHelper->timezoneDB.current_zone();
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
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		return isDaylightSavings;
	}

	std::chrono::minutes Message_Time::DaylightSavingsOffsetMin() const {
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		return zoneHelper->sysCache.save;
	}

	std::chrono::seconds Message_Time::UtcOffset() const {
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		return zoneHelper->sysCache.offset;
	}

	const std::string& serenity::msg_details::Message_Time::TimeZoneAbbrev() const {
		std::unique_lock<std::mutex> lock(zoneHelper->timeMutex);
		return zoneHelper->sysCache.abbrev;
	}
}    // namespace serenity::msg_details
