#pragma once

#include <serenity/Common.h>

#include <condition_variable>
#include <string_view>
#include <chrono>

namespace serenity::msg_details {

	struct ZoneThread
	{
		ZoneThread() = delete;
		explicit ZoneThread(const std::chrono::tzdb& db): timezoneDB(db), timeMutex(std::mutex {}), sysCache(std::chrono::sys_info {}) {
			sysCache = timezoneDB.current_zone()->get_info(std::chrono::system_clock::now());
		}
		ZoneThread(const ZoneThread&)            = delete;
		ZoneThread& operator=(const ZoneThread&) = delete;
		~ZoneThread()                            = default;

		const std::chrono::tzdb& timezoneDB;
		mutable std::mutex timeMutex;
		std::chrono::sys_info sysCache;
		std::jthread tzUpdateThread;
		std::condition_variable_any cv;
	};

	class Message_Time
	{
	      public:
		Message_Time() = delete;
		explicit Message_Time(message_time_mode mode);
		Message_Time(const Message_Time&)            = delete;
		Message_Time& operator=(const Message_Time&) = delete;
		~Message_Time();

		void CalculateCurrentYear(int yearOffset);
		std::string_view GetCurrentYearSV(bool shortened) const;
		void UpdateTimeDate(std::chrono::system_clock::time_point timePoint);
		void UpdateCache(std::chrono::system_clock::time_point timePoint);
		std::tm& Cache();
		message_time_mode& Mode();
		void SetTimeMode(message_time_mode mode);
		std::chrono::seconds& LastLogPoint();
		bool isLeapYear() const;
		int LeapYearsSinceEpoch(int yearIndex);
		const std::chrono::time_zone* GetTimeZone();
		bool IsDaylightSavings() const;
		std::chrono::minutes DaylightSavingsOffsetMin() const;
		std::chrono::seconds UtcOffset() const;
		const std::string& TimeZoneAbbrev() const;

	      private:
		message_time_mode m_mode;
		std::tm m_cache;
		std::chrono::seconds secsSinceLastLog;
		bool leapYear;
		int currentYear;
		std::string cachedShortYear;
		std::string cachedLongYear;
		bool isDaylightSavings;
		std::unique_ptr<ZoneThread> zoneHelper;

	      private:
		void UpdateTimeZoneInfoThread();
	};
}    // namespace serenity::msg_details