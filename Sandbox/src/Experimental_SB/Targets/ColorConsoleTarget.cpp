#include "ColorConsoleTarget.h"

#include "../Common.h"  // DB_PRINT()

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			ColorConsole::ColorConsole( )
			  : logLevel( LoggerLevel::trace ),
			    TargetBase( "Console Logger" ),
			    consoleMode( console_interface::std_out ),
			    coloredOutput( true )
			{
				SetOriginalColors( );
			}

			ColorConsole::ColorConsole( std::string_view name )
			  : logLevel( LoggerLevel::trace ),
			    TargetBase( name ),
			    consoleMode( console_interface::std_out ),
			    coloredOutput( true )
			{
				SetOriginalColors( );
			}

			ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
			  : logLevel( LoggerLevel::trace ),
			    TargetBase( name, msgPattern ),
			    consoleMode( console_interface::std_out ),
			    coloredOutput( true )
			{
				SetOriginalColors( );
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

			void ColorConsole::SetConsoleMode( console_interface mode )
			{
				consoleMode = mode;
			}

			console_interface ColorConsole::GetConsoleMode( )
			{
				return consoleMode;
			}

			void ColorConsole::PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args )
			{
				if( logLevel <= level ) {
					MsgInfo( )->SetMessageLevel( level );
					std::string_view msgColor;
					if( coloredOutput ) {
						msgColor = GetMsgColor( level );
					}
					if( consoleMode == console_interface::std_out ) {
						std::cout << msgColor << MsgFmt( )->FormatMessage( msg, args )
							  << se_colors::formats::reset << "\n";
					}
					else {
						std::cerr << msgColor << MsgFmt( )->FormatMessage( msg, args )
							  << se_colors::formats::reset << "\n";
					}
				}
				else {
					return;
				}
			}

			void ColorConsole::SetLogLevel( LoggerLevel level )
			{
				logLevel = level;
			}

			void ColorConsole::SetOriginalColors( )
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

		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity