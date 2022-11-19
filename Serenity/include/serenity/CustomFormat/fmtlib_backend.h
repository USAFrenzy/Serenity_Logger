#pragma once

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <serenity/Common.h>

template<> struct fmt::formatter<serenity::LoggerLevel>
{
	enum class LevelView
	{
		Invalid = 0,
		Long,
		Short
	};
	serenity::CustomFlagError::custom_error_handler errHandler {};
	LevelView view { LevelView::Invalid };

	constexpr auto parse(fmt::format_parse_context& parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto end { parse.end() };
		auto pos { parse.begin() };

		for( ;; ) {
				if( pos >= end ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				switch( *pos ) {
						case '}': return pos;
						case '%':
							break;
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
	auto format(serenity::LoggerLevel& lvl, fmt::format_context& ctx) const {
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