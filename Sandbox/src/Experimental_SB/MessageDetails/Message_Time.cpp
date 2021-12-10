#include "Message_Time.h"

#include <chrono>

namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Time::Message_Time( message_time_mode mode ) : m_mode( mode )
			{
				InitializeCache( UpdateTimeDate( ) );
			}

			std::string Message_Time::WeekdayString( int weekdayIndex, bool shortened )
			{
				if( !shortened ) {
					return long_weekdays.at( weekdayIndex );
				}
				else {
					return short_weekdays.at( weekdayIndex );
				}
			}

			int Message_Time::GetCurrentYear( int yearOffset, bool shortened )
			{
				if( !shortened ) {
					return 1900 + yearOffset;  //  Format XXXX, same as %Y strftime
				}
				else {
					return yearOffset - 100;  //  Format XX, Same as %y strftime
				}
			}

			std::string Message_Time::MonthString( int monthIndex, bool shortened )
			{
				if( !shortened ) {
					return long_months.at( monthIndex );
				}
				else {
					return short_months.at( monthIndex );
				}
			}

			std::string Message_Time::ZeroPadDecimal( int dec )
			{
				return ( dec >= 10 ) ? std::to_string( dec ) : "0" + std::to_string( dec );
			}

			std::tm *Message_Time::UpdateTimeDate( )
			{
				m_time    = std::chrono::system_clock( ).now( );
				auto time = std::chrono::system_clock::to_time_t( m_time );
				if( m_mode == message_time_mode::local ) {
					return t_struct = std::localtime( &time );
				}
				else {
					return t_struct = std::gmtime( &time );
				}
			}

			Cached_Date_Time Message_Time::UpdateCache( const std::tm *timeStruct )
			{
				return InitializeCache( std::move( timeStruct ) );
			}

			std::string Message_Time::DayHalf( int hour )
			{
				return ( hour >= 12 ) ? "PM" : "AM";
			}

			const message_time_mode Message_Time::Mode( )
			{
				return m_mode;
			}

			Cached_Date_Time Message_Time::InitializeCache( const std::tm *t )
			{
				m_cache.long_year        = std::move( GetCurrentYear( t->tm_year ) );
				m_cache.short_year       = std::move( GetCurrentYear( t->tm_year, true ) );
				m_cache.long_month       = std::move( MonthString( t->tm_mon ) );
				m_cache.dec_month        = std::move( ( t->tm_mon + 1 ) );
				m_cache.short_month      = std::move( MonthString( t->tm_mon, true ) );
				m_cache.long_weekday     = std::move( WeekdayString( t->tm_wday ) );
				m_cache.short_weekday    = std::move( WeekdayString( t->tm_wday, true ) );
				m_cache.dec_wkday        = t->tm_wday;
				m_cache.day              = t->tm_mday;
				m_cache.hour             = t->tm_hour;
				m_cache.min              = t->tm_min;
				m_cache.sec              = t->tm_sec;
				m_cache.secsSinceLastLog = std::chrono::duration_cast<std::chrono::seconds>( m_time.time_since_epoch( ) );
				return m_cache;
			}

			void Message_Time::SetTimeMode( message_time_mode mode )
			{
				m_mode = mode;
			}

			Cached_Date_Time Message_Time::Cache( )
			{
				return m_cache;
			}
		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity