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
				std::string_view long_weekday;
				std::string_view short_weekday;
				int              dec_wkday;
				int              hour;
				int              min;
				int              sec;
				int              long_year { 0 };
				int              short_year { 0 };
				std::string_view long_month;
				std::string_view short_month;
				int              dec_month;
				int              day;
			};

			class Message_Time
			{
			      public:
				Message_Time( message_time_mode mode );
				std::string_view        WeekdayString( int weekdayIndex, bool shortened = false );
				int                     GetCurrentYear( int yearOffset, bool shortened = false );
				std::string_view        MonthString( int monthIndex, bool shortened = false );
				std::string             ZeroPadDecimal( int dec );
				Cached_Date_Time        UpdateCache( );
				std::string_view        DayHalf( int hour );
				const message_time_mode Mode( );
				void                    SetTimeMode( message_time_mode mode );


			      private:
				message_time_mode m_mode;
				std::time_t       m_time;
				std::tm *         t_struct = { };
				Cached_Date_Time  m_cache  = { };
				Cached_Date_Time  InitializeCache( std::tm *t );
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity