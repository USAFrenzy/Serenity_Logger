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
		missing_src_modifiers,
		missing_thread_id_mod,
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

		static constexpr std::array<const char*, 8> custom_error_messages = {
			"Unkown Formatting Error Occured In Internal Serenity CustomFlag Formatting.",
			"Error In Internal CustomFlag Formatting For Serenity: Missing '%' before flag.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Escaped Opening Bracket Was Detected - Missing Escaped Closing Bracket.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Unsupported Flag Was Encountered.",
			"Error In Internal CustomFlag Formatting For Serenity:  Flag Specifier Token Found, But With No Flag Present.",
			"Error In Internal CustomFlag Formatting For Serenity:  ':' Present With No Flag Modifiers.",
			"Error In Internal CustomFlag Formatting For Serenity:  ':' Present For Source Flag With No Flag Modifiers Or Unkown Character.",
			"Error In Internal CustomFlag Formatting For Serenity:  ':' Present For Thread ID Flag With No Flag Modifiers Or Unkown Character.",
		};

		[[noreturn]] constexpr void ReportCustomError(CustomErrors err) {
			using enum CustomErrors;
			switch( err ) {
					case missing_flag_specifier: throw format_error(custom_error_messages[ 1 ]); break;
					case missing_enclosing_bracket: throw format_error(custom_error_messages[ 2 ]); break;
					case unknown_flag: throw format_error(custom_error_messages[ 3 ]); break;
					case token_with_no_flag: throw format_error(custom_error_messages[ 4 ]); break;
					case no_flag_mods: throw format_error(custom_error_messages[ 5 ]); break;
					case missing_src_modifiers: throw format_error(custom_error_messages[ 6 ]); break;
					case missing_thread_id_mod: throw format_error(custom_error_messages[ 7 ]); break;
					default: throw format_error(custom_error_messages[ 0 ]); break;
				}
		}
	};
};    // namespace serenity::CustomFlagError
namespace serenity::msg_details::custom_flags {

	// estimated size: assume logger name, long level, short level, and thread ID are all SSO, so max 16 for each, assume full legnth source and
	// reserve 80, and reserve an extra 255 for a message and then 93 for the pattern string minus the substitution brackets involved (=492)
	static constexpr size_t defaultSizeEstimate { 492 };          // used for full default status formatting
	inline static constexpr size_t defaultThreadIdLength = 10;    // Precision argument dictates the length of the hashed id returned (0-10)

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
			: srcLocation(info.SourceLocation()), buff(std::array<char, 6> {}), spec(flag), result(std::string {}) { }
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

		std::string& FormatUserPattern() {
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
			return result;
		}

	  private:
		const std::source_location& srcLocation;
		std::array<char, 6> buff;
		serenity::source_flag spec;
		std::string result;
	};

	struct Format_Thread_ID
	{
		Format_Thread_ID(): thread(std::hash<std::thread::id>()(std::this_thread::get_id())) { }

		std::string& FormatUserPattern(size_t precision) {
			result.clear();
			std::array<char, 64> buf {};
			std::to_chars(buf.data(), buf.data() + buf.size(), thread);
			std::string_view sv { buf.data(), buf.size() };
			(precision != 0) ? sv.remove_suffix(sv.size() - precision) : sv.remove_suffix(sv.size() - defaultThreadIdLength);
			result.append(sv.data(), sv.size());
			return result;
		}

	  private:
		std::string result;
		size_t thread;
	};
}    // namespace serenity::msg_details::custom_flags

// This is going to need to be sped up an immense amount.. this is taking too long right now
template<> struct formatter::CustomFormatter<serenity::msg_details::Message_Info>
{
	serenity::CustomFlagError::custom_error_handler errHandler {};
	serenity::msg_details::custom_flags::flag_type flag {};
	serenity::source_flag srcFlags {};
	size_t threadLength {};
	mutable std::string resultBuff {};

	inline constexpr void Parse(std::string_view parse) {
		namespace cf       = serenity::msg_details::custom_flags;
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		flag               = cf ::flag_type::Invalid;

		auto size { parse.size() };
		auto pos { -1 };

		for( ;; ) {
				if( ++pos >= size ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				switch( parse[ pos ] ) {
						case '}':
							{
								if( flag != cf::flag_type::Invalid ) return;
								// If we reach this point, the flag is still flag_type::Invalid, and the error hasn't been reported as an 'Invalid Flag', then we can
								// assume that there were no flags present to begin with and set the flag to default formatting and exit this function
								flag = cf::flag_type::Default;
								return;
							}
						case ':':
							{
								if( ++pos >= size ) {
										errHandler.ReportCustomError(CustomErrors::no_flag_mods);
								}
								if( parse[ pos ] != '%' ) {
										errHandler.ReportCustomError(CustomErrors::missing_flag_specifier);
								}
								if( ++pos >= size ) {
										errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
								}
								break;
							}
							// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't
							// exist and continue until we reach it or hit the end of the parse string
						default: continue;
					}
				/*********************** This point is only reached once the flag specifier token is detected, so now try matching the flag ***********************/
				// for 'L', 'l', 'N' and '+', if the flag is found up to that point, nothing special needs to be done afterwards, so just straight up exit the
				// function from this point since we already have determined that the bracket itself is valid from the root level formatting call before custom
				// formatting is reached. For the other flags, it's a little bit different though just due to some default behavior that's set up for source
				// location and thread ID formatting
				switch( parse[ pos ] ) {
						case 'L':
							{
								flag = cf::flag_type::LongLogLvl;
								return;
							}
						case 'l':
							{
								flag = cf::flag_type::ShortLogLvl;
								return;
							}
						case 'N':
							{
								flag = cf::flag_type::LoggerName;
								return;
							}
						case '+':
							{
								flag = cf::flag_type::LogMessage;
								return;
							}
						// LogSource is a little bit special given the modifiers that are attached to it
						case 's':
							{
								flag     = cf::flag_type::LogSource;
								srcFlags = serenity::source_flag::empty;

								for( ;; ) {
										if( ++pos >= size ) errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
										switch( parse[ pos ] ) {
												case ' ': continue;    // ignore whitespace when determining valid bracket for source location
												case ':': break;       // continue on to parsing modifiers
												case '}':
													{
														if( srcFlags == serenity::source_flag::empty ) {
																srcFlags = serenity::source_flag::all;
														}
														return;
													}
												default:
													{
														// if it's anything else, then assume it's not a valid bracket for source location formatting
														errHandler.ReportCustomError(CustomErrors::unknown_flag);
														break;
													}
											}
									}
								// this now assumes we've hit ':', therefore now we parse the source modifiers here
								for( ;; ) {
										if( ++pos >= size ) {
												errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
										}
										switch( parse[ pos ] ) {
												case 'a':
													srcFlags = serenity::source_flag::all;
													return;    // ignore everything else if this is present and default to all
												case 'l': srcFlags |= serenity::source_flag::line; continue;
												case 'c': srcFlags |= serenity::source_flag::column; continue;
												case 'f': srcFlags |= serenity::source_flag::file; continue;
												case 'F': srcFlags |= serenity::source_flag::function; continue;
												case '}':
													{
														if( srcFlags != serenity::source_flag::empty ) return;
														// we already noted a ':' present, therefore if there are no modifiers present, this isn't a valid source loc
														// bracket
														errHandler.ReportCustomError(CustomErrors::missing_src_modifiers);
													}
												default:
													{
														// if it's anything else, then assume it's not a valid bracket for source location formatting
														errHandler.ReportCustomError(CustomErrors::unknown_flag);
														break;
													}
											}
									}
							}
							// LoggerThreadID is also a little bit special due to it's length modifier
						case 't':
							{
								flag         = cf::flag_type::LoggerThreadID;
								threadLength = 0;
								for( ;; ) {
										switch( parse[ pos ] ) {
												case '}': return;
												case ' ': [[fallthrough]];    // ignore whitespace when determining if valid thread id bracket
												default:
													{
														// so long as the bracket is still valid and in bounds continue parsing
														if( ++pos >= size ) {
																errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
														}
														continue;
													}
												case ':':
													{
														if( ++pos >= size ) {
																errHandler.ReportCustomError(CustomErrors::missing_thread_id_mod);
														}
														switch( parse[ pos ] ) {
																case '0': threadLength = 0; return;
																case '1': threadLength = 1; return;
																case '2': threadLength = 2; return;
																case '3': threadLength = 3; return;
																case '4': threadLength = 4; return;
																case '5': threadLength = 5; return;
																case '6': threadLength = 6; return;
																case '8': threadLength = 8; return;
																case '7': threadLength = 7; return;
																case '9': threadLength = 9; return;
																case '10': threadLength = 10; return;
																default: errHandler.ReportCustomError(CustomErrors::unknown_flag);
															}
													}
											}
									}
							}
						default:
							{
								flag = cf::flag_type::Invalid;
								errHandler.ReportCustomError(CustomErrors::unknown_flag);
								return;
							}
					}
			}
	}

	/*!*********************************************************************** NOTE TO SELF ************************************************************************/
	//!  Removed static Format_Source_Loc object as it hadn't occured to me until now that that method would break with multi-threading given the fact that any
	//! thread after the first would just be using the first's source location instead due to static storage -> therefore, just have to live with the insane cost of
	//! formatting source location details if present (it's not terrible in general, just *MUCH* slower than the rest of the formatting flags).
	//!
	//!  I may revisit this if I can think of a better way to approach this -> the initial mindset of having a static initializer with a pseudo singleton like
	//!  pattern using locks is definitely not the way to go since I would still need a way to update at least the Message_Info parameter to swap over the usage of
	//!  where the source location is being collected from as well as a way to basically redundantly check the thread ID (redundant in the sense that if a thread ID
	//!  flag is present, as it stands right now, this method would have to perfrom two hashes on the thread -> could be optimized to work in tandem though I think).
	//!  The idea with the singleton approach was that if the thread hash doesn't match the previous thread's hashed value, then update the std::source_location
	//!  parameter based on the new thread's Message_Info's std::source_location data and if it *DOES* match, use the current setup as the source location is likely
	//!  different but is being called with the same Message_Info class data. So, in lieu of this, I'm just leaving the static instance out for now.
	//! **************************************************************************************************************************************************************
	template<typename resultCtx> constexpr auto Format(const serenity::msg_details::Message_Info& p, resultCtx& ctx) const {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		namespace fch      = formatter::custom_helper;
		namespace cf       = serenity::msg_details::custom_flags;
		auto contSize { ctx.size() };

		switch( flag ) {
				case cf::flag_type::LongLogLvl:
					{
						const auto& tmp { std::move(LevelToLongView(p.MsgLevel())) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::ShortLogLvl:
					{
						const auto& tmp { std::move(LevelToShortView(p.MsgLevel())) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::LoggerName:
					{
						const auto& tmp { p.Name() };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::LogSource:
					{
						auto tmp { std::move(cf::Format_Source_Loc { p, srcFlags }.FormatUserPattern()) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::LoggerThreadID:
					{
						auto tmp { std::move(cf::Format_Thread_ID {}.FormatUserPattern(threadLength)) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::LogMessage:
					{
						const auto& tmp { p.Message() };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case cf::flag_type::Default:
					{
						resultBuff.clear();
						auto size { contSize + cf::defaultSizeEstimate };
						if( resultBuff.capacity() < size ) resultBuff.reserve(size);
						resultBuff.append("- Logger Name:\t")
						.append(p.Name())
						.append("\n- Long Level:\t")
						.append(LevelToLongView(p.MsgLevel()))
						.append("\n- Short Level:\t")
						.append(LevelToShortView(p.MsgLevel()))
						.append("\n- Log Message:\t")
						.append(p.Message())
						.append("\n- Log Source:\t")
						.append(cf::Format_Source_Loc { p, srcFlags }.FormatUserPattern())
						.append("\n- Thread ID:\t")
						.append(cf::Format_Thread_ID {}.FormatUserPattern(threadLength))
						.append("\n");
						fch::WriteToContainer(std::string_view(resultBuff), resultBuff.size(), std::forward<resultCtx>(ctx));
						break;
					}
				// invalid flag should have been caught from the parsing, but include here and just don't format it
				case cf::flag_type::Invalid: [[fallthrough]];
				default: break;
			}
	}
};
