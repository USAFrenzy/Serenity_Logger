#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	bool ArgFormatter::IsFlagSet(TokenType checkValue) {
		return (m_tokenType & checkValue) == checkValue;
	}

	bool ArgFormatter::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		auto ch { sv[ start ] };

		// we're in automatic mode
		if( m_indexMode == IndexMode::automatic ) {
				if( ch == '}' ) {
						specValues.argPosition = ++argIndex;
						return false;
				} else if( ch == ' ' ) {
						for( ;; ) {
								if( start >= sv.size() ) break;
								if( ch = sv[ ++start ] != ' ' ) break;
							}
						if( ch != '}' && ch != ':' ) {
								throw std::runtime_error("Error In Position Field: Invalid Format Detected. A Position Field In Automatic Indexing Mode Should "
								                         "Be Followed By A ':' Or A '}'\n");
						} else {
								return true;
							}
				} else if( ch == ':' ) {
						++start;
						specValues.argPosition = argIndex;
						++argIndex;
						return true;
				} else {
						if( IsDigit(ch) ) {
								m_indexMode = IndexMode::manual;
								return ParsePositionalField(sv, argIndex, start);
						}
						specValues.argPosition = ++argIndex;
						return sv[ ++start ] == ':';
					}
		}
		// we're in manual mode
		if( ch == '}' ) {
				throw std::runtime_error("Error In Postion Field: Cannot Mix Manual And Automatic Indexing For "
				                         "Arguments\n");
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( start >= sv.size() ) break;
						if( ch = sv[ ++start ] != ' ' ) break;
					}
		}
		if( ch == ':' ) {
				throw std::runtime_error("Error In Position Field: Missing Positional Argument Before ':' In Manual Indexing Mode\n");
		}

		if( IsDigit(ch) ) {
				auto& position { specValues.argPosition };
				// from_chars() is much faster than the NoCheckIntFromChars() in this particular case
				std::from_chars(sv.data(), sv.data() + sv.size(), position);
				start += position > 9 ? position <= 24 ? 2 : -1 : 1;
				if( start == -1 ) {
						throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
				}
				if( sv[ start ] != ':' && sv[ start ] != '}' ) {
						throw std::runtime_error("Error In Position Field: Invalid Format Detected. A Position Field Should Be Followed By A ':' Or A "
						                         "'}'\n");
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
							throw std::runtime_error("Missing Closing '}' In Argument Spec Field\n");
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
				if( subPos >= size ) {
						throw std::runtime_error("Missing Closing '}' In Argument Spec Field\n");
				}
				auto ch { sv[ subPos ] };
				for( ;; ) {
						if( subPos > size ) {
								bracketResults.isValid = false;
								break;
						}
						ch = sv[ subPos ];
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
		// handle if '#' is a fill char and not a alt specifier
		if( currentPosition > 0 && sv[ currentPosition - 1 ] == ':' ) {
				++currentPosition;
				return;
		}
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
								specValues.nestedPrecArgPos = argCounter;
								++argCounter;
								m_tokenType |= TokenType::Precision;
						} else {
								specValues.nestedWidthArgPos = argCounter;
								++argCounter;
								m_tokenType |= TokenType::Width;
							}
						return;
				} else {
						throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
					}
		}
		if( IsDigit(ch) ) {
				if( type == NestedFieldType::Width ) {
						std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.nestedWidthArgPos);
						currentPosition += specValues.nestedWidthArgPos > 9 ? specValues.nestedWidthArgPos <= 24 ? 2 : -1 : 1;
						if( currentPosition == -1 ) {
								throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
						}
						m_tokenType |= TokenType::Width;
				} else {
						std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.nestedPrecArgPos);
						currentPosition += specValues.nestedPrecArgPos > 9 ? specValues.nestedPrecArgPos <= 24 ? 2 : -1 : 1;
						if( currentPosition == -1 ) {
								throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
						}
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
				} else {
						VerifyNestedBracket(sv, --currentPosition, bracketSize, type);
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
						specValues.nestedWidthArgPos = argCounter;
						++argCounter;
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
				auto& padding { specValues.alignmentPadding };
				std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), padding);
				currentPosition += padding > 9 ? padding >= 100 ? 3 : 2 : 1;
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
						specValues.nestedPrecArgPos = argCounter;
						++argCounter;
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
				auto& precision { specValues.precision };
				std::from_chars(sv.data(), sv.data() + sv.size(), precision);
				currentPosition += precision > 9 ? precision >= 100 ? 3 : 2 : 1;
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
		if( !IsAlpha(ch) ) {
				++currentPosition;
				return;
		}
		switch( sv[ ++currentPosition ] ) {
				case '}': VerifyTypeSpec(sv, currentPosition, bracketSize, ch); break;
				case ' ':
					for( ;; ) {
							if( currentPosition >= bracketSize ) break;
							if( sv[ ++currentPosition ] == '}' ) {
									VerifyTypeSpec(sv, currentPosition, bracketSize, ch);
							}
							break;
						}
					break;
				default: throw std::runtime_error("Error In Argument Format Field: No Closing '}' Found \n"); break;
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
	void ArgFormatter::FormatFillAlignToken(std::string& container) {
		temp.clear();
		size_t fillAmount {}, totalWidth {}, i { 0 };
		if( specValues.hasAltForm ) {
				temp.append(std::move(specValues.preAltForm));
		}
		// clang-format off
		int precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
			                                                                                            :  specValues.precision != 0 ? specValues.precision : 0 
		};
		// clang-format on
		argStorage.GetArgValueAsStr(temp, specValues.argPosition, std::move(specValues.typeSpec), precision);
		if( specValues.nestedWidthArgPos != 0 ) {
				totalWidth = argStorage.int_state(specValues.nestedWidthArgPos);
		} else {
				totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
			}

		auto size(temp.size());
		container.reserve(totalWidth + static_cast<size_t>(1));
		fillAmount = (totalWidth > size) ? totalWidth - size : 0;

		if( fillAmount == 0 ) {
				container.append(std::move(temp));
				return;
		}

		switch( specValues.align ) {
				case Alignment::AlignLeft:
					{
						container.append(std::move(temp)).append(fillAmount, specValues.fillCharacter);
					}
					break;
				case Alignment::AlignRight:
					{
						container.append(fillAmount, specValues.fillCharacter).append(std::move(temp));
					}
					break;
				case Alignment::AlignCenter:
					fillAmount /= 2;
					container.append(fillAmount, specValues.fillCharacter).append(std::move(temp));
					fillAmount = (totalWidth - size - fillAmount);
					container.append(fillAmount, specValues.fillCharacter);
					break;
				default: break;
			}
	}

	void ArgFormatter::FormatPositionalToken(std::string& container) { }
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
	void ArgFormatter::FormatSignToken(std::string& container) { }

	// Integers use 0b/0B for binary, 0 for octal and 0x/0X for hex
	// For floating point, the result contains a decimal point irregardless if the value contains
	// decimal points or not where the default is to only use decimal points only when present and
	// g/G doesn't remove trailing zeros (This is by the spec being followed)
	void ArgFormatter::FormatAlternateToken(std::string& container) { }

	void ArgFormatter::FormatZeroPadToken(std::string& container) { }

	void ArgFormatter::FormatLocaleToken(std::string& container) {
		// pass in the reference to the locale from msginfo so if it changes,
		// it'll be reflected here when creating the groupings
		std::locale loc { "" };

		auto argType { argStorage.SpecTypesCaptured()[ argCounter ] };

		argStorage.GetArgValueAsStr(temp, argCounter);
		size_t value;
		std::from_chars(temp.data(), temp.data() + temp.size(), value);

		std::string groupings { std::use_facet<std::numpunct<char>>(loc).grouping() };
		auto numOfSep { std::_Count_separators(temp.size(), groupings) };
	}

	void ArgFormatter::FormatWidthToken(std::string& container) { }

	void ArgFormatter::FormatPrecisionToken(std::string& container) { }

	void ArgFormatter::FormatTypeToken(std::string& container) { }

	void ArgFormatter::FormatCharAggregateToken(std::string& container) { }

	void ArgFormatter::FormatCustomToken(std::string& container) { }

	void ArgFormatter::SimpleFormat(std::string& container) {
		// once formatting is setup, this will be used to just format
	}

	void ArgFormatter::FormatTokens(std::string& container) {
		using enum TokenType;
		if( IsFlagSet(TokenType::FillAlign) ) {
				FormatFillAlignToken(container);
		} else if( IsFlagSet(TokenType::Sign) ) {
				FormatSignToken(container);
		} else if( IsFlagSet(TokenType::Alternate) ) {
				FormatAlternateToken(container);
		} else if( IsFlagSet(TokenType::ZeroPad) ) {
				FormatZeroPadToken(container);
		} else if( IsFlagSet(TokenType::Locale) ) {
				FormatLocaleToken(container);
		} else if( IsFlagSet(TokenType::Width) ) {
				FormatWidthToken(container);
		} else if( IsFlagSet(TokenType::Precision) ) {
				FormatPrecisionToken(container);
		} else if( IsFlagSet(TokenType::Type) ) {
				FormatTypeToken(container);
		} else if( IsFlagSet(TokenType::Positional) ) {
				FormatPositionalToken(container);
		} else if( IsFlagSet(TokenType::Custom) ) {
				FormatCustomToken(container);
		} else if( IsFlagSet(TokenType::Empty) ) {
				SimpleFormat(container);
		}
	}

	bool ArgFormatter::HandleIfEndOrWhiteSpace(std::string& container, std::string_view sv, size_t& currentPosition, const size_t& bracketSize) {
		auto ch { sv[ currentPosition + static_cast<size_t>(1) ] };
		if( ch == '}' ) {
				FormatTokens(container);
				++argCounter;
				return true;
		} else if( ch == ' ' ) {
				for( ;; ) {
						if( currentPosition > bracketSize ) break;
						if( ch = sv[ ++currentPosition ] != ' ' ) break;
					}
				if( ch == '}' ) {
						FormatTokens(container);
						++argCounter;
						return true;
				} else {
						++currentPosition;
					}
		}
		return false;
	}

	// Enum Types: FillAlign, Sign, Alternate, ZeroPad, Locale, Width, Precision, Type
	void ArgFormatter::Parse(std::string& container, std::string_view sv) {
		result.remainder = sv;
		specValues.ResetSpecs();
		for( ;; ) {
				if( sv.size() < 2 ) break;

				size_t pos { 0 };
				argCounter  = 0;
				m_tokenType = TokenType::Empty;

				FindBrackets(result.remainder);
				if( !bracketResults.isValid ) {
						container.append(sv.data(), sv.size());
						break;
				}
				container.append(sv.substr(0, bracketResults.beginPos));
				auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
				const size_t& bracketSize { argBracket.size() };

				/* Handle If Bracket Contained No Specs */
				if( HandleIfEndOrWhiteSpace(container, argBracket, pos, bracketSize) ) {
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
						FormatTokens(container);
						++argCounter;
						break;
						;
				}
				/* Handle What's Left Of The Bracket */
				auto endPos { bracketSize - 1 };
				for( ;; ) {
						if( pos >= endPos ) break;
						VerifyArgumentBracket(argBracket, pos, bracketSize);
					}
				FormatTokens(container);
				break;
			}
		container.append(sv.substr(bracketResults.endPos + 1, sv.size()));
	}

}    // namespace serenity::arg_formatter
