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

				LoggerLevel          MsgLevel( );
				std::string          Name( );
				Message_Time         TimeDetails( );
				void                 SetName( std::string name );
				void                 SetMsgLevel( LoggerLevel level );
				std::chrono::seconds MessageTimePoint( );
				void                 SetTimeMode( message_time_mode mode );
				const message_time_mode    TimeMode( );
				const std::tm *         TimeInfo( );
			      private:
				std::chrono::seconds logTime;
				std::string          m_name;
				LoggerLevel          msgLevel;
				std::string_view     msg;
				Message_Time         msgTime;
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity