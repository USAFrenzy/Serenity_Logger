#include <serenity/Common.h>
#include <serenity/MessageDetails/LazyParser.h>

namespace serenity::lazy_parser {

	bool LazyParser::IsFlagSet(TokenType& tokenFlags, TokenType checkValue) {
		return (tokenFlags & checkValue) == checkValue;
	}

	void LazyParser::FindDigitEnd(std::string_view sv, size_t& start) {
		for( ;; ) {
				switch( sv[ start ] ) {
						case '0': [[fallthrough]];
						case '1': [[fallthrough]];
						case '2': [[fallthrough]];
						case '3': [[fallthrough]];
						case '4': [[fallthrough]];
						case '5': [[fallthrough]];
						case '6': [[fallthrough]];
						case '7': [[fallthrough]];
						case '8': [[fallthrough]];
						case '9': ++start; continue;
						default: break;
					}
				break;
			}
	}

	int LazyParser::UnCheckedDigitFromChars(std::string_view sv, const size_t& begin, const size_t& end) {
		// This is a really nasty way of doing this but hand-rolled to take in account INT_MAX of 2,147,483,647.
		auto Digit = [](const char& ch) {
			switch( ch ) {
					case '0': return 0; break;
					case '1': return 1; break;
					case '2': return 2; break;
					case '3': return 3; break;
					case '4': return 4; break;
					case '5': return 5; break;
					case '6': return 6; break;
					case '7': return 7; break;
					case '8': return 8; break;
					case '9': return 9; break;
					default: break;
				}
			unreachable();
		};

		switch( end - begin ) {
				case 0: break;
				case 1: return Digit(sv[ 0 ]); break;
				case 2: return Digit(sv[ 0 ]) * 10 + Digit(sv[ 1 ]); break;
				case 3: return Digit(sv[ 0 ]) * 100 + Digit(sv[ 1 ]) * 10 + Digit(sv[ 2 ]); break;
				case 4: return Digit(sv[ 0 ]) * 1'000 + Digit(sv[ 1 ]) * 100 + Digit(sv[ 2 ]) * 10 + Digit(sv[ 3 ]); break;
				case 5:
					return Digit(sv[ 0 ]) * 10'000 + Digit(sv[ 1 ]) * 1'000 + Digit(sv[ 2 ]) * 100 + Digit(sv[ 3 ]) * 10 +
					       Digit(sv[ 4 ]);
					break;
				case 6:
					return Digit(sv[ 0 ]) * 100'000 + Digit(sv[ 1 ]) * 10'000 + Digit(sv[ 2 ]) * 1'000 +
					       Digit(sv[ 3 ]) * 100 + Digit(sv[ 4 ]) * 10 + Digit(sv[ 5 ]);
					break;
				case 7:
					return Digit(sv[ 0 ]) * 1'000'000 + Digit(sv[ 1 ]) * 100'000 + Digit(sv[ 2 ]) * 10'000 +
					       Digit(sv[ 3 ]) * 1'000 + Digit(sv[ 4 ]) * 100 + Digit(sv[ 5 ]) * 10 + Digit(sv[ 6 ]);
					break;
				case 8:
					return Digit(sv[ 0 ]) * 10'000'000 + Digit(sv[ 1 ]) * 1'000'000 + Digit(sv[ 2 ]) * 100'000 +
					       Digit(sv[ 3 ]) * 10'000 + Digit(sv[ 4 ]) * 1'000 + Digit(sv[ 5 ]) * 100 + Digit(sv[ 6 ]) * 10 +
					       Digit(sv[ 7 ]);
					break;
				case 9:
					return Digit(sv[ 0 ]) * 100'000'000 + Digit(sv[ 1 ]) * 10'000'000 + Digit(sv[ 2 ]) * 1'000'000 +
					       Digit(sv[ 3 ]) * 100'000 + Digit(sv[ 4 ]) * 10'000 + Digit(sv[ 5 ]) * 1'000 +
					       Digit(sv[ 6 ]) * 100 + Digit(sv[ 7 ]) * 10 + Digit(sv[ 8 ]);
					break;
				case 10:
					if( sv[ 0 ] > '2' ) break;
					if( sv[ 0 ] == '2' ) {
							if( sv[ 1 ] > '1' ) break;
							if( sv[ 2 ] > '4' ) break;
							if( sv[ 3 ] > '7' ) break;
							if( sv[ 4 ] > '4' ) break;
							if( sv[ 5 ] > '8' ) break;
							if( sv[ 6 ] > '3' ) break;
							if( sv[ 7 ] > '6' ) break;
							if( sv[ 8 ] > '4' ) break;
							if( sv[ 9 ] > '7' ) break;
					}
					return Digit(sv[ 0 ]) * 1'000'000'000 + Digit(sv[ 1 ]) * 100'000'000 + Digit(sv[ 2 ]) * 10'000'000 +
					       Digit(sv[ 3 ]) * 1'000'000 + Digit(sv[ 4 ]) * 100'000 + Digit(sv[ 5 ]) * 10'000 +
					       Digit(sv[ 6 ]) * 1'000 + Digit(sv[ 7 ]) * 100 + Digit(sv[ 8 ]) * 10 + Digit(sv[ 9 ]);
					break;
				default: break;
			}
		throw std::runtime_error("Error In UnCheckedDigitFromChars(): Char Representation Is Too Large To Fit In An "
		                         "Integer Type\n");
		unreachable();
	}

	bool LazyParser::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		FindDigitEnd(sv, start);
		auto size { sv.size() };

		if( start <= 2 ) {
				argIndex = UnCheckedDigitFromChars(sv, 0, start);
				sv.remove_prefix(start);
		} else {
				throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
			}
		m_tokenType |= TokenType::Positional;
		specValues.argPosition = argIndex;
		return true;
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
						if( (ch == ':' || ch == '.') && sv[ static_cast<size_t>(subPos) + static_cast<size_t>(1) ] == '{' )
						{
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
		result.remainder       = std::move(sv.substr(static_cast<size_t>(subPos) + static_cast<size_t>(1), size));
		bracketResults.isValid = true;
	}

	bool LazyParser::VerifyFillAlignField(std::string_view& sv, size_t& currentPos, const size_t& bracketSize) {
		auto ch { sv[ currentPos ] };
		size_t offset { 0 };

		switch( ch ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default: specValues.align = Alignment::Empty; break;
			}

		if( specValues.align != Alignment::Empty ) {
				++offset;
				auto potentialFillChar { sv[ currentPos - 1 ] };

				if( potentialFillChar != '{' && potentialFillChar != '}' ) {
						specValues.fillCharacter = potentialFillChar;
						++offset;
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
						if( specValues.alignmentPadding != 0 ) offset += specValues.alignmentPadding > 9 ? 2 : 1;
						// sv.remove_prefix(offset);
						m_tokenType |= TokenType::AlignmentPadding;
				}
		}
		return (specValues.align != Alignment::Empty);
	}

	bool LazyParser::VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		switch( sv[ currentPosition ] ) {
				case '+': specValues.signType = Sign::Plus; break;
				case '-': specValues.signType = Sign::Minus; break;
				case ' ': specValues.signType = Sign::Space; break;
				default: specValues.signType = Sign::Empty; break;
			}
		if( specValues.signType == Sign::Empty ) return false;
		sv.remove_prefix(currentPosition + static_cast<size_t>(1));
		m_tokenType |= TokenType::Sign;
		return true;
	}

	bool LazyParser::VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		// This requires a check for integer/floating point types, which is implemented in
		// ArgContainer at the moment but not over here
		// Integers use 0b/0B for binary, 0 for ocatal and 0x/0X for hex
		// For floating point, the result contains a decimal point irregardless if the value contains
		// decimal points or not where the default is to present decimal points only when present and
		// g/G doesn't remove trailing zeros (This is by the spec being followed)
		return false;
	}

	bool LazyParser::VerifyZeroPadField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, int currentArgIndex) {
		specValues.fillCharacter = '0';
		sv.remove_prefix(1);
		auto nextCh { sv[ currentPosition ] };
		if( nextCh == '{' ) {
				// Handle zeroPadded field followed by some nested arg value
				sv.remove_prefix(1);    // remove '{'
				specValues.nestedWidthArgPos = UnCheckedDigitFromChars(sv, currentPosition, ((bracketSize % 2) + 1));
				sv.remove_prefix(currentPosition + static_cast<size_t>(1));
		} else if( IsDigit(nextCh) ) {
				// Handle zeroPadded field followed by some integer literal
				specValues.zeroPadAmount = UnCheckedDigitFromChars(sv, currentPosition, ((bracketSize % 2) + 1));
				sv.remove_prefix(currentPosition + specValues.nestedWidthArgPos > 9 ? 2 : 1);
		} else {
				if( nextCh != ' ' && nextCh != '}' ) {
						// If the next char present after '0' isn't '{', an integer literal, a space delimiter
						// or a closing brace, then we have an invalid specifier in this field
						throw std::runtime_error("Error Detected In Argument Formatting Field: A '0' Was Found With No "
						                         "Accompanying Brace, Integer Literal, Space Delimiter, Or Closing "
						                         "Brace\n");
				}
			}
		return true;
	}

	bool LazyParser::VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		// TODO: MIRROR SIMILAR PARSE VERIFICATION AS PRECISION

		sv.remove_prefix(1);    // place_holder
		return false;
	}

	bool LazyParser::VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto size { sv.size() };
		++currentPosition;
		auto ch { sv[ currentPosition ] };
		if( m_indexMode == IndexMode::manual && size > 3 && ch == '{' ) {
				++currentPosition;
				for( ;; ) {
						if( currentPosition >= (size - 1) ) {
								// clang-format off
							throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						ch = sv[ currentPosition ];
						if( IsDigit(ch) ) {
								specValues.nestedPrecArgPos = UnCheckedDigitFromChars(sv, currentPosition, size);
								++currentPosition;
								// searching to make sure no other specs are found
								for( ;; ) {
										if( currentPosition >= size - 1 ) break;
										ch = sv[ currentPosition ];
										if( ch == '}' ) break;
										if( ch != ' ' ) {
												continue;
										} else {
												// clang-format off
												throw std::runtime_error("Error In Precision Field: Only Positional Argument Is A Valid Specifier\n");
												// clang-format on
											}
										++currentPosition;
									}
								break;
						}
						++currentPosition;
					}
		} else {
				++currentPosition;
				for( ;; ) {
						if( currentPosition >= (size - 1) ) break;
						ch = sv[ currentPosition ];
						if( IsDigit(ch) ) {
								// clang-format off
									throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						if( ch != ' ' && ch != '}' ) {
								// clang-format off
							throw std::runtime_error("Error In Precision Field: Only Positional Argument Is A Valid Specifier\n");
								// clang-format on
						}
						++currentPosition;
					}
			}
		// sv.remove_prefix(currentPosition + 1);
		return true;
	}

	bool LazyParser::VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		if( bracketSize > 2 ) {
				if( sv[ currentPosition + static_cast<size_t>(1) ] == '}' ) {
						std::string tmp { 1, '}' };
						tmp.append(result.remainder.data(), result.remainder.size());
						result.remainder = std::move(tmp);
						return true;
				}
		}
		return false;
	}

	bool LazyParser::VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		sv.remove_prefix(1);    // place_holder
		return false;
	}

	bool LazyParser::VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		sv.remove_prefix(1);    // place_holder
		return false;
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

				// remove empty whitespace
				if( argBracket[ 0 ] == ' ' ) {
						argBracket.remove_prefix(1);
						auto pos { 0 };
						for( ;; ) {
								auto ch = argBracket[ pos ];
								if( ch != ' ' ) break;
								++pos;
							}
				}

				// Nothing to Parse - just a simple substitution
				if( argBracket[ 0 ] == '}' ) {
						if( m_indexMode == IndexMode::manual ) {
								// clang-format off
						throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						++argCounter;
						break;
				}

				/*Handle Escaped Bracket*/
				if( argBracket[ 0 ] == '{' ) {
						std::string tmp { result.preTokenStr };
						tmp.append(1, '{');
						result.preTokenStr = std::move(tmp);
				}

				/*Handle Positional Args*/
				if( IsDigit(argBracket[ 0 ]) ) {
						if( (m_indexMode == IndexMode::automatic) && (argCounter != 0) ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						m_indexMode = IndexMode::manual;
						size_t pos { 0 };
						if( !ParsePositionalField(argBracket, argCounter, pos) ) {
								// clang-format off
								// ensuring size includes at least one flag, accounting for closing '}'if there are more fields to parse (denoted by the ':')
								if( argBracket.size() < 3 || argBracket[ 0 ] != ':' ) {
										break; // nothing more to do
								} else if( (argBracket.size() < 3 && argBracket[ 0 ] == ':') || ((argBracket.size() > 3 || (argBracket[ 0 ] != ':' && argBracket[ 0 ] != ' '))) )  {
									// Eliminate any whitespace just in case there is more to parse and user was just being dumb with specs	
									if( argBracket.size() > 3 && argBracket[ 0 ] == ' ' ) {
												auto pos { 0 };
												auto ch { argBracket[ pos ] };
												for( ;; ) {
														if( ch != ' ' ) break;
														++pos;
													}
												if( argBracket.size() > 2 && argBracket[ 0 ] != ':' && argBracket[ 0 ] != '}' ) {
													// No saving the user here -> throw hands up
													throw std::runtime_error("Error In Argument Field: Incorrect Formatting Detected\n");
														// clang-format on
												}
										}
								}
						}
						++argCounter;
				}

				if( argBracket[ 0 ] == ':' && argBracket.size() > 2 ) {
						argBracket.remove_prefix(1);
				} else {
					}

				// Work In Progress On Shifting The Crap Ton Of  If/Else calls To A
				// Simple For Loop Switch Case Iterative Approach
				size_t pos { 0 };
				for( ;; ) {
						auto argBracketSize { argBracket.size() };
						if( (argBracketSize <= 1) || (pos >= argBracketSize - 1) ) break;
						auto ch { argBracket[ pos ] };
						switch( ch ) {
								case '^': [[fallthrough]];
								case '<': [[fallthrough]];
								case '>': VerifyFillAlignField(argBracket, pos, argBracketSize); break;
								case '+': [[fallthrough]];
								case '-': [[fallthrough]];
								case ' ': VerifySignField(argBracket, pos, argBracketSize); break;
								case '#': VerifyAltField(argBracket, pos, argBracketSize); break;
								case '0': VerifyZeroPadField(argBracket, pos, argBracketSize, argCounter); break;
								case '{': VerifyWidthField(argBracket, pos, argBracketSize); break;
								case '.': VerifyPrecisionField(argBracket, pos, argBracketSize); break;
								case 'L': VerifyLocaleField(argBracket, pos, argBracketSize); break;
								case '}': VerifyEscapedBracket(argBracket, pos, argBracketSize); break;
								default:
									if( IsAlpha(ch) ) {
											VerifyTypeSpec(argBracket, pos, argBracketSize);
									}
									break;    // still no custom support
							}
						++pos;
					}

				// if( argBracketSize > 2 && argBracket[ 0 ] == '#' ) {
				//		m_tokenType |= Alternate;
				//		argBracket.remove_prefix(1);
				//}

				// if( argBracketSize > 2 && argBracket[ 0 ] == '{' ) {
				//		argBracket.remove_prefix(1);    // remove the '{'
				//		if( !HasValidNestedField(argBracket, NestedFieldType::Width, argCounter) ) {
				//				// clang-format off
				//				throw std::runtime_error("Error In Width Field: '{' Found With No
				// Appropriately
				// Matching '}'\n");
				//				// clang-format on
				//		}
				//		m_tokenType |= Width;
				//} else if( argBracketSize > 2 && IsDigit(argBracket[ 0 ]) ) {
				//		m_tokenType |= Width;
				//		argBracket.remove_prefix(1);
				//}

				// if( argBracketSize > 2 && sv[ 0 ] == 'L' ) {
				//		m_tokenType |= Locale;
				//		argBracket.remove_prefix(1);
				//}

				// if( argBracketSize > 2 && IsAlpha(argBracket[ 0 ]) ) {
				//		// Would need to verify the type spec is valid for the arg type supplied
				//		m_tokenType |= Type;
				//}

				break;
			}    // outtermost for(;;) loop
		return result;
	}

}    // namespace serenity::lazy_parser
