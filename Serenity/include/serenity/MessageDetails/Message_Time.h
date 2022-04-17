#pragma once

#include <serenity/Common.h>

#include <condition_variable>
#include <string_view>
#include <chrono>

namespace serenity::msg_details {

	class ZoneThread
	{
	      public:
		ZoneThread() = delete;
		explicit ZoneThread(const std::chrono::tzdb& db);
		ZoneThread(const ZoneThread&)            = delete;
		ZoneThread& operator=(const ZoneThread&) = delete;
		~ZoneThread()                            = default;

		void UpdateTimeZoneInfoThread();
		void StartZoneThread();
		void StopZoneThread();
		bool IsDayLightSavings() const;
		std::chrono::minutes DaylightSavingsOffsetMin() const;
		std::chrono::seconds UtcOffset() const;
		const std::string& TimeZoneAbbrev() const;
		const std::chrono::time_zone* TimeZone() const;
		void EnableZoneThread(bool enabled = true);
		bool IsZoneThreadActive() const;

	      private:
		const std::chrono::tzdb& timezoneDB;
		mutable std::mutex timeMutex;
		std::chrono::sys_info sysCache;
		std::atomic<bool> isThreadActive;
		bool isDaylightSavings;
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
		bool isLeapYear() const;
		int LeapYearsSinceEpoch(int yearIndex);
		bool IsDaylightSavings() const;
		void UpdateTimeDate(std::chrono::system_clock::time_point timePoint);
		void UpdateCache(std::chrono::system_clock::time_point timePoint);
		const std::tm& Cache() const;
		void SetTimeMode(message_time_mode mode);
		const message_time_mode& Mode() const;
		const std::chrono::seconds& LastLogPoint() const;
		const std::chrono::time_zone* GetTimeZone() const;
		const std::string& TimeZoneAbbrev() const;
		std::chrono::minutes DaylightSavingsOffsetMin() const;
		std::chrono::seconds UtcOffset() const;
		void EnableZoneThread(bool enabled = true);
		bool IsZoneThreadActive() const;

	      private:
		message_time_mode m_mode;
		std::tm m_cache;
		std::chrono::seconds secsSinceLastLog;
		bool leapYear;
		int currentYear;
		std::string cachedShortYear;
		std::string cachedLongYear;
		std::unique_ptr<ZoneThread> zoneHelper;
	};
}    // namespace serenity::msg_details