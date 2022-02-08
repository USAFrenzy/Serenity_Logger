#include <serenity/MessageDetails/Message_Time.h>

namespace serenity::msg_details
{
	Message_Time::Message_Time( message_time_mode mode ) : m_mode( mode )
	{
		m_mode = mode;
		UpdateTimeDate( std::chrono::system_clock::now( ) );
	}

	std::string_view Message_Time::GetCurrentYearSV( int yearOffset, bool shortened )
	{
		if( !shortened ) {
			auto year { 1900 + yearOffset };
			// could be clever here in order to use LUT instead of vformat - is it worth? Probs not
			return std::move( std::vformat( "{}", std::make_format_args( year ) ) );
		} else {
			auto year { yearOffset - 100 };
			return SERENITY_LUTS::numberStr[ year ];
		}
	}

	void Message_Time::UpdateTimeDate( std::chrono::system_clock::time_point timePoint )
	{
		auto time { std::chrono::system_clock::to_time_t( timePoint ) };
		secsSinceLastLog = std::chrono::duration_cast<std::chrono::seconds>( timePoint.time_since_epoch( ) );
		( m_mode == message_time_mode::local ) ? localtime_s( &m_cache, &time ) : gmtime_s( &m_cache, &time );
	}

	void Message_Time::UpdateCache( std::chrono::system_clock::time_point timePoint )
	{
		UpdateTimeDate( timePoint );
	}

	std::tm &Message_Time::Cache( )
	{
		return m_cache;
	}

	message_time_mode &Message_Time::Mode( )
	{
		return m_mode;
	}

	void Message_Time::SetTimeMode( message_time_mode mode )
	{
		m_mode = mode;
	}

	std::chrono::seconds &Message_Time::LastLogPoint( )
	{
		return secsSinceLastLog;
	}

}  // namespace serenity::msg_details
