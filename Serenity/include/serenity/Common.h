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

	namespace msg_details::custom_flags {
		class Format_Source_Loc
		{
		  public:
			explicit Format_Source_Loc(const std::source_location& src, const source_flag& flag)
				: srcLocation(src), buff(std::array<char, 6> {}), spec(flag), result(std::string {}) { }
			Format_Source_Loc(const Format_Source_Loc&)            = delete;
			Format_Source_Loc& operator=(const Format_Source_Loc&) = delete;
			~Format_Source_Loc()                                   = default;

			void FormatAll() {
				result.append("[ ").append(srcLocation.file_name()).append("(");
				std::memset(buff.data(), 0, buff.size());
				auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.line());
				result.append(buff.data(), end - buff.data()).append(":");
				std::memset(buff.data(), 0, buff.size());
				end = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.column()).ptr;
				result.append(buff.data(), end - buff.data()).append(") ");
				result.append(srcLocation.function_name()).append(" ]");
			}

			void FormatLine() {
				std::memset(buff.data(), 0, buff.size());
				auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.line());
				result.append(buff.data(), end - buff.data());
			}

			void FormatColumn() {
				std::memset(buff.data(), 0, buff.size());
				auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.column());
				result.append(buff.data(), end - buff.data());
			}

			void FormatFile() {
				result.append(srcLocation.file_name());
			}

			void FormatFunction() {
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
			std::array<char, 6> buff;
			const source_flag& spec;
			std::string result;
		};

		inline static constexpr size_t defaultThreadIdLength = 10;    // Precision argument dictates the length of the hashed id returned (0-10)
		struct Format_Thread_ID
		{
			Format_Thread_ID(const std::thread::id& id): thread(id) { }

			std::string FormatUserPattern(size_t precision) {
				std::array<char, 64> buf {};
				std::to_chars(buf.data(), buf.data() + buf.size(), std::hash<std::thread::id>()(thread));
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
				"Unkown Formatting Error Occured In Internal Serenity CustomFlag Formatting.",
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
