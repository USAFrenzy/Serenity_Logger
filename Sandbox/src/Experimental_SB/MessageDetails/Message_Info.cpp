#include "Message_Info.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Info::Message_Info( )
			  : m_name( ), msgLevel( LoggerLevel::trace ), msg( ), msgTime( message_time_mode::local )
			{
				time = std::chrono::system_clock::now( ).time_since_epoch( );
			}

			Message_Info::Message_Info( std::string name, LoggerLevel level, message_time_mode mode )
			  : m_name( name ), msgLevel( level ), msg( ), msgTime( mode )
			{
				time = std::chrono::system_clock::now( ).time_since_epoch( );
			}

			Message_Info &Message_Info::operator=( const Message_Info &t )
			{
				msg      = t.msg;
				msgLevel = t.msgLevel;
				msgTime  = t.msgTime;
				m_name   = t.m_name;
				time     = t.time;
				return *this;
			}

			LoggerLevel Message_Info::MsgLevel( )
			{
				return msgLevel;
			}

			void Message_Info::SetName( std::string name )
			{
				m_name = name;
			}

			void Message_Info::SetMsgLevel( LoggerLevel level )
			{
				msgLevel = level;
			}
			std::chrono::system_clock::duration Message_Info::MessageTimePoint( )
			{
				return time = std::chrono::system_clock::now( ).time_since_epoch( );
			}

			std::string Message_Info::Name( )
			{
				return m_name;
			}

			Message_Time Message_Info::TimeDetails( )
			{
				return msgTime;
			}

		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity