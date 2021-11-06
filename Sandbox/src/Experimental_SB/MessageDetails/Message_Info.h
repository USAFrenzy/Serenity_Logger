#pragma once
#include <string>
#include "../Common.h"
#include "Message_Time.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			class Message_Info
			{
			      public:
				Message_Info( );
				Message_Info( std::string name, LoggerLevel level, message_time_mode mode );
				Message_Info &operator=( const Message_Info &t );
				LoggerLevel   MsgLevel( );
				std::string   Name( );
				Message_Time  TimeDetails( );
				void          SetMessageLevel( LoggerLevel messageLvl );

				void SetName( std::string name );
				void SetMsgLevel( LoggerLevel level );


			      private:
				std::string      m_name;
				LoggerLevel      msgLevel;
				std::string_view msg;
				Message_Time     msgTime;
			};

		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity