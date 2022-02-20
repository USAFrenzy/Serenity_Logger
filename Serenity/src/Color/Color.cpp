#include <serenity/Color/Color.h>

namespace tag_helper {
	std::string toString(const std::string_view s)
	{
		return std::string(s.data(), s.size());
	}
}    // namespace tag_helper

// So Far, Only Real Issue I've Encountered Seems To Be The Very Common Issue Of
// Windows Terminal Issue #32 where when resizing the console window, the last
// color used fills in empty cells in that row. Not going to lie, kinda
// frustrating.. (https://github.com/microsoft/terminal/issues/32)
namespace se_colors {
	namespace Tag {
		// *************************************************************************************************************
		std::string Reset()
		{
			return formats::reset;
		}

		std::string Black(std::string_view s)
		{
			return basic_colors::foreground::black + tag_helper::toString(s) + Reset();
		}

		std::string Red(std::string_view s)
		{
			return basic_colors::foreground::red + tag_helper::toString(s) + Reset();
		}

		std::string Green(std::string_view s)
		{
			return basic_colors::foreground::green + tag_helper::toString(s) + Reset();
		}

		std::string Yellow(std::string_view s)
		{
			return basic_colors::foreground::yellow + tag_helper::toString(s) + Reset();
		}

		std::string Blue(std::string_view s)
		{
			return basic_colors::foreground::blue + tag_helper::toString(s) + Reset();
		}

		std::string Magenta(std::string_view s)
		{
			return basic_colors::foreground::magenta + tag_helper::toString(s) + Reset();
		}

		std::string Cyan(std::string_view s)
		{
			return basic_colors::foreground::cyan + tag_helper::toString(s) + Reset();
		}

		std::string White(std::string_view s)
		{
			return basic_colors::foreground::white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string On_Black(std::string_view s)
		{
			return basic_colors::background::black + tag_helper::toString(s) + Reset();
		}

		std::string On_Red(std::string_view s)
		{
			return basic_colors::background::red + tag_helper::toString(s) + Reset();
		}

		std::string On_Green(std::string_view s)
		{
			return basic_colors::background::green + tag_helper::toString(s) + Reset();
		}

		std::string On_Yellow(std::string_view s)
		{
			return basic_colors::background::yellow + tag_helper::toString(s) + Reset();
		}

		std::string On_Blue(std::string_view s)
		{
			return basic_colors::background::blue + tag_helper::toString(s) + Reset();
		}

		std::string On_Magenta(std::string_view s)
		{
			return basic_colors::background::magenta + tag_helper::toString(s) + Reset();
		}

		std::string On_Cyan(std::string_view s)
		{
			return basic_colors::background::cyan + tag_helper::toString(s) + Reset();
		}

		std::string On_White(std::string_view s)
		{
			return basic_colors::background::white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Black_On_Black(std::string_view s)
		{
			return basic_colors::combos::black::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Red(std::string_view s)
		{
			return basic_colors::combos::black::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Green(std::string_view s)
		{
			return basic_colors::combos::black::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::black::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Blue(std::string_view s)
		{
			return basic_colors::combos::black::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::black::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::black::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_White(std::string_view s)
		{
			return basic_colors::combos::black::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Grey(std::string_view s)
		{
			return basic_colors::combos::black::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Black_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::black::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Red_On_Black(std::string_view s)
		{
			return basic_colors::combos::red::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Red(std::string_view s)
		{
			return basic_colors::combos::red::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Green(std::string_view s)
		{
			return basic_colors::combos::red::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::red::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Blue(std::string_view s)
		{
			return basic_colors::combos::red::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::red::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::red::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_White(std::string_view s)
		{
			return basic_colors::combos::red::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Grey(std::string_view s)
		{
			return basic_colors::combos::red::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Red_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::red::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Green_On_Black(std::string_view s)
		{
			return basic_colors::combos::green::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Red(std::string_view s)
		{
			return basic_colors::combos::green::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Green(std::string_view s)
		{
			return basic_colors::combos::green::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::green::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Blue(std::string_view s)
		{
			return basic_colors::combos::green::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::green::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::green::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_White(std::string_view s)
		{
			return basic_colors::combos::green::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Grey(std::string_view s)
		{
			return basic_colors::combos::green::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Green_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::green::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Yellow_On_Black(std::string_view s)
		{
			return basic_colors::combos::yellow::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Red(std::string_view s)
		{
			return basic_colors::combos::yellow::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Green(std::string_view s)
		{
			return basic_colors::combos::yellow::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::yellow::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Blue(std::string_view s)
		{
			return basic_colors::combos::yellow::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::yellow::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::yellow::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_White(std::string_view s)
		{
			return basic_colors::combos::yellow::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Grey(std::string_view s)
		{
			return basic_colors::combos::yellow::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Yellow_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::yellow::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Blue_On_Black(std::string_view s)
		{
			return basic_colors::combos::blue::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Red(std::string_view s)
		{
			return basic_colors::combos::blue::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Green(std::string_view s)
		{
			return basic_colors::combos::blue::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::blue::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Blue(std::string_view s)
		{
			return basic_colors::combos::blue::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::blue::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::blue::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_White(std::string_view s)
		{
			return basic_colors::combos::blue::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Grey(std::string_view s)
		{
			return basic_colors::combos::blue::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Blue_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::blue::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Magenta_On_Black(std::string_view s)
		{
			return basic_colors::combos::magenta::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Red(std::string_view s)
		{
			return basic_colors::combos::magenta::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Green(std::string_view s)
		{
			return basic_colors::combos::magenta::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::magenta::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Blue(std::string_view s)
		{
			return basic_colors::combos::magenta::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::magenta::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::magenta::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_White(std::string_view s)
		{
			return basic_colors::combos::magenta::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Grey(std::string_view s)
		{
			return basic_colors::combos::magenta::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Magenta_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::magenta::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Cyan_On_Black(std::string_view s)
		{
			return basic_colors::combos::cyan::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Red(std::string_view s)
		{
			return basic_colors::combos::cyan::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Green(std::string_view s)
		{
			return basic_colors::combos::cyan::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::cyan::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Blue(std::string_view s)
		{
			return basic_colors::combos::cyan::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::cyan::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::cyan::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_White(std::string_view s)
		{
			return basic_colors::combos::cyan::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Grey(std::string_view s)
		{
			return basic_colors::combos::cyan::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Cyan_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::cyan::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string White_On_Black(std::string_view s)
		{
			return basic_colors::combos::white::on_black + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Red(std::string_view s)
		{
			return basic_colors::combos::white::on_red + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Green(std::string_view s)
		{
			return basic_colors::combos::white::on_green + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Yellow(std::string_view s)
		{
			return basic_colors::combos::white::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Blue(std::string_view s)
		{
			return basic_colors::combos::white::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Magenta(std::string_view s)
		{
			return basic_colors::combos::white::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Cyan(std::string_view s)
		{
			return basic_colors::combos::white::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string White_On_White(std::string_view s)
		{
			return basic_colors::combos::white::on_white + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Grey(std::string_view s)
		{
			return basic_colors::combos::white::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Red(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Green(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Yellow(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Blue(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Magenta(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_Cyan(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string White_On_Bright_White(std::string_view s)
		{
			return basic_colors::combos::white::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Grey(std::string_view s)
		{
			return bright_colors::foreground::grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red(std::string_view s)
		{
			return bright_colors::foreground::red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green(std::string_view s)
		{
			return bright_colors::foreground::green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow(std::string_view s)
		{
			return bright_colors::foreground::yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue(std::string_view s)
		{
			return bright_colors::foreground::blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta(std::string_view s)
		{
			return bright_colors::foreground::magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan(std::string_view s)
		{
			return bright_colors::foreground::cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White(std::string_view s)
		{
			return bright_colors::foreground::white + tag_helper::toString(s) + Reset();
		}

		std::string On_Grey(std::string_view s)
		{
			return bright_colors::background::grey + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Red(std::string_view s)
		{
			return bright_colors::background ::red + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Green(std::string_view s)
		{
			return bright_colors::background::green + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::background::yellow + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Blue(std::string_view s)
		{
			return bright_colors::background::blue + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::background::magenta + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::background::cyan + tag_helper::toString(s) + Reset();
		}

		std::string On_Bright_White(std::string_view s)
		{
			return bright_colors::background::white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Grey_On_Black(std::string_view s)
		{
			return bright_colors::combos::grey::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Red(std::string_view s)
		{
			return bright_colors::combos::grey::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Green(std::string_view s)
		{
			return bright_colors::combos::grey::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::grey::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Blue(std::string_view s)
		{
			return bright_colors::combos::grey::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::grey::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::grey::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_White(std::string_view s)
		{
			return bright_colors::combos::grey::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Grey(std::string_view s)
		{
			return bright_colors::combos::grey::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Grey_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::grey::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Red_On_Black(std::string_view s)
		{
			return bright_colors::combos::red::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Red(std::string_view s)
		{
			return bright_colors::combos::red::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Green(std::string_view s)
		{
			return bright_colors::combos::red::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::red::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Blue(std::string_view s)
		{
			return bright_colors::combos::red::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::red::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::red::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_White(std::string_view s)
		{
			return bright_colors::combos::red::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Grey(std::string_view s)
		{
			return bright_colors::combos::red::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Red_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::red::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Green_On_Black(std::string_view s)
		{
			return bright_colors::combos::green::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Red(std::string_view s)
		{
			return bright_colors::combos::green::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Green(std::string_view s)
		{
			return bright_colors::combos::green::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::green::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Blue(std::string_view s)
		{
			return bright_colors::combos::green::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::green::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::green::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_White(std::string_view s)
		{
			return bright_colors::combos::green::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Grey(std::string_view s)
		{
			return bright_colors::combos::green::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Green_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::green::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Yellow_On_Black(std::string_view s)
		{
			return bright_colors::combos::yellow::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Red(std::string_view s)
		{
			return bright_colors::combos::yellow::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Green(std::string_view s)
		{
			return bright_colors::combos::yellow::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::yellow::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Blue(std::string_view s)
		{
			return bright_colors::combos::yellow::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::yellow::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::yellow::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_White(std::string_view s)
		{
			return bright_colors::combos::yellow::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Grey(std::string_view s)
		{
			return bright_colors::combos::yellow::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Yellow_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::yellow::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Blue_On_Black(std::string_view s)
		{
			return bright_colors::combos::blue::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Red(std::string_view s)
		{
			return bright_colors::combos::blue::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Green(std::string_view s)
		{
			return bright_colors::combos::blue::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::blue::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Blue(std::string_view s)
		{
			return bright_colors::combos::blue::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::blue::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::blue::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_White(std::string_view s)
		{
			return bright_colors::combos::blue::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Grey(std::string_view s)
		{
			return bright_colors::combos::blue::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Blue_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::blue::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Magenta_On_Black(std::string_view s)
		{
			return bright_colors::combos::magenta::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Red(std::string_view s)
		{
			return bright_colors::combos::magenta::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Green(std::string_view s)
		{
			return bright_colors::combos::magenta::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::magenta::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Blue(std::string_view s)
		{
			return bright_colors::combos::magenta::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::magenta::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::magenta::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_White(std::string_view s)
		{
			return bright_colors::combos::magenta::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Grey(std::string_view s)
		{
			return bright_colors::combos::magenta::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Magenta_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::magenta::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_Cyan_On_Black(std::string_view s)
		{
			return bright_colors::combos::cyan::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Red(std::string_view s)
		{
			return bright_colors::combos::cyan::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Green(std::string_view s)
		{
			return bright_colors::combos::cyan::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::cyan::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Blue(std::string_view s)
		{
			return bright_colors::combos::cyan::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::cyan::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::cyan::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_White(std::string_view s)
		{
			return bright_colors::combos::cyan::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Grey(std::string_view s)
		{
			return bright_colors::combos::cyan::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_Cyan_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::cyan::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************
		std::string Bright_White_On_Black(std::string_view s)
		{
			return bright_colors::combos::white::on_black + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Red(std::string_view s)
		{
			return bright_colors::combos::white::on_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Green(std::string_view s)
		{
			return bright_colors::combos::white::on_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Yellow(std::string_view s)
		{
			return bright_colors::combos::white::on_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Blue(std::string_view s)
		{
			return bright_colors::combos::white::on_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Magenta(std::string_view s)
		{
			return bright_colors::combos::white::on_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Cyan(std::string_view s)
		{
			return bright_colors::combos::white::on_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_White(std::string_view s)
		{
			return bright_colors::combos::white::on_white + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Grey(std::string_view s)
		{
			return bright_colors::combos::white::on_grey + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Red(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_red + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Green(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_green + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Yellow(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_yellow + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Blue(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_blue + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Magenta(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_magenta + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_Cyan(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_cyan + tag_helper::toString(s) + Reset();
		}

		std::string Bright_White_On_Bright_White(std::string_view s)
		{
			return bright_colors::combos::white::on_bright_white + tag_helper::toString(s) + Reset();
		}

		// *************************************************************************************************************

	};    // namespace Tag
}    // namespace se_colors