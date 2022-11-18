#pragma once

#include <ArgFormatter/ArgFormatter.h>
#include <serenity/Common.h>

template<> struct formatter::CustomFormatter<serenity::LoggerLevel>
{
	enum class LevelView
	{
		Invalid = 0,
		Long,
		Short
	};
	serenity::CustomFlagError::custom_error_handler errHandler {};
	LevelView view { LevelView::Invalid };

	inline constexpr void Parse(std::string_view parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto size { parse.size() };
		auto pos { -1 };

		for( ;; ) {
				if( ++pos >= size ) {
						errHandler.ReportCustomError(CustomErrors::missing_enclosing_bracket);
				}
				switch( parse[ pos ] ) {
						case '}': return;
						case ':':
							{
								if( ++pos >= size ) {
										errHandler.ReportCustomError(CustomErrors::no_flag_mods);
								}
								if( parse[ pos ] != '%' ) {
										errHandler.ReportCustomError(CustomErrors::missing_flag_specifier);
								}
								break;
							}
						// if we don't hit ':' then assume that the flag hasn't been seen yet or doesn't
						// exist and continue until we reach it or hit the end of the parse string
						default: continue;
					}
				for( ;; ) {
						if( ++pos >= size ) {
								errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
						}
						switch( parse[ pos ] ) {
								case 'L': view = LevelView::Long; continue;
								case 'l': view = LevelView::Short; continue;
								case '}':
									{
										if( view == LevelView::Invalid ) errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
										return;
									}
								case ' ': [[fallthrough]];    // ignore whitespace when determining if valid thread id bracket
								default: continue;
							}
					}
			}
	}
	template<typename resultCtx> constexpr auto Format(const serenity::LoggerLevel& lvl, resultCtx& ctx) const {
		namespace fch = formatter::custom_helper;
		switch( view ) {
				case LevelView::Short:
					{
						const auto& tmp { std::move(serenity::LevelToShortView(lvl)) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case LevelView::Long:
					{
						const auto& tmp { std::move(serenity::LevelToLongView(lvl)) };
						fch::WriteToContainer(tmp, tmp.size(), std::forward<resultCtx>(ctx));
						break;
					}
				case LevelView::Invalid: [[fallthrough]];
				default:
					constexpr std::string_view invalidMessage { "INVALID_LEVEL" };
					fch::WriteToContainer(invalidMessage, 14, std::forward<resultCtx>(ctx));
					break;
			}
	}
};
