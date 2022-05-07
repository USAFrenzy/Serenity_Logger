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

	int LazyParser::TwoDigitFromChars(std::string_view sv, const size_t& begin, const size_t& end) {
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
		m_tokenType |= TokenType::Positional;
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

	void serenity::lazy_parser::LazyParser::FindNestedBrackets(std::string_view sv, int& currentPos) {
		int pos { currentPos };
		auto size { sv.size() - 1 };
		for( ;; ) {
				// search for any nested fields
				if( pos > size ) break;
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
								if( pos > size ) break;
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

	void LazyParser::FindBrackets(std::string_view& sv) {
		bracketResults.Reset();
		int pos { 0 }, subPos { 0 };
		auto size { sv.size() - 1 };
		// This is mainly when setting the sv to the remainder value in the Parse function
		if( size != 0 && sv[ 0 ] == '\0' ) {
				sv.remove_prefix(1);
		}
		for( ;; ) {
				if( pos > size ) {
						bracketResults.isValid = false;
						break;
				}
				if( sv[ pos ] != '{' ) {
						++pos;
						continue;
				}
				bracketResults.beginPos = pos;
				subPos                  = pos + 1;
				auto ch { sv[ subPos ] };
				for( ;; ) {
						ch = sv[ subPos ];
						if( subPos > size ) bracketResults.isValid = false;
						if( (ch == ':' || ch == '.') && sv[ subPos + 1 ] == '{' ) {
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
		result.remainder       = std::move(sv.substr(subPos + 1, size));
		bracketResults.isValid = true;
	}

	bool LazyParser::VerifyFillAlignField(std::string_view& sv, size_t& currentPos, const size_t& bracketSize) {
		auto ch { sv[ currentPos ] };
		switch( ch ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default: specValues.align = Alignment::Empty; break;
			}

		if( specValues.align != Alignment::Empty ) {
				auto potentialFillChar { sv[ currentPos - 1 ] };

				if( potentialFillChar != '{' && potentialFillChar != '}' ) {
						specValues.fillCharacter = potentialFillChar;
				} else {
						// clang-format off
							throw std::runtime_error("Error In Fill Align Field: Alignment Option Specified Without A Fill Character\n");
						// clang-format on
					}

				++currentPos;
				if( currentPos < bracketSize ) {
						if( IsDigit(sv[ currentPos ]) ) {
								++currentPos;
								for( ;; ) {
										if( currentPos >= bracketSize ) break;
										if( !IsDigit(sv[ currentPos ]) ) break;
										++currentPos;
									}
						}
						std::from_chars(sv.data() + 2, sv.data() + currentPos, specValues.alignmentPadding);
						m_tokenType |= TokenType::AlignmentPadding;
				}
		}
		return (specValues.align != Alignment::Empty);
	}

	bool LazyParser::HasSignField(std::string_view& sv, const size_t& bracketSize) {
		if( bracketSize < 2 ) return false;    // this accounts for at least sign + '}'
		switch( sv[ 0 ] ) {
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

	void LazyParser::FormatPositionalToken() { }

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
		switch( m_tokenType ) {
				case AlignmentPadding: [[fallthrough]];
				case FillAlign: FormatFillAlignToken(); break;
				case Sign: FormatSignToken(); break;
				case Alternate: FormatAlternateToken(); break;
				case ZeroPad: FormatZeroPadToken(); break;
				case Locale: FormatLocaleToken(); break;
				case Width: FormatWidthToken(); break;
				case Precision: FormatPrecisionToken(); break;
				case Type: FormatTypeToken(); break;
				case CharAggregate: FormatCharAggregateToken(); break;
				case Custom: FormatCustomToken(); break;
				case Positional: FormatPositionalToken(); break;
				default: break;
			}
	}

	bool LazyParser::HasValidNestedField(std::string_view& sv, NestedFieldType type, size_t index) {
		size_t primaryPos { 0 }, subPos { 0 }, argIndex { index }, svSize(sv.size() - 1);
		char ch { sv[ 0 ] };
		for( ;; ) {
				ch = sv[ primaryPos ];
				if( primaryPos > svSize ) {
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
						subPos = primaryPos + 1;
						for( ;; ) {
								if( subPos > svSize ) {
										subPos = std::string_view::npos;
										break;
								}
								if( !IsDigit(sv[ subPos ]) ) break;
								++subPos;
							}
						if( subPos > (primaryPos + 3) ) {
								// clang-format off
								throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
								// clang-format on
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

				const auto& argBracketSize { argBracket.size() };
				size_t pos { 0 };
				/* Work In Progress On Shifting The Crap Ton Of  If/Else calls To A Simple For Loop Switch Case Iterative
				 * Approach */
				// const auto& argBracketSize { argBracket.size() };
				// size_t pos { 0 };
				// for( ;; ) {
				//		if( argBracketSize > 2 ) {
				//				auto ch { argBracket[ 0 ] };
				//				switch( ch ) {
				//						case '^': [[fallthrough]];
				//						case '<': [[fallthrough]];
				//						case '>':
				//							VerifyFillAlignField(argBracket, pos, argBracketSize);
				//							break;
				//						case '+': [[fallthrough]];
				//						case '-': [[fallthrough]];
				//						case ' ': break;
				//					}
				//		}
				//		++pos;
				//	}

				if( VerifyFillAlignField(argBracket, pos, argBracketSize) ) {
						m_tokenType |= FillAlign;
				}

				if( HasSignField(argBracket, argBracketSize) ) {
						m_tokenType |= Sign;
				}

				if( argBracketSize > 2 && argBracket[ 0 ] == '#' ) {
						m_tokenType |= Alternate;
						argBracket.remove_prefix(1);
				}

				if( argBracketSize > 2 && argBracket[ 0 ] == '0' ) {
						m_tokenType |= ZeroPad;
						argBracket.remove_prefix(1);
				}

				if( argBracketSize > 2 && argBracket[ 0 ] == '{' ) {
						argBracket.remove_prefix(1);    // remove the '{'
						if( !HasValidNestedField(argBracket, NestedFieldType::Width, argCounter) ) {
								// clang-format off
								throw std::runtime_error("Error In Width Field: '{' Found With No Appropriately Matching '}'\n");
								// clang-format on
						}
						m_tokenType |= Width;
				} else if( argBracketSize > 2 && IsDigit(argBracket[ 0 ]) ) {
						m_tokenType |= Width;
						argBracket.remove_prefix(1);
				}

				if( argBracketSize > 2 && argBracket[ 0 ] == '.' ) {
						argBracket.remove_prefix(1);
						bool isPotentialNestedPrecision { argBracketSize > 2 && argBracket[ 0 ] == '{' };
						if( isPotentialNestedPrecision ) {
								argBracket.remove_prefix(1);    // remove the '{'
								if( !HasValidNestedField(argBracket, NestedFieldType::Precision, argCounter) ) {
										// clang-format off
										throw std::runtime_error("Error In Precision Field: '{' Found With No Appropriately Matching '}'\n");
										// clang-format on
								}
								m_tokenType |= Precision;
						} else if( argBracketSize > 2 && IsDigit(argBracket[ 0 ]) ) {
								m_tokenType |= Precision;
								argBracket.remove_prefix(1);
						}
				}

				if( argBracketSize > 2 && sv[ 0 ] == 'L' ) {
						m_tokenType |= Locale;
						argBracket.remove_prefix(1);
				}

				if( argBracketSize > 2 && IsAlpha(argBracket[ 0 ]) ) {
						// Would need to verify the type spec is valid for the arg type supplied
						m_tokenType |= Type;
				}

				break;
			}    // outtermost for(;;) loop
		FormatTokens();
		return result;
	}

}    // namespace serenity::lazy_parser
