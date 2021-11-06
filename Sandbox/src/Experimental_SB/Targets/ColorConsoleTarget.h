#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <iostream>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the
			// project I'll be using this in (< win 8.1 EOL).
			class ColorConsole
			{
			      public:
				ColorConsole( );
				ColorConsole( std::string_view name );
				ColorConsole( std::string_view name, std::string_view msgPattern );

				void                             SetMsgColor( LoggerLevel level, std::string_view color );
				std::string_view                 GetMsgColor( LoggerLevel level );
				void                             ColorizeOutput( bool colorize );
				bool                             ShouldLog( LoggerLevel level );
				template <typename... Args> void PrintMessage( LoggerLevel level, const std::string msg, Args... args )
				{
					msgDetails.SetMessageLevel( level );

					std::string_view msgColor;
					if( coloredOutput ) {
						msgColor = GetMsgColor( level );
					}
					std::cout << msgColor << msgPattern.FmtMessage( msg, std::forward<Args>( args )... )
						  << Reset( ) << "\n";
				}

				// Not Apart Of This Class - Just Here For Testing (Part Of Logger Class)
				// ##########################################################################
				template <typename... Args> inline void trace( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::trace ) ) {
						PrintMessage( LoggerLevel::trace, s, std::forward<Args>( args )... );
					}
				}
				template <typename... Args> inline void info( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::info ) ) {
						PrintMessage( LoggerLevel::info, s, std::forward<Args>( args )... );
					}
				}
				template <typename... Args> inline void debug( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::debug ) ) {
						PrintMessage( LoggerLevel::debug, s, std::forward<Args>( args )... );
					}
				}
				template <typename... Args> inline void warn( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::warning ) ) {
						PrintMessage( LoggerLevel::warning, s, std::forward<Args>( args )... );
					}
				}
				template <typename... Args> inline void error( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::error ) ) {
						PrintMessage( LoggerLevel::error, s, std::forward<Args>( args )... );
					}
				}
				template <typename... Args> inline void fatal( std::string s, Args &&...args )
				{
					if( ShouldLog( LoggerLevel::fatal ) ) {
						PrintMessage( LoggerLevel::fatal, s, std::forward<Args>( args )... );
					}
				}
				// ##########################################################################
				std::string svToString( const std::string_view s );

			      private:
				std::string Reset( );


			      private:
				std::string                                       loggerName;
				std::string                                       pattern;
				msg_details::Message_Info                         msgDetails;
				msg_details::Message_Pattern                      msgPattern;
				bool                                              coloredOutput;
				LoggerLevel                                       logLevel;
				std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
			};

			// class ColorConsole
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
