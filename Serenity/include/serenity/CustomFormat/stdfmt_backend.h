#pragma once

#include <serenity/Common.h>
#include <format>

template<> struct std::formatter<serenity::LoggerLevel>
{
	enum class LevelView
	{
		Invalid = 0,
		Long,
		Short
	};
	serenity::CustomFlagError::custom_error_handler errHandler {};
	LevelView view { LevelView::Invalid };

	constexpr auto parse(std::format_parse_context& parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto end { parse.end() };
		auto pos { parse.begin() };

		for( ;; ) {
				if( pos >= end ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				switch( *pos ) {
						case '}': return pos;
						case '%': break;
						// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't
						// exist and continue until we reach it or hit the end of the parse string
						default: ++pos; continue;
					}
				for( ;; ) {
						if( ++pos >= end ) {
								errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
						}
						switch( *pos ) {
								case 'L': view = LevelView::Long; continue;
								case 'l': view = LevelView::Short; continue;
								case '}':
									{
										if( view == LevelView::Invalid ) errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
										return pos;
									}
								case ' ': [[fallthrough]];    // ignore whitespace when determining if valid thread id bracket
								default: continue;
							}
					}
			}
	}
	auto format(const serenity::LoggerLevel& lvl, std::format_context& ctx) const {
		switch( view ) {
				case LevelView::Short:
					{
						std::format_to(ctx.out(), "{}", serenity::LevelToShortView(lvl));
						return ctx.out();
					}
				case LevelView::Long:
					{
						std::format_to(ctx.out(), "{}", serenity::LevelToLongView(lvl));
						return ctx.out();
					}
				case LevelView::Invalid: [[fallthrough]];
				default:
					{
						constexpr std::string_view invalidMessage { "INVALID_LEVEL" };
						std::format_to(ctx.out(), "{}", invalidMessage);
						return ctx.out();
					}
			}
	}
};

template<> struct std::formatter<std::tm>
{
	serenity::CustomFlagError::custom_error_handler errHandler {};
	// Bypassing the really slow std::chrono formatter in favor of strftime so we need to store the pattern in `fmtStr` to refer to in the formatting call. There
	// should be the same error checking that is in ArgFormatter ParseTimeField() here. Also *WILL* need a way to check for subsecond  precision and
	// handle that as well -> I may end up needing to copy pasta the ctime parsing AND formatting out of ArgFormatter and into these parse()/format()
	//  functions for this case since std::tm is *NOT* natively supported in <format> AND the std::chrono formatter is terribly slow for this use case.
	std::string fmtStr {};

	constexpr auto parse(std::format_parse_context& parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto end { parse.end() };
		auto pos { parse.begin() };
		for( ;; ) {
				if( pos >= end ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				if( *pos == '}' ) break;
				fmtStr += *pos++;
			}
		return pos;
	}

	auto format(const std::tm& tmStruct, std::format_context& ctx) const {
		std::array<char, 128> largeBuff {};
		auto written { strftime(largeBuff.data(), 255, fmtStr.data(), &tmStruct) };
		std::format_to(ctx.out(), ctx.locale(), "{}", std::string_view(largeBuff.data(), written));
		return ctx.out();
	}
};