#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"
#include "../MessageDetails/FlushPolicy.h"

#include <chrono>

namespace serenity::expiremental::targets
{
	class TargetBase
	{
	  public:
		/// <summary>
		/// Default Constructor that will automatically set flush policy to "never", log level to "trace", name to "Base_Logger", time
		/// mode to local, and format pattern to the default pattern of "|%l| %x %n %T [%N]: %+" which translates to the form of "|T|
		/// Sat 29Jan22 [Base_Logger]: message to log"
		/// </summary>
		TargetBase( );
		/// <summary>
		/// Default Constructor that will automatically set flush policy to "never", log level to "trace", logger name to the name
		/// paramater passed in, time mode to local, and format pattern to the default pattern of "|%l| %x %n %T [%N]: %+" which
		/// translates to the form of "|T| Sat 29Jan22 [Parameter_Name]: message to log"
		/// </summary>
		TargetBase( std::string_view name );
		/// <summary>
		/// Default Constructor that will automatically set flush policy to "never", log level to "trace", logger name to the "name"
		/// paramater passed in, time mode to local, and format pattern to the "msgPattern" passed in
		/// </summary>
		TargetBase( std::string_view name, std::string_view msgPattern );
		/// <summary> Default Cleanup </summary>
		~TargetBase( ) = default;
		/// <summary>
		/// Sets the current policy in use to refer to the policy passed in from "pPolicy"
		/// </summary>
		/// <param name="pPolicy:"> pPolicy refers to any settings that are added or changed by the user, including whether to flush
		/// always, never, or periodically. If periodical flushing, also includes the settings for whether flushing should occur based
		/// on a time-interval or log level </param>
		void SetFlushPolicy( Flush_Policy pPolicy );
		/// <summary> returns the current policy in use </summary>
		Flush_Policy &Policy( );
		/// <summary> returns the logger's name </summary>
		const std::string LoggerName( );
		/// <summary>
		/// Calls the handle to the Message_Formatter's SetPattern( ) function which will set the format pattern variable and then
		/// parse the format string to store each flag as its own individual Formmater struct that will be in charge of how each flag
		/// is formatted
		/// </summary>
		void SetPattern( std::string_view pattern );
		/// <summary>
		/// Resets the current format pattern in use to the default format pattern of "|%l| %x %n %T [%N]: %+" which translates to the
		/// form of "|T| Sat 29Jan22 [Base_Logger]: message to log"
		/// </summary>
		void ResetPatternToDefault( );
		/// <summary>
		/// Sets the log level that messages should be logged at. For example, if SetLogLevel(LoggerLevel::Error); is used, then no
		/// messages below LoggerLevel::Error will be logged, however once a Fatal or Error message is called to log
		/// </summary>
		/// <param name="level"></param>
		void                             SetLogLevel( LoggerLevel level );
		void                             WriteToBaseBuffer( bool fmtToBuf = true );
		bool                             isWriteToBuf( );
		std::string *                    Buffer( );
		LoggerLevel                      Level( );
		void                             SetLoggerName( std::string_view name );
		template <typename... Args> void trace( std::string_view s, Args &&...args );
		template <typename... Args> void info( std::string_view s, Args &&...args );
		template <typename... Args> void debug( std::string_view s, Args &&...args );
		template <typename... Args> void warn( std::string_view s, Args &&...args );
		template <typename... Args> void error( std::string_view s, Args &&...args );
		template <typename... Args> void fatal( std::string_view s, Args &&...args );

	  protected:
		virtual void                    PrintMessage( std::string_view formatted ) = 0;
		virtual void                    PolicyFlushOn( ) { }
		msg_details::Message_Formatter *MsgFmt( );
		msg_details::Message_Info *     MsgInfo( );

	  private:
		bool                           toBuffer;
		Flush_Policy                   policy;
		LoggerLevel                    logLevel;
		LoggerLevel                    msgLevel;
		std::string                    pattern;
		msg_details::Message_Info      msgDetails;
		msg_details::Message_Formatter msgPattern;
		std::string                    internalBuffer;
	};
#include "Target-impl.h"
}  // namespace serenity::expiremental::targets
