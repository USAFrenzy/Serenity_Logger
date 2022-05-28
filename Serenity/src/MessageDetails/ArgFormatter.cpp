#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	void SpecFormatting::ResetSpecs() {
		std::memset(this, 0, sizeof(SpecFormatting));
	}

	void BracketSearchResults::Reset() {
		std::memset(this, 0, sizeof(BracketSearchResults));
	}

	ArgFormatter::ArgFormatter(const std::locale& locale)
			: argCounter(0), m_indexMode(IndexMode::automatic), remainder(""), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}),
			  argStorage(serenity::msg_details::ArgContainer {}), rawValueTemp(), buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}),
			  charsResult(std::to_chars_result {}), loc(nullptr) {
		loc       = std::make_unique<std::locale>(locale);
		separator = std::use_facet<std::numpunct<char>>(*loc.get()).thousands_sep();
		decimal   = std::use_facet<std::numpunct<char>>(*loc.get()).decimal_point();
		falseStr  = std::use_facet<std::numpunct<char>>(*loc.get()).falsename();
		trueStr   = std::use_facet<std::numpunct<char>>(*loc.get()).truename();
		groupings = std::use_facet<std::numpunct<char>>(*loc.get()).grouping();
	}

	bool ArgFormatter::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		// we're in automatic mode
		if( m_indexMode == IndexMode::automatic ) {
				if( auto ch { sv[ start ] }; ch == '}' ) {
						specValues.argPosition = argIndex++;
						return false;
				} else if( ch == ' ' ) {
						for( ;; ) {
								if( start >= sv.size() ) break;
								if( ch = sv[ start++ ] != ' ' ) break;
							}
						switch( sv[ start ] ) {
								case ':': [[fallthrough]];
								case '}': return true; break;
								default: throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Automatic Indexing Mode\n"); break;
							}
				} else if( ch == ':' ) {
						specValues.argPosition = argIndex++;
						++start;
						return true;
				} else {
						if( IsDigit(ch) ) {
								m_indexMode = IndexMode::manual;
								return ParsePositionalField(sv, argIndex, start);
						}
					}
		} else {
				// we're in manual mode
				if( char ch { sv[ start ] }; IsDigit(ch) ) {
						// At the cost of some readability, this is much faster than std::from_chars() for this use case (~%10 faster).
						// Presumably because unlike std::from_chars(), I'm not doing any bounds/sign checking here, just seeing
						// if the next char is an ASCII digit and converting it to an integer while limiting the digits to 2 instead.
						// clang-format off
						auto ToInt = [ & ]() {
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
									default: unreachable(); break;
								}
						};
						// clang-format on
						auto& position { specValues.argPosition };
						position = ToInt();
						ch       = sv[ ++start ];
						if( IsDigit(ch) ) {
								position *= 10;
								position += ToInt();
								ch = sv[ ++start ];
								if( position > 24 || IsDigit(ch) ) {
										throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
								}
						}
						switch( sv[ start ] ) {
								case ':': [[fallthrough]];
								case '}':
									++argIndex;
									++start;
									return true;
									break;
								default: throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Manual Indexing Mode\n"); break;
							}
				} else {
						switch( sv[ start ] ) {
								case '}': throw std::runtime_error("Error In Postion Field: Cannot Mix Manual And Automatic Indexing For Arguments\n"); break;
								case ' ':
									{
										for( ;; ) {
												if( start >= sv.size() ) break;
												if( sv[ ++start ] != ' ' ) break;
											}
									}
									[[fallthrough]];
								case ':':
									throw std::runtime_error("Error In Position Field: Missing Positional Argument Before ':' In Manual Indexing Mode\n");
									break;
								default: throw std::runtime_error("Formatting Error Detected: Missing ':' Before Next Specifier\n"); break;
							}
					}
			}
		return true;
	}

	void serenity::arg_formatter::ArgFormatter::FindNestedBrackets(std::string_view sv, size_t& currentPos) {
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

	void ArgFormatter::FindBrackets(std::string_view sv, size_t svSize) {
		bracketResults.Reset();
		for( ;; ) {
				if( bracketResults.beginPos >= svSize ) {
						bracketResults.isValid = false;
						return;
				}
				if( sv[ bracketResults.beginPos ] != '{' ) {
						++bracketResults.beginPos;
						continue;
				}
				bracketResults.endPos = bracketResults.beginPos + 1;
				if( bracketResults.endPos >= svSize ) {
						throw std::runtime_error("Missing Closing '}' In Argument Spec Field\n");
				}
				for( ;; ) {
						if( bracketResults.endPos > svSize ) {
								bracketResults.isValid = false;
								return;
						}
						switch( sv[ bracketResults.endPos ] ) {
								case '{':
									FindNestedBrackets(sv, bracketResults.endPos);
									continue;
									break;
								case '}':
									bracketResults.isValid = true;
									return;
									break;
								default:
									++bracketResults.endPos;
									continue;
									break;
							}
					}
			}
	}

	void ArgFormatter::VerifyFillAlignField(std::string_view& sv, size_t& currentPos, const size_t& bracketSize, msg_details::SpecType& argType) {
		auto ch { sv[ currentPos ] };
		char nextCh {};
		if( currentPos + 1 < bracketSize ) {
				nextCh = sv[ currentPos + 1 ];
		}
		switch( nextCh ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default:
					using SpecType = msg_details::SpecType;
					switch( argType ) {
							case SpecType::IntType: [[fallthrough]];
							case SpecType::U_IntType: [[fallthrough]];
							case SpecType::DoubleType: [[fallthrough]];
							case SpecType::FloatType: [[fallthrough]];
							case SpecType::LongDoubleType: [[fallthrough]];
							case SpecType::LongLongType: [[fallthrough]];
							case SpecType::U_LongLongType: specValues.align = Alignment::AlignRight; break;
							default: specValues.align = Alignment::AlignLeft; break;
						}
					specValues.fillCharacter = ' ';
					return;
			}
		if( ch != '{' && ch != '}' ) {
				if( ch == ':' ) {
						specValues.fillCharacter = ' ';
				} else {
						specValues.fillCharacter = ch;
						++currentPos;
					}
		} else {
				std::string tmp { "Error In Fill/Align Field: \"" };
				tmp.append(1, sv[ currentPos - 1 ]).append("\" Is Not A Valid Fill Specifier\n");
				throw std::runtime_error(std::move(tmp));
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
	}

	static bool IsValidAltType(msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;
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

	static constexpr bool IsValidAltTypeSpec(msg_details::SpecType& type, const char& spec) {
		using SpecType = msg_details::SpecType;

		switch( type ) {
				case SpecType::CharType:
					switch( spec ) {
							case 'b': [[fallthrough]];
							case 'B': [[fallthrough]];
							case 'c': [[fallthrough]];
							case 'o': [[fallthrough]];
							case 'x': [[fallthrough]];
							case 'X': return true; break;
							default: return false; break;
						}
					break;

				case SpecType::IntType: [[fallthrough]];
				case SpecType::U_IntType: [[fallthrough]];
				case SpecType::LongLongType: [[fallthrough]];
				case SpecType::U_LongLongType:
					switch( spec ) {
							case 'b': [[fallthrough]];
							case 'B': [[fallthrough]];
							case 'o': [[fallthrough]];
							case 'x': [[fallthrough]];
							case 'X': return true; break;
							default: return false; break;
						}
					break;
				case SpecType::FloatType:
				case SpecType::DoubleType:
				case SpecType::LongDoubleType:
					switch( spec ) {
							case 'a': [[fallthrough]];
							case 'A': return true; break;
							default: return false; break;
						}
					break;

				default: break;
			}
		return false;
	}

	static constexpr const char* AlternatePreFormatChars(msg_details::SpecType type, const char& spec) {
		using SpecType = msg_details::SpecType;

		switch( type ) {
				case SpecType::CharType: [[fallthrough]];
				case SpecType::BoolType: [[fallthrough]];
				case SpecType::IntType: [[fallthrough]];
				case SpecType::U_IntType: [[fallthrough]];
				case SpecType::LongLongType: [[fallthrough]];
				case SpecType::U_LongLongType:
					switch( spec ) {
							case 'b': return "0b"; break;
							case 'B': return "0B"; break;
							case 'o': return "0"; break;
							case 'x': return "0x"; break;
							case 'X': return "0X"; break;
							default: break;
						}
					break;
				case SpecType::FloatType:
				case SpecType::DoubleType:
				case SpecType::LongDoubleType:
					switch( spec ) {
							case 'a': return "0x"; break;
							case 'A': return "0X"; break;
							default: break;
						}
					break;
				default: break;
			}
		return "\0";
	}

	void ArgFormatter::VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType) {
		if( !IsValidAltType(argType) ) {
				std::string tmp { "Error In Alternate Form Field: Argument Type \"" };
				tmp.append(msg_details::SpecTypeString(argType));
				tmp.append("\" Does Not Have A Supported Alternative Form\n");
				throw std::runtime_error(std::move(tmp));
		}
		specValues.hasAlt = true;
		size_t parseForAltSpecPos { currentPosition };
		for( ;; ) {
				if( ++parseForAltSpecPos >= bracketSize ) break;
				auto ch { sv[ parseForAltSpecPos ] };
				if( IsValidAltTypeSpec(argType, ch) ) {
						specValues.preAltForm = AlternatePreFormatChars(argType, ch);
						specValues.typeSpec   = ch;
						break;
				}
			}

		++currentPosition;
	}

	static std::string_view NestedFieldTypeStr(NestedFieldType type) {
		return type == NestedFieldType::Prec ? "Precision\0" : "Width\0";
	}

	void ArgFormatter::VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type,
	                                       msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;

		auto ch { sv[ ++currentPosition ] };
		if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						if( type == NestedFieldType::Prec ) {
								if( argType == SpecType::IntType ) {
										throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n");
								}
								specValues.nestedPrecArgPos = argCounter;
								++argCounter;
						} else {
								specValues.nestedWidthArgPos = argCounter;
								++argCounter;
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
				} else {
						std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.nestedPrecArgPos);
						currentPosition += specValues.nestedPrecArgPos > 9 ? specValues.nestedPrecArgPos <= 24 ? 2 : -1 : 1;
						if( currentPosition == -1 ) {
								throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
						}
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
						VerifyNestedBracket(sv, --currentPosition, bracketSize, type, argType);
					}
		}
		++currentPosition;
	}

	void ArgFormatter::VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType) {
		auto ch { sv[ currentPosition ] };
		if( ch == '{' ) {
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Width, argType);
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
		} else if( ch != '}' ) {
				throw std::runtime_error("Error In Width Field: Invalid Format Detected.\n");
		}
	}

	void ArgFormatter::VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;

		auto ch { sv[ ++currentPosition ] };
		if( ch == '{' ) {
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Prec, argType);
				return;
		} else if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						if( argType == SpecType::IntType ) {
								throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n");
						}
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
		switch( argType ) {
				case SpecType::CharType: [[fallthrough]];
				case SpecType::BoolType: [[fallthrough]];
				case SpecType::IntType: [[fallthrough]];
				case SpecType::U_IntType: [[fallthrough]];
				case SpecType::LongLongType: [[fallthrough]];
				case SpecType::U_LongLongType:
					throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n");
				default: break;
			}

		if( IsDigit(ch) ) {
				auto& precision { specValues.precision };
				std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), precision);
				currentPosition += precision > 9 ? precision >= 100 ? 3 : 2 : 1;
				++argCounter;
		} else if( ch != '}' ) {
				throw std::runtime_error("Error In Precision Field: Invalid Format Detected.\n");
		}
	}

	void ArgFormatter::VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		if( (currentPosition + 1) <= (bracketSize - 2) ) {
				if( sv[ currentPosition + static_cast<size_t>(1) ] == '}' ) {
						specValues.hasClosingBrace = true;
						++currentPosition;
				}
		}
	}

	void ArgFormatter::VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;
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
						tmp.append(msg_details::SpecTypeString(argType));
						tmp.append("\" Cannot Be Localized\n");
						throw std::runtime_error(std::move(tmp));
					}
				default:
					specValues.localize = true;
					return;
					break;
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

	void ArgFormatter::VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec, msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;
		if( VerifySpec(argType, spec) ) {
				++currentPosition;
				specValues.typeSpec = spec;
				return;
		} else {
				std::string tmp { "Error In Type Field: Type Specifier\"" };
				tmp.append(1, spec);
				tmp.append("\" Is Not A Valid Specifier For Argument Of Type \"");
				tmp.append(msg_details::SpecTypeString(argType)).append("\"\n");
				throw std::runtime_error(std::move(tmp));
			}
	}

	bool ArgFormatter::IsSimpleSubstitution(msg_details::SpecType& argType, int& precision, int& width) {
		using enum serenity::msg_details::SpecType;
		if( width != 0 ) return false;
		switch( argType ) {
				case StringType: [[fallthrough]];
				case CharPointerType: [[fallthrough]];
				case StringViewType:
					{
						return precision == 0 && (specValues.typeSpec == '\0' || specValues.typeSpec == 's');
					}
					break;
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType:
					{
						return !specValues.hasAlt && specValues.signType == Sign::Empty && !specValues.localize && specValues.typeSpec == '\0';
					}
					break;
				case BoolType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 's'); break;
				case CharType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c'); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType:
					{
						return !specValues.localize && precision == 0 && specValues.signType == Sign::Empty && !specValues.hasAlt && specValues.typeSpec == '\0';
					}
					break;
					// for pointer types, if the width field is 0, there's no fill/alignment to take into account and therefore it's a simple sub
				case ConstVoidPtrType: [[fallthrough]];
				case VoidPtrType: return true; break;
				default: return false; break;
			}
		unreachable();
	}

	void ArgFormatter::LocalizeIntegral(int precision, msg_details::SpecType type) {
		FormatRawValueToStr(precision, type);
		FormatIntegralGrouping(rawValueTemp, separator);
	}

	void ArgFormatter::LocalizeFloatingPoint(int precision, msg_details::SpecType type) {
		FormatRawValueToStr(precision, type);
		size_t pos { 0 };
		auto size { rawValueTemp.size() };
		auto data { rawValueTemp.begin() };
		std::string_view sv { rawValueTemp };
		localeTemp.clear();
		for( ;; ) {
				if( pos >= size ) break;
				if( sv[ pos ] == '.' ) {
						localeTemp.append(sv.substr(0, pos));
						FormatIntegralGrouping(localeTemp, separator);
						localeTemp += decimal;
						localeTemp.append(sv.substr(pos + 1, sv.size()));
						break;
				}
				++pos;
			}
		if( localeTemp.size() != 0 ) {
				rawValueTemp.clear();
				rawValueTemp.append(std::move(localeTemp));
				return;
		}
		FormatIntegralGrouping(rawValueTemp, separator);
	}

	void ArgFormatter::LocalizeBool() {
		rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? trueStr : falseStr);
	}

	void ArgFormatter::LocalizeArgument(int precision, msg_details::SpecType type) {
		using enum serenity::msg_details::SpecType;
		// NOTE: The following types should have been caught in the verification process:
		//       monostate, string, c-string, string view, const void*, void *
		switch( type ) {
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: LocalizeIntegral(precision, type); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType: [[fallthrough]];
				case U_LongLongType: LocalizeFloatingPoint(precision, type); break;
				case BoolType: LocalizeBool(); break;
				default: break;
			}
	}

	void ArgFormatter::HandlePotentialTypeField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType) {
		// If this case returns true, then we've already handled the type spec in the alternate form field
		if( specValues.typeSpec != '\0' ) {
				++currentPosition;
				return;
		}
		if( currentPosition + 1 >= bracketSize ) return;
		auto ch { sv[ currentPosition ] };
		switch( sv[ currentPosition + 1 ] ) {
				case '}': VerifyTypeSpec(sv, currentPosition, bracketSize, ch, argType); break;
				case ' ':
					for( ;; ) {
							if( currentPosition >= bracketSize ) break;
							if( sv[ ++currentPosition ] == '}' ) {
									VerifyTypeSpec(sv, currentPosition, bracketSize, ch, argType);
							}
							break;
						}
					break;
				default: throw std::runtime_error("Error In Argument Format Field: Unable To Verify Potential Type Specifier - No Closing '}' Found \n"); break;
			}
		++currentPosition;
		return;
	}

	void ArgFormatter::VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize, msg_details::SpecType& argType) {
		if( (bracketSize <= 1) || (start >= (bracketSize - 1)) ) return;
		using SpecType = msg_details::SpecType;
		VerifyFillAlignField(sv, start, bracketSize, argType);
		VerifySignField(sv, start, bracketSize);
		if( sv[ start ] == '#' ) {
				VerifyAltField(sv, start, bracketSize, argType);
		}
		if( sv[ start ] == '0' ) {
				if( specValues.fillCharacter == '\0' ) {
						specValues.fillCharacter = '0';
				}
				++start;
		}
		if( (sv[ start ] >= '1' && sv[ start ] <= '9') || (sv[ start ] == '{') ) {
				VerifyWidthField(sv, start, bracketSize, argType);
		}
		if( sv[ start ] == '.' ) {
				VerifyPrecisionField(sv, start, bracketSize, argType);
		}
		if( sv[ start ] == 'L' ) {
				VerifyLocaleField(sv, start, bracketSize, argType);
		}
		if( sv[ start ] == '}' ) {
				VerifyEscapedBracket(sv, start, bracketSize);
		}
		HandlePotentialTypeField(sv, start, bracketSize, argType);
	}

	bool ArgFormatter::IsValidStringSpec(const char& spec) {
		if( spec == 's' ) {
				return true;
		} else {
				std::string throwMessage { "Arg Specifier '" };
				throwMessage += spec;
				throwMessage.append("' For A String Is Not A Valid Spec Argument\n");
				throw(std::move(throwMessage));
			}
	}

	bool ArgFormatter::IsValidIntSpec(const char& spec) {
		switch( spec ) {
				case 'b': break;
				case 'B': break;
				case 'd': break;
				case 'o': break;
				case 'x': break;
				case 'X': break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For An Int Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgFormatter::IsValidBoolSpec(const char& spec) {
		switch( spec ) {
				case 's': break;
				case 'b': break;
				case 'B': break;
				case 'c': break;
				case 'o': break;
				case 'x': break;
				case 'X': break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Bool Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgFormatter::IsValidFloatingPointSpec(const char& spec) {
		switch( spec ) {
				case 'a': break;
				case 'A': break;
				case 'e': break;
				case 'E': break;
				case 'f': break;
				case 'F': break;
				case 'g': break;
				case 'G': break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Floating Point/Double Type Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgFormatter::IsValidCharSpec(const char& spec) {
		switch( spec ) {
				case 'b': break;
				case 'B': break;
				case 'c': break;
				case 'd': break;
				case 'o': break;
				case 'x': break;
				case 'X': break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Char Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgFormatter::VerifySpec(msg_details::SpecType type, const char& spec) {
		using enum msg_details::SpecType;
		if( spec == '\0' ) return true;
		if( !IsAlpha(spec) ) {
				std::string throwMsg { "Error In Verifying Argument Specifier \"" };
				throwMsg += spec;
				throwMsg.append("\": Invalid Argument Specifier Was Provided Or Specifier Format Is Incorrect.\n");
				throw std::runtime_error(std::move(throwMsg));
		}
		switch( type ) {
				case MonoType: return true; break;
				case StringType: [[fallthrough]];
				case CharPointerType: [[fallthrough]];
				case StringViewType:
					if( IsValidStringSpec(spec) ) return true;
					break;
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType:
					if( IsValidIntSpec(spec) ) return true;
					break;
				case BoolType:
					if( IsValidBoolSpec(spec) ) return true;
					break;
				case CharType:
					if( IsValidCharSpec(spec) ) return true;
					break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType:
					if( IsValidFloatingPointSpec(spec) ) return true;
					break;
				case ConstVoidPtrType: [[fallthrough]];
				case VoidPtrType: return true; break;
				default: return false; break;
			}
		return false;
	}

	void ArgFormatter::SetLocaleForUse(const std::locale& locale) {
		loc       = std::make_unique<std::locale>(locale);
		separator = std::use_facet<std::numpunct<char>>(*loc.get()).thousands_sep();
		decimal   = std::use_facet<std::numpunct<char>>(*loc.get()).decimal_point();
		falseStr  = std::use_facet<std::numpunct<char>>(*loc.get()).falsename();
		trueStr   = std::use_facet<std::numpunct<char>>(*loc.get()).truename();
		groupings = std::use_facet<std::numpunct<char>>(*loc.get()).grouping();
	}

	void ArgFormatter::FormatIntegralGrouping(std::string& section, char separator) {
		size_t groups { 0 };
		auto groupBegin { groupings.begin() };
		int groupGap { *groupBegin };
		auto end { section.size() };
		if( section.size() < groupGap ) return;
		localeTemp.clear();
		localeTemp.reserve(section.size() + groupGap);
		std::string_view sv { section };
		if( specValues.hasAlt ) {
				sv.remove_prefix(2);
		}

		// TODO: Rework this to avoid using insert (Used insert to just get this working, but it's very slow)
		if( groupings.size() > 1 ) {
				/********************* grouping is unique *********************/
				if( groupings.size() == 3 ) {
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);
						localeTemp.insert(0, 1, separator);

						groupGap = *(++groupBegin);
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);
						localeTemp.insert(0, 1, separator);

						groupGap = *(++groupBegin);
						groups   = end / groupGap - end % groupGap;
						for( ; groups; --groups ) {
								if( groups > 0 ) {
										localeTemp.insert(0, 1, separator);
								}
								if( sv.size() > groupGap ) {
										localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
								} else {
										localeTemp.insert(0, sv);
										break;
									}
								if( sv.size() >= groupGap ) {
										sv.remove_suffix(groupGap);
								}
							}
				} else {
						// grouping is one group and then uniform
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);

						groupGap = *(++groupBegin);
						groups   = end / groupGap - end % groupGap;

						for( ; groups; --groups ) {
								if( groups > 0 ) {
										localeTemp.insert(0, 1, separator);
								}
								if( sv.size() > groupGap ) {
										localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
								} else {
										localeTemp.insert(0, sv);
										break;
									}
								if( sv.size() >= groupGap ) {
										sv.remove_suffix(groupGap);
								}
							}
					}
		} else {
				/********************* grouping is uniform *********************/
				groups = end / groupGap + end % groupGap;
				for( ; groups; --groups ) {
						if( sv.size() > groupGap ) {
								localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
						} else {
								localeTemp.insert(0, sv);
								break;
							}
						if( groups > 0 ) {
								localeTemp.insert(0, 1, separator);
						}
						if( sv.size() >= groupGap ) {
								sv.remove_suffix(groupGap);
						}
					}
			}
		if( specValues.hasAlt ) {
				localeTemp.insert(0, section.substr(0, 2));
		}
		section.clear();
		section.reserve(localeTemp.size());
		section.append(std::move(localeTemp));
	}

	void serenity::arg_formatter::ArgFormatter::FormatRawValueToStr(int& precision, msg_details::SpecType type) {
		using SpecType = msg_details::SpecType;
		switch( type ) {
				case SpecType::StringType: AppendByPrecision(std::move(argStorage.string_state(specValues.argPosition)), precision); break;
				case SpecType::CharPointerType: AppendByPrecision(std::move(argStorage.c_string_state(specValues.argPosition)), precision); break;
				case SpecType::StringViewType: AppendByPrecision(std::move(argStorage.string_view_state(specValues.argPosition)), precision); break;
				case SpecType::IntType: FormatIntTypeArg(argStorage.int_state(specValues.argPosition)); break;
				case SpecType::U_IntType: FormatIntTypeArg(argStorage.uint_state(specValues.argPosition)); break;
				case SpecType::LongLongType: FormatIntTypeArg(argStorage.long_long_state(specValues.argPosition)); break;
				case SpecType::U_LongLongType: FormatIntTypeArg(argStorage.u_long_long_state(specValues.argPosition)); break;
				case SpecType::BoolType:
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 's' ) {
							FormatIntTypeArg(static_cast<unsigned char>(argStorage.bool_state(specValues.argPosition)));
					} else {
							rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
						}
					break;
				case SpecType::CharType:
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ) {
							FormatIntTypeArg(static_cast<int>(argStorage.char_state(specValues.argPosition)));
					} else {
							rawValueTemp += argStorage.char_state(specValues.argPosition);
						}
					break;
				case SpecType::FloatType: FormatFloatTypeArg(argStorage.float_state(specValues.argPosition), precision); break;
				case SpecType::DoubleType: FormatFloatTypeArg(argStorage.double_state(specValues.argPosition), precision); break;
				case SpecType::LongDoubleType: FormatFloatTypeArg(argStorage.long_double_state(specValues.argPosition), precision); break;
				case SpecType::ConstVoidPtrType:
					{
						auto data { buffer.data() };
						charsResult =
						std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16);
						rawValueTemp.reserve(charsResult.ptr - buffer.data() + 2);
						rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
					}
					break;
				case SpecType::VoidPtrType:
					{
						auto data { buffer.data() };
						charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16);
						rawValueTemp.reserve(charsResult.ptr - buffer.data() + 2);
						rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
					}
					break;
				default: break;
			}
	}

	void serenity::arg_formatter::ArgFormatter::AppendByPrecision(std::string_view val, int precision) {
		int size { static_cast<int>(val.size()) };
		precision = precision > 0 ? precision > size ? size : precision : size;
		rawValueTemp.reserve(size);
		rawValueTemp.append(val.data(), precision);
	}

}    // namespace serenity::arg_formatter
