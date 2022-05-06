#include <serenity/Common.h>
#include <serenity/MessageDetails/LazyParser.h>

namespace serenity::lazy_parser {

	bool LazyParser::IsFlagSet(TokenType& tokenFlags, TokenType checkValue) {
		return (tokenFlags & checkValue) == checkValue;
	}

	size_t LazyParser::FindDigitEnd(std::string_view sv) {
		size_t pos { 0 };
		char ch { 0 };
		for( ;; ) {
				ch = sv[ pos ];
				if( IsDigit(ch) ) {
						++pos;
						continue;
				}
				break;
			}
		return pos;
	}

	int LazyParser::TwoDigitFromChars(std::string_view sv, size_t begin, size_t end) {
		// Given what the changes here are, can probably just write some logic to
		// get the length of the view and modulo it to get positions and generalize
		// this function a bit
		auto pos { static_cast<int>(begin) };
		switch( (end - begin) ) {
				case 0: break;
				case 1:
					switch( sv[ begin ] ) {
							case '0': break;
							case '1': ++pos; break;
							case '2': pos += 2; break;
							case '3': pos += 3; break;
							case '4': pos += 4; break;
							case '5': pos += 5; break;
							case '6': pos += 6; break;
							case '7': pos += 7; break;
							case '8': pos += 8; break;
							case '9': pos += 9; break;
							default: break;
						}
					break;
				case 2:
					switch( sv[ begin ] ) {
							case '0': break;
							case '1': ++pos; break;
							case '2': pos += 2; break;
							case '3': pos += 3; break;
							case '4': pos += 4; break;
							case '5': pos += 5; break;
							case '6': pos += 6; break;
							case '7': pos += 7; break;
							case '8': pos += 8; break;
							case '9': pos += 9; break;
							default: break;
						}
					pos *= 10;
					switch( sv[ begin + static_cast<size_t>(1) ] ) {
							case '0': break;
							case '1': ++pos; break;
							case '2': pos += 2; break;
							case '3': pos += 3; break;
							case '4': pos += 4; break;
							case '5': pos += 5; break;
							case '6': pos += 6; break;
							case '7': pos += 7; break;
							case '8': pos += 8; break;
							case '9': pos += 9; break;
							default: break;
						}
					break;
				default:
					std::string throwMsg { "TwoDigitFromChars() Only Handles String Types Of Size 2 Or Less: \"" };
					throwMsg.append(sv.data(), sv.size()).append("\" Doesn't Adhere To This Limitation\n");
					throw std::runtime_error(std::move(throwMsg));
					break;
			}
		return pos;
	}

	bool LazyParser::ParsePositionalField(std::string_view& sv, int& argIndex) {
		size_t endPos { FindDigitEnd(sv) };
		auto size { sv.size() };

		if( endPos <= 2 ) {
				argIndex = TwoDigitFromChars(sv, 0, endPos);
				sv.remove_prefix(endPos);
		} else {
				throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
			}
		endPos = 0;
		for( ;; ) {
				if( endPos >= size ) {
						endPos = std::string_view::npos;
						break;
				}
				if( sv[ endPos ] == ':' || sv[ endPos ] == '}' ) break;
				++endPos;
			}
		// probably add a handle to argContainer so that a comparison can
		// be made on the argIndex vs the # of args supplied
		bool moreSpecsToParse { (endPos != std::string_view::npos) && sv[ endPos ] != '}' };
		if( moreSpecsToParse ) {
				sv.remove_prefix(endPos + 1);
		} else {
				if( sv[ endPos ] != '}' ) {
						std::string throwMsg { "Error In Argument Field: Valid Argument Index Of '" };
						std::array<char, 2> buff { '\0', '\0' };
						std::to_chars(buff.data(), buff.data() + buff.size(), argIndex);
						throwMsg.append(buff.data(), buff.data() + (buff[ 1 ] == '\0' ? 1 : 2));
						throwMsg.append("' Provided But No Valid Specs Found After ':' \n ");
						throw std::runtime_error(throwMsg);
				}
			}
		return moreSpecsToParse;
	}

	void serenity::lazy_parser::LazyParser::FindNestedBrackets(std::string_view sv, size_t& currentPos) {
		size_t pos { currentPos };
		auto size { sv.size() };
		for( ;; ) {
				// search for any nested fields
				if( pos >= size ) break;
				// reset on each iteration
				bool nestedOpenFound { false };
				bool nestedCloseFound { false };

				if( sv[ pos ] != '{' ) {
						// potential nested arg
						++pos;
						continue;
				} else {
						nestedOpenFound = true;
						for( ;; ) {
								// found a potential nested bracket, now find a closing bracket
								if( pos >= size ) break;
								if( sv[ pos ] != '}' ) {
										++pos;
										continue;
								} else {
										nestedCloseFound = true;
										++pos;
										break;
									}
							}    // nested closing brace loop
					}                    // if nested opening brace found
				if( nestedOpenFound && nestedCloseFound ) {
						currentPos = pos;
				}
			}    // nested opening brace loop
	}

	void LazyParser::FindBrackets(std::string_view sv) {
		bracketResults.Reset();
		size_t pos { 0 };
		size_t subPos { 0 };
		auto size { sv.size() };
		// This is mainly when setting the sv to the remainder value in the Parse function
		if( size != 0 && sv[ 0 ] == '\0' ) {
				sv.remove_prefix(1);
		}
		for( ;; ) {
				if( pos >= size ) {
						bracketResults.isValid = false;
						break;
				}
				if( sv[ pos ] != '{' ) {
						++pos;
						continue;
				}
				bracketResults.beginPos = pos;
				subPos                  = pos + 1;
				for( ;; ) {
						if( subPos >= size ) bracketResults.isValid = false;
						auto ch { sv[ subPos ] };
						if( (ch == ':' || ch == '.') && sv[ subPos + static_cast<size_t>(1) ] == '{' ) {
								FindNestedBrackets(sv, subPos);
						}
						if( sv[ subPos ] != '}' ) {
								++subPos;
								continue;
						}
						bracketResults.endPos = subPos;
						break;
					}
				break;
			}
		result.preTokenStr     = std::move(sv.substr(0, pos));
		result.remainder       = std::move(sv.substr(subPos + static_cast<size_t>(1), size));
		bracketResults.isValid = true;
	}

	static constexpr std::array<char, 3> fillAlignSpecs = { '<', '>', '^' };

	bool LazyParser::HasFillAlignField(std::string_view& sv) {
		auto svSize { sv.size() };
		if( svSize < 2 ) return false;    // accounting for fill-align spec and '}'
		size_t pos { 0 };

		auto ch { sv[ pos ] };
		auto nextCh { sv[ ++pos ] };
		if( ch == '}' ) return false;
		if( ch != '{' ) {
				switch( ch ) {
						case '<': [[fallthrough]];
						case '>': [[fallthrough]];
						case '^':
							// clang-format off
							throw std::runtime_error("Error In Fill Align Field: Alignment Option Specified Without A Fill Character\n");
							// clang-format on
							break;
						default: break;
					}

				switch( nextCh ) {
						case '<': specValues.align = Alignment::AlignLeft; break;
						case '>': specValues.align = Alignment::AlignRight; break;
						case '^': specValues.align = Alignment::AlignCenter; break;
						default: specValues.align = Alignment::Empty; break;
					}

				if( specValues.align != Alignment::Empty ) {
						specValues.fillCharacter = ch;
				}

				++pos;
				if( pos < svSize ) {
						if( IsDigit(sv[ pos ]) ) {
								++pos;
								for( ;; ) {
										if( pos >= svSize ) break;
										if( !IsDigit(sv[ pos ]) ) break;
										++pos;
									}
						}
						std::from_chars(sv.data() + 2, sv.data() + pos, specValues.alignmentPadding);
						m_tokenType |= TokenType::AlignmentPadding;
				}
		}
		return (specValues.align != Alignment::Empty);
	}

	static constexpr std::array<char, 3> signSpecs = { '+', '-', ' ' };
	bool LazyParser::HasSignField(std::string_view& sv) {
		if( sv.size() < 2 ) return false;    // this accounts for at least sign + '}'
		auto ch { sv[ 0 ] };
		switch( ch ) {
				case '+': specValues.signType = Sign::Plus; break;
				case '-': specValues.signType = Sign::Minus; break;
				case ' ': specValues.signType = Sign::Space; break;
				default: return false; break;
			}
		sv.remove_prefix(1);
		return true;
	}

	static std::string_view NestedFieldTypeStr(NestedFieldType type) {
		return type == NestedFieldType::Precision ? "Precision" : "Width";
	}

	void LazyParser::FormatFillAlignToken() { }

	void LazyParser::FormatEmptyToken() { }

	void LazyParser::FormatSignToken() { }

	void LazyParser::FormatAlternateToken() { }

	void LazyParser::FormatZeroPadToken() { }

	void LazyParser::FormatLocaleToken() { }

	void LazyParser::FormatWidthToken() { }

	void LazyParser::FormatPrecisionToken() { }

	void LazyParser::FormatTypeToken() { }

	void LazyParser::FormatCharAggregateToken() { }

	void LazyParser::FormatCustomToken() { }

	void LazyParser::FormatTokens() {
		using enum TokenType;
		if( IsFlagSet(m_tokenType, FillAlign) ) {
				FormatFillAlignToken();

				/*			m_tokenStorage.emplace_back(
				                        std::make_unique<FillAlignToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Sign) ) {
				FormatSignToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<SignToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Alternate) ) {
				FormatAlternateToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<AlternateToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, ZeroPad) ) {
				FormatZeroPadToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<ZeroPadToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Width) ) {
				FormatWidthToken();
				/*m_tokenStorage.emplace_back(
				std::make_unique<WidthToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Precision) ) {
				FormatPrecisionToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<PrecisionToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Locale) ) {
				FormatLocaleToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<LocaleToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Type) ) {
				FormatTypeToken();
				/*	m_tokenStorage.emplace_back(
				        std::make_unique<TypeToken>(originalBracket, m_tokenType, argCounter));*/
		} else if( IsFlagSet(m_tokenType, Custom) ) {
				// TODO: Actually implement a way to forward user spec and formatting
				// NOTE: I literally have no idea how fmtlib/<format> handles this, so
				//       this will be saved for last
				FormatCustomToken();
				/*m_tokenStorage.emplace_back(
				std::make_unique<CustomToken>(originalBracket, m_tokenType, argCounter));*/
		}
	}

	bool LazyParser::HasValidNestedField(std::string_view& sv, NestedFieldType type, size_t index) {
		size_t primaryPos { 0 }, subPos { 0 }, argIndex { index };
		char ch { 0 };
		for( ;; ) {
				ch = sv[ primaryPos ];
				if( primaryPos >= sv.size() ) {
						primaryPos = std::string_view::npos;
						break;
				}
				if( ch == '}' ) break;

				if( !IsDigit(ch) && ch != ' ' ) {
						std::string throwMsg { "Error In Nested Argument Bracket For " };
						throwMsg.append(NestedFieldTypeStr(type));
						throwMsg.append(": Only A Position Field May Be Declared In A Nested Bracket Argument\n");
						throw std::runtime_error(std::move(throwMsg));
				}

				if( IsDigit(ch) ) {
						// TODO: Add index mode check here as well
						subPos = primaryPos + 1;
						for( ;; ) {
								if( subPos >= sv.size() ) break;
								if( !IsDigit(sv[ subPos ]) ) break;
								++subPos;
							}
						if( subPos > (primaryPos + 3) ) {
								throw std::runtime_error("Error In Positional Argument Field: Max Position (99) "
								                         "Exceeded\n");
						}
						argIndex = TwoDigitFromChars(sv, primaryPos, subPos);
				}
				++primaryPos;
			}
		if( (primaryPos != std::string_view::npos && subPos != std::string_view::npos) ) {
				if( type == NestedFieldType::Width ) {
						specValues.nestedWidthArgPos = argIndex;
				} else {
						specValues.nestedPrecArgPos = argIndex;
					}
				sv.remove_prefix(subPos + 1);
				return true;
		} else {
				return false;
			}
	}

	// Enum Types: FillAlign, Sign, Alternate, ZeroPad, Locale, Width, Precision, Type
	ParseResult& LazyParser::Parse(std::string_view sv) {
		using enum TokenType;
		result.Reset();
		for( ;; ) {
				if( sv.size() == 0 ) return result;
				m_tokenType = Empty;
				FindBrackets(sv);
				if( !bracketResults.isValid ) {
						break;
				}
				auto startPos { bracketResults.beginPos };
				auto endPos { bracketResults.endPos };
				if( endPos - startPos == 0 ) break;
				auto argBracket { sv.substr(startPos + 1, (endPos - startPos)) };
				auto firstChar { argBracket[ 0 ] };

				if( firstChar == '}' ) {
						if( m_indexMode == IndexMode::manual ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						++argCounter;
						break;
				}

				if( firstChar == '{' ) {
						/*Handle Escaped Bracket*/
				} else if( (firstChar == ':' && IsDigit(argBracket[ 1 ])) || (IsDigit(firstChar)) ) {
						/*Handle Positional Args*/
						if( (m_indexMode == IndexMode::automatic) && (argCounter != 0) ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						m_indexMode = IndexMode::manual;
						if( !ParsePositionalField(argBracket, argCounter) ) {
								++argCounter;
						}
				} else if( firstChar == ':' && argBracket.size() > 2 ) {
						// ensuring size includes at least one flag, accounting for closing '}'
						argBracket.remove_prefix(1);
				} else {
						throw std::runtime_error("Error In Argument Field: Incorrect Formatting Detected\n");
					}

				if( HasFillAlignField(argBracket) ) {
						m_tokenType |= FillAlign;
				}

				if( HasSignField(argBracket) ) {
						m_tokenType |= Sign;
				}

				if( argBracket.size() > 2 && argBracket[ 0 ] == '#' ) {
						m_tokenType |= Alternate;
						argBracket.remove_prefix(1);
				}

				if( argBracket.size() > 2 && argBracket[ 0 ] == '0' ) {
						m_tokenType |= ZeroPad;
						argBracket.remove_prefix(1);
				}

				if( argBracket.size() > 2 && argBracket[ 0 ] == '{' ) {
						argBracket.remove_prefix(1);    // remove the '{'
						if( !HasValidNestedField(argBracket, NestedFieldType::Width, argCounter) ) {
								// clang-format off
								throw std::runtime_error("Error In Width Field: '{' Found With No Appropriately Matching '}'\n");
								// clang-format on
						}
						m_tokenType |= Width;
				} else if( argBracket.size() > 2 && IsDigit(argBracket[ 0 ]) ) {
						m_tokenType |= Width;
						argBracket.remove_prefix(1);
				}

				if( argBracket.size() > 2 && argBracket[ 0 ] == '.' ) {
						argBracket.remove_prefix(1);
						bool isPotentialNestedPrecision { argBracket.size() > 2 && argBracket[ 0 ] == '{' };
						if( isPotentialNestedPrecision ) {
								argBracket.remove_prefix(1);    // remove the '{'
								if( !HasValidNestedField(argBracket, NestedFieldType::Precision, argCounter) ) {
										// clang-format off
										throw std::runtime_error("Error In Precision Field: '{' Found With No Appropriately Matching '}'\n");
										// clang-format on
								}
								m_tokenType |= Precision;
						} else if( sv.size() > 2 && IsDigit(argBracket[ 0 ]) ) {
								m_tokenType |= Precision;
								argBracket.remove_prefix(1);
						}
				}

				if( argBracket.size() > 2 && sv[ 0 ] == 'L' ) {
						m_tokenType |= Locale;
						argBracket.remove_prefix(1);
				}

				if( argBracket.size() > 2 && IsAlpha(argBracket[ 0 ]) ) {
						// Would need to verify the type spec is valid for the arg type supplied
						m_tokenType |= Type;
				}

				break;
			}    // outtermost for(;;) loop
		FormatTokens();
		return result;
	}

}    // namespace serenity::lazy_parser
