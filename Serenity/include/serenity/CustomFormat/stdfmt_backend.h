#pragma once

#include <serenity/Common.h>
#include <chrono>
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
						case 'L':
							{
								view = LevelView::Long;
								++pos;
								continue;
							}
						case 'l':
							{
								view = LevelView::Short;
								++pos;
								continue;
							}
						case '}':
							{
								if( view == LevelView::Invalid ) errHandler.ReportCustomError(CustomErrors::token_with_no_flag);
								return pos;
							}
						case ' ': [[fallthrough]];    // ignore whitespace
						default: ++pos; continue;
					}
			}
		se_unreachable();
		return pos;
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
	std::string fmtStr {};
	std::string subSecStr { "." };
	// Bypassing the really slow std::chrono formatter in favor of strftime so we need to store the pattern in `fmtStr` to refer to in the formatting call. There
	// should be the same error checking that is in ArgFormatter ParseTimeField() here. Also *WILL* need a way to check for subsecond  precision and
	// handle that as well -> I may end up needing to copy pasta the ctime parsing AND formatting out of ArgFormatter and into these parse()/format()
	//  functions for this case since std::tm is *NOT* natively supported in <format> AND the std::chrono formatter is terribly slow for this use case.
	constexpr auto parse(std::format_parse_context& parse) {
		using CustomErrors = serenity::CustomFlagError::CustomErrors;
		auto end { parse.end() };
		auto pos { parse.begin() };
		size_t lastMarker { 0 };
		for( ;; ) {
				switch( *pos ) {
						default:
							{
								if( ++pos < end ) continue;
								return pos;
							}
						case '}':
							{
								if( subSecStr.size() == 1 ) {
										fmtStr.append(parse.begin(), pos);
								} else if( static_cast<size_t>(end - pos) >  lastMarker ) {
									std::string_view tmp {parse.begin() + lastMarker, end};
									tmp.remove_suffix(end-pos);
									fmtStr.append(tmp.data(), tmp.size());
								}
								return pos;
							}
						case 'T':
							{
								if( ++pos != end && *pos == '.' ) {
										fmtStr.append(parse.begin(), pos);
										++pos;
										int precision {};
										std::array<char, 64> buff {};
										auto data { buff.data() };
										auto subSeconds { std::chrono::floor<std::chrono::nanoseconds>(std::chrono::system_clock::now()) };
										pos += (std::from_chars(&*pos, &*(end - 1), precision).ptr - &*pos);
										auto length { std::to_chars(data, data + 64, subSeconds.time_since_epoch().count()).ptr - data };
										//  Set the offset that only deals with subseconds
										auto offset { data + ((length % 10) + (length / 10)) };
										subSecStr.append(offset, offset + precision);
										lastMarker = (end - pos);
								}
								continue;
							}
					}
			}
		return pos;
	}

	auto format(const std::tm& tmStruct, std::format_context& ctx) const {
		std::array<char, 128> largeBuff {};
		if( subSecStr.size() == 1 ) {
				auto written { strftime(largeBuff.data(), 255, fmtStr.data(), &tmStruct) };
				std::format_to(ctx.out(), ctx.locale(), "{}", std::string_view(largeBuff.data(), written));
		} else {
				auto pos { -1 };
				std::string_view fmt { fmtStr };
				auto size { fmt.size() };
				for( ;; ) {
						if( ++pos >= size ) break;
						if( fmt[ pos ] != 'T' ) continue;
						auto firstPortion { std::move(fmt.substr(0, ++pos)) };
						fmt.remove_prefix(pos);
						auto written { strftime(largeBuff.data(), 255, firstPortion.data(), &tmStruct) };
						for( const auto& ch: subSecStr ) {
								largeBuff[ written ] = ch;
								++written;
							}
						if( fmt.size() != 0 ) {
								firstPortion = fmt.substr(written, fmt.size());
								written += strftime(largeBuff.data() + pos, 255, firstPortion.data(), &tmStruct);
						}
						std::format_to(ctx.out(), ctx.locale(), "{}", std::string_view(largeBuff.data(), written));
						break;
					}
			}
		return ctx.out();
	}
};