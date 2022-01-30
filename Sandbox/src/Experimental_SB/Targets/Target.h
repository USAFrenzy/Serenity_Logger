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
		// clang-format off
		/*************************************************************************************************************//**
		 *                                For All Taarget Base Constructors:
		 * The default following default values are set regardless of which constructor is used:
		 * - Flush Policy is set to "never"
		 * - Log Level is set to "LoggerLevel::trace"
		 * - Default pattern is set to "|%l| %x %n %T [%N]: %+" which translates to the form of:
		 *    "|T| Sat 29Jan22 [Base_Logger]: message to log"
		 * - Time mode used is set to "time_mode::local"
		 ***************************************************************************************************************/
		// clang-format on

		TargetBase( );
		/// Constructor that will set the logger name to the name paramater passed in
		TargetBase( std::string_view name );
		/// Constructor that will set logger name to the "name" paramater passed in, and format pattern to the "msgPattern"
		/// passed in
		TargetBase( std::string_view name, std::string_view msgPattern );
		///  Default Cleanup
		~TargetBase( ) = default;
		/// Sets the current policy in use to refer to the policy passed in from "pPolicy".
		/// "pPolicy" refers to any settings that are added or changed by the user, including whether to flush always, never,
		/// or periodically. If periodical flushing, also includes the settings for whether flushing should occur based on a
		/// time-interval or log level.
		void SetFlushPolicy( Flush_Policy pPolicy );
		///  returns the current policy in use
		Flush_Policy &Policy( );
		///  returns the logger's name
		const std::string LoggerName( );
		/// Calls the handle to the Message_Formatter's SetPattern( ) function which will set the format pattern variable and
		/// then parse the format string to store each flag as its own individual Formmater struct that will be in charge of
		/// how each flag is formatted
		void SetPattern( std::string_view pattern );
		/// Resets the current format pattern in use to the default format pattern of "|%l| %x %n %T [%N]: %+"
		/// which translates to the form of "|T| Sat 29Jan22 [Base_Logger]: message to log"
		void ResetPatternToDefault( );
		/// Sets the log level that messages should be logged at. For example, if "SetLogLevel(LoggerLevel::Error);"
		/// is used, then no messages below LoggerLevel::Error will be logged, however once a Fatal or Error message is made,
		/// then they would be logged to the output destination
		void SetLogLevel( LoggerLevel level );
		/// Enables/Disables writing to a buffer. When writing to the buffer, messages will be appended with the
		/// platform-specific end of line before being added to the buffer. When Flush( ) is called, if writing to the buffer
		/// was enabled, the buffer contents will now be written to the file and then flushed to disk. Disabled by default.
		void WriteToBaseBuffer( bool fmtToBuf = true );
		///  Returns true if buffer writes are enabled and false if they are disabled
		bool isWriteToBuf( );
		///  Returns the buffer container
		std::string *Buffer( );
		///  Returns the current log level setting (the one used as a threshold of whether to make a log or not)
		const LoggerLevel Level( );
		///  Sets the name of the logger
		void SetLoggerName( std::string_view name );

		// clang-format off
		/**************************************************************************************************************//**
		*            For the logging functions Trace( ), Info( ), Debug( ), Warn( ), Error( ), and Fatal( ):                
		******************************************************************************************************************/
		/// Checks if the message should be logged via the log level threshold setting. If it isn't, immediately returns. If
		/// it is, this function will then perform a quick check on whether or not writes to the buffer were enabled and will
		/// write to the buffer if they were. If writes to the buffers weren't enabled, then PrintMessage( ) will be called
		/// and the target handles how this is implemented. After writing the message, will follow the target specific
		/// PolicyFlushOn( ) implementation
		/// Parameter 's': The message being passed in. Follows C++20 format and libfmt's substitution model using "{}"
		/// Parameter '...args': Variadic placeholder for any number of and any type of arguments to use in substituion
		// clang-format on

		template <typename... Args> void Trace( std::string_view s, Args &&...args );
		template <typename... Args> void Info( std::string_view s, Args &&...args );
		template <typename... Args> void Debug( std::string_view s, Args &&...args );
		template <typename... Args> void Warn( std::string_view s, Args &&...args );
		template <typename... Args> void Error( std::string_view s, Args &&...args );
		template <typename... Args> void Fatal( std::string_view s, Args &&...args );

	  protected:
		///  Pure virtual function that all derived classes must implement. Used to write the formatted message to a specific
		///  target
		virtual void PrintMessage( std::string_view formatted ) = 0;
		/// Virtual function that can be omitted if derived class has no need to implement. This function is used to handle
		/// how and when the target should flush to disk
		virtual void PolicyFlushOn( ) { }
		///  Returns a pointer to the handle for the Message_Formatter class instance
		msg_details::Message_Formatter *MsgFmt( );
		///  Returns a pointer to the handle for the Message_Info class instance
		msg_details::Message_Info *MsgInfo( );

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
