
#include <string_view>
#include <iostream>
#include <map>

namespace se_colors
{
	struct ConsoleColors
	{
		// ansi color codes supported in Win 10+, therfore,
		// targetting Win 10+ due to what the timeframe of
		// the project I'll be using this in (< win 8.1 EOL)


		// Format
		enum class format
		{
			reset,
			bold,
			dark,
			underline,
			blink,
			reverse,
			concealed,
			strike_out,
			clear_line,
		};
		// Foreground colors
		enum class fg_colors
		{
			black = 0,
			red,
			green,
			yellow,
			blue,
			magenta,
			cyan,
			white,
		};


		// Background colors
		enum class bg_colors
		{
			black,
			red,
			green,
			yellow,
			blue,
			magenta,
			cyan,
			white,
		};
		struct message_color
		{
			format    fmt = format::reset;
			fg_colors fg  = fg_colors::white;
			bg_colors bg  = bg_colors::black;
		};


		/// Bold colors
		const std::string_view yellow_bold = "\033[33m\033[1m";
		const std::string_view red_bold    = "\033[31m\033[1m";
		const std::string_view bold_on_red = "\033[1m\033[41m";


		void ColorPrint( std::string msg, message_color color )
		{
			std::cout << toColorCode( color.fg ) << toColorCode( color.bg ) << msg << toColorCode( format::reset ) << "\n";
		}

		std::string toString( const std::string_view &stringView )
		{
			return std::string( stringView.data( ), stringView.size( ) );
		}

	      private:
		std::string toColorCode( format fmt )
		{
			std::map<format, std::string_view> formatCode = {
			  { format::blink, "\033[5m" },     { format::bold, "\033[1m" },       { format::clear_line, "\033[K" },
			  { format::concealed, "\033[8m" }, { format::dark, "\033[2m" },       { format::reset, "\033[0m" },
			  { format::reverse, "\033[7m" },   { format::strike_out, "\033[9m" }, { format::underline, "\033[4m" } };
			std::string_view result;
			auto             iterator = formatCode.find( fmt );
			if( iterator != formatCode.end( ) ) {
				result = iterator->second;
			}
			return toString( result );
		}

		std::string toColorCode( fg_colors color )
		{
			std::map<fg_colors, std::string_view> colorCode = {
			  { fg_colors::black, "\033[30m" },  { fg_colors::red, "\033[31m" },  { fg_colors::green, "\033[32m" },
			  { fg_colors::yellow, "\033[33m" }, { fg_colors::blue, "\033[34m" }, { fg_colors::magenta, "\033[35m" },
			  { fg_colors::cyan, "\033[36m" },   { fg_colors::white, "\033[37m" } };
			std::string_view result;
			auto             iterator = colorCode.find( color );
			if( iterator != colorCode.end( ) ) {
				result = iterator->second;
			}
			return toString( result );
		}

		std::string toColorCode( bg_colors color )
		{
			std::map<bg_colors, std::string_view> colorCode = {
			  { bg_colors::black, "\033[40m" },  { bg_colors::red, "\033[41m" },  { bg_colors::green, "\033[42m" },
			  { bg_colors::yellow, "\033[43m" }, { bg_colors::blue, "\033[44m" }, { bg_colors::magenta, "\033[45m" },
			  { bg_colors::cyan, "\033[46m" },   { bg_colors::white, "\033[47m" } };
			std::string_view result;
			auto             iterator = colorCode.find( color );
			if( iterator != colorCode.end( ) ) {
				result = iterator->second;
			}
			return toString( result );
		}

	};  // struct ConsoleColors

}  // namespace se_colors


int main( )
{
	using color = se_colors::ConsoleColors;
	color                C;
	color::message_color msgColor;

	msgColor.fg = color::fg_colors::white;
	C.ColorPrint( "Trace = White", msgColor );
	msgColor.fg = color::fg_colors::green;
	C.ColorPrint( "Info = Green", msgColor );
	msgColor.fg = color::fg_colors::yellow;
	C.ColorPrint( "Warning = Yellow", msgColor );
	msgColor.fg = color::fg_colors::red;
	C.ColorPrint( "Error = Red", msgColor );
	msgColor.fg = color::fg_colors::black;
	msgColor.bg = color::bg_colors::yellow;
	C.ColorPrint( "Fatal = Black On Yellow", msgColor );
}
