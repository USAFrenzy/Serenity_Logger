#include "ColorConsoleTarget.h"


namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			ColorConsole::ColorConsole( )
			  : loggerName( std::move( svToString( "Console_Logger" ) ) ),
			    pattern( std::move( svToString( "|%l| %x %n %T [%N]: " ) ) ),
			    msgDetails( loggerName, logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    coloredOutput( true ),
			    logLevel( LoggerLevel::trace )
			{
				msgLevelColors = {
				  { LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
				  { LoggerLevel::info, se_colors::bright_colors::foreground::green },
				  { LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
				  { LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
				  { LoggerLevel::error, se_colors::basic_colors::foreground::red },
				  { LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
				  { LoggerLevel::off, se_colors::formats::reset },
				};
			}
			ColorConsole::ColorConsole( std::string_view name )
			  : loggerName( std::move( svToString( name ) ) ),
			    pattern( std::move( svToString( "|%l| %x %n %T [%N]: " ) ) ),
			    msgDetails( loggerName, logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    coloredOutput( true ),
			    logLevel( LoggerLevel::trace )
			{
				loggerName     = std::move( name );
				msgLevelColors = {
				  { LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
				  { LoggerLevel::info, se_colors::bright_colors::foreground::green },
				  { LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
				  { LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
				  { LoggerLevel::error, se_colors::basic_colors::foreground::red },
				  { LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
				  { LoggerLevel::off, se_colors::formats::reset },
				};
			}
			ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
			  : loggerName( std::move( svToString( name ) ) ),
			    pattern( std::move( svToString( msgPattern ) ) ),
			    msgDetails( loggerName, logLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    coloredOutput( true ),
			    logLevel( LoggerLevel::trace )
			{
				loggerName     = std::move( name );
				msgLevelColors = {
				  { LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
				  { LoggerLevel::info, se_colors::bright_colors::foreground::green },
				  { LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
				  { LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
				  { LoggerLevel::error, se_colors::basic_colors::foreground::red },
				  { LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
				  { LoggerLevel::off, se_colors::formats::reset },
				};
			}
			void ColorConsole::SetMsgColor( LoggerLevel level, std::string_view color )
			{
				msgLevelColors.at( level ) = color;
			}
			std::string_view ColorConsole::GetMsgColor( LoggerLevel level )
			{
				return msgLevelColors.at( level );
			}
			void ColorConsole::ColorizeOutput( bool colorize )
			{
				coloredOutput = colorize;
			}
			bool ColorConsole::ShouldLog( LoggerLevel level )
			{
				return ( logLevel <= msgDetails.MsgLevel( ) ) ? true : false;
			}
			std::string ColorConsole::svToString( const std::string_view s )
			{
				return std::string( s.data( ), s.size( ) );
			}
			std::string ColorConsole::Reset( )
			{
				return se_colors::formats::reset;
			}
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity