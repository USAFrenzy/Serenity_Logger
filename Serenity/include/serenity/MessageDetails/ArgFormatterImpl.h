#pragma once

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
			++argCounter;
			return true;
	} else if( ch == ' ' ) {
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
	result.remainder = sv;
	specValues.ResetSpecs();
	for( ;; ) {
			if( sv.size() < 2 ) break;

			size_t pos { 0 };
			argCounter = 0;

			FindBrackets(result.remainder);
			if( !bracketResults.isValid ) {
					std::move(sv.begin(), sv.end(), Iter);
					break;
			}
			auto preToken { sv.substr(0, bracketResults.beginPos) };
			std::move(preToken.begin(), preToken.end(), Iter);
			auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
			const size_t& bracketSize { argBracket.size() };

			/* Handle If Bracket Contained No Specs */
			if( HandleIfEndOrWhiteSpace(std::forward<std::back_insert_iterator<T>>(Iter), argBracket, pos, bracketSize) ) {
					break;
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
			auto endPos { bracketSize - 1 };
			for( ;; ) {
					if( pos >= endPos ) break;
					VerifyArgumentBracket(argBracket, pos, bracketSize);
				}
			FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
			break;
		}
	if( sv.size() < bracketResults.endPos + 1 ) return;
	auto remainder { sv.substr(bracketResults.endPos + 1, sv.size()) };
	std::move(remainder.begin(), remainder.end(), Iter);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTokens(std::back_insert_iterator<T>&& Iter) {
	size_t fillAmount {}, totalWidth {}, i { 0 };

	// clang-format off
	auto precision{ specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
																									: specValues.precision != 0 ? specValues.precision : 0
	};
	// clang-format on

	auto formattedArg { FormatRawValueToStr(precision) };

	if( specValues.nestedWidthArgPos != 0 ) {
			totalWidth = argStorage.int_state(specValues.nestedWidthArgPos);
	} else {
			totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
		}

	auto size(formattedArg.size());
	fillAmount = (totalWidth > size) ? totalWidth - size : 0;

	if( fillAmount == 0 ) {
			for( auto& ch: formattedArg ) {
					Iter = ch;
				}
			return;
	}
	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					for( auto& ch: formattedArg ) {
							Iter = ch;
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
					for( auto& ch: formattedArg ) {
							Iter = ch;
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
				for( auto& ch: formattedArg ) {
						Iter = ch;
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

template<typename T> std::string_view serenity::arg_formatter::ArgFormatter::FormatFloatTypeArg(T&& value, int precision) {
	auto data { buffer.data() };
	int pos { 0 };

	if( specValues.signType == Sign::Space ) {
			specValues.signType = value < 0 ? Sign::Minus : Sign::Plus;
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

	std::chars_format format { std::chars_format::general };
	if( specValues.preAltForm != "\0" ) {
			format = std::chars_format::fixed;
	} else {
			switch( specValues.typeSpec ) {
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
		}

	if( precision != 0 ) {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format, precision);
	} else {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format);
		}
	switch( specValues.typeSpec ) {
			case 'A': [[fallthrough]];
			case 'E': [[fallthrough]];
			case 'G':
				for( auto& ch: buffer ) {
						if( ch == *charsResult.ptr ) break;
						if( IsAlpha(ch) ) {
								ch -= 32;
						}
					}
				break;
			default: break;
		}    // capitilization
	return std::string_view(data, charsResult.ptr);
}
