#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::lazy_parser {

	bool ArgFormatter::IsFlagSet(TokenType& tokenFlags, TokenType checkValue) {
		return (tokenFlags & checkValue) == checkValue;
	}

	size_t ArgFormatter::FindDigitEnd(std::string_view sv, size_t start) {
		// This was somehow faster than doing a call to IsDigit and then incrementing
		// the position (as far as I'm aware, this basically should be very similar
		// instructions?)
		auto originalValue { start };
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
				if( start == originalValue ) {
						start = std::string_view::npos;
				}
				break;
			}
		return start;
	}

	static constexpr std::string_view maxValue { "2147483647" };

	static int Digit(const char& ch) {
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

	// Up to user to define valid range
	int ArgFormatter::NoCheckIntFromChars(std::string_view sv, const size_t& begin, const size_t& end) {
		// This is a really nasty way of doing this but hand-rolled to take in account INT_MAX of 2,147,483,647.
		// Haven't tested larger numbers as I have only used a max of two digits as of right now, but for at least
		// only 1 or 2 digits, this was faster than std::from_chars and eliminated the need for buffer construction
		// every time it was called
		auto i { begin };
		switch( end - begin ) {
				case 0: break;
				case 1: return Digit(sv[ i ]); break;
				case 2: return Digit(sv[ i ]) * 10 + Digit(sv[ ++i ]); break;
				case 3: return Digit(sv[ i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]); break;
				case 4: return Digit(sv[ i ]) * 1'000 + Digit(sv[ ++i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]); break;
				case 5:
					return Digit(sv[ i ]) * 10'000 + Digit(sv[ ++i ]) * 1'000 + Digit(sv[ ++i ]) * 100 +
					       Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				case 6:
					return Digit(sv[ i ]) * 100'000 + Digit(sv[ ++i ]) * 10'000 + Digit(sv[ ++i ]) * 1'000 +
					       Digit(sv[ ++i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				case 7:
					return Digit(sv[ i ]) * 1'000'000 + Digit(sv[ ++i ]) * 100'000 + Digit(sv[ ++i ]) * 10'000 +
					       Digit(sv[ ++i ]) * 1'000 + Digit(sv[ ++i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				case 8:
					return Digit(sv[ i ]) * 10'000'000 + Digit(sv[ ++i ]) * 1'000'000 + Digit(sv[ ++i ]) * 100'000 +
					       Digit(sv[ ++i ]) * 10'000 + Digit(sv[ ++i ]) * 1'000 + Digit(sv[ ++i ]) * 100 +
					       Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				case 9:
					return Digit(sv[ i ]) * 100'000'000 + Digit(sv[ ++i ]) * 10'000'000 + Digit(sv[ ++i ]) * 1'000'000 +
					       Digit(sv[ ++i ]) * 100'000 + Digit(sv[ ++i ]) * 10'000 + Digit(sv[ ++i ]) * 1'000 +
					       Digit(sv[ ++i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				case 10:
					if( sv[ 0 ] == '2' ) {
							for( auto& ch: sv ) {
									for( auto& mCh: maxValue ) {
											if( ch > mCh ) {
													// clang-format off
													throw std::runtime_error("Error In NoCheckIntFromChars(): Char "
														                                              "Representation Is Too Large To Fit In An Integer\n");
													// clang-format on
											}
										}
								}
					}
					return Digit(sv[ i ]) * 1'000'000'000 + Digit(sv[ ++i ]) * 100'000'000 + Digit(sv[ ++i ]) * 10'000'000 +
					       Digit(sv[ ++i ]) * 1'000'000 + Digit(sv[ ++i ]) * 100'000 + Digit(sv[ ++i ]) * 10'000 +
					       Digit(sv[ ++i ]) * 1'000 + Digit(sv[ ++i ]) * 100 + Digit(sv[ ++i ]) * 10 + Digit(sv[ ++i ]);
					break;
				default: break;
			}

		unreachable();
	}

	bool ArgFormatter::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		if( size_t endPos = FindDigitEnd(sv, start) != std::string_view::npos ) {
				argIndex = NoCheckIntFromChars(sv, start, endPos);
				sv.remove_prefix(endPos);
				if( (endPos - start) >= 2 ) {
						throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
				}
		}
		m_tokenType |= TokenType::Positional;
		specValues.argPosition = argIndex;
		return true;
	}

	void serenity::lazy_parser::ArgFormatter::FindNestedBrackets(std::string_view sv, int& currentPos) {
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
										auto nextCh { sv[ pos + static_cast<size_t>(1) ] };
										if( nextCh == '}' ) {
												currentPos = ++pos;
												return;
										}
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

	void ArgFormatter::FindBrackets(std::string_view& sv) {
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
						if( ((ch == ':' || ch == '.') &&
						     sv[ static_cast<size_t>(subPos) + static_cast<size_t>(1) ] == '{') ||
						    (ch == '{') ) {
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
		bracketResults.isValid = true;
	}

	bool ArgFormatter::VerifyFillAlignField(std::string_view& sv, size_t& currentPos, const size_t& bracketSize) {
		auto ch { sv[ currentPos ] };

		switch( ch ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default: specValues.align = Alignment::Empty; break;
			}

		if( specValues.align != Alignment::Empty ) {
				++currentPos;
				auto potentialFillChar { sv[ currentPos - 1 ] };

				if( potentialFillChar != '{' && potentialFillChar != '}' ) {
						specValues.fillCharacter = potentialFillChar;
				} else {
						// clang-format off
							throw std::runtime_error("Error In Fill Align Field: Alignment Option Specified Without A Fill Character\n");
						// clang-format on
					}

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
				// increment position offsets
				if( specValues.fillCharacter != '\0' ) ++currentPos;
				if( specValues.align != Alignment::Empty ) ++currentPos;
				if( specValues.alignmentPadding != 0 ) currentPos += specValues.alignmentPadding > 9 ? 2 : 1;
		}
		return (specValues.align != Alignment::Empty);
	}

	bool ArgFormatter::VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		switch( sv[ currentPosition ] ) {
				case '+': specValues.signType = Sign::Plus; break;
				case '-': specValues.signType = Sign::Minus; break;
				case ' ': specValues.signType = Sign::Space; break;
				default: specValues.signType = Sign::Empty; break;
			}
		if( specValues.signType == Sign::Empty ) {
				return false;
		}
		++currentPosition;
		m_tokenType |= TokenType::Sign;
		return true;
	}

	static bool IsValidAltType(experimental::msg_details::SpecType argType) {
		using SpecType = experimental::msg_details::SpecType;
		switch( argType ) {
				case SpecType::CharType: [[fallthrough]]; /*May not be supported?*/
				case SpecType::IntType: [[fallthrough]];
				case SpecType::U_IntType: [[fallthrough]];
				case SpecType::DoubleType: [[fallthrough]];
				case SpecType::FloatType: [[fallthrough]];
				case SpecType::LongDoubleType: [[fallthrough]];
				case SpecType::LongLongType: [[fallthrough]];
				case SpecType::U_LongLongType: return true;
				default: return false; break;
			}
	}

	static bool IsValidAltTypeSpec(const char& spec) {
		switch( spec ) {
				case 'b': [[fallthrough]];
				case 'B': [[fallthrough]];
				case 'o': [[fallthrough]];
				case 'x': [[fallthrough]];
				case 'X': return true;
				default: return false; break;
			}
	}

	static const char* AlternatePreFormatChars(const char& spec) {
		using SpecType = experimental::msg_details::SpecType;

		switch( spec ) {
				case 'b': return "0b";
				case 'B': return "0B";
				case 'o': return "0";
				case 'x': return "0x";
				case 'X': return "0X";
				default: break;
			}
		return "\0";
	}

	bool ArgFormatter::VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, int argIndex) {
		auto specStorage { argStorage.SpecTypesCaptured() };
		auto cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { argIndex <= cSize ? specStorage[ argIndex ] : SpecType::MonoType };
		if( !IsValidAltType(argType) ) {
				throw std::runtime_error("Error In Alternative Format Spec '#': Argument Type Is Not An Integer Or Floating "
				                         "Point Type\n");
		}
		++currentPosition;    // advance past '#'
		auto spec { sv[ currentPosition ] };
		for( ;; ) {
				if( currentPosition >= bracketSize ) break;
				spec = sv[ currentPosition ];
				if( spec == '}' ) break;
				if( spec == ' ' ) {
						++currentPosition;
						continue;
				}
				if( spec == '0' ) {
						VerifyZeroPadField(sv, currentPosition, bracketSize, argIndex);
						break;
				} else if( spec == '{' ) {
						VerifyWidthField(sv, currentPosition, bracketSize);
						break;
				} else if( spec == '.' ) {
						VerifyPrecisionField(sv, currentPosition, bracketSize);
						break;
				} else if( spec == 'L' ) {
						VerifyLocaleField(sv, currentPosition, bracketSize);
						break;
				} else if( IsValidAltTypeSpec(spec) ) {
						specValues.hasAltForm = true;
						specValues.preAltForm = AlternatePreFormatChars(spec);
				} else {
						std::string throwMsg { "Error In Alternative Format Spec '#': Argument Spec: \"" };
						throwMsg.append(1, spec);
						throwMsg.append("\" Is Not A Valid Type Spec For This Field\n");
						throw std::runtime_error(std::move(throwMsg));
					}
			}
		return true;
	}

	bool ArgFormatter::VerifyZeroPadField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, int currentArgIndex) {
		specValues.fillCharacter = '0';
		++currentPosition;
		auto nextCh { sv[ currentPosition ] };
		if( nextCh == '{' ) {
				// Handle zeroPadded field followed by some nested arg value being the width value
				++currentPosition;
				specValues.nestedWidthArgPos = NoCheckIntFromChars(sv, currentPosition, FindDigitEnd(sv, currentPosition));
				++currentPosition;
		} else if( IsDigit(nextCh) ) {
				// Handle zeroPadded field followed by some integer literal being the width value
				specValues.zeroPadAmount = NoCheckIntFromChars(sv, currentPosition, FindDigitEnd(sv, currentPosition));
				currentPosition += specValues.nestedWidthArgPos > 9 ? 2 : 1;
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

	static std::string_view NestedFieldTypeStr(NestedFieldType type) {
		return type == NestedFieldType::Prec ? "Precision\0" : "Width\0";
	}

	bool ArgFormatter::VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type) {
		char ch { sv[ currentPosition ] };
		for( ;; ) {
				if( currentPosition >= (bracketSize - 1) ) {
						// clang-format off
				throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
						// clang-format on
				}
				ch = sv[ currentPosition ];
				if( IsDigit(ch) ) {
						// clang-format off
						if( type == NestedFieldType::Width ) {
								specValues.nestedWidthArgPos = NoCheckIntFromChars(sv, currentPosition, FindDigitEnd(sv, currentPosition));
								m_tokenType |= TokenType::Width;
						} else {
								specValues.nestedPrecArgPos = NoCheckIntFromChars(sv, currentPosition, FindDigitEnd(sv, currentPosition));
								m_tokenType |= TokenType::Precision;
							}
						// clang-format on
						++currentPosition;
						// searching to make sure no other specs are found
						for( ;; ) {
								if( currentPosition >= bracketSize - 1 ) break;
								ch = sv[ currentPosition ];
								if( ch == '}' ) break;
								if( ch != ' ' ) {
										continue;
								} else {
										// clang-format off
										std::string throwMsg { "Error In Nested Bracket For \"" };
										throwMsg.append(NestedFieldTypeStr(type));
										throwMsg.append("\": Only A Positional Argument Is A Valid Specifier For Nested Specifiers\n");
										throw std::runtime_error(std::move(throwMsg));
										// clang-format on
									}
								++currentPosition;
							}
						break;
				}
				++currentPosition;
			}
		return true;
	}

	bool ArgFormatter::VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto ch { sv[ currentPosition ] };
		if( m_indexMode == IndexMode::manual && bracketSize > 3 && ch == '{' ) {
				++currentPosition;
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Width);
		} else {
				++currentPosition;
				for( ;; ) {
						if( currentPosition >= (bracketSize - 1) ) break;
						ch = sv[ currentPosition ];
						if( IsDigit(ch) ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						if( ch != ' ' && ch != '}' ) {
								// clang-format off
								throw std::runtime_error("Error In Width Field: Only Positional Argument Is A Valid Specifier\n");
								// clang-format on
						}
						++currentPosition;
					}
			}
		return true;
	}

	bool ArgFormatter::VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto size { sv.size() };
		++currentPosition;
		auto ch { sv[ currentPosition ] };
		if( m_indexMode == IndexMode::manual && size > 3 && ch == '{' ) {
				++currentPosition;
				return VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Prec);
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
		return true;
	}

	bool ArgFormatter::VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
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

	bool ArgFormatter::VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		++currentPosition;
		return false;
	}

	static bool IsAllowedSpec(const char& ch) {
		switch( ch ) {
				case 'a': [[fallthrough]];
				case 'A': [[fallthrough]];
				case 'b': [[fallthrough]];
				case 'B': [[fallthrough]];
				case 'c': [[fallthrough]];
				case 'd': [[fallthrough]];
				case 'e': [[fallthrough]];
				case 'E': [[fallthrough]];
				case 'f': [[fallthrough]];
				case 'F': [[fallthrough]];
				case 'g': [[fallthrough]];
				case 'G': [[fallthrough]];
				case 'o': [[fallthrough]];
				case 'p': [[fallthrough]];
				case 's': [[fallthrough]];
				case 'x': [[fallthrough]];
				case 'X': return true;
				default: break;
			}
		return false;
	}

	bool ArgFormatter::VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec,
	                                  int argIndex) {
		auto specStorage { argStorage.SpecTypesCaptured() };
		auto cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { argIndex <= cSize ? specStorage[ argIndex ] : SpecType::MonoType };
		if( argStorage.VerifySpec(argType, spec) ) {
				++currentPosition;
				m_tokenType |= TokenType::Type;
				specValues.typeSpec = spec;
				return true;
		} else {
				// right now this just throws, but, in the event that I change how error
				// handling is done - this would be where that logic would go
				return false;
			}
	}

	void ArgFormatter::VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize) {
		// Work In Progress On Shifting The Crap Ton Of  If/Else calls To A
		// Simple For Loop Switch Case Iterative Approach
		if( bracketSize <= 1 ) return;
		if( start >= bracketSize - 1 ) return;
		auto ch { sv[ start ] };
		switch( ch ) {
				case '^': [[fallthrough]];
				case '<': [[fallthrough]];
				case '>': VerifyFillAlignField(sv, start, bracketSize); break;
				case '+': [[fallthrough]];
				case '-': [[fallthrough]];
				case ' ': VerifySignField(sv, start, bracketSize); break;
				case '#': VerifyAltField(sv, start, bracketSize, argCounter); break;
				case '0': VerifyZeroPadField(sv, start, bracketSize, argCounter); break;
				case '{': VerifyWidthField(sv, start, bracketSize); break;
				case '.': VerifyPrecisionField(sv, start, bracketSize); break;
				case 'L': VerifyLocaleField(sv, start, bracketSize); break;
				case '}': VerifyEscapedBracket(sv, start, bracketSize); break;
				default:
					if( IsAlpha(ch) && IsAllowedSpec(ch) ) {
							VerifyTypeSpec(sv, start, bracketSize, ch, argCounter);
					}
					break;    // still no custom support
			}
		++start;
	}
	/************************ Taken From https://en.cppreference.com/w/cpp/utility/format/formatter ***********************
	        fill-and-align is an optional fill character (which can be any character other than { or }), followed by one of the
	        align options <, >, ^. The meaning of align options is as follows:

	        <: Forces the field to be aligned to the start of the available space. This is the default when a non-integer
	           non-floating-point presentation type is used.
	        >: Forces the field to be aligned to the end of the available space. This is the default when an integer or
	           floating-point presentation type is used.
	        ^: Forces the field to be centered within the available space by inserting [n/2] characters before and [n/2]
	           characters after the value, where n is the total number of fill characters to insert.

	        char c = 120;
	        auto s0 = std::format("{:6}", 42);      // value of s0 is "    42"
	        auto s1 = std::format("{:6}", 'x');       // value of s1 is "x     "
	        auto s2 = std::format("{:*<6}", 'x');  // value of s2 is "x*****"
	        auto s3 = std::format("{:*>6}", 'x');  // value of s3 is "*****x"
	        auto s4 = std::format("{:*^6}", 'x');   // value of s4 is "**x***"
	        auto s5 = std::format("{:6d}", c);       // value of s5 is "   120"
	        auto s6 = std::format("{:6}", true);   // value of s6 is "true  "
	**********************************************************************************************************************/
	void ArgFormatter::FormatFillAlignToken() { }

	void ArgFormatter::FormatPositionalToken() { }
	/************************ Taken From https://en.cppreference.com/w/cpp/utility/format/formatter ***********************
	        The sign option can be one of following:

	        +: Indicates that a sign should be used for both non-negative and negative numbers. The + sign is inserted before the
	        output value for non-negative numbers.

	        -: Indicates that a sign should be used for negative numbers only (this is the default behavior).

	        space: Indicates that a leading space should be used for non-negative numbers, and a minus sign for negative numbers.
	        Negative zero is treated as a negative number.

	        The sign option applies to floating-point infinity and NaN.

	        double inf = std::numeric_limits<double>::infinity();
	        double nan = std::numeric_limits<double>::quiet_NaN();
	        auto s0 = std::format("{0:},{0:+},{0:-},{0: }", 1);   // value of s0 is "1,+1,1, 1"
	        auto s1 = std::format("{0:},{0:+},{0:-},{0: }", -1);  // value of s1 is "-1,-1,-1,-1"
	        auto s2 = std::format("{0:},{0:+},{0:-},{0: }", inf); // value of s2 is "inf,+inf,inf, inf"
	        auto s3 = std::format("{0:},{0:+},{0:-},{0: }", nan); // value of s3 is "nan,+nan,nan, nan"
	**********************************************************************************************************************/
	void ArgFormatter::FormatSignToken() { }

	// Integers use 0b/0B for binary, 0 for octal and 0x/0X for hex
	// For floating point, the result contains a decimal point irregardless if the value contains
	// decimal points or not where the default is to only use decimal points only when present and
	// g/G doesn't remove trailing zeros (This is by the spec being followed)
	void ArgFormatter::FormatAlternateToken() { }

	void ArgFormatter::FormatZeroPadToken() { }

	void ArgFormatter::FormatLocaleToken() { }

	void ArgFormatter::FormatWidthToken() { }

	void ArgFormatter::FormatPrecisionToken() { }

	void ArgFormatter::FormatTypeToken() { }

	void ArgFormatter::FormatCharAggregateToken() { }

	void ArgFormatter::FormatCustomToken() { }

	void ArgFormatter::FormatTokens() {
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
	ParseResult& ArgFormatter::Parse(std::string_view sv) {
		using enum TokenType;
		result.Reset();
		if( sv.size() == 0 ) return result;
		m_tokenType = Empty;
		FindBrackets(sv);
		if( !bracketResults.isValid ) {
				return result;
		}
		auto startPos { bracketResults.beginPos };
		auto endPos { bracketResults.endPos };
		if( endPos - startPos == 0 ) return result;
		auto argBracket { sv.substr(startPos + 1, (endPos - startPos)) };
		result.preTokenStr = std::move(sv.substr(0, startPos));
		result.remainder   = std::move(sv.substr(endPos + 1, sv.size()));

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
				return result;
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
										return result; // nothing more to do
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

		if( argBracket.size() < 3 ) return result;
		if( argBracket[ 0 ] == ':' ) {
				argBracket.remove_prefix(1);
		} else if( argBracket[ 0 ] == ' ' ) {
				size_t pos { 1 };
				for( ;; ) {
						if( argBracket[ pos ] == ' ' ) ++pos;
						break;
					}
				argBracket.remove_prefix(pos);
				if( argBracket[ 0 ] != '}' )
					throw std::runtime_error("Error In Argument Field: Incorrect Formatting Detected - Too Much White Space "
					                         "Detected Before Possible Specifier\n");
		}

		// verify whats left of the bracket
		size_t pos { 0 }, argBracketSize { argBracket.size() };
		for( ;; ) {
				if( pos >= argBracketSize - 1 ) break;
				VerifyArgumentBracket(argBracket, pos, argBracketSize);
			}

		/***************** If/Else calls that still need to be translated over *****************/
		//
		// if( argBracketSize > 2 && sv[ 0 ] == 'L' ) {
		//		m_tokenType |= Locale;
		//		argBracket.remove_prefix(1);
		//}
		//*******************************And Parsing A Custom Flag*******************************
		/***************************************************************************************/
		return result;
	}

}    // namespace serenity::lazy_parser
