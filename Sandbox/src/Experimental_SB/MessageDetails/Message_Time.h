#pragma once

#include "../Common.h"
#include <string_view>
#include <ctime>
#include <time.h>
#include <chrono>

namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			struct Cached_Date_Time
			{
				std::string          long_weekday;
				std::string          short_weekday;
				int                  dec_wkday;
				int                  hour;
				int                  min;
				int                  sec;
				int                  long_year { 0 };
				int                  short_year { 0 };
				std::string          long_month;
				std::string          short_month;
				int                  dec_month;
				int                  day;
				std::chrono::seconds secsSinceLastLog;
			};

			class Message_Time
			{
			  public:
				Message_Time( message_time_mode mode );
				std::string             WeekdayString( int weekdayIndex, bool shortened = false );
				int                     GetCurrentYear( int yearOffset, bool shortened = false );
				std::string             MonthString( int monthIndex, bool shortened = false );
				std::string             ZeroPadDecimal( int dec );
				std::tm *               UpdateTimeDate( );
				Cached_Date_Time        UpdateCache( const std::tm *timeStruct );
				Cached_Date_Time        Cache( );
				std::string             DayHalf( int hour );
				const message_time_mode Mode( );
				void                    SetTimeMode( message_time_mode mode );

			  private:
				message_time_mode                     m_mode;
				std::chrono::system_clock::time_point m_time;
				std::tm *                             t_struct = { };
				Cached_Date_Time                      m_cache  = { };

			  private:
				Cached_Date_Time InitializeCache( const std::tm *t );
			};
		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity