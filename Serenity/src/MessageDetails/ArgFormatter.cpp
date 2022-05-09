#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	bool ArgFormatter::IsFlagSet(TokenType checkValue) {
		return (m_tokenType & checkValue) == checkValue;
	}

	size_t ArgFormatter::FindDigitEnd(std::string_view sv, size_t start) {
		// This was somehow faster than doing a call to IsDigit and then incrementing
		// the position (as far as I'm aware, this basically should be very similar
		// instructions?)
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
				default: return -1; break;
			}
	};

	// Up to user to define valid range
	int ArgFormatter::NoCheckIntFromChars(std::string_view sv, const size_t& begin, const size_t& end) {
		// This is a really nasty way of doing this but hand-rolled to take in account INT_MAX of 2,147,483,647.
		// NOTE: Tested up to 7 digits at the moment.This function is roughly 50 ns slower than std::from_chars();
		//       this is faster with only one digit but after two digits, std::from_chars() wins out marginally
		//       every time except for the case of VerifyPositionalField() -> std::from_chars was much faster than
		//       this call and reduced overall timing by ~20-25 ns.
		//       Therefore, keeping this function for calls in other functions where it happens to be faster than
		//       the call to std::from_chars()
		auto i { begin };
		switch( end - begin ) {
				case 0: break;
				case 1: return Digit(sv[ i ]); break;
				case 2: return Digit(sv[ i ]) * 10 + Digit(sv[ i + 1 ]); break;
				case 3: return Digit(sv[ i ]) * 100 + Digit(sv[ i + 1 ]) * 10 + Digit(sv[ i + 2 ]); break;
				case 4:
					return Digit(sv[ i ]) * 1'000 + Digit(sv[ i + 1 ]) * 100 + Digit(sv[ i + 2 ]) * 10 + Digit(sv[ i + 3 ]);
					break;
				case 5:
					return Digit(sv[ i ]) * 10'000 + Digit(sv[ i + 1 ]) * 1'000 + Digit(sv[ i + 2 ]) * 100 +
					       Digit(sv[ i + 3 ]) * 10 + Digit(sv[ i + 4 ]);
					break;
				case 6:
					return Digit(sv[ i ]) * 100'000 + Digit(sv[ i + 1 ]) * 10'000 + Digit(sv[ i + 2 ]) * 1'000 +
					       Digit(sv[ i + 3 ]) * 100 + Digit(sv[ i + 4 ]) * 10 + Digit(sv[ i + 5 ]);
					break;
				case 7:
					return Digit(sv[ i ]) * 1'000'000 + Digit(sv[ i + 1 ]) * 100'000 + Digit(sv[ i + 2 ]) * 10'000 +
					       Digit(sv[ i + 3 ]) * 1'000 + Digit(sv[ i + 4 ]) * 100 + Digit(sv[ i + 5 ]) * 10 +
					       Digit(sv[ i + 6 ]);
					break;
				case 8:
					return Digit(sv[ i ]) * 10'000'000 + Digit(sv[ i + 1 ]) * 1'000'000 + Digit(sv[ i + 2 ]) * 100'000 +
					       Digit(sv[ i + 3 ]) * 10'000 + Digit(sv[ i + 4 ]) * 1'000 + Digit(sv[ i + 5 ]) * 100 +
					       Digit(sv[ i + 6 ]) * 10 + Digit(sv[ i + 7 ]);
					break;
				case 9:
					return Digit(sv[ i ]) * 100'000'000 + Digit(sv[ i + 1 ]) * 10'000'000 + Digit(sv[ i + 2 ]) * 1'000'000 +
					       Digit(sv[ i + 3 ]) * 100'000 + Digit(sv[ i + 4 ]) * 10'000 + Digit(sv[ i + 5 ]) * 1'000 +
					       Digit(sv[ i + 6 ]) * 100 + Digit(sv[ i + 7 ]) * 10 + Digit(sv[ i + 8 ]);
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
					return Digit(sv[ i ]) * 1'000'000'000 + Digit(sv[ i + 1 ]) * 100'000'000 +
					       Digit(sv[ i + 2 ]) * 10'000'000 + Digit(sv[ i + 3 ]) * 1'000'000 + Digit(sv[ i + 4 ]) * 100'000 +
					       Digit(sv[ i + 5 ]) * 10'000 + Digit(sv[ i + 6 ]) * 1'000 + Digit(sv[ i + 7 ]) * 100 +
					       Digit(sv[ i + 8 ]) * 10 + Digit(sv[ i + 9 ]);
					break;
			}
		unreachable();
	}

	bool ArgFormatter::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		auto ch { sv[ start ] };

		if( m_indexMode == IndexMode::automatic ) {
				if( ch == '}' ) {
						if( m_indexMode == IndexMode::automatic ) {
								specValues.argPosition = ++argIndex;
								return false;
						} else {
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For "
								                         "Arguments\n");
							}
				} else if( ch == ' ' ) {
						for( ;; ) {
								if( start >= sv.size() ) break;
								if( ch = sv[ ++start ] != ' ' ) break;
							}
						if( ch != '}' ) {
								throw std::runtime_error("Error In Positional Field: Invalid Format Detected. A "
								                         "Position Field Should Be "
								                         "Followed By A ':' Or A '}'");
						} else {
								specValues.argPosition = ++argIndex;
								return sv[ ++start ] == ':';
							}
				}
		}
		if( IsDigit(ch) ) {
				auto& position { specValues.argPosition };
				// from_chars() is much faster than the NoCheckIntFromChars() in this particular case
				std::from_chars(sv.data(), sv.data() + FindDigitEnd(sv, start), position);
				start += position > 9 ? position <= 99 ? 2 : -1 : 1;
				if( start == -1 ) {
						throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
				}
				if( sv[ start ] != ':' && sv[ start ] != '}' ) {
						throw std::runtime_error("Error In Positional Field: Invalid Format Detected. A Position Field "
						                         "Should Be "
						                         "Followed By A ':' Or A '}'");
				}
				++argIndex;
				m_tokenType |= TokenType::Positional;
		}
		return true;
	}

	void serenity::arg_formatter::ArgFormatter::FindNestedBrackets(std::string_view sv, int& currentPos) {
		auto size { sv.size() - 1 };

		auto FindOpenBracket = [ & ]() {
			for( ;; ) {
					if( currentPos > size ) {
							return false;
							break;
					} else if( sv[ currentPos ] != '{' ) {
							++currentPos;
							continue;
					} else {
							return true;
						}
				}
		};

		auto FindCloseBracket = [ & ]() {
			for( ;; ) {
					if( currentPos > size ) {
							break;
					} else if( sv[ currentPos ] != '}' ) {
							++currentPos;
							continue;
					}
					break;
				}
		};

		if( FindOpenBracket() ) {
				FindCloseBracket();
				++currentPos;
		}
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
				auto ch { sv[ subPos ] };
				for( ;; ) {
						ch = sv[ subPos ];
						if( subPos > size ) bracketResults.isValid = false;
						if( ch == '{' ) {
								FindNestedBrackets(sv, subPos);
						}
						if( sv[ subPos ] != '}' ) {
								++subPos;
								continue;
						}
						bracketResults.endPos  = subPos;
						bracketResults.isValid = true;
						break;
					}
				break;
			}
	}

	void ArgFormatter::VerifyFillAlignField(std::string_view& sv, size_t& currentPos, const size_t& bracketSize) {
		auto ch { sv[ currentPos ] };

		switch( ch ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default: specValues.align = Alignment::Empty; break;
			}
		if( currentPos - 1 >= 0 ) {
				auto ch { sv[ currentPos - 1 ] };
				if( ch != '{' && ch != '}' ) {
						if( ch == ':' ) {
								specValues.fillCharacter = ' ';
						} else {
								specValues.fillCharacter = sv[ currentPos - 1 ];
							}
						m_tokenType |= TokenType::FillAlign;
				} else {
						std::string tmp { "Error In Fill/Align Field: \"" };
						tmp.append(1, sv[ currentPos - 1 ]).append("\" Is Not A Valid Fill Specifier\n");
						throw std::runtime_error(std::move(tmp));
					}
		}
		++currentPos;
	}

	void ArgFormatter::VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		switch( sv[ currentPosition ] ) {
				case '+': specValues.signType = Sign::Plus; break;
				case '-': specValues.signType = Sign::Minus; break;
				case ' ': specValues.signType = Sign::Space; break;
				default:
					specValues.signType = Sign::Empty;
					return;
					break;
			}
		++currentPosition;
		m_tokenType |= TokenType::Sign;
	}

	static bool IsValidAltType(experimental::msg_details::SpecType argType) {
		using SpecType = experimental::msg_details::SpecType;
		switch( argType ) {
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

	void ArgFormatter::VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
		if( !IsValidAltType(argType) ) {
				std::string tmp { "Error In Alternate Form Field: Argument Type \"" };
				tmp.append(experimental::msg_details::SpecTypeString(argType));
				tmp.append("\" Does Not Have A Supported Alternative Form\n");
				throw std::runtime_error(std::move(tmp));
		}
		specValues.hasAltForm = true;
		++currentPosition;    // advance past '#' and search for alt spec
		size_t parseForAltSpecPos { currentPosition };
		for( ;; ) {
				if( parseForAltSpecPos >= bracketSize ) break;
				auto ch { sv[ ++parseForAltSpecPos ] };
				if( IsValidAltTypeSpec(ch) ) {
						specValues.preAltForm = AlternatePreFormatChars(ch);
						break;
				}
			}
	}

	static std::string_view NestedFieldTypeStr(NestedFieldType type) {
		return type == NestedFieldType::Prec ? "Precision\0" : "Width\0";
	}

	void ArgFormatter::VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type) {
		auto ch { sv[ ++currentPosition ] };
		if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						if( type == NestedFieldType::Prec ) {
								specValues.nestedPrecArgPos = ++argCounter;
								m_tokenType |= TokenType::Precision;
						} else {
								specValues.nestedWidthArgPos = ++argCounter;
								m_tokenType |= TokenType::Width;
							}
						return;
				} else {
						throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
					}
		}
		if( IsDigit(ch) ) {
				if( type == NestedFieldType::Width ) {
						specValues.nestedWidthArgPos = NoCheckIntFromChars(sv, currentPosition,
						                                                   FindDigitEnd(sv, currentPosition));
						currentPosition += specValues.nestedWidthArgPos > 9 ? 2 : 1;
						m_tokenType |= TokenType::Width;
				} else {
						specValues.nestedPrecArgPos = NoCheckIntFromChars(sv, currentPosition,
						                                                  FindDigitEnd(sv, currentPosition));
						currentPosition += specValues.nestedPrecArgPos > 9 ? 2 : 1;
						m_tokenType |= TokenType::Precision;
					}
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( currentPosition >= bracketSize ) break;
						if( ch = sv[ ++currentPosition ] != ' ' ) break;
					}
				if( !IsDigit(ch) ) {
						std::string throwMsg { "Error In Nested Bracket For \"" };
						throwMsg.append(NestedFieldTypeStr(type));
						throwMsg.append("\": Only A Positional Argument Is Allowed\n ");
						throw std::runtime_error(std::move(throwMsg));
				}
		}
		++currentPosition;
	}

	void ArgFormatter::VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, bool isDigit) {
		auto ch { sv[ currentPosition ] };
		if( ch == '{' ) {
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Width);
				return;
		} else if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						specValues.nestedWidthArgPos = ++argCounter;
				} else {
						throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
					}
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( currentPosition >= bracketSize ) break;
						if( ch = sv[ ++currentPosition ] != ' ' ) break;
					}
		}
		if( IsDigit(ch) ) {
				auto& position { specValues.alignmentPadding };
				// from_chars() is much faster than the NoCheckIntFromChars() in this particular case
				std::from_chars(sv.data(), sv.data() + FindDigitEnd(sv, currentPosition), position);
				currentPosition += position > 9 ? position <= 99 ? 2 : -1 : 1;
				if( currentPosition == -1 ) {
						throw std::runtime_error("Error In Positional Argument Field: Max Position (99) "
						                         "Exceeded\n");
				}
				++argCounter;
				m_tokenType |= TokenType::Width;
		} else if( ch != '}' ) {
				throw std::runtime_error("Error In Width Field: Invalid Format Detected.\n");
		}
	}

	void ArgFormatter::VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto ch { sv[ ++currentPosition ] };
		if( ch == '{' ) {
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Prec);
				return;
		} else if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						specValues.nestedPrecArgPos = ++argCounter;
				} else {
						throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
					}
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( currentPosition >= bracketSize ) break;
						if( ch = sv[ ++currentPosition ] != ' ' ) break;
					}
		}
		if( IsDigit(ch) ) {
				auto& position { specValues.nestedPrecArgPos };
				// from_chars() is much faster than the NoCheckIntFromChars() in this particular case
				std::from_chars(sv.data(), sv.data() + FindDigitEnd(sv, currentPosition), position);
				currentPosition += position > 9 ? position <= 99 ? 2 : -1 : 1;
				if( currentPosition == -1 ) {
						throw std::runtime_error("Error In Positional Argument Field: Max Position (99) "
						                         "Exceeded\n");
				}
				++argCounter;
				m_tokenType |= TokenType::Precision;
		} else if( ch != '}' ) {
				throw std::runtime_error("Error In Precision Field: Invalid Format Detected.\n");
		}
	}

	void ArgFormatter::VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		++currentPosition;    // advance past the first '}'
		if( currentPosition <= (bracketSize - 2) ) {
				if( sv[ currentPosition + static_cast<size_t>(1) ] == '}' ) {
						std::string tmp { 1, '}' };
						tmp.append(result.remainder.data(), result.remainder.size());
						result.remainder = std::move(tmp);
				}
		}
	}

	void ArgFormatter::VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
		++currentPosition;
		switch( argType ) {
				case SpecType::CharType: [[fallthrough]];
				case SpecType::ConstVoidPtrType: [[fallthrough]];
				case SpecType::MonoType: [[fallthrough]];
				case SpecType::StringType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::VoidPtrType:
					{
						std::string tmp { "Error In Locale Field: Argument Of Type \"" };
						tmp.append(experimental::msg_details::SpecTypeString(argType));
						tmp.append("\" Cannot Be Localized\n");
						throw std::runtime_error(std::move(tmp));
					}
				default: return; break;
			}
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

	void ArgFormatter::VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec) {
		const auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
		if( argStorage.VerifySpec(argType, spec) ) {
				++currentPosition;
				m_tokenType |= TokenType::Type;
				specValues.typeSpec = spec;
				return;
		} else {
				std::string tmp { "Error In Type Field: Type Specifier\"" };
				tmp.append(1, spec);
				tmp.append("\" Is Not A Valid Specifier For Argument Of Type \"");
				tmp.append(experimental::msg_details::SpecTypeString(argType)).append("\"\n");
				throw std::runtime_error(std::move(tmp));
			}
	}

	void ArgFormatter::HandlePotentialTypeField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto ch { sv[ currentPosition ] };
		if( IsAlpha(ch) ) {
				switch( sv[ currentPosition + 1 ] ) {
						case '}': VerifyTypeSpec(sv, currentPosition, bracketSize, ch); break;
						case ' ':
							++currentPosition;
							for( ;; ) {
									if( currentPosition >= bracketSize ) break;
									if( sv[ ++currentPosition ] == '}' ) break;
									VerifyTypeSpec(sv, currentPosition, bracketSize, ch);
									break;
								}
							break;
						default:
							std::string tmp { "Invalid Type Specifier Found: \"" };
							tmp.append(1, sv[ currentPosition ]).append("\"\n");
							throw std::runtime_error(std::move(tmp));
							break;
					}
		} else {
				++currentPosition;
			}
	}

	void ArgFormatter::VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize) {
		if( (bracketSize <= 1) || (start >= (bracketSize - 1)) ) return;
		auto ch { sv[ start ] };
		switch( ch ) {
				case ':': ++start; break;
				case '^': [[fallthrough]];
				case '<': [[fallthrough]];
				case '>': VerifyFillAlignField(sv, start, bracketSize); break;
				case '+': [[fallthrough]];
				case '-': [[fallthrough]];
				case ' ': VerifySignField(sv, start, bracketSize); break;
				case '#': VerifyAltField(sv, start, bracketSize); break;
				case '0':
					specValues.fillCharacter = '0';
					++start;
					break;
				case '1': [[fallthrough]];
				case '2': [[fallthrough]];
				case '3': [[fallthrough]];
				case '4': [[fallthrough]];
				case '5': [[fallthrough]];
				case '6': [[fallthrough]];
				case '7': [[fallthrough]];
				case '8': [[fallthrough]];
				case '9': VerifyWidthField(sv, start, bracketSize, true); break;
				case '{': VerifyWidthField(sv, start, bracketSize, false); break;
				case '.': VerifyPrecisionField(sv, start, bracketSize); break;
				case 'L': VerifyLocaleField(sv, start, bracketSize); break;
				case '}': VerifyEscapedBracket(sv, start, bracketSize); break;
				default: HandlePotentialTypeField(sv, start, bracketSize); break;
			}
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
	void ArgFormatter::FormatFillAlignToken() {
		temp.clear();
		size_t fillAmount {}, totalWidth {}, i { 0 };
		auto data { buffer.data() };
		if( specValues.hasAltForm ) {
				temp.append(std::move(specValues.preAltForm));
		}
		argStorage.GetArgValue(temp, specValues.argPosition, std::move(specValues.typeSpec));
		if( specValues.nestedWidthArgPos != 0 ) {
				argStorage.GetArgValue(result.tokenResult, specValues.nestedWidthArgPos);
				std::from_chars(result.tokenResult.data(), result.tokenResult.data() + result.tokenResult.size(), totalWidth);
				result.tokenResult.clear();
		} else {
				totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
			}
		fillAmount = (totalWidth > temp.size()) ? totalWidth - temp.size() : 0;
		std::fill(data, data + totalWidth + 1, '\0');

		switch( specValues.align ) {
				case Alignment::AlignLeft:
					{
						for( auto& ch: temp ) {
								buffer[ i ] = std::move(ch);
								++i;
							}
						std::fill(data + i, data + totalWidth, specValues.fillCharacter);
						result.tokenResult.append(data, totalWidth);
					}
					break;
				case Alignment::AlignRight:
					{
						std::fill(data, data + fillAmount, specValues.fillCharacter);
						i = fillAmount;
						for( auto& ch: temp ) {
								buffer[ i ] = std::move(ch);
								++i;
							}
						result.tokenResult.append(data, data + i);
					}
					break;
				case Alignment::AlignCenter:
					{
						i = (fillAmount /= 2);
						std::fill(data, data + fillAmount, specValues.fillCharacter);
						for( auto& ch: temp ) {
								buffer[ i ] = ch;
								++i;
							}
						auto endPos { i + fillAmount + (fillAmount % 2) + 1 };
						std::fill(data + i, data + endPos, specValues.fillCharacter);
						result.tokenResult.append(data, data + endPos);
					}
					break;
				default: break;
			}
	}

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

	void ArgFormatter::SimpleFormat() {
		// once formatting is setup, this will be used to just format
	}

	void ArgFormatter::FormatTokens() {
		using enum TokenType;
		if( IsFlagSet(TokenType::FillAlign) ) {
				FormatFillAlignToken();
		} else if( IsFlagSet(TokenType::Sign) ) {
				FormatSignToken();
		} else if( IsFlagSet(TokenType::Alternate) ) {
				FormatAlternateToken();
		} else if( IsFlagSet(TokenType::ZeroPad) ) {
				FormatZeroPadToken();
		} else if( IsFlagSet(TokenType::Locale) ) {
				FormatLocaleToken();
		} else if( IsFlagSet(TokenType::Width) ) {
				FormatWidthToken();
		} else if( IsFlagSet(TokenType::Precision) ) {
				FormatPrecisionToken();
		} else if( IsFlagSet(TokenType::Type) ) {
				FormatTypeToken();
		} else if( IsFlagSet(TokenType::Positional) ) {
				FormatPositionalToken();
		} else if( IsFlagSet(TokenType::Custom) ) {
				FormatCustomToken();
		} else if( IsFlagSet(TokenType::Empty) ) {
				SimpleFormat();
		}
	}

	bool ArgFormatter::HandleIfEndOrWhiteSpace(std::string_view sv, size_t& currentPosition, const size_t& bracketSize) {
		auto ch { sv[ ++currentPosition ] };
		if( ch == '}' ) {
				FormatTokens();
				++argCounter;
				return true;
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( currentPosition > bracketSize ) break;
						if( ch = sv[ ++currentPosition ] != ' ' ) break;
					}
				if( ch == '}' ) {
						FormatTokens();
						++argCounter;
						return true;
				} else {
						++currentPosition;
					}
		}
		return false;
	}

	// Enum Types: FillAlign, Sign, Alternate, ZeroPad, Locale, Width, Precision, Type
	ParseResult& ArgFormatter::Parse(std::string_view sv) {
		result.Reset();
		if( sv.size() < 2 ) return result;

		size_t pos { 0 };
		argCounter  = 0;
		m_tokenType = TokenType::Empty;

		FindBrackets(sv);
		if( !bracketResults.isValid ) {
				result.preTokenStr = std::move(sv);
				return result;
		}
		result.preTokenStr = std::move(sv.substr(0, bracketResults.beginPos));
		result.remainder   = std::move(sv.substr(bracketResults.endPos + 1, sv.size()));
		auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
		const size_t& bracketSize { argBracket.size() };

		/* Handle If Bracket Contained No Specs */
		if( HandleIfEndOrWhiteSpace(sv, pos, bracketSize) ) {
				return result;
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
				FormatTokens();
				++argCounter;
				return result;
		}
		/* Handle What's Left Of The Bracket */
		auto endPos { bracketSize - 1 };
		for( ;; ) {
				if( pos >= endPos ) break;
				VerifyArgumentBracket(argBracket, pos, bracketSize);
			}
		FormatTokens();
		return result;
	}

}    // namespace serenity::arg_formatter
