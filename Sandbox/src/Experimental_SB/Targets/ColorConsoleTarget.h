#pragma once

#include "Target.h"

#include <serenity/Color/Color.h>
#include <unordered_map>
#include <iostream>

#include <iterator>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the
			// project I'll be using this in (< win 8.1 EOL).
			class ColorConsole : public TargetBase
			{
			      public:
				ColorConsole( );
				ColorConsole( std::string_view name );
				ColorConsole( std::string_view name, std::string_view msgPattern );

				void PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args ) override;

				std::string_view GetMsgColor( LoggerLevel level );
				void             SetMsgColor( LoggerLevel level, std::string_view color );
				void             ColorizeOutput( bool colorize );

			      private:
				bool                                              coloredOutput;
				std::unordered_map<LoggerLevel, std::string_view> msgLevelColors;
			};  // class ColorConsole
		}           // namespace targets
	}                   // namespace expiremental
}  // namespace serenity
