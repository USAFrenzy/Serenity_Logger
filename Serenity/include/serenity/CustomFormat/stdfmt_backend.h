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
		auto pos { parse.begin() - 1 };

		for( ;; ) {
				if( ++pos >= end ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				switch( *pos ) {
						case '}': return parse.begin();
						case ':':
							{
								if( ++pos >= end ) {
										errHandler.ReportCustomError(CustomErrors::no_flag_mods);
								}
								if( *pos != '%' ) {
										errHandler.ReportCustomError(CustomErrors::missing_flag_specifier);
								}
								break;
							}
						// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't
						// exist and continue until we reach it or hit the end of the parse string
						default: continue;
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
										return parse.begin();;
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
						return std::format_to(ctx.out(), "{}", serenity::LevelToShortView(lvl));
					}
				case LevelView::Long:
					{
						return std::format_to(ctx.out(), "{}", serenity::LevelToLongView(lvl));
					}
				case LevelView::Invalid: [[fallthrough]];
				default: constexpr std::string_view invalidMessage { "INVALID_LEVEL" }; return std::format_to(ctx.out(), "{}", invalidMessage);
			}
	}
};

template<> struct std::formatter<std::tm>
{
	constexpr auto parse(std::format_parse_context& parse) {
		return parse.begin();
	}

	auto format(const std::tm& tmStruct, std::format_context& ctx) const {
		auto tmp { tmStruct };
		return std::format_to(ctx.out(), ctx.locale(), "{}", std::mktime(&tmp));
	}
};