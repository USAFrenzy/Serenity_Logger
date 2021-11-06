#include "ColorConsoleTarget.h"


namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			ColorConsole::ColorConsole( ) : coloredOutput( true )
			{
				SetLoggerName( "Console Logger" );
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
			ColorConsole::ColorConsole( std::string_view name ) : coloredOutput( true )
			{
				SetLoggerName( name );
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
			ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern ) : coloredOutput( true )
			{
				SetLoggerName( name );
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

			void ColorConsole::PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args )
			{
				MsgInfo( )->SetMessageLevel( level );

				buffer = std::vformat( msg, args );

				std::string_view msgColor = { };
				if( coloredOutput ) {
					msgColor = GetMsgColor( level );
				}
				std::cout << msgColor << MsgFmt( )->FmtMessage( buffer ) << se_colors::formats::reset << "\n";
			}
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity