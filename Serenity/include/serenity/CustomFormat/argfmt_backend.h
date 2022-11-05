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

		static constexpr std::array<const char*, 4> custom_error_messages = {
			"Unkown Formatting Error Occured In Internal Serenity CustomFlag Formatting.",
			"Error In Internal CustomFlag Formatting For Serenity: Missing '%' before flag.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Escaped Opening Bracket Was Detected - Missing Escaped Closing Bracket.",
			"Error In Internal CustomFlag Formatting For Serenity:  An Unsupported Flag Was Encountered Or There Were No Flags Present.",

		};

		[[noreturn]] constexpr void ReportCustomError(CustomErrors err) {
			using enum CustomErrors;
			switch( err ) {
					case missing_flag_specifier: throw format_error(custom_error_messages[ 1 ]); break;
					case missing_enclosing_bracket: throw format_error(custom_error_messages[ 2 ]); break;
					case unknown_flag: throw format_error(custom_error_messages[ 3 ]); break;
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
};

// just here as a placeholder for now
enum class SourceLocFlags
{
};

template<> struct formatter::CustomFormatter<serenity::msg_details::Message_Info>
{
	serenity::CustomFlagError::custom_error_handler errHandler {};
	flag_type flag;

	inline constexpr void Parse(std::string_view parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto size { parse.size() };
		auto pos { -1 };

		for( ;; ) {
				if( ++pos >= size ) {
						// since we break early if '}' is detected, the fact that we reached this part means no matching brace was found
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				// Not sure how I want to handle positional arguments here or if it's even neccessary right now
				// [PLACEHOLDER FOR POSITIONAL ARG DETECTION]

				// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't exist
				if( parse[ pos ] != ':' ) continue;
				++pos;
				if( parse[ pos ] != '%' ) {
						errHandler.ReportCustomError(CustomErrors::missing_flag_specifier);
				}
				++pos;
				// the flag specifier token was detected, now try matching the flag
				switch( parse[ pos ] ) {
						case 'L': flag = flag_type::LongLogLvl; break;
						case 'l': flag = flag_type::ShortLogLvl; break;
						case 'N': flag = flag_type::LoggerName; break;
						// LogSource is a little bit special given the modifiers that are attached to it
						case 's':
							flag = flag_type::LogSource;
							if( ++pos < size ) {
									if( parse[ pos ] != ':' ) break;
									++pos;
									for( ;; ) {
											// scan for modifiers
										}
							}
							break;
							// LoggerThreadID is also a little bit special due to it's length modifier
						case 't':
							flag = flag_type::LoggerThreadID;
							if( ++pos < size ) {
									if( parse[ pos ] != ':' ) break;
									++pos;
									for( ;; ) {
											// scan for legnth modifier
										}
							}
							break;
						case '+': flag = flag_type::LogMessage; break;
						default:
							flag = flag_type::Invalid;
							errHandler.ReportCustomError(CustomErrors::unknown_flag);
							break;
					}
				// flag is now set so advance and break early if closing brace is found, enclosing braces that
				// needed to be escaped are accounted for in the root ArgFormatter Parse() function
				++pos;
				if( parse[ pos ] == '}' ) break;
			}

		return;
	}

	template<typename ContainerCtx> constexpr auto Format(const serenity::msg_details::Message_Info& p, ContainerCtx& ctx) const {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		switch( flag ) {
					// invalid flag should have been caught from the parsing, but include here and just don't format it
				case flag_type::Invalid: break;
				case flag_type::LongLogLvl: formatter::format_to(std::back_inserter(ctx), "{}", LevelToLongView(p.MsgLevel())); break;
				case flag_type::ShortLogLvl: formatter::format_to(std::back_inserter(ctx), "{}", LevelToShortView(p.MsgLevel())); break;
				case flag_type::LoggerName: formatter::format_to(std::back_inserter(ctx), "{}", p.Name()); break;
				case flag_type::LogSource: break;
				case flag_type::LoggerThreadID: break;
				case flag_type::LogMessage: formatter::format_to(std::back_inserter(ctx), "{}", p.Message()); break;
				default: break;
			}
	}
};
