#pragma once

#include <string_view>

template<typename T, typename... Args>
void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename... Args> void serenity::arg_formatter::ArgFormatter::se_format_to(std::string& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(Iter, sv);
}

template<typename T>
inline bool serenity::arg_formatter::ArgFormatter::HandleIfEndOrWhiteSpace(std::back_insert_iterator<T>&& Iter, std::string_view sv, size_t& currentPosition,
                                                                           const size_t& bracketSize) {
	auto ch { sv[ currentPosition + static_cast<size_t>(1) ] };
	if( ch == '}' ) {
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
			argCounter  = 0;
			m_tokenType = TokenType::Empty;

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
	auto remainder { sv.substr(bracketResults.endPos + 1, sv.size()) };
	std::move(remainder.begin(), remainder.end(), Iter);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatFillAlignToken(std::back_insert_iterator<T>&& Iter) {
	temp.clear();
	size_t fillAmount {}, totalWidth {}, i { 0 };
	if( specValues.hasAltForm ) {
			temp.append(std::move(specValues.preAltForm));
	}
	// clang-format off
	int precision{ specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
																									: specValues.precision != 0 ? specValues.precision : 0
	};
	// clang-format on
	argStorage.GetArgValueAsStr(temp, specValues.argPosition, std::move(specValues.typeSpec), precision);
	if( specValues.nestedWidthArgPos != 0 ) {
			totalWidth = argStorage.int_state(specValues.nestedWidthArgPos);
	} else {
			totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
		}

	auto size(temp.size());
	fillAmount = (totalWidth > size) ? totalWidth - size : 0;

	if( fillAmount == 0 ) {
			std::move(temp.begin(), temp.end(), Iter);
			return;
	}
	std::string_view fillCh { &specValues.fillCharacter, 1 };
	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					std::move(temp.begin(), temp.end(), Iter);
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillCh.begin(), fillCh.end(), Iter);
							++i;
						}
				}
				break;
			case Alignment::AlignRight:
				{
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillCh.begin(), fillCh.end(), Iter);
							++i;
						}
					std::move(temp.begin(), temp.end(), Iter);
				}
				break;
			case Alignment::AlignCenter:
				fillAmount /= 2;
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillCh.begin(), fillCh.end(), Iter);
						++i;
					}
				std::move(temp.begin(), temp.end(), Iter);
				i          = 0;
				fillAmount = (totalWidth - size - fillAmount);
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillCh.begin(), fillCh.end(), Iter);
						++i;
					}
				break;
			default: break;
		}
}
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatSignToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatAlternateToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatZeroPadToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatLocaleToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatWidthToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatPrecisionToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTypeToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatCharAggregateToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatCustomToken(std::back_insert_iterator<T>&& Iter) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatPositionalToken(std::back_insert_iterator<T>&& Iter) { }

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTokens(std::back_insert_iterator<T>&& Iter) {
	using enum TokenType;
	if( IsFlagSet(TokenType::FillAlign) ) {
			FormatFillAlignToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Sign) ) {
			FormatSignToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Alternate) ) {
			FormatAlternateToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::ZeroPad) ) {
			FormatZeroPadToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Locale) ) {
			FormatLocaleToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Width) ) {
			FormatWidthToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Precision) ) {
			FormatPrecisionToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Type) ) {
			FormatTypeToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Positional) ) {
			FormatPositionalToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Custom) ) {
			FormatCustomToken(std::forward<std::back_insert_iterator<T>>(Iter));
	} else if( IsFlagSet(TokenType::Empty) ) {
			SimpleFormat(std::forward<std::back_insert_iterator<T>>(Iter));
	}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::SimpleFormat(std::back_insert_iterator<T>&& Iter) { }

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}