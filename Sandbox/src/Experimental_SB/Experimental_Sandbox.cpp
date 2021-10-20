#include <string_view>
#include <iostream>
#include <map>


// So Far, Only Real Issue I've Encountered Seems To Be The Very Common Issue Of Windows Terminal Issue #32
// where when resizing the console window, the last color used line wraps..
// (https://github.com/microsoft/terminal/issues/32)

// for both the basic/bright variants, still need to implement their inverses:
// for basic_colors  -> basic_colors::red::on_bright_white for an example
// for bright_colors -> bright_colors::red::on_basic_white for an example

namespace se_colors
{
	// Note: According to the ansi wiki, formats can be appended together as long as they're separated by a semi-colon
	namespace formats
	{
		const char *reset              = "\033[0m";
		const char *bold               = "\033[1m";
		const char *dim                = "\033[2m";
		const char *underline          = "\033[4m";
		const char *blink              = "\033[5m";
		const char *invert             = "\033[7m";
		const char *strike_out         = "\033[9m";
		const char *diasble_bold       = "\033[21m";
		const char *revert_intensity   = "\033[23m";
		const char *disable_underline  = "\033[24m";
		const char *disable_blink      = "\033[25m";
		const char *disable_invert     = "\033[27m";
		const char *disable_strike_out = "\033[29m";
	}  // namespace formats

	namespace basic_colors
	{
		namespace foreground
		{
			const char *black   = "\033[30m";
			const char *red     = "\033[31m";
			const char *green   = "\033[32m";
			const char *yellow  = "\033[33m";
			const char *blue    = "\033[34m";
			const char *magenta = "\033[35m";
			const char *cyan    = "\033[36m";
			const char *white   = "\033[37m";
		}  // namespace foreground

		namespace background
		{
			const char *black   = "\033[40m";
			const char *red     = "\033[41m";
			const char *green   = "\033[42m";
			const char *yellow  = "\033[43m";
			const char *blue    = "\033[44m";
			const char *magenta = "\033[45m";
			const char *cyan    = "\033[46m";
			const char *white   = "\033[47m";
		}  // namespace background

		namespace combos
		{
			namespace black
			{
				const char *on_black   = "\033[30m\033[40m";
				const char *on_red     = "\033[30m\033[41m";
				const char *on_green   = "\033[30m\033[42m";
				const char *on_yellow  = "\033[30m\033[43m";
				const char *on_blue    = "\033[30m\033[44m";
				const char *on_magenta = "\033[30m\033[45m";
				const char *on_cyan    = "\033[30m\033[46m";
				const char *on_white   = "\033[30m\033[47m";
			}  // namespace black

			namespace red
			{
				const char *on_black   = "\033[31m\033[40m";
				const char *on_red     = "\033[31m\033[41m";
				const char *on_green   = "\033[31m\033[42m";
				const char *on_yellow  = "\033[31m\033[43m";
				const char *on_blue    = "\033[31m\033[44m";
				const char *on_magenta = "\033[31m\033[45m";
				const char *on_cyan    = "\033[31m\033[46m";
				const char *on_white   = "\033[31m\033[47m";
			}  // namespace red

			namespace green
			{
				const char *on_black   = "\033[32m\033[40m";
				const char *on_red     = "\033[32m\033[41m";
				const char *on_green   = "\033[32m\033[42m";
				const char *on_yellow  = "\033[32m\033[43m";
				const char *on_blue    = "\033[32m\033[44m";
				const char *on_magenta = "\033[32m\033[45m";
				const char *on_cyan    = "\033[32m\033[46m";
				const char *on_white   = "\033[32m\033[47m";
			}  // namespace green

			namespace yellow
			{
				const char *on_black   = "\033[33m\033[40m";
				const char *on_red     = "\033[33m\033[41m";
				const char *on_green   = "\033[33m\033[42m";
				const char *on_yellow  = "\033[33m\033[43m";
				const char *on_blue    = "\033[33m\033[44m";
				const char *on_magenta = "\033[33m\033[45m";
				const char *on_cyan    = "\033[33m\033[46m";
				const char *on_white   = "\033[33m\033[47m";
			}  // namespace yellow

			namespace blue
			{
				const char *on_black   = "\033[34m\033[40m";
				const char *on_red     = "\033[34m\033[41m";
				const char *on_green   = "\033[34m\033[42m";
				const char *on_yellow  = "\033[34m\033[43m";
				const char *on_blue    = "\033[34m\033[044m";
				const char *on_magenta = "\033[34m\033[45m";
				const char *on_cyan    = "\033[34m\033[46m";
				const char *on_white   = "\033[34m\033[47m";
			}  // namespace blue

			namespace magenta
			{
				const char *on_black   = "\033[35m\033[40m";
				const char *on_red     = "\033[35m\033[41m";
				const char *on_green   = "\033[35m\033[42m";
				const char *on_yellow  = "\033[35m\033[43m";
				const char *on_blue    = "\033[35m\033[44m";
				const char *on_magenta = "\033[35m\033[45m";
				const char *on_cyan    = "\033[35m\033[46m";
				const char *on_white   = "\033[35m\033[47m";
			}  // namespace magenta

			namespace cyan
			{
				const char *on_black   = "\033[36m\033[40m";
				const char *on_red     = "\033[36m\033[41m";
				const char *on_green   = "\033[36m\033[42m";
				const char *on_yellow  = "\033[36m\033[43m";
				const char *on_blue    = "\033[36m\033[44m";
				const char *on_magenta = "\033[36m\033[45m";
				const char *on_cyan    = "\033[36m\033[46m";
				const char *on_white   = "\033[36m\033[47m";
			}  // namespace cyan

			namespace white
			{
				const char *on_black   = "\033[37m\033[40m";
				const char *on_red     = "\033[37m\033[41m";
				const char *on_green   = "\033[37m\033[42m";
				const char *on_yellow  = "\033[37m\033[43m";
				const char *on_blue    = "\033[37m\033[44m";
				const char *on_magenta = "\033[37m\033[45m";
				const char *on_cyan    = "\033[37m\033[46m";
				const char *on_white   = "\033[37m\033[47m";
			}  // namespace white

		}  // namespace combos
	}          // namespace basic_colors


	namespace bright_colors
	{
		namespace foreground
		{
			const char *grey    = "\033[90m";
			const char *red     = "\033[91m";
			const char *green   = "\033[92m";
			const char *yellow  = "\033[93m";
			const char *blue    = "\033[94m";
			const char *magenta = "\033[95m";
			const char *cyan    = "\033[96m";
			const char *white   = "\033[97m";
		}  // namespace foreground

		namespace background
		{ }

		namespace combos
		{
			namespace grey
			{ }
			namespace red
			{ }
			namespace green
			{ }
			namespace yellow
			{ }
			namespace blue
			{
				const char *on_black   = "\033[94m\033[40m";
				const char *on_red     = "\033[94m\033[41m";
				const char *on_green   = "\033[94m\033[42m";
				const char *on_yellow  = "\033[94m\033[43m";
				const char *on_blue    = "\033[94m\033[44m";
				const char *on_magenta = "\033[94m\033[45m";
				const char *on_cyan    = "\033[94m\033[46m";
				const char *on_white   = "\033[94m\033[47m";
			}  // namespace blue
			namespace magenta
			{ }
			namespace cyan
			{ }
			namespace white
			{ }
		}  // namespace combos
	}          // namespace bright_colors

	// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
	// clang-format on

	namespace tag_helper
	{
		std::string toString( const std::string_view s )
		{
			return std::string( s.data( ), s.size( ) );
		}
	}  // namespace tag_helper

	// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
	// clang-format on

	namespace Tag
	{
		using namespace se_colors;

		std::string Reset( )
		{
			return formats::reset;
		}

		std::string Blue( std::string_view s )
		{
			return basic_colors::foreground::blue + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Blue( std::string_view s )
		{
			return bright_colors::foreground::blue + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Blue( std::string_view s )
		{
			return basic_colors::background::blue + tag_helper::toString( s ) + Reset( );
		}
		// Regular Blue Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Blue_On_Black( std::string_view s )
		{
			return basic_colors::combos::blue::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Blue( std::string_view s )
		{
			return basic_colors::combos::blue::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::blue::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Green( std::string_view s )
		{
			return basic_colors::combos::blue::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::blue::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Red( std::string_view s )
		{
			return basic_colors::combos::blue::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_White( std::string_view s )
		{
			return basic_colors::combos::blue::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Blue_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::blue::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		// Light Blue Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Bright_Blue_On_Black( std::string_view s )
		{
			return bright_colors::combos::blue::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Blue( std::string_view s )
		{
			return bright_colors::combos::blue::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Cyan( std::string_view s )
		{
			return bright_colors::combos::blue::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Green( std::string_view s )
		{
			return bright_colors::combos::blue::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Magenta( std::string_view s )
		{
			return bright_colors::combos::blue::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Red( std::string_view s )
		{
			return bright_colors::combos::blue::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_White( std::string_view s )
		{
			return bright_colors::combos::blue::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Bright_Blue_On_Yellow( std::string_view s )
		{
			return bright_colors::combos::blue::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Green( std::string_view s )
		{
			return basic_colors::foreground::green + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Green( std::string_view s )
		{
			return bright_colors::foreground::green + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Green( std::string_view s )
		{
			return basic_colors::background::green + tag_helper::toString( s ) + Reset( );
		}
		// Regular Green Foreground On Regular [X] Color Background
		// ****************************************************************************************
		//######################################################################################################################################################################################################################################################################################
		std::string Green_On_Black( std::string_view s )
		{
			return basic_colors::combos::green::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Blue( std::string_view s )
		{
			return basic_colors::combos::green::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::green::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Green( std::string_view s )
		{
			return basic_colors::combos::green::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::green::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Red( std::string_view s )
		{
			return basic_colors::combos::green::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_White( std::string_view s )
		{
			return basic_colors::combos::green::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Green_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::green::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Cyan( std::string_view s )
		{
			return basic_colors::foreground::cyan + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Cyan( std::string_view s )
		{
			return bright_colors::foreground::cyan + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Cyan( std::string_view s )
		{
			return basic_colors::background::cyan + tag_helper::toString( s ) + Reset( );
		}
		// Regular Cyan Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Cyan_On_Black( std::string_view s )
		{
			return basic_colors::combos::cyan::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Blue( std::string_view s )
		{
			return basic_colors::combos::cyan::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::cyan::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Green( std::string_view s )
		{
			return basic_colors::combos::cyan::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::cyan::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Red( std::string_view s )
		{
			return basic_colors::combos::cyan::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_White( std::string_view s )
		{
			return basic_colors::combos::cyan::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Cyan_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::cyan::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Black( std::string_view s )
		{
			return basic_colors::foreground::black + tag_helper::toString( s ) + Reset( );
		}

		std::string Grey( std::string_view s )
		{
			return bright_colors::foreground::grey + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Black( std::string_view s )
		{
			return basic_colors::background::black + tag_helper::toString( s ) + Reset( );
		}
		// Regular Black Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Black_On_Black( std::string_view s )
		{
			return basic_colors::combos::black::on_black + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Blue( std::string_view s )
		{
			return basic_colors::combos::black::on_blue + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::black::on_cyan + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Green( std::string_view s )
		{
			return basic_colors::combos::black::on_green + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::black::on_magenta + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Red( std::string_view s )
		{
			return basic_colors::combos::black::on_red + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_White( std::string_view s )
		{
			return basic_colors::combos::black::on_white + tag_helper::toString( s ) + Reset( );
		}

		std::string Black_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::black::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Magenta( std::string_view s )
		{
			return basic_colors::foreground::magenta + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Magenta( std::string_view s )
		{
			return bright_colors::foreground::magenta + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Magenta( std::string_view s )
		{
			return basic_colors::background::magenta + tag_helper::toString( s ) + Reset( );
		}
		// Regular Magenta Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Magenta_On_Black( std::string_view s )
		{
			return basic_colors::combos::magenta::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Blue( std::string_view s )
		{
			return basic_colors::combos::magenta::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::magenta::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Green( std::string_view s )
		{
			return basic_colors::combos::magenta::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::magenta::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Red( std::string_view s )
		{
			return basic_colors::combos::magenta::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_White( std::string_view s )
		{
			return basic_colors::combos::magenta::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Magenta_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::magenta::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Red( std::string_view s )
		{
			return basic_colors::foreground::red + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Red( std::string_view s )
		{
			return bright_colors::foreground::red + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Red( std::string_view s )
		{
			return basic_colors::background::red + tag_helper::toString( s ) + Reset( );
		}
		// Regular Red Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Red_On_Black( std::string_view s )
		{
			return basic_colors::combos::red::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Blue( std::string_view s )
		{
			return basic_colors::combos::red::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::red::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Green( std::string_view s )
		{
			return basic_colors::combos::red::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::red::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Red( std::string_view s )
		{
			return basic_colors::combos::red::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_White( std::string_view s )
		{
			return basic_colors::combos::red::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Red_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::red::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string White( std::string_view s )
		{
			return basic_colors::foreground::white + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_White( std::string_view s )
		{
			return bright_colors::foreground::white + tag_helper::toString( s ) + Reset( );
		}

		std::string On_White( std::string_view s )
		{
			return basic_colors::background::white + tag_helper::toString( s ) + Reset( );
		}
		// Regular White Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string White_On_Black( std::string_view s )
		{
			return basic_colors::combos::white::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Blue( std::string_view s )
		{
			return basic_colors::combos::white::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::white::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Green( std::string_view s )
		{
			return basic_colors::combos::white::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::white::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Red( std::string_view s )
		{
			return basic_colors::combos::white::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_White( std::string_view s )
		{
			return basic_colors::combos::white::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string White_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::white::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
		std::string Yellow( std::string_view s )
		{
			return basic_colors::foreground::yellow + tag_helper::toString( s ) + Reset( );
		}

		std::string Bright_Yellow( std::string_view s )
		{
			return bright_colors::foreground::yellow + tag_helper::toString( s ) + Reset( );
		}

		std::string On_Yellow( std::string_view s )
		{
			return basic_colors::background::yellow + tag_helper::toString( s ) + Reset( );
		}
		// Regular Yellow Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Yellow_On_Black( std::string_view s )
		{
			return basic_colors::combos::yellow::on_black + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Blue( std::string_view s )
		{
			return basic_colors::combos::yellow::on_blue + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Cyan( std::string_view s )
		{
			return basic_colors::combos::yellow::on_cyan + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Green( std::string_view s )
		{
			return basic_colors::combos::yellow::on_green + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Magenta( std::string_view s )
		{
			return basic_colors::combos::yellow::on_magenta + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Red( std::string_view s )
		{
			return basic_colors::combos::yellow::on_red + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_White( std::string_view s )
		{
			return basic_colors::combos::yellow::on_white + tag_helper::toString( s ) + Reset( );
		}
		std::string Yellow_On_Yellow( std::string_view s )
		{
			return basic_colors::combos::yellow::on_yellow + tag_helper::toString( s ) + Reset( );
		}
		// ****************************************************************************************
	};  // namespace Tag


	// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
	// clang-format on

	struct ConsoleColors  // depends on fg/bg/fmt structs and tag_helper functions
	{
		// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the project I'll
		// be using this in (< win 8.1 EOL). Technically, could add the option to enable virtual terminal as a startup option
		// in constructor - just so ANSI can still be used as-is here w/o the need of HANDLE/WORD-like variables for < Win 10?

		ConsoleColors( ) { }
		// Message Handle For Color Info
		struct msg_color
		{
			const char *fmt = se_colors::formats::reset;
			const char *fg  = se_colors::basic_colors::foreground::white;
			const char *bg  = se_colors::basic_colors::background::black;
		};

		void ColorPrint( const std::string_view message, msg_color color )
		{
			std::cout << MsgColorToStr( color ) << toString( message ) << Reset( ) << "\n";
		}

		std::string toString( const std::string_view s )
		{
			return std::string( s.data( ), s.size( ) );
		}

	      private:
		std::string MsgColorToStr( msg_color options )
		{
			std::string fullOptionString = options.fmt;
			fullOptionString.append( options.fg );
			fullOptionString.append( options.bg );
			return fullOptionString;
		}
		std::string Reset( )
		{
			return se_colors::formats::reset;
		}
	};  // struct ConsoleColors
}  // namespace se_colors

// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
// clang-format on

#define ALLOC_TEST 1

#if ALLOC_TEST  // Testing Allocations
size_t total_bytes = 0;
void * operator new( std::size_t n )
{
	// std::cout << "[Allocating " << n << " bytes]";
	total_bytes += n;
	return malloc( n );
}
void operator delete( void *p ) throw( )
{
	total_bytes -= sizeof( p );
	free( p );
}
#endif


int main( )
{
	using namespace se_colors;

	// ConsoleColors            C;
	// ConsoleColors::msg_color msgColor;

	//// Trace Is Deafult Color
	// msgColor.fg  = basic_colors::foreground::white;
	// msgColor.bg  = basic_colors::background::black;
	// msgColor.fmt = formats::reset;
	// C.ColorPrint( "[Trace]: White", msgColor );
	//// Info Is Light Green
	// msgColor.fg  = basic_colors::foreground::green;
	// msgColor.bg  = basic_colors::background::black;
	// msgColor.fmt = formats::bold;
	// C.ColorPrint( "[Info]: Green", msgColor );
	//// Warning Is Light Yellow
	// msgColor.fg  = basic_colors::foreground::yellow;
	// msgColor.bg  = basic_colors::background::black;
	// msgColor.fmt = formats::bold;

	// C.ColorPrint( "[Warning]: Yellow", msgColor );
	//// Error Is Dark Red
	// msgColor.fg  = basic_colors::foreground::red;
	// msgColor.bg  = basic_colors::background::black;
	// msgColor.fmt = "";
	// C.ColorPrint( "[Error]: Red", msgColor );
	//// Fatal Is Light Yellow On Dark Red
	// msgColor.fg  = basic_colors::foreground::yellow;
	// msgColor.bg  = basic_colors::background::red;
	// msgColor.fmt = formats::bold;
	// C.ColorPrint( "[Fatal]: Light Yellow On Red", msgColor );
	// Above Are The Settings For Logging Message Colors (Influenced by spdlog message level colors)
	// - "fatal" changed to something I personally thought was more visually appealing as a default.
	//  (spdlog does offer changes to default colors via set_color() either way)

	/*****************************************************************************************************************************
	 *	Would Like To See If Possible To Add Tagging In Messages, Something Like:
	 *  ColorPrint("This Would Be ^CT A Color &CT Tagged ^CT Section &CT", msgColor, tagColor1, tagColor2);
	 *  Where The Tag Colors Are Optional And Variadic.. Honestly Might Be More Difficult Then I Initially Thought =/
	 ****************************************************************************************************************************/
	std::cout << Tag::Blue( "\n\nSome " ) << Tag::Cyan( "Examples " ) << Tag::Green( "Of " ) << Tag::Magenta( "Nice " )
		  << Tag::Red( "Color " ) << Tag::Yellow( "Tagging" ) << Tag::Bright_Green( "!\n" ) << Tag::Reset( );


	std::cout << Tag::Blue( "Blue\t\t" ) << Tag::White( "White\t\t" ) << Tag::Cyan( "Cyan\t\t" ) << Tag::Green( "Green\t\t" )
		  << Tag::Magenta( "Magenta\t\t" ) << Tag::Red( "Red\t\t" ) << Tag::Yellow( "Yellow\n" ) << Tag::Reset( );


	std::cout << Tag::Bright_Blue( "Bright Blue\t" ) << Tag::Bright_White( "Bright White\t" ) << Tag::Bright_Cyan( "Bright Cyan\t" )
		  << Tag::Bright_Green( "Bright Green\t" ) << Tag::Bright_Magenta( "Bright Magenta\t" )
		  << Tag::Bright_Red( "Bright Red\t" ) << Tag::Bright_Yellow( "Bright Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::On_Blue( "On Blue\t\t" ) << "On White->" << Tag::On_White( "On" ) << "    " << Tag::On_Cyan( "On Cyan\t\t" )
		  << Tag::On_Green( "On Green\t" ) << Tag::On_Magenta( "On Magenta\t" ) << Tag::On_Red( "On Red\t\t" )
		  << Tag::On_Yellow( "On Yellow\n" ) << Tag::Reset( );

	std::cout << "Blue Fg/Bg->" << Tag::Blue_On_Blue( "___\t" ) << Tag::Blue_On_White( "Blue On White\t" )
		  << Tag::Blue_On_Cyan( "Blue On Cyan\t" ) << Tag::Blue_On_Green( "Blue On Green\t" )
		  << Tag::Blue_On_Magenta( "Blue On Magenta\t" ) << Tag::Blue_On_Red( "Blue On Red\t" )
		  << Tag::Blue_On_Yellow( "Blue On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Green_On_Blue( "Green On Blue\t" ) << Tag::Green_On_White( "Green On White\t" )
		  << Tag::Green_On_Cyan( "Green On Cyan\t" ) << "Green Fg/Bg->" << Tag::Green_On_Green( "___" )
		  << Tag::Green_On_Magenta( "Green On Magenta" ) << Tag::Green_On_Red( "Green On Red\t" )
		  << Tag::Green_On_Yellow( "Green On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Cyan_On_Blue( "Cyan On Blue\t" ) << Tag::Cyan_On_White( "Cyan On White\t" ) << "Cyan Fg/Bg->"
		  << Tag::Cyan_On_Cyan( "___\t" ) << Tag::Cyan_On_Green( "Cyan On Green\t" )
		  << Tag::Cyan_On_Magenta( "Cyan On Magenta\t" ) << Tag::Cyan_On_Red( "Cyan On Red\t" )
		  << Tag::Cyan_On_Yellow( "Cyan On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Black_On_Blue( "Black On Blue\t" ) << Tag::Black_On_White( "Black On White\t" )
		  << Tag::Black_On_Cyan( "Black On Cyan\t" ) << Tag::Black_On_Green( "Black On Green\t" )
		  << Tag::Black_On_Magenta( "Black On Magenta" ) << Tag::Black_On_Red( "Black On Red\t" )
		  << Tag::Black_On_Yellow( "Black On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Magenta_On_Blue( "Magenta On Blue\t" ) << Tag::Magenta_On_White( "Magenta On White" )
		  << Tag::Magenta_On_Cyan( "Magenta On Cyan\t" ) << Tag::Magenta_On_Green( "Magenta On Green" ) << "Mag Fg/Bg->"
		  << Tag::Magenta_On_Magenta( "___\t" ) << Tag::Magenta_On_Red( "Magenta On Red\t" )
		  << Tag::Magenta_On_Yellow( "Magenta On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Red_On_Blue( "Red On Blue\t" ) << Tag::Red_On_White( "Red On White\t" ) << Tag::Red_On_Cyan( "Red On Cyan\t" )
		  << Tag::Red_On_Green( "Red On Green\t" ) << Tag::Red_On_Magenta( "Red On Magenta\t" ) << "Red Fg/Bg->"
		  << Tag::Red_On_Red( "___\t" ) << Tag::Red_On_Yellow( "Red On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::White_On_Blue( "White On Blue\t" ) << "White Fg/Bg->" << Tag::White_On_White( "___" )
		  << Tag::White_On_Cyan( "White On Cyan\t" ) << Tag::White_On_Green( "White On Green\t" )
		  << Tag::White_On_Magenta( "White On Magenta" ) << Tag::White_On_Red( "White On Red\t" )
		  << Tag::White_On_Yellow( "White On Yellow\n" ) << Tag::Reset( );

	std::cout << Tag::Yellow_On_Blue( "Yellow On Blue\t" ) << Tag::Yellow_On_White( "Yellow On White" )
		  << Tag::Yellow_On_Cyan( "Yellow On Cyan\t" ) << Tag::Yellow_On_Green( "Yellow On Green\t" )
		  << Tag::Yellow_On_Magenta( "Yellow On Magenta" ) << Tag::Yellow_On_Red( "Yellow On Red\t" ) << "Yellow Fg/Bg->"
		  << Tag::Yellow_On_Yellow( "___\n" ) << Tag::Reset( );

	std::cout << Tag::Bright_Blue_On_Blue( "Br_Blue On Blue" ) << Tag::Bright_Blue_On_White( "Br_Blue On White" )
		  << Tag::Bright_Blue_On_Cyan( "Br_Blue On Cyan\t" ) << Tag::Bright_Blue_On_Green( "Br_Blue On Green" )
		  << Tag::Bright_Blue_On_Magenta( "Br_Blue On Magenta" ) << Tag::Bright_Blue_On_Red( "Br_Blue On Red" )
		  << Tag::Bright_Blue_On_Yellow( "Br_Blue On Yellow\n" ) << Tag::Reset( );
#if ALLOC_TEST
	// Original Debug Mode Total Memory Allocated:   [115032 bytes] OR [0.115032 MB]
	// Original Release Mode Total Memory Allocated: [108816 bytes] OR [0.108816 MB]
	// New Debug Mode Total Memory Allocated:        [5128 bytes]   OR [0.005128 MB]
	// New Release Mode Total Memory Allocated:      [2112 bytes]   OR [0.002112 MB
	std::cout << "\n\nTotal Memory Allocated: [ " << total_bytes << " bytes] "
		  << "OR [" << total_bytes / 1000000.0 << " MB]\n\n";
	// With The Above Statistics for this section of code, definitely well worth changing to const char* for color tags (some
	// color tags >15 chars, so small string optimization wasn't going to occur if they were std::strings instead - opted for const
	// char* for consistency throughout)
#endif
	/******************************************************************************************************************************
	 * Have Some Basic Tagging Set (Rough Draft Idea Anyways), But Still No Way Of Setting Up A Message Parser For Substitution
	 *Yet
	 * - Currently ColorPrint() and tagIt() Accomplish A Very Similar Function -> Hopefully Will Differentiate Soon Though
	 *******************************************************************************************************************************/
}
