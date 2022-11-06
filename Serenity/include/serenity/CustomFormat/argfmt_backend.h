#pragma once

#include <ArgFormatter/ArgContainer.h>
#include <serenity/MessageDetails/Message_Info.h>

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

// just here as a placeholder for now
enum class SrcLocMod
{
};

// This is going to need to be sped up an immense amount.. this is taking too long right now
template<> struct formatter::CustomFormatter<serenity::msg_details::Message_Info>
{
	serenity::CustomFlagError::custom_error_handler errHandler {};
	flag_type flag { flag_type::Invalid };

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
							flag = flag_type::LogSource;
							break;
							// LoggerThreadID is also a little bit special due to it's length modifier
						case 't': flag = flag_type::LoggerThreadID; return;
						case '+': flag = flag_type::LogMessage; return;
						default:
							flag = flag_type::Invalid;
							errHandler.ReportCustomError(CustomErrors::unknown_flag);
							return;
					}
			}
	}

	template<typename ContainerCtx> constexpr auto Format(const serenity::msg_details::Message_Info& p, ContainerCtx& ctx) const {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		switch( flag ) {
					// invalid flag should have been caught from the parsing, but include here and just don't format it
				case flag_type::LongLogLvl: return formatter::format_to(std::back_inserter(ctx), "{}", LevelToLongView(p.MsgLevel()));
				case flag_type::ShortLogLvl: return formatter::format_to(std::back_inserter(ctx), "{}", LevelToShortView(p.MsgLevel()));
				case flag_type::LoggerName: return formatter::format_to(std::back_inserter(ctx), "{}", p.Name());
				case flag_type::LogSource: return formatter::format_to(std::back_inserter(ctx), "{}", "[PlaceHolder]");
				case flag_type::LoggerThreadID: return formatter::format_to(std::back_inserter(ctx), "{}", "[PlaceHolder]");
				case flag_type::LogMessage: return formatter::format_to(std::back_inserter(ctx), "{}", "[PlaceHolder]");
				case flag_type::Default:
					return formatter::format_to(std::back_inserter(ctx),
					                            "- Logger Name:\t{0}\n- Long Level:\t{1}\n- Short Level:\t{2}\n- Log Message:\t{3}\n- Log Source:\t{3}\n- Thread "
					                            "ID:\t{3}\n",
					                            p.Name(), LevelToLongView(p.MsgLevel()), LevelToShortView(p.MsgLevel()), "[PlaceHolder]");

				case flag_type::Invalid: [[fallthrough]];
				default: break;
			}
	}
};
