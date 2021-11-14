#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			class TargetBase
			{
			      public:
				TargetBase( );
				TargetBase( std::string_view name );
				TargetBase( std::string_view name, std::string_view msgPattern );
				~TargetBase( ) = default;

				std::string LoggerName( );
				void        SetPattern( std::string_view pattern );
				void        ResetPatternToDefault( );

				template <typename... Args> void trace( std::string s, Args &&...args );
				template <typename... Args> void info( std::string s, Args &&...args );
				template <typename... Args> void debug( std::string s, Args &&...args );
				template <typename... Args> void warn( std::string s, Args &&...args );
				template <typename... Args> void error( std::string s, Args &&...args );
				template <typename... Args> void fatal( std::string s, Args &&...args );


			      protected:
				virtual void PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args ) = 0;
				msg_details::Message_Formatter *MsgFmt( );
				msg_details::Message_Info *     MsgInfo( );

			      private:
				LoggerLevel                    msgLevel;
				std::string                    pattern;
				std::string                    loggerName;
				msg_details::Message_Info      msgDetails;
				msg_details::Message_Formatter msgPattern;
			};
#include "Target-impl.h"
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
