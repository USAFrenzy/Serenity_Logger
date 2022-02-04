#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"
#include "../MessageDetails/FlushPolicy.h"

#include <chrono>

namespace serenity::expiremental::targets
{
	/// @brief The base class that derived target classes inherit from. Contains common functions between all targets for
	/// message logging and logging settings.
	///
	/// @details For All TargetBase Constructors: \n
	/// The following default values are set regardless of which constructor is used: \n
	/// - Flush Policy is set to "never". \n
	/// - Log Level is set to "LoggerLevel::trace". \n
	/// - Default pattern is set to "|%l| %x %n %T [%N]: %+" which translates to the form of: \n
	///   "|T| Sat 29Jan22 [Base_Logger]: The message to log". \n
	/// - Time mode used is set to "time_mode::local".\n
	class TargetBase
	{
	  public:
		///
		/// @brief Default constructor that sets the logger name to "Base_Target" and sets all other values to their defaults
		///
		TargetBase( );
		/// @brief Constructor that will set the logger name to the name paramater passed in and all other values to their
		/// defaults.
		/// @param name: the name that the logger itself will use and be identified by
		TargetBase( std::string_view name );
		/// @brief Constructor that will set the logger name to the name paramater passed in as well as the format pattern to
		/// the msgPattern passed in. All other values will be assigned their default values.
		/// @param name: the name that the logger itself will use and be identified by
		/// @param msgPattern: the format pattern that determines how the prepended text will be displayed before the log
		/// message
		TargetBase( std::string_view name, std::string_view msgPattern );
		~TargetBase( ) = default;
		/// @brief Sets the current policy in use to defer to the policy passed in from "pPolicy".
		/// @param pPolicy: refers to any settings that are added or changed by the user, including whether to flush always,
		/// never, or periodically.
		/// @details On top of having options to never flush, always flush, and periodically flush, the Flush_Policy class
		/// includes the settings for whether flushing should occur based on a time-interval or log level if the periodical
		/// flushing option is enabled.
		void SetFlushPolicy( Flush_Policy pPolicy );
		///
		/// @brief Returns the current policy in use
		///
		Flush_Policy &Policy( );
		///
		/// @brief Returns the logger's name
		///
		const std::string LoggerName( );
		/// @brief Calls the handle to the Message_Formatter's SetPattern( ) function.
		/// @details 	Sets the format pattern variable and parses the format string for internal storage and usage of this
		/// pattern by initializing and moving the initialized formatter struct in charge of the respective flag to a
		/// Formatters container
		/// @param pattern - the format pattern to store. This pattern is what determines how the prepended text will be
		/// displayed before the log message
		void SetPattern( std::string_view pattern );
		///
		/// @brief Resets the current format pattern in use to the default format pattern
		/// 
		void ResetPatternToDefault( );
		/// @brief Sets the log level that messages should be logged at.
		/// @details For example, if "SetLogLevel(LoggerLevel::Error);"
		/// is used, then no messages below LoggerLevel::Error will be logged, however once a Fatal or Error message is made,
		/// then they would be logged to the output destination
		/// @param level - the logger level threshold that will determine if a message should be logged or not
		void SetLogLevel( LoggerLevel level );
		/// @brief Enables/Disables writing to a buffer.
		/// @details When writing to the buffer, messages will be appended with the
		/// platform-specific end of line before being added to the buffer. When Flush( ) is called, if writing to the buffer
		/// was enabled, the buffer contents will now be written to the file and then flushed to disk. Disabled by default.
		/// @param fmtToBuf: the value that controls whether or not buffer writes are enabled/disabled
		void WriteToBaseBuffer( bool fmtToBuf = true );
		///
		///  @brief Returns true if buffer writes are enabled and false if they are disabled
		///
		bool isWriteToBuf( );
		///
		///  @brief Returns the buffer container
		///
		std::string *Buffer( );
		///
		///  @brief Returns the current log level setting (the threshold of whether to log a message or not).
		///
		const LoggerLevel Level( );
		///  @brief Sets the name of the logger
		/// @param name: the name that the logger itself will use and be identified by
		void SetLoggerName( std::string_view name );
		/// @brief Logs a message giving the message a LoggerLevel::trace setting
		/// @details Checks if the message should be logged via the log level threshold setting. If it isn't, immediately
		/// returns. If the message should be logged, this function will then perform a quick check on whether or not writes
		/// to the buffer were enabled and will write to the buffer if they were. If writes to the buffers weren't enabled,
		/// then PrintMessage( ) will be called and the derived target handles how this is implemented. After writing the
		/// message, follows the derived target's PolicyFlushOn( ) implementation
		/// @tparam msg: The message being passed in. Follows C++20's format library and libfmt's substitution model using
		/// "{}" to replace any arguments from the \p args parameter.
		/// @tparam args: Variadic placeholder for any number of and any type of arguments to use in substituion.
		template <typename... Args> void Trace( std::string_view msg, Args &&...args );
		///
		/// @brief Logs a message giving the message a LoggerLevel::info setting
		/// @details @copydetails Trace()
		///
		template <typename... Args> void Info( std::string_view msg, Args &&...args );
		///
		/// @brief Logs a message giving the message a LoggerLevel::debug setting
		/// @details @copydetails Trace()
		///
		template <typename... Args> void Debug( std::string_view msg, Args &&...args );
		///
		/// @brief Logs a message giving the message a LoggerLevel::warning setting
		/// @details @copydetails Trace()
		///
		template <typename... Args> void Warn( std::string_view msg, Args &&...args );
		///
		/// @brief Logs a message giving the message a LoggerLevel::error setting
		/// @details @copydetails Trace()
		///
		template <typename... Args> void Error( std::string_view msg, Args &&...args );
		///
		/// @brief Logs a message giving the message a LoggerLevel::fatal setting
		/// @details @copydetails Trace()
		/// 
		template <typename... Args> void Fatal( std::string_view msg, Args &&...args );

	  protected:
		/// @brief Pure virtual function that all derived classes must implement and is used to write the formatted
		///  message to a specific target.
		/// @param formatted: The actual message in its entirety to send to the output destination.
		/// @details For built in targets, the message is formatted before reaching this point by calling Message_Info's
		/// SetMessage( ) function. PrintMessage( )'s intended usage is the last stop call before a log message is sent to
		/// its destination
		virtual void PrintMessage( std::string_view formatted ) = 0;
		/// @brief Virtual function that can be omitted if derived class has no need to implement and is used to handle
		/// how and when the derived target should flush its contents to disk.
		virtual void PolicyFlushOn( ) { }
		///
		///  @brief Returns a pointer to the handle for the Message_Formatter class instance
		///
		msg_details::Message_Formatter *MsgFmt( );
		///
		///  @brief Returns a pointer to the handle for the Message_Info class instance
		///
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
