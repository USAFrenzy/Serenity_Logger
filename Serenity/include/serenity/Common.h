#pragma once

#ifdef DOXYGEN_DOCUMENTATION
/// @brief If _WIN32 is defined, then this is also defined.
/// @details If this macro is defined, includes the Windows.h and io.h headers as well as defines ISATTY to _isatty and
/// FILENO to _fileno. If ENABLE_VIRTUAL_TERMINAL_PROCESSING is not defined, also defines this macro.
	#define WINDOWS_PLATFORM
/// @brief If __APPLE__ or __MACH__ are defined, then this macro is also defined.
/// @details If this macro is defined, includes the unistd.h header and defines ISATTY to isatty and FILENO to fileno
	#define MAC_PLATFORM
#endif

#ifdef _WIN32
	#define WINDOWS_PLATFORM
#elif defined( __APPLE__ ) || defined( __MACH__ )
	#define MAC_PLATFORM
#else
	#define LINUX_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM
	#ifndef DOXYGEN_DOCUMENTATION
		#define WIN32_LEAN_AND_MEAN
		#define VC_EXTRALEAN
	#endif  // !DOXYGEN_DOCUMENTATION

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

#define KB                  ( 1024 )
#define MB                  ( 1024 * KB )
#define GB                  ( 1024 * MB )
#define DEFAULT_BUFFER_SIZE ( 64 * KB )

#include <filesystem>
#include <string_view>
#include <array>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <format>

// declaring for use later and for doc purposes
namespace serenity::experimental
{ }

namespace serenity
{
	enum class LineEnd
	{
		linux   = 0,
		windows = 1,
		mac     = 2,
	};

	namespace SERENITY_LUTS
	{
		// clang-format off
		static constexpr std::array<std::string_view, 22> allValidFlags = 
		{ 
			"%a", "%b", "%d", "%l", "%n", "%t", "%w", "%x",
			"%y", "%A", "%B", "%D", "%F", "%H", "%L", "%M",
			"%N", "%S", "%T", "%X", "%Y", "%+" 
		};

		static constexpr std::array<std::string_view, 7> short_weekdays = 
		{ 
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" 
		};

		static constexpr std::array<std::string_view, 7> long_weekdays = 
		{
			"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" 
		};

		static constexpr std::array<std::string_view, 12> short_months = 
		{
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
		};

		static constexpr std::array<const char *, 12> long_months = 
		{
			"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" 
		};

		static constexpr std::array<std::string_view, 100> numberStr = 
		{
			"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16",
		    "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33",
			"34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
			"51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67",
			"68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84",
			"85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99" 
		};

		static std::unordered_map<LineEnd, std::string_view> line_ending = 
		{
			{ LineEnd::linux, "\n" },
			{ LineEnd::windows, "\r\n" },
			{ LineEnd::mac, "\r" },
		};

		// clang-format on

	}  // namespace SERENITY_LUTS

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

	static std::string_view MsgLevelToShortString( LoggerLevel level )
	{
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

	static std::string_view MsgLevelToString( LoggerLevel level )
	{
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
		std::atomic<bool>  cleanUpThreads { false };
		std::atomic<bool>  flushThreadEnabled { false };
		mutable std::mutex readWriteMutex;
		std::thread        flushThread;
	};

	struct FileSettings
	{
		std::filesystem::path filePath;
		std::vector<char>     fileBuffer;
		size_t                bufferSize { DEFAULT_BUFFER_SIZE };
	};

	namespace experimental
	{
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
			int    dayModeSettingHour { 0 };
			int    dayModeSettingMinute { 0 };
			int    weekModeSetting { 0 };
			int    monthModeSetting { 1 };

		  protected:
			const std::filesystem::path &OriginalPath( );
			const std::filesystem::path &OriginalDirectory( );
			const std::string &          OriginalName( );
			const std::string &          OriginalExtension( );
			const size_t &               FileSize( );
			const bool                   IsIntervalRotationEnabled( );
			void                         CacheOriginalPathComponents( const std::filesystem::path &filePath );
			void                         SetCurrentFileSize( size_t currentSize );
			void                         InitFirstRotation( bool enabled = true );

		  private:
			size_t                currentFileSize { 0 };
			std::string           ext, fileName;
			std::filesystem::path path, directory;
			bool                  initalRotationEnabled { true };
		};

	}  // namespace experimental
}  // namespace serenity

#ifndef NDEBUG
	#define DB_PRINT( msg, ... ) ( std::cout << std::format( msg, __VA_ARGS__ ) )
#else
	#define DB_PRINT( msg, ... )
#endif  // !NDEBUG