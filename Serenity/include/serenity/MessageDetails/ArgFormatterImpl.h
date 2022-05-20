#pragma once
/************************************** What The ArgFormatter And ArgContainer Classes Offer **************************************/
// This work is a very simple reimplementation with limititations on my end of Victor Zverovich's fmt library.
// Currently the standard's implementation of his work is still underway although for the most part, it's feature
// complete with Victor's library - there are some huge performance drops when it's not compiled under the /utf-8
// flag on MSVC though.
//
// The ArgFormatter and ArgContainer classes work in tandem to deliver a very bare-bones version of what the fmt,
// and MSVC's implementation of fmt, libraries provides and is only intended for usage until MSVC's code is as performant
// as it is when compiled with /utf-8 for compilation without the need for this flag.
//
// With that being said, these classes provide the functionality of formatting to a container with a back insert iterator
// object which mirrors the std::format_to()/std::vformat_to() via the se_format_to() function, as well as a way to recieve
// a string with the formatted result via the se_format() function, mirroring std::format()/std::vformat().
// Unlike MSVC's implementations, however, the default locale used when the locale specifier is present will refer to the
// default locale created on construction of this class. The downside is that this will not reflect any changes when the
// locale is changed globally; but the benefit of this approach is reducing the construction of an empty locale on every
// format, as well as the ability to change the locale with "SetLocale(const std::locale &)" function without affecting
// the locale of the rest of the program. All formatting specifiers and manual/automatic indexing from the fmt library
// are available and supported.
//
// EDIT: It now seems that MSVC build  192930145 fixes the performance issues among other things with the <format> lib;
// however, the performance times of serenity is STILL faster than the MSVC's implementation - the consistency of their
// performance is now a non-issue though (same performance with or without the UTF-8 flag)
/**********************************************************************************************************************************/

#include "ArgFormatter.h"
#include <string_view>

template<typename T, typename... Args>
void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename... Args> std::string serenity::arg_formatter::ArgFormatter::se_format(std::string_view sv, Args&&... args) {
	std::string tmp;
	se_format_to(std::back_inserter(tmp), sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename T>
inline bool serenity::arg_formatter::ArgFormatter::HandleIfEndOrWhiteSpace(std::back_insert_iterator<T>&& Iter, std::string_view sv, size_t& currentPosition,
                                                                           const size_t& bracketSize) {
	char ch;
	if( bracketSize > 1 ) {
			ch = sv[ currentPosition + static_cast<size_t>(1) ];
	} else {
			ch = sv[ currentPosition ];
		}
	if( ch == '}' ) {
			ParsePositionalField(sv, argCounter, currentPosition);
			FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
			return true;
	} else if( ch == ' ' ) {
			// handle appropriately if it's only one space as a Sign spec,
			// otherwise, remove any extra spaces past the first one
			if( currentPosition >= 0 ) {
					if( sv[ currentPosition ] == ':' ) {
							specValues.signType = Sign::Space;
							currentPosition += 2;    // move past ':' & ' '
							return false;
					}
			}
			for( ;; ) {
					if( currentPosition > bracketSize ) break;
					if( ch = sv[ ++currentPosition ] != ' ' ) break;
				}
			if( ch == '}' ) {
					FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
					++argCounter;
					return true;
			} else {
					++currentPosition;
				}
	}
	return false;
}

template<typename T> void serenity::arg_formatter::ArgFormatter::Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter = 0;
	for( ;; ) {
			if( sv.size() < 2 ) return;
			size_t pos { 0 };
			result.remainder = sv;
			specValues.ResetSpecs();
			FindBrackets(result.remainder);
			if( !bracketResults.isValid ) {
					for( auto& ch: result.remainder ) {
							Iter = ch;
						}
					break;
			}
			auto preToken { sv.substr(0, bracketResults.beginPos) };
			for( auto& ch: preToken ) {
					Iter = ch;
				}
			auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
			const size_t& bracketSize { argBracket.size() };
			/* Handle If Bracket Contained No Specs */
			if( HandleIfEndOrWhiteSpace(std::forward<std::back_insert_iterator<T>>(Iter), argBracket, pos, bracketSize) ) {
					sv.remove_prefix(argBracket.size() + preToken.size() + 1);
					continue;
			}
			/*Handle Escaped Bracket*/
			if( argBracket[ pos ] == '{' ) {
					std::string tmp { result.preTokenStr };
					tmp.append(1, '{');
					result.preTokenStr = std::move(tmp);
			}
			/*Handle Positional Args*/
			if( !ParsePositionalField(argBracket, argCounter, pos) ) {
					// Nothing to Parse - just a simple substitution
					FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
					++argCounter;
					break;
			}
			/* Handle What's Left Of The Bracket */
			VerifyArgumentBracket(argBracket, pos, bracketSize);
			FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
			sv.remove_prefix(argBracket.size() + preToken.size() + 1);
		}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::AppendDirectly(std::back_insert_iterator<T>&& Iter, experimental::msg_details::SpecType type) {
	using SpecType = experimental::msg_details::SpecType;
	std::string_view sv;
	switch( type ) {
			case SpecType::StringType: sv = std::move(argStorage.string_state(specValues.argPosition)); break;
			case SpecType::CharPointerType: sv = std::move(argStorage.c_string_state(specValues.argPosition)); break;
			case SpecType::StringViewType: sv = std::move(argStorage.string_view_state(specValues.argPosition)); break;
			default: break;
		}
	for( auto& ch: sv ) {
			Iter = ch;
		}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTokens(std::back_insert_iterator<T>&& Iter) {
	size_t fillAmount {}, totalWidth {}, i { 0 };
	// clang-format off
	auto precision{ specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
																									: specValues.precision != 0 ? specValues.precision : 0
	};
	// clang-format on
	if( specValues.nestedWidthArgPos != 0 ) {
			totalWidth = argStorage.int_state(specValues.nestedWidthArgPos);
	} else {
			totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
		}

	using SpecType = experimental::msg_details::SpecType;
	auto argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };

	if( totalWidth == 0 && precision == 0 ) {
			switch( argType ) {
					case SpecType::StringType: [[fallthrough]];
					case SpecType::CharPointerType: [[fallthrough]];
					case SpecType::StringViewType:
						AppendDirectly(std::forward<std::back_insert_iterator<T>>(Iter), argType);
						return;
						break;
					default: break;
				}
	}

	rawValueTemp.clear();

	if( specValues.localize ) {
			LocalizeArgument(precision, argType);
	} else {
			FormatRawValueToStr(precision, argType);
		}

	auto size(rawValueTemp.size());
	fillAmount = (totalWidth > size) ? totalWidth - size : 0;

	if( fillAmount == 0 ) {
			for( auto& ch: rawValueTemp ) {
					Iter = std::move(ch);
				}
			return;
	}
	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					for( auto& ch: rawValueTemp ) {
							Iter = std::move(ch);
						}
					for( ;; ) {
							if( i >= fillAmount ) break;
							Iter = specValues.fillCharacter;
							++i;
						}
				}
				break;
			case Alignment::AlignRight:
				{
					for( ;; ) {
							if( i >= fillAmount ) break;
							Iter = specValues.fillCharacter;
							++i;
						}
					for( auto& ch: rawValueTemp ) {
							Iter = std::move(ch);
						}
				}
				break;
			case Alignment::AlignCenter:
				fillAmount /= 2;
				for( ;; ) {
						if( i >= fillAmount ) break;
						Iter = specValues.fillCharacter;
						++i;
					}
				i = 0;
				for( auto& ch: rawValueTemp ) {
						Iter = std::move(ch);
					}
				i          = 0;
				fillAmount = (totalWidth - size - fillAmount);
				for( ;; ) {
						if( i >= fillAmount ) break;
						Iter = specValues.fillCharacter;
						++i;
					}
				break;
			default: break;
		}
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatFloatTypeArg(T&& value, int precision) {
	std::chars_format format {};
	auto data { buffer.data() };
	std::fill(data, data + buffer.size(), 0);
	int pos { 0 };

	if( specValues.signType == Sign::Space ) {
			specValues.signType = value < 0 ? Sign::Minus : Sign::Space;
	}
	switch( specValues.signType ) {
			case Sign::Space:
				data[ pos ] = ' ';
				++pos;
				break;
			case Sign::Plus:
				data[ pos ] = '+';
				++pos;
				break;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: break;
		}

	// default behaviors
	if( specValues.hasAlt && precision == 0 ) {
			format = std::chars_format::scientific;
	} else if( !specValues.localize && specValues.alignmentPadding == 0 && precision == 0 && specValues.signType == Sign::Empty ) {
			format = std::chars_format::fixed;
	} else {
			format = std::chars_format::general;
		}
	// default behavior skipped, so process the type spec that was found
	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'a': [[fallthrough]];
			case 'A':
				precision = precision > 0 ? precision : 0;
				format    = std::chars_format::hex;
				break;
			case 'e': [[fallthrough]];
			case 'E':
				format    = std::chars_format::scientific;
				precision = precision > 0 ? precision : 6;
				break;
			case 'f': [[fallthrough]];
			case 'F':
				format    = std::chars_format::fixed;
				precision = precision > 0 ? precision : 6;
				break;
			case 'g': [[fallthrough]];
			case 'G':
				format    = std::chars_format::general;
				precision = precision > 0 ? precision : 6;
				break;
			default: break;
		}

	if( precision != 0 ) {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format, precision);
	} else {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format);
		}
	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'A': [[fallthrough]];
			case 'E': [[fallthrough]];
			case 'G':
				for( auto& ch: buffer ) {
						if( ch == *charsResult.ptr ) break;
						if( ch >= 'a' && ch <= 'z' ) {
								ch -= 32;
						}
					}
				break;
			default: break;
		}    // capitilization
	rawValueTemp.append(data, charsResult.ptr);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatIntTypeArg(T&& value) {
	int base { 10 };
	auto data { buffer.data() };
	std::fill(data, data + buffer.size(), 0);
	int pos { 0 };

	if( specValues.signType == Sign::Space ) {
			specValues.signType = value < 0 ? Sign::Minus : Sign::Space;
	}
	switch( specValues.signType ) {
			case Sign::Space:
				data[ pos ] = ' ';
				++pos;
				break;
			case Sign::Plus:
				data[ pos ] = '+';
				++pos;
				break;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: break;
		}

	if( specValues.preAltForm != "\0" ) {
			size_t i { 0 };
			auto size { specValues.preAltForm.size() };
			for( auto& ch: specValues.preAltForm ) {
					data[ pos ] = ch;
					++pos;
				}
	}

	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'b': [[fallthrough]];
			case 'B': base = 2; break;
			case 'c': rawValueTemp += static_cast<char>(value); return;
			case 'o': base = 8; break;
			case 'x': [[fallthrough]];
			case 'X': base = 16; break;
			default: break;
		}

	charsResult = std::to_chars(data + pos, data + buffer.size(), value, base);

	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'B': [[fallthrough]];
			case 'X':
				for( auto& ch: buffer ) {
						if( ch == *charsResult.ptr ) break;
						if( ch >= 'a' && ch <= 'z' ) {
								ch -= 32;
						}
					}
				break;
			default: break;
		}    // capitilization
	rawValueTemp.append(data, charsResult.ptr);
}