#pragma once

#include <string_view>
#include <unordered_map>
#include <array>
#include <atomic>
#include <filesystem>
#include <format>
#include <mutex>
#include <thread>

#ifdef DOXYGEN_DOCUMENTATION
	/// @brief If _WIN32 is defined, then this is also defined.
        /// @details If this macro is defined, includes the Windows.h and io.h headers
        /// as well as defines ISATTY to _isatty and FILENO to _fileno. If
        /// ENABLE_VIRTUAL_TERMINAL_PROCESSING is not defined, also defines this macro.
	#define WINDOWS_PLATFORM
	/// @brief If __APPLE__ or __MACH__ are defined, then this macro is also
        /// defined.
        /// @details If this macro is defined, includes the unistd.h header and defines
        /// ISATTY to isatty and FILENO to fileno
	#define MAC_PLATFORM
#endif

#ifdef _WIN32
	#define WINDOWS_PLATFORM
	// I believe the below macro defines *should* cover some basic corner cases.
        // Mostly noticed this issue when I built this for VS 2022 to try out.
	#if _MSC_VER >= 1930 && (_MSVC_LANG >= 202002L)
		#define CONTEXT std::back_insert_iterator<std::basic_string<char>>
		#define VFORMAT_TO(container, locale, message, ...)                                                                             \
			std::vformat_to<CONTEXT>(std::back_inserter(container), locale, message, std::make_format_args(__VA_ARGS__))
	#elif(_MSC_VER >= 1929) && (_MSVC_LANG >= 202002L)
		#define CONTEXT std::basic_format_context<std::back_insert_iterator<std::basic_string<char>>, char>
		#define VFORMAT_TO(container, locale, message, ...)                                                                             \
			std::vformat_to(std::back_inserter(container), locale, message, std::make_format_args<CONTEXT>(__VA_ARGS__))
	#else
		#if( _MSC_VER < 1929 )
			#error                                                                                                                  \
			"MSVC's Implementation Of <format> Not Supported On This Compiler Version. Please Use A Newer MSVC Compiler Version (VS 2019 v16.10/ VS 2022 v17.0 Or Later)'"
		#elif(_MSVC_LANG < 202002L)
			#error                                                                                                                  \
			"MSVC's Implementation Of <format> Not Fully Implemented Prior To C++20. Please Use The  C++ Latest Compiler Flag'"
		#else    // This one is probably uneccessary, but it's here for completeness I guess
			#error                                                                                                                  \
			"Unkown Error: Compiler And Language Standard Being Used Should Include <format> Header, But No <format> Header Was Detected"
		#endif

	#endif

#elif defined(__APPLE__) || defined(__MACH__)
	#define MAC_PLATFORM
#else
	#define LINUX_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM
	#ifndef DOXYGEN_DOCUMENTATION
		#define WIN32_LEAN_AND_MEAN
		#define VC_EXTRALEAN
	#endif    // !DOXYGEN_DOCUMENTATION

	#include <Windows.h>
	#include <io.h>

	#define ISATTY _isatty
	#define FILENO _fileno
	#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
		#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
	#endif
#else
	#include <unistd.h>
	#define ISATTY isatty
	#define FILENO fileno
#endif

#define KB                  (1024)
#define MB                  (1024 * KB)
#define GB                  (1024 * MB)
#define DEFAULT_BUFFER_SIZE (64 * KB)

// declaring for use later and for doc purposes
namespace serenity::experimental { }

namespace serenity {

	namespace globals {
		static const std::locale default_locale { std::locale(".UTF-8") };
	}

	enum class LineEnd
	{
		linux   = 0,
		windows = 1,
		mac     = 2,
	};

	namespace SERENITY_LUTS {
		// clang-format off

		static constexpr std::array<const char*, 22> allValidFlags = {
			"%a", "%b", "%d", "%l", "%n", "%t", "%w", "%x", "%y", "%A", "%B",
			"%D", "%F", "%H", "%L", "%M", "%N", "%S", "%T", "%X", "%Y", "%+"
		};

		static constexpr std::array<const char*, 7> short_weekdays = {
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
		};

		static constexpr std::array<const char*, 7> long_weekdays = {
			"Sunday",   "Monday",  "Tuesday",  "Wednesday",  "Thursday",  "Friday",  "Saturday"
		};

		static constexpr std::array<const char*, 12> short_months = {
			"Jan",  "Feb",  "Mar",  "Apr",  "May",  "Jun",  "Jul",  "Aug",  "Sep",  "Oct",  "Nov",  "Dec"
		};

		static constexpr std::array<const char*, 12> long_months = {
			"January",   "February", "March",  "April",  "May",  "June",  "July",
			"August",  "September",  "October",   "November",  "December"
		};

		static constexpr std::array<const char*, 100> numberStr = {
			"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
			"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
			"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
			"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
			"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
		};

		static std::unordered_map<LineEnd, const char*> line_ending = {
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

	enum class message_time_mode
	{
		local,
		utc
	};

	struct BackgroundThread
	{
		std::jthread flushThread;
		std::stop_token interruptThread {};
		std::atomic<bool> cleanUpThreads { false };
		std::atomic<bool> flushThreadEnabled { false };
		std::atomic<bool> flushComplete { true };
		std::atomic<bool> threadWriting { false };
		std::atomic<bool> pauseThread { false };
	};

	struct FileSettings
	{
		std::filesystem::path filePath;
		std::vector<char> fileBuffer;
		size_t bufferSize { DEFAULT_BUFFER_SIZE };
	};

	namespace experimental {
		struct RotateSettings
		{
			enum class IntervalMode
			{
				file_size = 0,
				hourly    = 1,
				daily     = 2,
				weekly    = 3,
				monthly   = 4,
			};

			size_t maxNumberOfFiles { 5 };
			size_t fileSizeLimit { 512 * KB };
			int dayModeSettingHour { 0 };
			int dayModeSettingMinute { 0 };
			int weekModeSetting { 0 };
			int monthModeSetting { 1 };

		      protected:
			const std::filesystem::path& OriginalPath();
			const std::filesystem::path& OriginalDirectory();
			const std::string& OriginalName();
			const std::string& OriginalExtension();
			const size_t& FileSize();
			const bool IsIntervalRotationEnabled();
			void CacheOriginalPathComponents(const std::filesystem::path& filePath);
			void SetCurrentFileSize(size_t currentSize);
			void EnableFirstRotation(bool enabled = true);

		      private:
			size_t currentFileSize { 0 };
			std::string ext, fileName;
			std::filesystem::path path, directory;
			bool initalRotationEnabled { true };
		};

	}    // namespace experimental
}    // namespace serenity

#ifndef NDEBUG
	#define DB_PRINT(msg, ...) (std::cout << std::format(msg, __VA_ARGS__))
#else
	#define DB_PRINT(msg, ...)
#endif    // !NDEBUG