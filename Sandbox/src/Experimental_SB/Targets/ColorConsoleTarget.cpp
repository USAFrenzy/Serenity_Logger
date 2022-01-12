#include "ColorConsoleTarget.h"

#include <iostream>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			ColorConsole::ColorConsole( )
			  : TargetBase( "Console Logger" ), consoleMode( console_interface::std_out ), coloredOutput( true )
			{
				WriteToBaseBuffer( false );
				SetOriginalColors( );
#ifdef WINDOWS_PLATFORM
				WinConsoleMode( consoleMode );
#endif  // WINDOWS_PLATFORM
			}

			ColorConsole::ColorConsole( std::string_view name )
			  : TargetBase( name ), consoleMode( console_interface::std_out ), coloredOutput( true )
			{
				WriteToBaseBuffer( false );
				SetOriginalColors( );
#ifdef WINDOWS_PLATFORM
				WinConsoleMode( consoleMode );
#endif  // WINDOWS_PLATFORM
			}

			ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
			  : TargetBase( name, msgPattern ), consoleMode( console_interface::std_out ), coloredOutput( true )
			{
				WriteToBaseBuffer( false );
				SetOriginalColors( );
#ifdef WINDOWS_PLATFORM
				WinConsoleMode( consoleMode );
#endif  // WINDOWS_PLATFORM
			}

			ColorConsole::~ColorConsole( )
			{
#ifdef WINDOWS_PLATFORM
				ResetWinConsole( );
#endif  // WINDOWS_PLATFORM
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

			void ColorConsole::WinConsoleMode( console_interface cInterface )
			{
				if( cInterface == console_interface::std_out ) {
					outputHandle = GetStdHandle( STD_OUTPUT_HANDLE );
				}
				else {
					outputHandle = GetStdHandle( STD_ERROR_HANDLE );
				}
				consoleMode = cInterface;
				DWORD opMode { ENABLE_VIRTUAL_TERMINAL_PROCESSING };
				if( !GetConsoleMode( outputHandle, &opMode ) ) {
					exit( GetLastError( ) );
				}
			}

			void ColorConsole::ResetWinConsole( )
			{
				DWORD opMode { 0 };
				if( !SetConsoleMode( outputHandle, opMode ) ) {
					exit( GetLastError( ) );
				}
			}

			void ColorConsole::SetConsoleInterface( console_interface mode )
			{
				consoleMode = mode;
			}

			console_interface ColorConsole::ConsoleMode( )
			{
				return consoleMode;
			}

			void ColorConsole::PrintMessage( std::string_view formatted )
			{
				std::string_view msgColor { "" }, reset { "" };
				if( coloredOutput ) {
					msgColor = GetMsgColor( MsgInfo( )->MsgLevel( ) );
					reset    = msgLevelColors.at( LoggerLevel::off );
				}
				switch( consoleMode ) {
					case serenity::expiremental::targets::console_interface::std_out:
						std::cout << msgColor << formatted << reset;
						break;
					case serenity::expiremental::targets::console_interface::std_log:
						std::clog << msgColor << formatted << reset;
						break;
					case serenity::expiremental::targets::console_interface::std_err:
						std::cerr << msgColor << formatted << reset;
						break;
				}
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
	}      // namespace expiremental
}  // namespace serenity