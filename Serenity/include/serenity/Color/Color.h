#pragma once

#include <string_view>

/******************************************************************************************************************************
 * Have Some Basic Tagging Set Up, But Still No Way Of Setting Up A Message Parser For Substitution Yet
 *******************************************************************************************************************************/
/*****************************************************************************************************************************
 *	Would Like To See If Possible To Add Tagging In Messages, Something Like:
 *  ColorPrint("This Would Be ^CT A Color &CT Tagged ^CT Section &CT", msgColor, tagColor1, tagColor2);
 *  Where The Tag Colors Are Optional And Variadic.. Honestly Might Be More Difficult Then I Initially Thought =/
 ****************************************************************************************************************************/
namespace se_colors {
	// Note: According to the ansi wiki, formats can be appended together as long as they're separated by a semi-colon
	namespace formats {
		constexpr const char *reset              = "\033[0m";
		constexpr const char *bold               = "\033[1m";
		constexpr const char *dim                = "\033[2m";
		constexpr const char *underline          = "\033[4m";
		constexpr const char *blink              = "\033[5m";
		constexpr const char *invert             = "\033[7m";
		constexpr const char *strike_out         = "\033[9m";
		constexpr const char *diasble_bold       = "\033[21m";
		constexpr const char *revert_intensity   = "\033[23m";
		constexpr const char *disable_underline  = "\033[24m";
		constexpr const char *disable_blink      = "\033[25m";
		constexpr const char *disable_invert     = "\033[27m";
		constexpr const char *disable_strike_out = "\033[29m";
		constexpr const char *clear_to_end       = "\033[k";
	}  // namespace formats

	constexpr const char *default_colors = "\033[39m\033[49m";

	namespace basic_colors {
		namespace foreground {
			constexpr const char *black   = "\033[30m";
			constexpr const char *red     = "\033[31m";
			constexpr const char *green   = "\033[32m";
			constexpr const char *yellow  = "\033[33m";
			constexpr const char *blue    = "\033[34m";
			constexpr const char *magenta = "\033[35m";
			constexpr const char *cyan    = "\033[36m";
			constexpr const char *white   = "\033[37m";
		}  // namespace foreground

		namespace background {
			constexpr const char *black   = "\033[40m";
			constexpr const char *red     = "\033[41m";
			constexpr const char *green   = "\033[42m";
			constexpr const char *yellow  = "\033[43m";
			constexpr const char *blue    = "\033[44m";
			constexpr const char *magenta = "\033[45m";
			constexpr const char *cyan    = "\033[46m";
			constexpr const char *white   = "\033[47m";
		}  // namespace background

		namespace combos {
			namespace black {
				constexpr const char *on_black          = "\033[30m\033[40m";
				constexpr const char *on_red            = "\033[30m\033[41m";
				constexpr const char *on_green          = "\033[30m\033[42m";
				constexpr const char *on_yellow         = "\033[30m\033[43m";
				constexpr const char *on_blue           = "\033[30m\033[44m";
				constexpr const char *on_magenta        = "\033[30m\033[45m";
				constexpr const char *on_cyan           = "\033[30m\033[46m";
				constexpr const char *on_white          = "\033[30m\033[47m";
				constexpr const char *on_grey           = "\033[30m\033[100m";
				constexpr const char *on_bright_red     = "\033[30m\033[101m";
				constexpr const char *on_bright_green   = "\033[30m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[30m\033[103m";
				constexpr const char *on_bright_blue    = "\033[30m\033[104m";
				constexpr const char *on_bright_magenta = "\033[30m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[30m\033[106m";
				constexpr const char *on_bright_white   = "\033[30m\033[107m";
			}  // namespace black

			namespace red {
				constexpr const char *on_black          = "\033[31m\033[40m";
				constexpr const char *on_red            = "\033[31m\033[41m";
				constexpr const char *on_green          = "\033[31m\033[42m";
				constexpr const char *on_yellow         = "\033[31m\033[43m";
				constexpr const char *on_blue           = "\033[31m\033[44m";
				constexpr const char *on_magenta        = "\033[31m\033[45m";
				constexpr const char *on_cyan           = "\033[31m\033[46m";
				constexpr const char *on_white          = "\033[31m\033[47m";
				constexpr const char *on_grey           = "\033[31m\033[100m";
				constexpr const char *on_bright_red     = "\033[31m\033[101m";
				constexpr const char *on_bright_green   = "\033[31m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[31m\033[103m";
				constexpr const char *on_bright_blue    = "\033[31m\033[104m";
				constexpr const char *on_bright_magenta = "\033[31m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[31m\033[106m";
				constexpr const char *on_bright_white   = "\033[31m\033[107m";
			}  // namespace red

			namespace green {
				constexpr const char *on_black          = "\033[32m\033[40m";
				constexpr const char *on_red            = "\033[32m\033[41m";
				constexpr const char *on_green          = "\033[32m\033[42m";
				constexpr const char *on_yellow         = "\033[32m\033[43m";
				constexpr const char *on_blue           = "\033[32m\033[44m";
				constexpr const char *on_magenta        = "\033[32m\033[45m";
				constexpr const char *on_cyan           = "\033[32m\033[46m";
				constexpr const char *on_white          = "\033[32m\033[47m";
				constexpr const char *on_grey           = "\033[32m\033[100m";
				constexpr const char *on_bright_red     = "\033[32m\033[101m";
				constexpr const char *on_bright_green   = "\033[32m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[32m\033[103m";
				constexpr const char *on_bright_blue    = "\033[32m\033[104m";
				constexpr const char *on_bright_magenta = "\033[32m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[32m\033[106m";
				constexpr const char *on_bright_white   = "\033[32m\033[107m";
			}  // namespace green

			namespace yellow {
				constexpr const char *on_black          = "\033[33m\033[40m";
				constexpr const char *on_red            = "\033[33m\033[41m";
				constexpr const char *on_green          = "\033[33m\033[42m";
				constexpr const char *on_yellow         = "\033[33m\033[43m";
				constexpr const char *on_blue           = "\033[33m\033[44m";
				constexpr const char *on_magenta        = "\033[33m\033[45m";
				constexpr const char *on_cyan           = "\033[33m\033[46m";
				constexpr const char *on_white          = "\033[33m\033[47m";
				constexpr const char *on_grey           = "\033[33m\033[100m";
				constexpr const char *on_bright_red     = "\033[33m\033[101m";
				constexpr const char *on_bright_green   = "\033[33m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[33m\033[103m";
				constexpr const char *on_bright_blue    = "\033[33m\033[104m";
				constexpr const char *on_bright_magenta = "\033[33m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[33m\033[106m";
				constexpr const char *on_bright_white   = "\033[33m\033[107m";
			}  // namespace yellow

			namespace blue {
				constexpr const char *on_black          = "\033[34m\033[40m";
				constexpr const char *on_red            = "\033[34m\033[41m";
				constexpr const char *on_green          = "\033[34m\033[42m";
				constexpr const char *on_yellow         = "\033[34m\033[43m";
				constexpr const char *on_blue           = "\033[34m\033[044m";
				constexpr const char *on_magenta        = "\033[34m\033[45m";
				constexpr const char *on_cyan           = "\033[34m\033[46m";
				constexpr const char *on_white          = "\033[34m\033[47m";
				constexpr const char *on_grey           = "\033[34m\033[100m";
				constexpr const char *on_bright_red     = "\033[34m\033[101m";
				constexpr const char *on_bright_green   = "\033[34m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[34m\033[103m";
				constexpr const char *on_bright_blue    = "\033[34m\033[104m";
				constexpr const char *on_bright_magenta = "\033[34m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[34m\033[106m";
				constexpr const char *on_bright_white   = "\033[34m\033[107m";
			}  // namespace blue

			namespace magenta {
				constexpr const char *on_black          = "\033[35m\033[40m";
				constexpr const char *on_red            = "\033[35m\033[41m";
				constexpr const char *on_green          = "\033[35m\033[42m";
				constexpr const char *on_yellow         = "\033[35m\033[43m";
				constexpr const char *on_blue           = "\033[35m\033[44m";
				constexpr const char *on_magenta        = "\033[35m\033[45m";
				constexpr const char *on_cyan           = "\033[35m\033[46m";
				constexpr const char *on_white          = "\033[35m\033[47m";
				constexpr const char *on_grey           = "\033[35m\033[100m";
				constexpr const char *on_bright_red     = "\033[35m\033[101m";
				constexpr const char *on_bright_green   = "\033[35m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[35m\033[103m";
				constexpr const char *on_bright_blue    = "\033[35m\033[104m";
				constexpr const char *on_bright_magenta = "\033[35m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[35m\033[106m";
				constexpr const char *on_bright_white   = "\033[35m\033[107m";
			}  // namespace magenta

			namespace cyan {
				constexpr const char *on_black          = "\033[36m\033[40m";
				constexpr const char *on_red            = "\033[36m\033[41m";
				constexpr const char *on_green          = "\033[36m\033[42m";
				constexpr const char *on_yellow         = "\033[36m\033[43m";
				constexpr const char *on_blue           = "\033[36m\033[44m";
				constexpr const char *on_magenta        = "\033[36m\033[45m";
				constexpr const char *on_cyan           = "\033[36m\033[46m";
				constexpr const char *on_white          = "\033[36m\033[47m";
				constexpr const char *on_grey           = "\033[36m\033[100m";
				constexpr const char *on_bright_red     = "\033[36m\033[101m";
				constexpr const char *on_bright_green   = "\033[36m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[36m\033[103m";
				constexpr const char *on_bright_blue    = "\033[36m\033[104m";
				constexpr const char *on_bright_magenta = "\033[36m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[36m\033[106m";
				constexpr const char *on_bright_white   = "\033[36m\033[107m";
			}  // namespace cyan

			namespace white {
				constexpr const char *on_black          = "\033[37m\033[40m";
				constexpr const char *on_red            = "\033[37m\033[41m";
				constexpr const char *on_green          = "\033[37m\033[42m";
				constexpr const char *on_yellow         = "\033[37m\033[43m";
				constexpr const char *on_blue           = "\033[37m\033[44m";
				constexpr const char *on_magenta        = "\033[37m\033[45m";
				constexpr const char *on_cyan           = "\033[37m\033[46m";
				constexpr const char *on_white          = "\033[37m\033[47m";
				constexpr const char *on_grey           = "\033[37m\033[100m";
				constexpr const char *on_bright_red     = "\033[37m\033[101m";
				constexpr const char *on_bright_green   = "\033[37m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[37m\033[103m";
				constexpr const char *on_bright_blue    = "\033[37m\033[104m";
				constexpr const char *on_bright_magenta = "\033[37m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[37m\033[106m";
				constexpr const char *on_bright_white   = "\033[37m\033[107m";
			}  // namespace white

		}  // namespace combos
	}      // namespace basic_colors

	namespace bright_colors {
		namespace foreground {
			constexpr const char *grey    = "\033[90m";
			constexpr const char *red     = "\033[91m";
			constexpr const char *green   = "\033[92m";
			constexpr const char *yellow  = "\033[93m";
			constexpr const char *blue    = "\033[94m";
			constexpr const char *magenta = "\033[95m";
			constexpr const char *cyan    = "\033[96m";
			constexpr const char *white   = "\033[97m";
		}  // namespace foreground

		namespace background {
			constexpr const char *grey    = "\033[100m";
			constexpr const char *red     = "\033[101m";
			constexpr const char *green   = "\033[102m";
			constexpr const char *yellow  = "\033[103m";
			constexpr const char *blue    = "\033[104m";
			constexpr const char *magenta = "\033[105m";
			constexpr const char *cyan    = "\033[106m";
			constexpr const char *white   = "\033[107m";
		}  // namespace background

		namespace combos {
			namespace grey {
				constexpr const char *on_black          = "\033[90m\033[40m";
				constexpr const char *on_red            = "\033[90m\033[41m";
				constexpr const char *on_green          = "\033[90m\033[42m";
				constexpr const char *on_yellow         = "\033[90m\033[43m";
				constexpr const char *on_blue           = "\033[90m\033[44m";
				constexpr const char *on_magenta        = "\033[90m\033[45m";
				constexpr const char *on_cyan           = "\033[90m\033[46m";
				constexpr const char *on_white          = "\033[90m\033[47m";
				constexpr const char *on_grey           = "\033[90m\033[100m";
				constexpr const char *on_bright_red     = "\033[90m\033[101m";
				constexpr const char *on_bright_green   = "\033[90m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[90m\033[103m";
				constexpr const char *on_bright_blue    = "\033[90m\033[104m";
				constexpr const char *on_bright_magenta = "\033[90m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[90m\033[106m";
				constexpr const char *on_bright_white   = "\033[90m\033[107m";
			}  // namespace grey
			namespace red {
				constexpr const char *on_black          = "\033[91m\033[40m";
				constexpr const char *on_red            = "\033[91m\033[41m";
				constexpr const char *on_green          = "\033[91m\033[42m";
				constexpr const char *on_yellow         = "\033[91m\033[43m";
				constexpr const char *on_blue           = "\033[91m\033[44m";
				constexpr const char *on_magenta        = "\033[91m\033[45m";
				constexpr const char *on_cyan           = "\033[91m\033[46m";
				constexpr const char *on_white          = "\033[91m\033[47m";
				constexpr const char *on_grey           = "\033[91m\033[100m";
				constexpr const char *on_bright_red     = "\033[91m\033[101m";
				constexpr const char *on_bright_green   = "\033[91m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[91m\033[103m";
				constexpr const char *on_bright_blue    = "\033[91m\033[104m";
				constexpr const char *on_bright_magenta = "\033[91m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[91m\033[106m";
				constexpr const char *on_bright_white   = "\033[91m\033[107m";
			}  // namespace red
			namespace green {
				constexpr const char *on_black          = "\033[92m\033[40m";
				constexpr const char *on_red            = "\033[92m\033[41m";
				constexpr const char *on_green          = "\033[92m\033[42m";
				constexpr const char *on_yellow         = "\033[92m\033[43m";
				constexpr const char *on_blue           = "\033[92m\033[44m";
				constexpr const char *on_magenta        = "\033[92m\033[45m";
				constexpr const char *on_cyan           = "\033[92m\033[46m";
				constexpr const char *on_white          = "\033[92m\033[47m";
				constexpr const char *on_grey           = "\033[92m\033[100m";
				constexpr const char *on_bright_red     = "\033[92m\033[101m";
				constexpr const char *on_bright_green   = "\033[92m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[92m\033[103m";
				constexpr const char *on_bright_blue    = "\033[92m\033[104m";
				constexpr const char *on_bright_magenta = "\033[92m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[92m\033[106m";
				constexpr const char *on_bright_white   = "\033[92m\033[107m";
			}  // namespace green
			namespace yellow {
				constexpr const char *on_black          = "\033[93m\033[40m";
				constexpr const char *on_red            = "\033[93m\033[41m";
				constexpr const char *on_green          = "\033[93m\033[42m";
				constexpr const char *on_yellow         = "\033[93m\033[43m";
				constexpr const char *on_blue           = "\033[93m\033[44m";
				constexpr const char *on_magenta        = "\033[93m\033[45m";
				constexpr const char *on_cyan           = "\033[93m\033[46m";
				constexpr const char *on_white          = "\033[93m\033[47m";
				constexpr const char *on_grey           = "\033[93m\033[100m";
				constexpr const char *on_bright_red     = "\033[93m\033[101m";
				constexpr const char *on_bright_green   = "\033[93m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[93m\033[103m";
				constexpr const char *on_bright_blue    = "\033[93m\033[104m";
				constexpr const char *on_bright_magenta = "\033[93m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[93m\033[106m";
				constexpr const char *on_bright_white   = "\033[93m\033[107m";
			}  // namespace yellow
			namespace blue {
				constexpr const char *on_black          = "\033[94m\033[40m";
				constexpr const char *on_red            = "\033[94m\033[41m";
				constexpr const char *on_green          = "\033[94m\033[42m";
				constexpr const char *on_yellow         = "\033[94m\033[43m";
				constexpr const char *on_blue           = "\033[94m\033[44m";
				constexpr const char *on_magenta        = "\033[94m\033[45m";
				constexpr const char *on_cyan           = "\033[94m\033[46m";
				constexpr const char *on_white          = "\033[94m\033[47m";
				constexpr const char *on_grey           = "\033[94m\033[100m";
				constexpr const char *on_bright_red     = "\033[94m\033[101m";
				constexpr const char *on_bright_green   = "\033[94m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[94m\033[103m";
				constexpr const char *on_bright_blue    = "\033[94m\033[104m";
				constexpr const char *on_bright_magenta = "\033[94m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[94m\033[106m";
				constexpr const char *on_bright_white   = "\033[94m\033[107m";
			}  // namespace blue
			namespace magenta {
				constexpr const char *on_black          = "\033[95m\033[40m";
				constexpr const char *on_red            = "\033[95m\033[41m";
				constexpr const char *on_green          = "\033[95m\033[42m";
				constexpr const char *on_yellow         = "\033[95m\033[43m";
				constexpr const char *on_blue           = "\033[95m\033[44m";
				constexpr const char *on_magenta        = "\033[95m\033[45m";
				constexpr const char *on_cyan           = "\033[95m\033[46m";
				constexpr const char *on_white          = "\033[95m\033[47m";
				constexpr const char *on_grey           = "\033[95m\033[100m";
				constexpr const char *on_bright_red     = "\033[95m\033[101m";
				constexpr const char *on_bright_green   = "\033[95m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[95m\033[103m";
				constexpr const char *on_bright_blue    = "\033[95m\033[104m";
				constexpr const char *on_bright_magenta = "\033[95m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[95m\033[106m";
				constexpr const char *on_bright_white   = "\033[95m\033[107m";
			}  // namespace magenta
			namespace cyan {
				constexpr const char *on_black          = "\033[94m\033[40m";
				constexpr const char *on_red            = "\033[96m\033[41m";
				constexpr const char *on_green          = "\033[96m\033[42m";
				constexpr const char *on_yellow         = "\033[96m\033[43m";
				constexpr const char *on_blue           = "\033[96m\033[44m";
				constexpr const char *on_magenta        = "\033[96m\033[45m";
				constexpr const char *on_cyan           = "\033[96m\033[46m";
				constexpr const char *on_white          = "\033[96m\033[47m";
				constexpr const char *on_grey           = "\033[96m\033[100m";
				constexpr const char *on_bright_red     = "\033[96m\033[101m";
				constexpr const char *on_bright_green   = "\033[96m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[96m\033[103m";
				constexpr const char *on_bright_blue    = "\033[96m\033[104m";
				constexpr const char *on_bright_magenta = "\033[96m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[96m\033[106m";
				constexpr const char *on_bright_white   = "\033[96m\033[107m";
			}  // namespace cyan
			namespace white {
				constexpr const char *on_black          = "\033[97m\033[40m";
				constexpr const char *on_red            = "\033[97m\033[41m";
				constexpr const char *on_green          = "\033[97m\033[42m";
				constexpr const char *on_yellow         = "\033[97m\033[43m";
				constexpr const char *on_blue           = "\033[97m\033[44m";
				constexpr const char *on_magenta        = "\033[97m\033[45m";
				constexpr const char *on_cyan           = "\033[97m\033[46m";
				constexpr const char *on_white          = "\033[97m\033[47m";
				constexpr const char *on_grey           = "\033[97m\033[100m";
				constexpr const char *on_bright_red     = "\033[97m\033[101m";
				constexpr const char *on_bright_green   = "\033[97m\033[102m";
				constexpr const char *on_bright_yellow  = "\033[97m\033[103m";
				constexpr const char *on_bright_blue    = "\033[97m\033[104m";
				constexpr const char *on_bright_magenta = "\033[97m\033[105m";
				constexpr const char *on_bright_cyan    = "\033[97m\033[106m";
				constexpr const char *on_bright_white   = "\033[97m\033[107m";
			}  // namespace white
		}      // namespace combos
	}          // namespace bright_colors
}  // namespace se_colors

namespace se_colors {
	namespace Tag {
		std::string Reset( );
		std::string Black( std::string_view s );
		std::string Red( std::string_view s );
		std::string Green( std::string_view s );
		std::string Yellow( std::string_view s );
		std::string Blue( std::string_view s );
		std::string Magenta( std::string_view s );
		std::string Cyan( std::string_view s );
		std::string White( std::string_view s );
		std::string On_Black( std::string_view s );
		std::string On_Red( std::string_view s );
		std::string On_Green( std::string_view s );
		std::string On_Yellow( std::string_view s );
		std::string On_Blue( std::string_view s );
		std::string On_Magenta( std::string_view s );
		std::string On_Cyan( std::string_view s );
		std::string On_White( std::string_view s );
		std::string Black_On_Black( std::string_view s );
		std::string Black_On_Red( std::string_view s );
		std::string Black_On_Green( std::string_view s );
		std::string Black_On_Yellow( std::string_view s );
		std::string Black_On_Blue( std::string_view s );
		std::string Black_On_Magenta( std::string_view s );
		std::string Black_On_Cyan( std::string_view s );
		std::string Black_On_White( std::string_view s );
		std::string Black_On_Grey( std::string_view s );
		std::string Black_On_Bright_Red( std::string_view s );
		std::string Black_On_Bright_Green( std::string_view s );
		std::string Black_On_Bright_Yellow( std::string_view s );
		std::string Black_On_Bright_Blue( std::string_view s );
		std::string Black_On_Bright_Magenta( std::string_view s );
		std::string Black_On_Bright_Cyan( std::string_view s );
		std::string Black_On_Bright_White( std::string_view s );
		std::string Red_On_Black( std::string_view s );
		std::string Red_On_Red( std::string_view s );
		std::string Red_On_Green( std::string_view s );
		std::string Red_On_Yellow( std::string_view s );
		std::string Red_On_Blue( std::string_view s );
		std::string Red_On_Magenta( std::string_view s );
		std::string Red_On_Cyan( std::string_view s );
		std::string Red_On_White( std::string_view s );
		std::string Red_On_Grey( std::string_view s );
		std::string Red_On_Bright_Red( std::string_view s );
		std::string Red_On_Bright_Green( std::string_view s );
		std::string Red_On_Bright_Yellow( std::string_view s );
		std::string Red_On_Bright_Blue( std::string_view s );
		std::string Red_On_Bright_Magenta( std::string_view s );
		std::string Red_On_Bright_Cyan( std::string_view s );
		std::string Red_On_Bright_White( std::string_view s );
		std::string Green_On_Black( std::string_view s );
		std::string Green_On_Red( std::string_view s );
		std::string Green_On_Green( std::string_view s );
		std::string Green_On_Yellow( std::string_view s );
		std::string Green_On_Blue( std::string_view s );
		std::string Green_On_Magenta( std::string_view s );
		std::string Green_On_Cyan( std::string_view s );
		std::string Green_On_White( std::string_view s );
		std::string Green_On_Grey( std::string_view s );
		std::string Green_On_Bright_Red( std::string_view s );
		std::string Green_On_Bright_Green( std::string_view s );
		std::string Green_On_Bright_Yellow( std::string_view s );
		std::string Green_On_Bright_Blue( std::string_view s );
		std::string Green_On_Bright_Magenta( std::string_view s );
		std::string Green_On_Bright_Cyan( std::string_view s );
		std::string Green_On_Bright_White( std::string_view s );
		std::string Yellow_On_Black( std::string_view s );
		std::string Yellow_On_Red( std::string_view s );
		std::string Yellow_On_Green( std::string_view s );
		std::string Yellow_On_Yellow( std::string_view s );
		std::string Yellow_On_Blue( std::string_view s );
		std::string Yellow_On_Magenta( std::string_view s );
		std::string Yellow_On_Cyan( std::string_view s );
		std::string Yellow_On_White( std::string_view s );
		std::string Yellow_On_Grey( std::string_view s );
		std::string Yellow_On_Bright_Red( std::string_view s );
		std::string Yellow_On_Bright_Green( std::string_view s );
		std::string Yellow_On_Bright_Yellow( std::string_view s );
		std::string Yellow_On_Bright_Blue( std::string_view s );
		std::string Yellow_On_Bright_Magenta( std::string_view s );
		std::string Yellow_On_Bright_Cyan( std::string_view s );
		std::string Yellow_On_Bright_White( std::string_view s );
		std::string Blue_On_Black( std::string_view s );
		std::string Blue_On_Red( std::string_view s );
		std::string Blue_On_Green( std::string_view s );
		std::string Blue_On_Yellow( std::string_view s );
		std::string Blue_On_Blue( std::string_view s );
		std::string Blue_On_Magenta( std::string_view s );
		std::string Blue_On_Cyan( std::string_view s );
		std::string Blue_On_White( std::string_view s );
		std::string Blue_On_Grey( std::string_view s );
		std::string Blue_On_Bright_Red( std::string_view s );
		std::string Blue_On_Bright_Green( std::string_view s );
		std::string Blue_On_Bright_Yellow( std::string_view s );
		std::string Blue_On_Bright_Blue( std::string_view s );
		std::string Blue_On_Bright_Magenta( std::string_view s );
		std::string Blue_On_Bright_Cyan( std::string_view s );
		std::string Blue_On_Bright_White( std::string_view s );
		std::string Magenta_On_Black( std::string_view s );
		std::string Magenta_On_Red( std::string_view s );
		std::string Magenta_On_Green( std::string_view s );
		std::string Magenta_On_Yellow( std::string_view s );
		std::string Magenta_On_Blue( std::string_view s );
		std::string Magenta_On_Magenta( std::string_view s );
		std::string Magenta_On_Cyan( std::string_view s );
		std::string Magenta_On_White( std::string_view s );
		std::string Magenta_On_Grey( std::string_view s );
		std::string Magenta_On_Bright_Red( std::string_view s );
		std::string Magenta_On_Bright_Green( std::string_view s );
		std::string Magenta_On_Bright_Yellow( std::string_view s );
		std::string Magenta_On_Bright_Blue( std::string_view s );
		std::string Magenta_On_Bright_Magenta( std::string_view s );
		std::string Magenta_On_Bright_Cyan( std::string_view s );
		std::string Magenta_On_Bright_White( std::string_view s );
		std::string Cyan_On_Black( std::string_view s );
		std::string Cyan_On_Red( std::string_view s );
		std::string Cyan_On_Green( std::string_view s );
		std::string Cyan_On_Yellow( std::string_view s );
		std::string Cyan_On_Blue( std::string_view s );
		std::string Cyan_On_Magenta( std::string_view s );
		std::string Cyan_On_Cyan( std::string_view s );
		std::string Cyan_On_White( std::string_view s );
		std::string Cyan_On_Grey( std::string_view s );
		std::string Cyan_On_Bright_Red( std::string_view s );
		std::string Cyan_On_Bright_Green( std::string_view s );
		std::string Cyan_On_Bright_Yellow( std::string_view s );
		std::string Cyan_On_Bright_Blue( std::string_view s );
		std::string Cyan_On_Bright_Magenta( std::string_view s );
		std::string Cyan_On_Bright_Cyan( std::string_view s );
		std::string Cyan_On_Bright_White( std::string_view s );
		std::string White_On_Black( std::string_view s );
		std::string White_On_Red( std::string_view s );
		std::string White_On_Green( std::string_view s );
		std::string White_On_Yellow( std::string_view s );
		std::string White_On_Blue( std::string_view s );
		std::string White_On_Magenta( std::string_view s );
		std::string White_On_Cyan( std::string_view s );
		std::string White_On_White( std::string_view s );
		std::string White_On_Grey( std::string_view s );
		std::string White_On_Bright_Red( std::string_view s );
		std::string White_On_Bright_Green( std::string_view s );
		std::string White_On_Bright_Yellow( std::string_view s );
		std::string White_On_Bright_Blue( std::string_view s );
		std::string White_On_Bright_Magenta( std::string_view s );
		std::string White_On_Bright_Cyan( std::string_view s );
		std::string White_On_Bright_White( std::string_view s );
		std::string Grey( std::string_view s );
		std::string Bright_Red( std::string_view s );
		std::string Bright_Green( std::string_view s );
		std::string Bright_Yellow( std::string_view s );
		std::string Bright_Blue( std::string_view s );
		std::string Bright_Magenta( std::string_view s );
		std::string Bright_Cyan( std::string_view s );
		std::string Bright_White( std::string_view s );
		std::string On_Grey( std::string_view s );
		std::string On_Bright_Red( std::string_view s );
		std::string On_Bright_Green( std::string_view s );
		std::string On_Bright_Yellow( std::string_view s );
		std::string On_Bright_Blue( std::string_view s );
		std::string On_Bright_Magenta( std::string_view s );
		std::string On_Bright_Cyan( std::string_view s );
		std::string On_Bright_White( std::string_view s );
		std::string Grey_On_Black( std::string_view s );
		std::string Grey_On_Red( std::string_view s );
		std::string Grey_On_Green( std::string_view s );
		std::string Grey_On_Yellow( std::string_view s );
		std::string Grey_On_Blue( std::string_view s );
		std::string Grey_On_Magenta( std::string_view s );
		std::string Grey_On_Cyan( std::string_view s );
		std::string Grey_On_White( std::string_view s );
		std::string Grey_On_Grey( std::string_view s );
		std::string Grey_On_Bright_Red( std::string_view s );
		std::string Grey_On_Bright_Green( std::string_view s );
		std::string Grey_On_Bright_Yellow( std::string_view s );
		std::string Grey_On_Bright_Blue( std::string_view s );
		std::string Grey_On_Bright_Magenta( std::string_view s );
		std::string Grey_On_Bright_Cyan( std::string_view s );
		std::string Grey_On_Bright_White( std::string_view s );
		std::string Bright_Red_On_Black( std::string_view s );
		std::string Bright_Red_On_Red( std::string_view s );
		std::string Bright_Red_On_Green( std::string_view s );
		std::string Bright_Red_On_Yellow( std::string_view s );
		std::string Bright_Red_On_Blue( std::string_view s );
		std::string Bright_Red_On_Magenta( std::string_view s );
		std::string Bright_Red_On_Cyan( std::string_view s );
		std::string Bright_Red_On_White( std::string_view s );
		std::string Bright_Red_On_Grey( std::string_view s );
		std::string Bright_Red_On_Bright_Red( std::string_view s );
		std::string Bright_Red_On_Bright_Green( std::string_view s );
		std::string Bright_Red_On_Bright_Yellow( std::string_view s );
		std::string Bright_Red_On_Bright_Blue( std::string_view s );
		std::string Bright_Red_On_Bright_Magenta( std::string_view s );
		std::string Bright_Red_On_Bright_Cyan( std::string_view s );
		std::string Bright_Red_On_Bright_White( std::string_view s );
		std::string Bright_Green_On_Black( std::string_view s );
		std::string Bright_Green_On_Red( std::string_view s );
		std::string Bright_Green_On_Green( std::string_view s );
		std::string Bright_Green_On_Yellow( std::string_view s );
		std::string Bright_Green_On_Blue( std::string_view s );
		std::string Bright_Green_On_Magenta( std::string_view s );
		std::string Bright_Green_On_Cyan( std::string_view s );
		std::string Bright_Green_On_White( std::string_view s );
		std::string Bright_Green_On_Grey( std::string_view s );
		std::string Bright_Green_On_Bright_Red( std::string_view s );
		std::string Bright_Green_On_Bright_Green( std::string_view s );
		std::string Bright_Green_On_Bright_Yellow( std::string_view s );
		std::string Bright_Green_On_Bright_Blue( std::string_view s );
		std::string Bright_Green_On_Bright_Magenta( std::string_view s );
		std::string Bright_Green_On_Bright_Cyan( std::string_view s );
		std::string Bright_Green_On_Bright_White( std::string_view s );
		std::string Bright_Yellow_On_Black( std::string_view s );
		std::string Bright_Yellow_On_Red( std::string_view s );
		std::string Bright_Yellow_On_Green( std::string_view s );
		std::string Bright_Yellow_On_Yellow( std::string_view s );
		std::string Bright_Yellow_On_Blue( std::string_view s );
		std::string Bright_Yellow_On_Magenta( std::string_view s );
		std::string Bright_Yellow_On_Cyan( std::string_view s );
		std::string Bright_Yellow_On_White( std::string_view s );
		std::string Bright_Yellow_On_Grey( std::string_view s );
		std::string Bright_Yellow_On_Bright_Red( std::string_view s );
		std::string Bright_Yellow_On_Bright_Green( std::string_view s );
		std::string Bright_Yellow_On_Bright_Yellow( std::string_view s );
		std::string Bright_Yellow_On_Bright_Blue( std::string_view s );
		std::string Bright_Yellow_On_Bright_Magenta( std::string_view s );
		std::string Bright_Yellow_On_Bright_Cyan( std::string_view s );
		std::string Bright_Yellow_On_Bright_White( std::string_view s );
		std::string Bright_Blue_On_Black( std::string_view s );
		std::string Bright_Blue_On_Red( std::string_view s );
		std::string Bright_Blue_On_Green( std::string_view s );
		std::string Bright_Blue_On_Yellow( std::string_view s );
		std::string Bright_Blue_On_Blue( std::string_view s );
		std::string Bright_Blue_On_Magenta( std::string_view s );
		std::string Bright_Blue_On_Cyan( std::string_view s );
		std::string Bright_Blue_On_White( std::string_view s );
		std::string Bright_Blue_On_Grey( std::string_view s );
		std::string Bright_Blue_On_Bright_Red( std::string_view s );
		std::string Bright_Blue_On_Bright_Green( std::string_view s );
		std::string Bright_Blue_On_Bright_Yellow( std::string_view s );
		std::string Bright_Blue_On_Bright_Blue( std::string_view s );
		std::string Bright_Blue_On_Bright_Magenta( std::string_view s );
		std::string Bright_Blue_On_Bright_Cyan( std::string_view s );
		std::string Bright_Blue_On_Bright_White( std::string_view s );
		std::string Bright_Magenta_On_Black( std::string_view s );
		std::string Bright_Magenta_On_Red( std::string_view s );
		std::string Bright_Magenta_On_Green( std::string_view s );
		std::string Bright_Magenta_On_Yellow( std::string_view s );
		std::string Bright_Magenta_On_Blue( std::string_view s );
		std::string Bright_Magenta_On_Magenta( std::string_view s );
		std::string Bright_Magenta_On_Cyan( std::string_view s );
		std::string Bright_Magenta_On_White( std::string_view s );
		std::string Bright_Magenta_On_Grey( std::string_view s );
		std::string Bright_Magenta_On_Bright_Red( std::string_view s );
		std::string Bright_Magenta_On_Bright_Green( std::string_view s );
		std::string Bright_Magenta_On_Bright_Yellow( std::string_view s );
		std::string Bright_Magenta_On_Bright_Blue( std::string_view s );
		std::string Bright_Magenta_On_Bright_Magenta( std::string_view s );
		std::string Bright_Magenta_On_Bright_Cyan( std::string_view s );
		std::string Bright_Magenta_On_Bright_White( std::string_view s );
		std::string Bright_Cyan_On_Black( std::string_view s );
		std::string Bright_Cyan_On_Red( std::string_view s );
		std::string Bright_Cyan_On_Green( std::string_view s );
		std::string Bright_Cyan_On_Yellow( std::string_view s );
		std::string Bright_Cyan_On_Blue( std::string_view s );
		std::string Bright_Cyan_On_Magenta( std::string_view s );
		std::string Bright_Cyan_On_Cyan( std::string_view s );
		std::string Bright_Cyan_On_White( std::string_view s );
		std::string Bright_Cyan_On_Grey( std::string_view s );
		std::string Bright_Cyan_On_Bright_Red( std::string_view s );
		std::string Bright_Cyan_On_Bright_Green( std::string_view s );
		std::string Bright_Cyan_On_Bright_Yellow( std::string_view s );
		std::string Bright_Cyan_On_Bright_Blue( std::string_view s );
		std::string Bright_Cyan_On_Bright_Magenta( std::string_view s );
		std::string Bright_Cyan_On_Bright_Cyan( std::string_view s );
		std::string Bright_Cyan_On_Bright_White( std::string_view s );
		std::string Bright_White_On_Black( std::string_view s );
		std::string Bright_White_On_Red( std::string_view s );
		std::string Bright_White_On_Green( std::string_view s );
		std::string Bright_White_On_Yellow( std::string_view s );
		std::string Bright_White_On_Blue( std::string_view s );
		std::string Bright_White_On_Magenta( std::string_view s );
		std::string Bright_White_On_Cyan( std::string_view s );
		std::string Bright_White_On_White( std::string_view s );
		std::string Bright_White_On_Grey( std::string_view s );
		std::string Bright_White_On_Bright_Red( std::string_view s );
		std::string Bright_White_On_Bright_Green( std::string_view s );
		std::string Bright_White_On_Bright_Yellow( std::string_view s );
		std::string Bright_White_On_Bright_Blue( std::string_view s );
		std::string Bright_White_On_Bright_Magenta( std::string_view s );
		std::string Bright_White_On_Bright_Cyan( std::string_view s );
		std::string Bright_White_On_Bright_White( std::string_view s );
	}  // namespace Tag
}  // namespace se_colors