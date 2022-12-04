#include <serenity/MessageDetails/Message_Time.h>

#include <charconv>

namespace serenity::msg_details {

	ZoneThread::ZoneThread(const std::chrono::tzdb& db)
		: timezoneDB(db), timeMutex(std::mutex {}), sysCache(std::chrono::sys_info {}), isThreadActive(false), isDaylightSavings(false) {
		sysCache          = timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
		isDaylightSavings = (sysCache.save != std::chrono::minutes(0));
	}

	void ZoneThread::UpdateTimeZoneInfoThread() {
		std::unique_lock<std::mutex> lock(timeMutex);
		auto stopToken = tzUpdateThread.get_stop_token();
		const int maxAttempt { 2 };

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
				cv.wait_until(lock, sysCache.end, [ &, this ]() { return stopToken.stop_requested(); });
				for( int tries { 0 }; tries <= maxAttempt; ++tries ) {
						// Check on each attempt if the thread is supposed to be wrapping up before trying to update
						if( stopToken.stop_requested() ) return;
						auto [ result, errStr ] = tryUpdating();
						if( result ) break;
						if( tries == maxAttempt ) {
								printf("%s\n", errStr.c_str());
								printf("Max Attempts Reached. Zone Update Thread Will Now Terminate Itself.\n");
								return;
						}
						printf("%s\nRetrying...\n", errStr.c_str());
					}
				sysCache          = timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
				isDaylightSavings = (sysCache.save != std::chrono::minutes(0));
			}
	}

	void ZoneThread::StartZoneThread() {
		if( !isThreadActive.load() ) {
				tzUpdateThread = std::jthread { &ZoneThread::UpdateTimeZoneInfoThread, this };
				isThreadActive.store(true);
		}
	}

	void ZoneThread::StopZoneThread() {
		if( isThreadActive.load() && tzUpdateThread.joinable() ) {
				tzUpdateThread.request_stop();
				cv.notify_one();
				isThreadActive.store(false);
		}
	}

	bool ZoneThread::IsDayLightSavings() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return isDaylightSavings;
	}

	std::chrono::minutes ZoneThread::DaylightSavingsOffsetMin() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return sysCache.save;
	}

	std::chrono::seconds ZoneThread::UtcOffset() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return sysCache.offset;
	}

	const std::string& ZoneThread::TimeZoneAbbrev() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return sysCache.abbrev;
	}

	void ZoneThread::EnableZoneThread(bool enabled) {
		std::unique_lock<std::mutex> lock(timeMutex);
		enabled ? StartZoneThread() : StopZoneThread();
	}

	bool ZoneThread::IsZoneThreadActive() const {
		return isThreadActive.load();
	}

	const std::chrono::time_zone* ZoneThread::TimeZone() const {
		std::unique_lock<std::mutex> lock(timeMutex);
		return timezoneDB.current_zone();
	}

	Message_Time::Message_Time(message_time_mode mode): m_mode(mode), zoneHelper(std::make_unique<ZoneThread>(std::chrono::get_tzdb())) {
		auto currentTimePoint { std::chrono::system_clock::now() };
		UpdateTimeDate(currentTimePoint);
		auto yr { m_cache.tm_year + 1900 };
		(yr % 4 == 0 && (yr % 100 != 0 || yr % 400 == 0)) ? leapYear = true : leapYear = false;
	}

	Message_Time::~Message_Time() {
		zoneHelper->EnableZoneThread(false);
	}

	const std::chrono::time_zone* Message_Time::GetTimeZone() const {
		return zoneHelper->TimeZone();
	}

	void serenity::msg_details::Message_Time::CalculateCurrentYear(int yearOffset) {
		auto longYear { 1900 + yearOffset };
		std::array<char, 6> buff {};
		cachedLongYear.clear();
		cachedLongYear.append(buff.data(), std::to_chars(buff.data(), buff.data() + 6, longYear).ptr);
		std::memset(buff.data(), 0, 4);
		auto shortYear { yearOffset - 100 };
		cachedShortYear.clear();
		cachedShortYear.append(buff.data(), std::to_chars(buff.data(), buff.data() + 6, shortYear).ptr);
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

	std::tm& Message_Time::Cache() const {
		return m_cache;
	}

	const message_time_mode& Message_Time::Mode() const {
		return m_mode;
	}

	void Message_Time::SetTimeMode(message_time_mode mode) {
		m_mode = mode;
	}

	const std::chrono::seconds& Message_Time::LastLogPoint() const {
		return secsSinceLastLog;
	}

	bool Message_Time::isLeapYear() const {
		return leapYear;
	}

	bool Message_Time::IsDaylightSavings() const {
		return zoneHelper->IsDayLightSavings();
	}

	std::chrono::minutes Message_Time::DaylightSavingsOffsetMin() const {
		return zoneHelper->DaylightSavingsOffsetMin();
	}

	std::chrono::seconds Message_Time::UtcOffset() const {
		return zoneHelper->UtcOffset();
	}

	const std::string& serenity::msg_details::Message_Time::TimeZoneAbbrev() const {
		return zoneHelper->TimeZoneAbbrev();
	}
	void Message_Time::EnableZoneThread(bool enabled) {
		zoneHelper->EnableZoneThread(enabled);
	}
	bool Message_Time::IsZoneThreadActive() const {
		return zoneHelper->IsZoneThreadActive();
	}
}    // namespace serenity::msg_details
