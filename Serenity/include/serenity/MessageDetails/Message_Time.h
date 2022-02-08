#pragma once

#include <serenity/Common.h>

#include <string_view>
#include <chrono>

namespace serenity::msg_details
{
	class Message_Time
	{
	  public:
		Message_Time( ) = delete;
		explicit Message_Time( message_time_mode mode );
		Message_Time( const Message_Time & ) = delete;
		Message_Time &operator=( const Message_Time & ) = delete;
		~Message_Time( )                                = default;

		std::string_view      GetCurrentYearSV( int yearOffset, bool shortened = false );
		void                  UpdateTimeDate( std::chrono::system_clock::time_point timePoint );
		void                  UpdateCache( std::chrono::system_clock::time_point timePoint );
		std::tm &             Cache( );
		message_time_mode &   Mode( );
		void                  SetTimeMode( message_time_mode mode );
		std::chrono::seconds &LastLogPoint( );

	  private:
		message_time_mode    m_mode;
		std::tm              m_cache;
		std::chrono::seconds secsSinceLastLog;
	};
}  // namespace serenity::experimental::msg_details