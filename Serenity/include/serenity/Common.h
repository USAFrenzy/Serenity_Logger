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
