#include "Message_Time.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Time::Message_Time( message_time_mode mode ) : m_mode( mode )
			{
				UpdateTimeInfo( );
			}

			std::string_view Message_Time::WeekdayString( int weekdayIndex, bool shortened )
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

			std::string_view Message_Time::MonthString( int monthIndex, bool shortened )
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

			Cached_Date_Time Message_Time::UpdateTimeInfo( )
			{
				// For both modes, check if the cache has been initialized, if so, then just update time variables. If
				// not, then populate t_struct with time date variables and initialize cache
				if( m_cache.initialized ) {
					time( &m_time );
					if( m_mode == message_time_mode::local ) {
						t_struct = std::localtime( &m_time );
					}
					else {
						t_struct = std::gmtime( &m_time );
					}
					m_cache.hour = t_struct->tm_hour;
					m_cache.min  = t_struct->tm_min;
					m_cache.sec  = t_struct->tm_sec;
					return m_cache;
				}
				else {
					m_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now( ) );
					if( m_mode == message_time_mode::local ) {
						t_struct = std::localtime( &m_time );
						InitializeCache( t_struct );
						return m_cache;
					}
					else {
						t_struct = std::gmtime( &m_time );
						InitializeCache( t_struct );
						return m_cache;
					}
				}
			}

			const message_time_mode Message_Time::Mode( )
			{
				return m_mode;
			}

			const Cached_Date_Time Message_Time::Cache( )
			{
				return m_cache;
			}

			void Message_Time::InitializeCache( std::tm *t )
			{
				m_cache.long_year     = std::move( GetCurrentYear( t->tm_year ) );
				m_cache.short_year    = std::move( GetCurrentYear( t->tm_year, true ) );
				m_cache.long_month    = std::move( MonthString( t->tm_mon ) );
				m_cache.dec_month     = std::move( ( t->tm_mon + 1 ) );
				m_cache.short_month   = std::move( MonthString( t->tm_mon, true ) );
				m_cache.long_weekday  = std::move( WeekdayString( t->tm_wday ) );
				m_cache.short_weekday = std::move( WeekdayString( t->tm_wday, true ) );
				m_cache.dec_wkday     = t->tm_wday;
				m_cache.day           = t->tm_mday;
				m_cache.hour          = t->tm_hour;
				m_cache.min           = t->tm_min;
				m_cache.sec           = t->tm_sec;
				m_cache.initialized   = true;
			}
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity