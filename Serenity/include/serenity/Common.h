#pragma once

#include <serenity/Defines.h>

#include <source_location>
#include <string_view>
#include <unordered_map>
#include <array>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <thread>

#include <serenity/Utilities/FormatBackend.h>

namespace serenity {

	// This lovely and amazing end to my headaches for getting the correct call site was provided by ivank at :
	//   https://stackoverflow.com/a/66402319/11410972
	// TODO: Probably In the top level formatting call, but just like how string types are converted to utf-8 in the formatting section,
	//                I should handle encoding to utf-8 in the message itself as well in case other encodings are present and so that I can
	//               appropriately encode back to the appropriate  encoding for the char type present in the container provided.
	struct MsgWithLoc
	{
		std::string_view msg;
		std::source_location source;
		MsgWithLoc(std::string_view sv, const std::source_location& src = std::source_location::current()): msg(sv), source(src) { }
		MsgWithLoc(std::string& sv, const std::source_location& src = std::source_location::current()): msg(sv), source(src) { }
		MsgWithLoc(const char* sv, const std::source_location& src = std::source_location::current()): msg(sv), source(src) { }
		MsgWithLoc(const std::string& sv, const std::source_location& src = std::source_location::current()): msg(sv), source(src) { }
	};

	namespace globals {
		static std::locale default_locale { std::locale("en_US.UTF-8") };
	}

	enum class LineEnd
	{
		linux   = 0,
		windows = 1,
		mac     = 2,
	};

	namespace SERENITY_LUTS {
		// clang-format off

		static constexpr std::array<std::string_view, 34> allValidFlags = {
			"%a", "%b", "%c", "%d", "%e", "%h","%l", "%m", "%n", "%p", "%r", 
			"%s" , "%t", "%w", "%x", "%y", "%z", "%A", " %B", "%C", "%D", "%F", "%H", 
			"%I", "%L", "%M", "%N", "%R", "%S", "%T", "%X","%Y", "%Z", "%+"
		};

		// NOTE: If the Argformatter version takes over for the FormatterArgs version, then the arrays below can be removed, 
		//              as well as the entirety of FormatterArgs.h and FormatterArgs.cpp. Would just need to update Message_Time's
		//              CalculateCurrentYear(), StoreFormatPattern(), and anything invoking a struct instatiation of FormatterArgs' 
		//              structs to reflect this  change.
		static constexpr std::array<std::string_view, 7> short_weekdays = {
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
		};

		static constexpr std::array<std::string_view, 7> long_weekdays = {
			"Sunday",   "Monday",  "Tuesday",  "Wednesday",  "Thursday",  "Friday",  "Saturday"
		};

		static constexpr std::array<std::string_view, 12> short_months = {
			"Jan",  "Feb",  "Mar",  "Apr",  "May",  "Jun",  "Jul",  "Aug",  "Sep",  "Oct",  "Nov",  "Dec"
		};

		static constexpr std::array<std::string_view, 12> long_months = {
			"January",   "February", "March",  "April",  "May",  "June",  "July",
			"August",  "September",  "October",   "November",  "December"
		};

		static constexpr std::array<std::string_view, 100> numberStr = {
			"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
			"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
			"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
			"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
			"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
		};

		static std::unordered_map<LineEnd, std::string_view> line_ending = {
			{LineEnd::linux,    "\n"  },
			{ LineEnd::windows, "\r\n"},
			{ LineEnd::mac,     "\r"  },
		};

		constexpr int JANUARY = 0;
		constexpr int FEBRUARY = 1;
		constexpr int MARCH = 2;
		constexpr int APRIL = 3;
		constexpr int MAY = 4;
		constexpr int JUNE = 5;
		constexpr int JULY = 6;
		constexpr int AUGUST = 7;
		constexpr int SEPTEMBER = 8;
		constexpr int OCTOBER = 9;
		constexpr int NOVEMBER = 10;
		constexpr int DECEMBER = 11;

		static std::unordered_map<int, int> daysPerMonth = {
			{JANUARY,    31},
			{ FEBRUARY,  28},
			{ MARCH,     31},
			{ APRIL,     30},
			{ MAY,       31},
			{ JUNE,      30},
			{ JULY,      31},
			{ AUGUST,    31},
			{ SEPTEMBER, 30},
			{ OCTOBER,   31},
			{ NOVEMBER,  30},
			{ DECEMBER,  31},
		};
		// clang-format on
	}    // namespace SERENITY_LUTS

	enum class LoggerLevel
	{
		trace   = 0,
		info    = 1,
		debug   = 2,
		warning = 3,
		error   = 4,
		fatal   = 5,
		off     = 6,
	};

	static std::string_view LevelToShortView(LoggerLevel level) {
		switch( level ) {
				case LoggerLevel::info: return "I"; break;
				case LoggerLevel::trace: return "T"; break;
				case LoggerLevel::debug: return "D"; break;
				case LoggerLevel::warning: return "W"; break;
				case LoggerLevel::error: return "E"; break;
				case LoggerLevel::fatal: return "F"; break;
				default: return ""; break;
			}
	}

	static std::string_view LevelToLongView(LoggerLevel level) {
		switch( level ) {
				case LoggerLevel::info: return "Info"; break;
				case LoggerLevel::trace: return "Trace"; break;
				case LoggerLevel::debug: return "Debug"; break;
				case LoggerLevel::warning: return "Warn"; break;
				case LoggerLevel::error: return "Error"; break;
				case LoggerLevel::fatal: return "Fatal"; break;
				default: return ""; break;
			}
	}

	enum class source_flag
	{
		empty    = 0,
		line     = 1,
		column   = 2,
		file     = 4,
		function = 8,
		all      = 16,
	};
	constexpr source_flag operator|(source_flag lhs, source_flag rhs) {
		return static_cast<source_flag>(static_cast<std::underlying_type<source_flag>::type>(lhs) | static_cast<std::underlying_type<source_flag>::type>(rhs));
	}
	constexpr source_flag operator|=(source_flag& lhs, source_flag rhs) {
		return static_cast<source_flag>(lhs = lhs | rhs);
	}

	// clang-format off
	
	// Warning Messages Specific To The User Supplied Format Pattern
	static constexpr std::array<const char*, 2> precisionWarningMessage =
	{
		"Warning: Format string token \"%e\" contains an invalid precision specifier.",
		"Warning: Format string token \"%t\" contains an invalid precision specifier.",
	};
	// clang-format on

	enum class message_time_mode
	{
		local,
		utc
	};

	static constexpr bool IsDigit(const char& ch) {
		return ((ch >= '0') && (ch <= '9'));
	}

	static constexpr bool IsAlpha(const char& ch) {
		return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
	}
}    // namespace serenity
