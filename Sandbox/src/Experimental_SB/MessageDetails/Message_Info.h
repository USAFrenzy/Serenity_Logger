#pragma once

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
				Message_Info( std::string_view name, LoggerLevel level, message_time_mode mode );
				Message_Info &operator=( const Message_Info &t );

				std::string &                         MessageBuffer( );
				LoggerLevel &                         MsgLevel( );
				std::string &                         Name( );
				Message_Time &                        TimeDetails( );
				void                                  SetName( const std::string name );
				void                                  SetMsgLevel( const LoggerLevel level );
				std::chrono::system_clock::time_point MessageTimePoint( );
				void                                  SetTimeMode( const message_time_mode mode );
				message_time_mode                     TimeMode( );
				std::tm &                             TimeInfo( );
				std::string &                         Message( );

				template <typename... Args> void SetMessage( const std::string_view message, Args &&...args )
				{
					m_message.clear( );
					using iterator = std::back_insert_iterator<std::basic_string<char>>;
					using context  = std::basic_format_context<iterator, char>;
					std::vformat_to( std::back_inserter( m_message ),
									 message,
									 std::make_format_args<context>( std::forward<Args>( args )... ) );
#ifdef WINDOWS_PLATFORM
					m_message.append( "\r\n" );
#else
					m_message.append( "\n" );
#endif  // WINDOWS_PLATFORM
				}

			  private:
				std::string  m_name;
				LoggerLevel  m_msgLevel;
				std::string  m_message;
				Message_Time m_msgTime;
				std::string  m_msgStrBuf;
			};
		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity