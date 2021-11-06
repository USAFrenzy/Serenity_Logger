#include "Message_Info.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Info::Message_Info( std::string name, LoggerLevel level, message_time_mode mode )
			  : m_name( name ), msgLevel( level ), msg( ), msgTime( mode )
			{
			}

			Message_Info &Message_Info::operator=( const Message_Info &t )
			{
				msg      = t.msg;
				msgLevel = t.msgLevel;
				msgTime  = t.msgTime;
				m_name   = t.m_name;
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

			std::string Message_Info::Name( )
			{
				return m_name;
			}

			Message_Time Message_Info::TimeDetails( )
			{
				return msgTime;
			}

			void Message_Info::SetMessageLevel( LoggerLevel messageLvl )
			{
				msgLevel = messageLvl;
			}

		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity