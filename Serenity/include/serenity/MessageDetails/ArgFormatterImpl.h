#pragma once

#include <string_view>

template<typename T, typename... Args> void serenity::arg_formatter::ArgFormatter::se_format_to(T&& container, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<T>(container), sv);
}

template<typename... Args> void serenity::arg_formatter::ArgFormatter::se_format_to(std::string& container, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(container, sv);
}

template<typename T>
inline bool serenity::arg_formatter::ArgFormatter::HandleIfEndOrWhiteSpace(T&& container, std::string_view sv, size_t& currentPosition, const size_t& bracketSize) {
	auto ch { sv[ currentPosition + static_cast<size_t>(1) ] };
	if( ch == '}' ) {
			FormatTokens(std::forward<T>(container));
			++argCounter;
			return true;
	} else if( ch == ' ' ) {
			for( ;; ) {
					if( currentPosition > bracketSize ) break;
					if( ch = sv[ ++currentPosition ] != ' ' ) break;
				}
			if( ch == '}' ) {
					FormatTokens(std::forward<T>(container));
					++argCounter;
					return true;
			} else {
					++currentPosition;
				}
	}
	return false;
}

template<typename T> void serenity::arg_formatter::ArgFormatter::Parse(T&& container, std::string_view sv) {
	result.remainder = sv;
	specValues.ResetSpecs();
	for( ;; ) {
			if( sv.size() < 2 ) break;

			size_t pos { 0 };
			argCounter  = 0;
			m_tokenType = TokenType::Empty;

			FindBrackets(result.remainder);
			if( !bracketResults.isValid ) {
					std::move(sv.begin(), sv.end(), container);
					break;
			}
			auto preToken { sv.substr(0, bracketResults.beginPos) };
			std::move(preToken.begin(), preToken.end(), container);
			auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
			const size_t& bracketSize { argBracket.size() };

			/* Handle If Bracket Contained No Specs */
			if( HandleIfEndOrWhiteSpace(std::forward<T>(container), argBracket, pos, bracketSize) ) {
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
					FormatTokens(std::forward<T>(container));
					++argCounter;
					break;
			}
			/* Handle What's Left Of The Bracket */
			auto endPos { bracketSize - 1 };
			for( ;; ) {
					if( pos >= endPos ) break;
					VerifyArgumentBracket(argBracket, pos, bracketSize);
				}
			FormatTokens(std::forward<T>(container));
			break;
		}
	auto remainder { sv.substr(bracketResults.endPos + 1, sv.size()) };
	std::move(remainder.begin(), remainder.end(), container);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatFillAlignToken(T&& container) {
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
			std::move(temp.begin(), temp.end(), container);
			return;
	}
	std::string_view fillCh { &specValues.fillCharacter, 1 };
	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					std::move(temp.begin(), temp.end(), container);
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillCh.begin(), fillCh.end(), container);
							++i;
						}
				}
				break;
			case Alignment::AlignRight:
				{
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillCh.begin(), fillCh.end(), container);
							++i;
						}
					std::move(temp.begin(), temp.end(), container);
				}
				break;
			case Alignment::AlignCenter:
				fillAmount /= 2;
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillCh.begin(), fillCh.end(), container);
						++i;
					}
				std::move(temp.begin(), temp.end(), container);
				i          = 0;
				fillAmount = (totalWidth - size - fillAmount);
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillCh.begin(), fillCh.end(), container);
						++i;
					}
				break;
			default: break;
		}
}
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatSignToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatAlternateToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatZeroPadToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatLocaleToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatWidthToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatPrecisionToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTypeToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatCharAggregateToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatCustomToken(T&& container) { }
template<typename T> void serenity::arg_formatter::ArgFormatter::FormatPositionalToken(T&& container) { }

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTokens(T&& container) {
	using enum TokenType;
	if( IsFlagSet(TokenType::FillAlign) ) {
			FormatFillAlignToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Sign) ) {
			FormatSignToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Alternate) ) {
			FormatAlternateToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::ZeroPad) ) {
			FormatZeroPadToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Locale) ) {
			FormatLocaleToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Width) ) {
			FormatWidthToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Precision) ) {
			FormatPrecisionToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Type) ) {
			FormatTypeToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Positional) ) {
			FormatPositionalToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Custom) ) {
			FormatCustomToken(std::forward<T>(container));
	} else if( IsFlagSet(TokenType::Empty) ) {
			SimpleFormat(std::forward<T>(container));
	}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::SimpleFormat(T&& container) { }

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}