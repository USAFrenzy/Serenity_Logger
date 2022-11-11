#pragma once

#include <ArgFormatter/ArgFormatter.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <source_location>

// This bit will most likely be abstracted into a core file that the backends will all use
namespace serenity::CustomFlagError {
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
			inline format_error(const format_error&) = default;
			inline format_error& operator=(const format_error&) = default;
			inline format_error(format_error&&)                 = default;
			inline format_error& operator=(format_error&&) = default;
			inline ~format_error() noexcept override       = default;
		};

		static constexpr std::array<const char*, 6> custom_error_messages = {
			"Unkown Formatting Error Occured In Internal Serenity CustomFlag Formatting.",
			"Error In Internal CustomFlag Formatting For Serenity: Missing '%' before flag.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Escaped Opening Bracket Was Detected - Missing Escaped Closing Bracket.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Unsupported Flag Was Encountered.",
			"Error In Internal CustomFlag Formatting For Serenity:  Flag Specifier Token Found, But With No Flag Present.",
			"Error In Internal CustomFlag Formatting For Serenity:  ':' Present With No Flag Modifiers.",

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
};    // namespace serenity::CustomFlagError

enum class flag_type
{
	Invalid,
	LongLogLvl,
	ShortLogLvl,
	LoggerName,
	LogSource,
	LoggerThreadID,
	LogMessage,
	Default,
};

class Format_Source_Loc
{
  public:
	explicit Format_Source_Loc(const serenity::msg_details::Message_Info& info, const serenity::source_flag& flag)
		: srcLocation(info.SourceLocation()), buff(std::array<char, 6> {}), spec(flag) { }
	Format_Source_Loc(const Format_Source_Loc&) = delete;
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

	std::string_view FormatUserPattern() {
		using enum serenity::source_flag;
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
		return std::string_view(result.data(), result.size());
	}

  public:
	bool isInitialized { false };

  private:
	const std::source_location& srcLocation;
	std::array<char, 6> buff;
	serenity::source_flag spec;
	std::string result;
};

struct Format_Thread_ID
{
	// Precision argument dictates the length of the hashed id returned (0-10)
	static constexpr size_t defaultThreadIdLength = 10;
	Format_Thread_ID(): thread(std::hash<std::thread::id>()(std::this_thread::get_id())) { }

	std::string_view FormatUserPattern(size_t precision) {
		result.clear();
		std::array<char, 64> buf {};
		std::to_chars(buf.data(), buf.data() + buf.size(), thread);
		std::string_view sv { buf.data(), buf.size() };
		(precision != 0) ? sv.remove_suffix(sv.size() - precision) : sv.remove_suffix(sv.size() - defaultThreadIdLength);
		result.append(sv.data(), sv.size());
		return std::string_view(result.data(), result.size());
	}

  private:
	std::string result;
	size_t thread;
};

// This is going to need to be sped up an immense amount.. this is taking too long right now
template<> struct formatter::CustomFormatter<serenity::msg_details::Message_Info>
{
	serenity::CustomFlagError::custom_error_handler errHandler {};
	flag_type flag { flag_type::Invalid };
	serenity::source_flag srcFlags { serenity::source_flag::empty };
	size_t threadLength { 0 };
	// clang-format off
	static constexpr std::string_view defaultFmt { "- Logger Name:\t{0}\n- Long Level:\t{1}\n- Short Level:\t{2}\n"
		                                                                                    "- Log Message:\t{3}\n- Log Source:\t{4}\n- Thread ID:\t{5}\n" };
	// clang-format on

	inline constexpr void Parse(std::string_view parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto size { parse.size() };
		auto pos { -1 };

		for( ;; ) {
				if( ++pos >= size ) {
						if( parse[ --pos ] == '}' ) {
								if( flag != flag_type::Invalid ) return;
								// If we reach this point, the flag is still flag_type::Invalid, and the error hasn't been reported as an 'Invalid Flag', then we can
								// assume that there were no flags present to begin with and set the flag to default formatting
								flag = flag_type::Default;
								break;
						}
						// otherwise, we're missing a closing bracket
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}

				// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't
				// exist and continue until we reach it or hit the end of the parse string
				if( parse[ pos ] != ':' ) continue;
				if( ++pos >= size ) {
						errHandler.ReportCustomError(CustomErrors::no_flag_mods);
				}
				if( parse[ pos ] != '%' ) {
						errHandler.ReportCustomError(CustomErrors::missing_flag_specifier);
				}
				if( ++pos >= size ) {
						errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
				}

				// the flag specifier token was detected, now try matching the flag
				switch( parse[ pos ] ) {
						case 'L': flag = flag_type::LongLogLvl; return;
						case 'l': flag = flag_type::ShortLogLvl; return;
						case 'N': flag = flag_type::LoggerName; return;
						// LogSource is a little bit special given the modifiers that are attached to it
						case 's':
							{
								flag     = flag_type::LogSource;
								srcFlags = serenity::source_flag::empty;
								if( auto nxtPos { ++pos }; (nxtPos >= size) || (parse[ nxtPos ] != ':') ) {
										if( nxtPos < size ) {
												--pos;
												srcFlags = serenity::source_flag::all;
												return;
										}
										errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
								}
								for( ;; ) {
										if( ++pos >= size ) {
												errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
										}
										switch( parse[ pos ] ) {
												case 'l': srcFlags |= serenity::source_flag::line; continue;
												case 'c': srcFlags |= serenity::source_flag::column; continue;
												case 'f': srcFlags |= serenity::source_flag::file; continue;
												case 'F': srcFlags |= serenity::source_flag::function; continue;
												case '}': return;
											}
									}
							}
							// LoggerThreadID is also a little bit special due to it's length modifier
						case 't':
							flag = flag_type::LoggerThreadID;
							if( ++pos >= size ) {
									errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
							}
							if( parse[ pos ] != ':' ) {
									threadLength = 0;
									continue;
							}
							if (++pos >= size) {
								errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
							}
							switch( parse[ pos ] ) {
									case '0': threadLength = 0; continue;
									case '1': threadLength = 1; continue;
									case '2': threadLength = 2; continue;
									case '3': threadLength = 3; continue;
									case '4': threadLength = 4; continue;
									case '5': threadLength = 5; continue;
									case '6': threadLength = 6; continue;
									case '8': threadLength = 8; continue;
									case '7': threadLength = 7; continue;
									case '9': threadLength = 9; continue;
									default: continue;
								}
							return;
						case '+': flag = flag_type::LogMessage; return;
						default:
							flag = flag_type::Invalid;
							errHandler.ReportCustomError(CustomErrors::unknown_flag);
							return;
					}
			}
	}

	template<typename resultCtx> constexpr auto Format(const serenity::msg_details::Message_Info& p, resultCtx& ctx) const {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		static Format_Source_Loc srcFormatter(p, srcFlags);

		switch( flag ) {
				case flag_type::LongLogLvl:
					{
						return formatter::format_to(std::back_inserter(ctx), "{}", LevelToLongView(p.MsgLevel()));
					}
				case flag_type::ShortLogLvl:
					{
						return formatter::format_to(std::back_inserter(ctx), "{}", LevelToShortView(p.MsgLevel()));
					}
				case flag_type::LoggerName:
					{
						return formatter::format_to(std::back_inserter(ctx), "{}", p.Name());
					}
				case flag_type::LogSource:
					{
						return formatter::format_to(std::back_inserter(ctx), "{}", srcFormatter.FormatUserPattern());
					}
				case flag_type::LoggerThreadID:
					{
						Format_Thread_ID threadID {};
						return formatter::format_to(std::back_inserter(ctx), "{}", threadID.FormatUserPattern(threadLength));
					}
				case flag_type::LogMessage:
					{
						return formatter::format_to(std::back_inserter(ctx), "{}", p.Message());
					}
				case flag_type::Default:
					{
						Format_Thread_ID threadID {};
						// clang-format off
						return formatter::format_to(std::back_inserter(ctx), defaultFmt, p.Name(), LevelToLongView(p.MsgLevel()), LevelToShortView(p.MsgLevel()), p.Message(), srcFormatter.FormatUserPattern(), threadID.FormatUserPattern(threadLength));
						// clang-format on	
				}
					// invalid flag should have been caught from the parsing, but include here and just don't format it
				case flag_type::Invalid: [[fallthrough]];
				default: break;
			}
	}
};
