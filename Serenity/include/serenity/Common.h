#pragma once

#include <UTF-Utils/utf-utils.h>
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

	struct MsgWithLoc: public utf_utils::InputSource
	{
		const std::source_location& source;
		template<typename T>
		requires utf_utils::utf_constraints::IsSupportedUSource<T> MsgWithLoc(T&& msg, const std::source_location& src = std::source_location::current())
			: utf_utils::InputSource(std::forward<T>(msg)), source(src) { }
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

	static std::string_view LevelToShortView(const LoggerLevel& level) {
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

	static std::string_view LevelToLongView(const LoggerLevel& level) {
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

	namespace msg_details::custom_flags {

		inline static constexpr size_t SOURCE_BUFF_SIZE { 6 };

		// Not All Functions Can Be Made `constexpr` due to the usage of `std::memset()` and `std::to_chars()`
		class Format_Source_Loc
		{
		  public:
			explicit Format_Source_Loc(const std::source_location& src, const source_flag& flag)
				: srcLocation(src), buff(std::array<char, SOURCE_BUFF_SIZE> {}), spec(flag), result(std::string {}) { }
			Format_Source_Loc(const Format_Source_Loc&)            = delete;
			Format_Source_Loc& operator=(const Format_Source_Loc&) = delete;
			~Format_Source_Loc()                                   = default;

			void FormatAll() {
				auto data { buff.data() };
				result.append("[ ").append(std::move(std::filesystem::path { srcLocation.file_name() }.filename().string())).append("(");
				std::memset(data, 0, SOURCE_BUFF_SIZE);
				result.append(data, std::to_chars(data, data + SOURCE_BUFF_SIZE, srcLocation.line()).ptr - data).append(":");
				std::memset(data, 0, SOURCE_BUFF_SIZE);
				result.append(data, std::to_chars(data, data + SOURCE_BUFF_SIZE, srcLocation.column()).ptr - data).append(") ");
				result.append(srcLocation.function_name()).append(" ]");
			}

			void FormatLine() {
				auto data { buff.data() };
				std::memset(data, 0, SOURCE_BUFF_SIZE);
				result.append(data, std::to_chars(data, data + SOURCE_BUFF_SIZE, srcLocation.line()).ptr - data);
			}

			void FormatColumn() {
				auto data { buff.data() };
				std::memset(data, 0, SOURCE_BUFF_SIZE);
				result.append(data, std::to_chars(data, data + SOURCE_BUFF_SIZE, srcLocation.column()).ptr - data);
			}

			void FormatFile() {
				result.append(std::move(std::filesystem::path { srcLocation.file_name() }.filename().string()));
			}

			inline constexpr void FormatFunction() {
				result.append(srcLocation.function_name());
			}

			std::string FormatUserPattern() {
				using enum source_flag;
				result.clear();
				switch( static_cast<int>(spec) ) {
						case 1: FormatLine(); break;
						case 2: FormatColumn(); break;
						case 3:
							FormatLine();
							result.append(":");
							FormatColumn();
							break;
						case 4: FormatFile(); break;
						case 5:
							FormatFile();
							result.append(" ");
							FormatLine();
							break;
						case 7:
							FormatFile();
							result.append("(");
							FormatLine();
							result.append(":");
							FormatColumn();
							result.append(")");
							break;
						case 8: FormatFunction(); break;
						case 9:
							FormatFunction();
							result.append(" ");
							FormatLine();
							break;
						case 11:
							FormatFunction();
							result.append(": (");
							FormatLine();
							result.append(":");
							FormatColumn();
							result.append(")");
							break;
						case 12:
							FormatFile();
							result.append(" ");
							FormatFunction();
							break;
						default: FormatAll(); break;
					}
				return result;
			}

		  private:
			const std::source_location& srcLocation;
			std::array<char, SOURCE_BUFF_SIZE> buff;
			const source_flag& spec;
			std::string result;
		};

		inline static constexpr size_t defaultThreadIdLength = 10;    // Precision argument dictates the length of the hashed id returned (0-10)
		inline static constexpr size_t THREAD_BUFF_SIZE      = 64;
		class Format_Thread_ID
		{
		  public:
			Format_Thread_ID(const std::thread::id& id): thread(id) { }
			Format_Thread_ID(const Format_Thread_ID&)            = delete;
			Format_Thread_ID& operator=(const Format_Thread_ID&) = delete;
			~Format_Thread_ID()                                  = default;

			std::string FormatUserPattern(size_t precision) {
				std::array<char, THREAD_BUFF_SIZE> buf {};
				std::to_chars(buf.data(), buf.data() + THREAD_BUFF_SIZE, std::hash<std::thread::id>()(thread));
				return std::string(buf.data(), buf.data() + ((precision != 0) ? precision : defaultThreadIdLength));
			}

		  private:
			const std::thread::id& thread;
		};
	}    // namespace msg_details::custom_flags

	// This bit will most likely be abstracted into a core file that the backends will all use
	namespace CustomFlagError {
		enum class CustomErrors
		{
			missing_enclosing_bracket,
			missing_flag_specifier,
			unknown_flag,
			token_with_no_flag,
			no_flag_mods,
		};

		struct custom_error_handler
		{
			class format_error: public std::runtime_error
			{
			  public:
				inline explicit format_error(const char* message): std::runtime_error(message) { }
				inline explicit format_error(const std::string& message): std::runtime_error(message) { }
				inline format_error(const format_error&)            = default;
				inline format_error& operator=(const format_error&) = default;
				inline format_error(format_error&&)                 = default;
				inline format_error& operator=(format_error&&)      = default;
				inline ~format_error() noexcept override            = default;
			};

			static constexpr std::array<const char*, 5> custom_error_messages = {
				"Unknown Formatting Error Occurred In Internal Serenity CustomFlag Formatting.",
				"Error In Internal CustomFlag Formatting For Serenity: Missing '%' before flag.",
				"Error In Internal CustomFlag Formatting For Serenity:  An Escaped Opening Bracket Was Detected - Missing Escaped Closing Bracket.",
				"Error In Internal CustomFlag Formatting For Serenity:  An Unsupported Flag Was Encountered.",
				"Error In Internal CustomFlag Formatting For Serenity:  Flag Specifier Token Found, But With No Flag Present.",
			};

			[[noreturn]] constexpr void ReportCustomError(CustomErrors err) {
				using enum CustomErrors;
				switch( err ) {
						case missing_flag_specifier: throw format_error(custom_error_messages[ 1 ]); break;
						case missing_enclosing_bracket: throw format_error(custom_error_messages[ 2 ]); break;
						case unknown_flag: throw format_error(custom_error_messages[ 3 ]); break;
						case token_with_no_flag: throw format_error(custom_error_messages[ 4 ]); break;
						case no_flag_mods: throw format_error(custom_error_messages[ 5 ]); break;
						default: throw format_error(custom_error_messages[ 0 ]); break;
					}
			}
		};
	};    // namespace CustomFlagError

}    // namespace serenity
