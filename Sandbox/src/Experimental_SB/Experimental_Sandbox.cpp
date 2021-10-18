#include <string_view>
#include <iostream>
#include <map>


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
		grey,
		red,
		bright_red,
		green,
		bright_green,
		yellow,
		bright_yellow,
		blue,
		bright_blue,
		magenta,
		bright_magenta,
		cyan,
		bright_cyan,
		white,
		bright_white,
		reset,
		no_change,
	};
	// Background colors
	enum class bg_colors
	{
		black,
		grey,
		red,
		bright_red,
		green,
		bright_green,
		yellow,
		bright_yellow,
		blue,
		bright_blue,
		magenta,
		bright_magenta,
		cyan,
		bright_cyan,
		white,
		bright_white,
		reset,
		no_change,
	};

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

		// So Far, Only Real Issue I've Encountered Seems To Be The Very Common Issue Of Windows Terminal Issue #32
		// where when resizing the console window, the last color used line wraps..
		// (https://github.com/microsoft/terminal/issues/32)
		std::string Reset( )
		{
			return toString( "\033[0m" );
		}

		template <typename T> std::string MapColorOption_impl( T msgOption )
		{
			std::string result;

			if constexpr( std::is_same_v<T, format> ) {
				std::map<format, std::string_view> formatCode = {
				  { format::blink, "\033[5m" },     { format::bold, "\033[1m" },      { format::clear_line, "\033[K" },
				  { format::concealed, "\033[8m" }, { format::dark, "\033[2m" },      { format::reset, "\033[0m" },
				  { format::reverse, "\033[7m" },   { format::underline, "\033[4m" }, { format::no_change, "" } };
				auto iterator = formatCode.find( msgOption );
				if( iterator != formatCode.end( ) ) {
					result = iterator->second;
				}
				return result;
			}
			else if constexpr( std::is_same_v<T, fg_colors> ) {
				std::map<fg_colors, std::string_view> colorCode = {
				  { fg_colors::black, "\033[30m" },   { fg_colors::grey, "\033[90m" },
				  { fg_colors::red, "\033[31m" },     { fg_colors::bright_red, "\033[91m" },
				  { fg_colors::green, "\033[32m" },   { fg_colors::bright_green, "\033[92m" },
				  { fg_colors::yellow, "\033[33m" },  { fg_colors::bright_yellow, "\033[93m" },
				  { fg_colors::blue, "\033[34m" },    { fg_colors::bright_blue, "\033[94m" },
				  { fg_colors::magenta, "\033[35m" }, { fg_colors::bright_magenta, "\033[95m" },
				  { fg_colors::cyan, "\033[36m" },    { fg_colors::bright_cyan, "\033[96m" },
				  { fg_colors::white, "\033[37m" },   { fg_colors::bright_white, "\033[97m" },
				  { fg_colors::reset, "\033[39" },    { fg_colors::no_change, "" } };
				auto iterator = colorCode.find( msgOption );
				if( iterator != colorCode.end( ) ) {
					result = iterator->second;
				}
				return result;
			}
			else if constexpr( std::is_same_v<T, bg_colors> ) {
				std::map<bg_colors, std::string_view> colorCode = {
				  { bg_colors::black, "\033[40m" },   { bg_colors::grey, "\033[100m" },
				  { bg_colors::red, "\033[41m" },     { bg_colors::bright_red, "\033[101m" },
				  { bg_colors::green, "\033[42m" },   { bg_colors::bright_green, "\033[102m" },
				  { bg_colors::yellow, "\033[43m" },  { bg_colors::bright_yellow, "\033[103m" },
				  { bg_colors::blue, "\033[44m" },    { bg_colors::bright_blue, "\033[104m" },
				  { bg_colors::magenta, "\033[45m" }, { bg_colors::bright_magenta, "\033[105m" },
				  { bg_colors::cyan, "\033[46m" },    { bg_colors::bright_cyan, "\033[106m" },
				  { bg_colors::white, "\033[47m" },   { bg_colors::bright_white, "\033[107m" },
				  { bg_colors::reset, "\033[49" },    { bg_colors::no_change, "" } };
				auto iterator = colorCode.find( msgOption );
				if( iterator != colorCode.end( ) ) {
					result = iterator->second;
				}
				return result;
			}
			else {
				return result;
			}
		}  // mapMsgOption()

		template <typename T> std::string tagIt( std::string_view s, T fg )
		{
			return MapColorOption_impl( fg ) + toString( s ) + Reset( );
		}
		template <typename T, typename U> std::string tagIt( std::string_view s, T fg, U bg )
		{
			return MapColorOption_impl( fg ) + MapColorOption_impl( bg ) + toString( s ) + Reset( );
		}
		template <typename T, typename U, typename V> std::string tagIt( std::string_view s, T fmt, U fg, V bg )
		{
			return MapColorOption_impl( fmt ) + MapColorOption_impl( fg ) + MapColorOption_impl( bg ) + toString( s ) +
			       Reset( );
		}
	}  // namespace tag_helper

	// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
	// clang-format on

	namespace Tag
	{
		std::string Reset( )
		{
			return tag_helper::Reset( );
		}

		// This Does Far Less Aloocation Than The Original Red_On_Blue()
		// 56 bytes for this version Vs. 1936 bytes for the original...
		constexpr const char *r_on_b = "\033[31m\033[44m";
		std::string           red_on_blue( std::string_view s )
		{
			return r_on_b + tag_helper::toString( s ) + Reset( );
		}


		std::string Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue );
		}

		std::string Bright_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue );
		}

		std::string On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::blue );
		}

		// Regular Blue Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Blue_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::black );
		}
		std::string Blue_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::blue );
		}
		std::string Blue_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::cyan );
		}
		std::string Blue_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::green );
		}
		std::string Blue_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::magenta );
		}
		std::string Blue_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::red );
		}
		std::string Blue_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::white );
		}
		std::string Blue_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::blue, bg_colors::yellow );
		}
		// ****************************************************************************************
		// Light Blue Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Bright_Blue_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::black );
		}
		std::string Bright_Blue_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::blue );
		}
		std::string Bright_Blue_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::cyan );
		}
		std::string Bright_Blue_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::green );
		}
		std::string Bright_Blue_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::magenta );
		}
		std::string Bright_Blue_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::red );
		}
		std::string Bright_Blue_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::white );
		}
		std::string Bright_Blue_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_blue, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green );
		}

		std::string Bright_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_green );
		}

		std::string On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::green );
		}
		// Regular Green Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Green_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::black );
		}
		std::string Green_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::blue );
		}
		std::string Green_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::cyan );
		}
		std::string Green_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::green );
		}
		std::string Green_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::magenta );
		}
		std::string Green_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::red );
		}
		std::string Green_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::white );
		}
		std::string Green_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::green, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan );
		}

		std::string Bright_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_cyan );
		}

		std::string On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::cyan );
		}
		// Regular Cyan Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Cyan_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::black );
		}
		std::string Cyan_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::blue );
		}
		std::string Cyan_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::cyan );
		}
		std::string Cyan_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::green );
		}
		std::string Cyan_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::magenta );
		}
		std::string Cyan_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::red );
		}
		std::string Cyan_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::white );
		}
		std::string Cyan_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::cyan, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black );
		}

		std::string Grey( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::grey );
		}

		std::string On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::black );
		}
		// Regular Black Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Black_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::black );
		}
		std::string Black_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::blue );
		}
		std::string Black_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::cyan );
		}
		std::string Black_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::green );
		}
		std::string Black_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::magenta );
		}
		std::string Black_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::red );
		}
		std::string Black_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::white );
		}
		std::string Black_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::black, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta );
		}

		std::string Bright_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_magenta );
		}

		std::string On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::magenta );
		}
		// Regular Magenta Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Magenta_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::black );
		}
		std::string Magenta_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::blue );
		}
		std::string Magenta_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::cyan );
		}
		std::string Magenta_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::green );
		}
		std::string Magenta_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::magenta );
		}
		std::string Magenta_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::red );
		}
		std::string Magenta_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::white );
		}
		std::string Magenta_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::magenta, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red );
		}

		std::string Bright_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_red );
		}

		std::string On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::red );
		}
		// Regular Red Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Red_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::black );
		}
		std::string Red_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::blue );
		}
		std::string Red_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::cyan );
		}
		std::string Red_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::green );
		}
		std::string Red_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::magenta );
		}
		std::string Red_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::red );
		}
		std::string Red_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::white );
		}
		std::string Red_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::red, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white );
		}

		std::string Bright_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_white );
		}

		std::string On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::white );
		}
		// Regular White Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string White_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::black );
		}
		std::string White_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::blue );
		}
		std::string White_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::cyan );
		}
		std::string White_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::green );
		}
		std::string White_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::magenta );
		}
		std::string White_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::red );
		}
		std::string White_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::white );
		}
		std::string White_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::white, bg_colors::yellow );
		}
		// ****************************************************************************************
		std::string Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow );
		}

		std::string Bright_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::bright_yellow );
		}

		std::string On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, bg_colors::yellow );
		}
		// Regular Yellow Foreground On Regular [X] Color Background
		// ****************************************************************************************
		std::string Yellow_On_Black( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::black );
		}
		std::string Yellow_On_Blue( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::blue );
		}
		std::string Yellow_On_Cyan( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::cyan );
		}
		std::string Yellow_On_Green( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::green );
		}
		std::string Yellow_On_Magenta( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::magenta );
		}
		std::string Yellow_On_Red( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::red );
		}
		std::string Yellow_On_White( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::white );
		}
		std::string Yellow_On_Yellow( std::string_view s )
		{
			return tag_helper::tagIt( s, fg_colors::yellow, bg_colors::yellow );
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
			format    fmt = format::reset;
			fg_colors fg  = fg_colors::white;
			bg_colors bg  = bg_colors::black;
		};

		void ColorPrint( const std::string_view message, msg_color color )
		{
			auto msgcolor = MapColorOption( color.fmt ) + MapColorOption( color.fg ) + MapColorOption( color.bg );
			std::cout << msgcolor << toString( message ) << Reset( ) << "\n";
		}

		std::string toString( const std::string_view &stringView )
		{
			return tag_helper::toString( stringView );
		}

		template <typename T> std::string toColorCode( T msgOption )
		{
			return MapColorOption( msgOption );
		}

	      private:
		template <typename T> std::string MapColorOption( T msgOption )
		{
			return tag_helper::MapColorOption_impl( msgOption );
		}
		std::string Reset( )
		{
			return tag_helper::Reset( );
		}
	};  // struct ConsoleColors
}  // namespace se_colors

// clang-format off
/*#######################################################################################################################################################################################################################################################
################################################################################################### Just A Visual Separator Since This Is All In One File ATM ###########################################################################################
#######################################################################################################################################################################################################################################################*/
// clang-format on

#define ALLOC_TEST 0

#if ALLOC_TEST  // Testing Allocations
size_t total_bytes = 0;
void * operator new( std::size_t n )
{
	std::cout << "[Allocating " << n << " bytes]";
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

#if ALLOC_TEST
	size_t bytes = 0;
	std::cout << Tag::red_on_blue( "\n\nTotal Bytes: " );
	bytes = total_bytes;
	std::cout << total_bytes << "\n\n";
	std::cout << Tag::Red_On_Blue( "\n\nTotal Bytes: " );
	auto new_total = total_bytes - bytes;
	std::cout << new_total << "\n\n";
// 56 bytes for new version Vs. 1936 bytes for the original...
// Pretty sure that just with that test, I'd much rather go the constexpr route than the indirection I had with mapping and the
// like... Less allocation obviously means faster so it seems like the right way to go I believe
#else


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


	/******************************************************************************************************************************
	 * Have Some Basic Tagging Set (Rough Draft Idea Anyways), But Still No Way Of Setting Up A Message Parser For Substitution
	 *Yet
	 * - Currently ColorPrint() and tagIt() Accomplish A Very Similar Function -> Hopefully Will Differentiate Soon Though
	 *******************************************************************************************************************************/

#endif
}
