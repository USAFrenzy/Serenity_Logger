
#include <string_view>
#include <iostream>
#include <map>


#define PLATFORM_WINDOWS
#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#endif


namespace se_colors
{
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
		clear_line,
		no_change,
	};
	/*************************************************************************
	 * For The Foreground Colors, The Bright Options Can Be Used Via The
	 * Format Bold Option And The Respective Color Option.
	 ************************************************************************/
	// Foreground colors
	enum class fg_colors
	{
		black,
		red,
		green,
		yellow,
		blue,
		magenta,
		cyan,
		white,
		no_change,
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
		no_change,
	};

	struct ConsoleColors
	{
		// ansi color codes supported in Win 10+, therefore, targetting Win 10+ due to what the timeframe of the project I'll
		// be using this in (< win 8.1 EOL). Technically, could add the option to enable virtual terminal as a startup option
		// in constructor - just so ANSI can still be used as-is here w/o the need of HANDLE/WORD-like variables for < Win 10?


		// Message Handle For Color Info
		struct msg_color
		{
			fg_colors fg  = fg_colors::white;
			bg_colors bg  = bg_colors::black;
			format    fmt = format::no_change;
		};

		void ColorPrint( const std::string_view &message, msg_color color )
		{
			auto colorAttributes = toColorCode( format::reset ) + toColorCode( color.fg ) + toColorCode( color.bg ) +
					       toColorCode( color.fmt );
			auto msg = toString( message );
			std::cout << colorAttributes << msg << toColorCode( format::reset ) << "\n";
		}

		std::string toString( const std::string_view &stringView )
		{
			return std::string( stringView.data( ), stringView.size( ) );
		}

		template <typename T> std::string toColorCode( T msgOption )
		{
			return mapMsgOption( msgOption );
		}

		template <typename T> std::string mapMsgOption( T msgOption )
		{
			if constexpr( std::is_same_v<T, format> ) {
				std::map<format, std::string_view> formatCode = {
				  { format::blink, "\033[5m" },     { format::bold, "\033[1m" },      { format::clear_line, "\033[K" },
				  { format::concealed, "\033[8m" }, { format::dark, "\033[2m" },      { format::reset, "\033[0m" },
				  { format::reverse, "\033[7m" },   { format::underline, "\033[4m" }, { format::no_change, "" } };
				std::string_view result;
				auto             iterator = formatCode.find( msgOption );
				if( iterator != formatCode.end( ) ) {
					result = iterator->second;
				}
				return toString( result );
			}
			else if constexpr( std::is_same_v<T, fg_colors> ) {
				std::map<fg_colors, std::string_view> colorCode = {
				  { fg_colors::black, "\033[30m" }, { fg_colors::red, "\033[31m" },
				  { fg_colors::green, "\033[32m" }, { fg_colors::yellow, "\033[33m" },
				  { fg_colors::blue, "\033[34m" },  { fg_colors::magenta, "\033[35m" },
				  { fg_colors::cyan, "\033[36m" },  { fg_colors::white, "\033[37m" },
				  { fg_colors::no_change, "" } };
				std::string_view result;
				auto             iterator = colorCode.find( msgOption );
				if( iterator != colorCode.end( ) ) {
					result = iterator->second;
				}
				return toString( result );
			}
			else if constexpr( std::is_same_v<T, bg_colors> ) {
				std::map<bg_colors, std::string_view> colorCode = {
				  { bg_colors::black, "\033[40m" }, { bg_colors::red, "\033[41m" },
				  { bg_colors::green, "\033[42m" }, { bg_colors::yellow, "\033[43m" },
				  { bg_colors::blue, "\033[44m" },  { bg_colors::magenta, "\033[45m" },
				  { bg_colors::cyan, "\033[46m" },  { bg_colors::white, "\033[47m" },
				  { bg_colors::no_change, "" } };
				std::string_view result;
				auto             iterator = colorCode.find( msgOption );
				if( iterator != colorCode.end( ) ) {
					result = iterator->second;
				}
				return toString( result );
			}
			else {
				return std::string( );
			}
		}


	};  // struct ConsoleColors


	namespace Tag
	{
#ifdef PLATFORM_WINDOWS
		void enableVirCon( )
		{
			HANDLE handleOut = GetStdHandle( STD_OUTPUT_HANDLE );
			DWORD  consoleMode;
			GetConsoleMode( handleOut, &consoleMode );
			consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			consoleMode |= DISABLE_NEWLINE_AUTO_RETURN;
			SetConsoleMode( handleOut, consoleMode );
		}
#endif  // PLATFORM_WINDOWS

		// TODO: void return types not the way to go - fix when looking at again!

		std::string toString( const std::string_view s )
		{
			return std::string( s.data( ), s.size( ) );
		}
		void Flush( std::ostream &os )
		{
			os.flush( );
		}
		std::ostream &reset( std::ostream &os )
		{
			Flush( os );
			return os << "\033[0m";
		}
		std::ostream &tagIt( std::string_view s, void color( std::ostream &os ), std::ostream &os = std::cout )
		{
			Flush( os );
			return os << color << toString( s ) << reset;
		}

		void black( std::ostream &os )
		{
			Flush( os );

			os << "\033[30m ";
		}
		void blue( std::ostream &os )
		{
			Flush( os );

			os << "\033[34m";
		}
		void green( std::ostream &os )
		{
			Flush( os );

			os << "\033[32m";
		}
		void cyan( std::ostream &os )
		{
			Flush( os );

			os << "\033[36m";
		}
		void red( std::ostream &os )
		{
			Flush( os );

			os << "\033[31";
		}
		void magenta( std::ostream &os )
		{
			Flush( os );

			os << "\033[35";
		}
		void yellow( std::ostream &os )
		{
			Flush( os );

			os << "\033[33";
		}
		void white( std::ostream &os )
		{
			Flush( os );

			os << "\033[37";
		}
		void grey( std::ostream &os )
		{
			Flush( os );

			os << "\033[90";
		}
		void light_blue( std::ostream &os )
		{
			Flush( os );

			os << "\033[94";
		}
		void light_green( std::ostream &os )
		{
			Flush( os );

			os << "\033[92";
		}
		void light_cyan( std::ostream &os )
		{
			Flush( os );

			os << "\033[96";
		}
		void light_red( std::ostream &os )
		{
			Flush( os );
			os << "\033[91";
		}
		void light_magenta( std::ostream &os )
		{
			Flush( os );

			os << "\033[95";
		}
		void light_yellow( std::ostream &os )
		{
			Flush( os );

			os << "\033[93";
		}
		void bright_white( std::ostream &os )
		{
			Flush( os );

			os << "\033[97";
		}
	}  // namespace Tag
}  // namespace se_colors


int main( )
{
	using namespace se_colors;


	ConsoleColors            C;
	ConsoleColors::msg_color msgColor;

	// Trace Is Deafult Color
	msgColor.fg  = fg_colors::white;
	msgColor.bg  = bg_colors::black;
	msgColor.fmt = format::reset;
	C.ColorPrint( "[Trace]: White", msgColor );
	// Info Is Light Green
	msgColor.fg  = fg_colors::green;
	msgColor.bg  = bg_colors::black;
	msgColor.fmt = format::bold;
	C.ColorPrint( "[Info]: Green", msgColor );
	// Warning Is Light Yellow
	msgColor.fg  = fg_colors::yellow;
	msgColor.bg  = bg_colors::black;
	msgColor.fmt = format::bold;

	C.ColorPrint( "[Warning]: Yellow", msgColor );
	// Error Is Dark Red
	msgColor.fg  = fg_colors::red;
	msgColor.bg  = bg_colors::black;
	msgColor.fmt = format::no_change;
	C.ColorPrint( "[Error]: Red", msgColor );
	// Fatal Is Light Yellow On Dark Red
	msgColor.fg  = fg_colors::yellow;
	msgColor.bg  = bg_colors::red;
	msgColor.fmt = format::bold;
	C.ColorPrint( "[Fatal]: Light Yellow On Red", msgColor );
	// Above Are The Settings For Logging Message Colors (Influenced by spdlog message level colors)
	// - "fatal" changed to something I personally thought was more visually appealing as a default.
	//  (spdlog does offer changes to default colors via set_color() either way)

	/*****************************************************************************************************************************
	 *	Would Like To See If Possible To Add Tagging In Messages, Something Like:
	 *  ColorPrint("This Would Be ^CT A Color &CT Tagged ^CT Section &CT", msgColor, tagColor1, tagColor2);
	 *  Where The Tag Colors Are Optional And Variadic.. Honestly Might Be More Difficult Then I Initially Thought =/
	 ****************************************************************************************************************************/

	std::cout << "Some Examples Of ";
	Tag::tagIt( "Color", Tag::blue );
	Tag::tagIt( " Tagging\n", Tag::green );
	Tag::tagIt( "Magenta", Tag::magenta );
	Tag::tagIt( "Cyan", Tag::cyan );
	Tag::tagIt( "Yellow", Tag::yellow );
	Tag::tagIt( "Red\n", Tag::red );

	// Kinda Works So Far? Need To overload "<<" or change return types so using std::cout/cerr can be streamlined..
	// EDIT: Some weird stuff happening with some of the color choices in regards to cutting off chars...

	/******************************************************************************************************************************
	 * Have Some Basic Tagging Set (Rough Draft Idea Anyways), But Still No Way Of Setting Up A Message Parser For Substitution Yet
	 *******************************************************************************************************************************/
}
