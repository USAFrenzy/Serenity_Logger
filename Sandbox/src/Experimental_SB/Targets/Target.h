#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Pattern.h"

namespace serenity
{
	namespace expiremental
	{
		class TargetBase
		{
		      public:
			TargetBase( )
			  : pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( "Base Logger", logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    logLevel( LoggerLevel::trace )
			{
			}

			TargetBase( std::string_view name )
			  : pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( std::move( svToString( name ) ), logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    logLevel( LoggerLevel::trace )
			{
			}


			TargetBase( std::string_view name, std::string_view msgPattern )
			  : pattern( std::move( svToString( msgPattern ) ) ),
			    msgDetails( std::move( svToString( name ) ), logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    logLevel( LoggerLevel::trace )
			{
			}


			~TargetBase( ) = default;

			template <typename... Args> inline void trace( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::trace, s, std::make_format_args( args... ) );
			}
			template <typename... Args> inline void info( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::info, s, std::make_format_args( args... ) );
			}
			template <typename... Args> inline void debug( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::debug, s, std::make_format_args( args... ) );
			}
			template <typename... Args> inline void warn( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::warning, s, std::make_format_args( args... ) );
			}
			template <typename... Args> inline void error( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::error, s, std::make_format_args( args... ) );
			}
			template <typename... Args> inline void fatal( std::string s, Args &&...args )
			{
				PrintMessage( LoggerLevel::fatal, s, std::make_format_args( args... ) );
			}


			std::string LoggerName( )
			{
				return loggerName;
			}

		      protected:
			msg_details::Message_Pattern *MsgFmt( )
			{
				return &msgPattern;
			}
			msg_details::Message_Info *MsgInfo( )
			{
				return &msgDetails;
			}

			virtual void PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args ) = 0;
			void         SetLoggerName( std::string_view name )
			{
				msgDetails.SetName( svToString( name ) );
			}

		      private:
			LoggerLevel                  logLevel;
			std::string                  pattern;
			std::string                  loggerName;
			msg_details::Message_Info    msgDetails;
			msg_details::Message_Pattern msgPattern;
		};
	}  // namespace expiremental
}  // namespace serenity