#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	void SpecFormatting::ResetSpecs() {
		std::memset(this, 0, sizeof(SpecFormatting));
	}

	void BracketSearchResults::Reset() {
		beginPos = endPos = 0;
	}

	ArgFormatter::ArgFormatter(const std::locale& locale)
			: argCounter(0), m_indexMode(IndexMode::automatic), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}),
			  argStorage(serenity::msg_details::ArgContainer {}), rawValueTemp(), buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}),
			  charsResult(std::to_chars_result {}), loc(nullptr) {
		loc       = std::make_unique<std::locale>(locale);
		separator = std::use_facet<std::numpunct<char>>(*loc.get()).thousands_sep();
		decimal   = std::use_facet<std::numpunct<char>>(*loc.get()).decimal_point();
		falseStr  = std::use_facet<std::numpunct<char>>(*loc.get()).falsename();
		trueStr   = std::use_facet<std::numpunct<char>>(*loc.get()).truename();
		groupings = std::use_facet<std::numpunct<char>>(*loc.get()).grouping();
	}

	static void PositionFromChars() { }

	bool ArgFormatter::ParsePositionalField(std::string_view sv, size_t& start, size_t& positionValue) {
		// we're in automatic mode
		if( m_indexMode == IndexMode::automatic ) {
				if( const auto& ch { sv[ start ] }; IsDigit(ch) ) {
						m_indexMode = IndexMode::manual;
						return ParsePositionalField(sv, start, positionValue);
				} else if( ch == '}' ) {
						positionValue = argCounter++;
						return false;
				} else if( ch == ':' ) {
						positionValue = argCounter++;
						++start;
						return true;
				} else if( ch == ' ' ) {
						for( ;; ) {
								if( start >= sv.size() ) break;
								if( sv[ start++ ] != ' ' ) break;
							}
						switch( sv[ start ] ) {
								case ':': [[fallthrough]];
								case '}':
									positionValue = argCounter++;
									++start;
									return true;
									break;
								default: throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Automatic Indexing Mode\n"); break;
							}
				}
		} else {
				// we're in manual mode
				auto data { sv.data() };
				if( auto offset { std::from_chars(data + start, data + sv.size(), positionValue).ptr - data }; offset != 0 ) {
						start = offset;
						if( positionValue < 24 ) {
								switch( sv[ start ] ) {
										case ':': [[fallthrough]];
										case '}':
											++argCounter;
											++start;
											return true;
											break;
										default:
											throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Manual Indexing Mode\n");
											break;
									}
						} else {
								throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
							}
				} else {
						switch( sv[ start ] ) {
								case '}': throw std::runtime_error("Error In Postion Field: Cannot Mix Manual And Automatic Indexing For Arguments\n"); break;
								case ' ':
									{
										auto size { sv.size() };
										for( ;; ) {
												if( start >= size ) break;
												if( sv[ ++start ] != ' ' ) break;
											}
										return ParsePositionalField(sv, start, positionValue);
										break;
									}
								case ':':
									throw std::runtime_error("Error In Position Field: Missing Positional Argument Before ':' In Manual Indexing Mode\n");
									break;
								default: throw std::runtime_error("Formatting Error Detected: Missing ':' Before Next Specifier\n"); break;
							}
					}
			}
		unreachable();
	}

	bool ArgFormatter::FindBrackets(std::string_view sv) {
		bracketResults.Reset();
		const auto svSize { sv.size() };
		for( ;; ) {
				if( bracketResults.beginPos >= svSize ) {
						return false;
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
								return false;
						}
						switch( sv[ bracketResults.endPos ] ) {
								case '{':
									for( ;; ) {
											if( ++bracketResults.endPos >= svSize ) {
													return false;
											}
											if( sv[ bracketResults.endPos ] == '}' ) break;
										}
									++bracketResults.endPos;
									continue;
									break;
								case '}': return true; break;
								default:
									++bracketResults.endPos;
									continue;
									break;
							}
					}
			}
	}

	void ArgFormatter::VerifyFillAlignField(std::string_view sv, size_t& currentPos, const msg_details::SpecType& argType) {
		using enum msg_details::SpecType;
		auto& ch { sv[ currentPos ] };
		switch( ++currentPos >= sv.size() ? sv.back() : sv[ currentPos ] ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default:
					// This sets the default behavior for the case that no alignment option is found
					specValues.fillCharacter = ' ';
					--currentPos;
					switch( argType ) {
							case IntType: [[fallthrough]];
							case U_IntType: [[fallthrough]];
							case DoubleType: [[fallthrough]];
							case FloatType: [[fallthrough]];
							case LongDoubleType: [[fallthrough]];
							case LongLongType: [[fallthrough]];
							case U_LongLongType: specValues.align = Alignment::AlignRight; return;
							default: specValues.align = Alignment::AlignLeft; return;
						}
			}
		if( ch != '{' && ch != '}' ) {
				specValues.fillCharacter = ch;
				++currentPos;
				return;
		}
		if( ch == ':' ) {
				specValues.fillCharacter = ' ';
				return;
		}
		throw std::runtime_error("Error In Fill/Align Field: Invalid Fill Character Provided\n");
	}

	void ArgFormatter::VerifySignField(const char& ch, size_t& currentPosition) {
		switch( ch ) {
				case '+': specValues.signType = Sign::Plus; break;
				case '-': specValues.signType = Sign::Minus; break;
				case ' ': specValues.signType = Sign::Space; break;
				default: specValues.signType = Sign::Empty; return;
			}
		++currentPosition;
	}

	void ArgFormatter::VerifyAltField(std::string_view sv, const msg_details::SpecType& argType) {
		using enum msg_details::SpecType;
		switch( argType ) {
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case FloatType: [[fallthrough]];
				case LongDoubleType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType: [[fallthrough]];
				case CharType: [[fallthrough]];
				case BoolType: specValues.hasAlt = true; return;
				default: throw std::runtime_error("Error In Alternate Field: Argument Type Has No Alternate Form\n"); break;
			}
	}

	void ArgFormatter::VerifyWidthField(std::string_view sv, size_t& currentPosition) {
		if( const auto& ch { sv[ currentPosition ] }; IsDigit(ch) ) {
				currentPosition = std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.alignmentPadding).ptr - sv.data();
				return;
		} else {
				switch( ch ) {
						case '{':
							{
								ParsePositionalField(sv, ++currentPosition, specValues.nestedWidthArgPos);
								return;
							}
						case '}':
							{
								ParsePositionalField(sv, currentPosition, specValues.nestedWidthArgPos);
								return;
							}
						case ' ':
							{
								auto size { sv.size() };
								for( ;; ) {
										if( currentPosition >= size ) break;
										if( sv[ ++currentPosition ] != ' ' ) break;
									}
								return VerifyWidthField(sv, currentPosition);
							}
					}
			}
	}

	void ArgFormatter::VerifyPrecisionField(std::string_view sv, size_t& currentPosition, const msg_details::SpecType& argType) {
		using enum msg_details::SpecType;
		switch( argType ) {
				case StringType: break;
				case StringViewType: break;
				case CharPointerType: break;
				case FloatType: break;
				case DoubleType: break;
				case LongDoubleType: break;
				default: throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n"); break;
			}
		if( const auto& ch { sv[ ++currentPosition ] }; IsDigit(ch) ) {
				auto data { sv.data() };
				currentPosition = std::from_chars(data + currentPosition, data + sv.size(), specValues.precision).ptr - data;
				++argCounter;
				return;
		} else {
				switch( ch ) {
						case '{':
							{
								ParsePositionalField(sv, ++currentPosition, specValues.nestedPrecArgPos);
								return;
							}
						case '}':
							{
								ParsePositionalField(sv, currentPosition, specValues.nestedPrecArgPos);
								return;
							}
						case ' ':
							{
								auto size { sv.size() };
								for( ;; ) {
										if( currentPosition >= size ) break;
										if( sv[ ++currentPosition ] != ' ' ) break;
									}
								return VerifyPrecisionField(sv, currentPosition, argType);
							}
					}
			}
	}

	void ArgFormatter::VerifyEscapedBracket(std::string_view sv, size_t& currentPosition) {
		if( (currentPosition + 1) <= (sv.size() - 2) && (sv[ currentPosition + static_cast<size_t>(1) ] == '}') ) {
				specValues.hasClosingBrace = true;
		}
	}

	void ArgFormatter::VerifyLocaleField(std::string_view sv, size_t& currentPosition, const msg_details::SpecType& argType) {
		using SpecType = msg_details::SpecType;
		++currentPosition;
		switch( argType ) {
				case SpecType::CharType: [[fallthrough]];
				case SpecType::ConstVoidPtrType: [[fallthrough]];
				case SpecType::MonoType: [[fallthrough]];
				case SpecType::StringType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::VoidPtrType: throw std::runtime_error("Error In Locale Field: Argument Type Cannot Be Localized\n"); break;
				default: specValues.localize = true; return;
			}
	}

	bool ArgFormatter::IsSimpleSubstitution(const msg_details::SpecType& argType, const int& precision) {
		using enum serenity::msg_details::SpecType;
		switch( argType ) {
				case StringType: [[fallthrough]];
				case CharPointerType: [[fallthrough]];
				case StringViewType: return precision == 0; break;
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType:
					{
						return !specValues.hasAlt && specValues.signType == Sign::Empty && !specValues.localize && specValues.typeSpec == '\0';
					}
				case BoolType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 's'); break;
				case CharType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c'); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType:
					{
						return !specValues.localize && precision == 0 && specValues.signType == Sign::Empty && !specValues.hasAlt && specValues.typeSpec == '\0';
					}
					// for pointer types, if the width field is 0, there's no fill/alignment to take into account and therefore it's a simple sub
				case ConstVoidPtrType: [[fallthrough]];
				case VoidPtrType: return true; break;
				default: return false; break;
			}
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
				case LongLongType: return LocalizeIntegral(precision, type); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType: [[fallthrough]];
				case U_LongLongType: return LocalizeFloatingPoint(precision, type); break;
				case BoolType: return LocalizeBool(); break;
			}
		unreachable();
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
				case 'X': return true; break;
				default: return false; break;
			}
	}

	static constexpr std::array<const char*, 6> TypeFieldMessages = {
		"Error In Format: Invalid Type Specifier For Int Type Argument\n",    "Error In Format: Invalid Type Specifier For Float Type Argument\n",
		"Error In Format: Invalid Type Specifier For String Type Argument\n", "Error In Format: Invalid Type Specifier For Bool Type Argument\n",
		"Error In Format: Invalid Type Specifier For Char Type Argument\n",   "Error In Format: Invalid Type Specifier For Pointer Type Argument\n",
	};
	void ArgFormatter::HandlePotentialTypeField(const char& ch, const msg_details::SpecType& argType) {
		using namespace std::literals::string_view_literals;
		using enum msg_details::SpecType;
		if( !IsAllowedSpec(ch) ) {
				switch( argType ) {
						case IntType: [[fallthrough]];
						case U_IntType: [[fallthrough]];
						case LongLongType: [[fallthrough]];
						case U_LongLongType: throw std::runtime_error(TypeFieldMessages[ 0 ]); break;
						case FloatType: [[fallthrough]];
						case DoubleType: [[fallthrough]];
						case LongDoubleType: throw std::runtime_error(TypeFieldMessages[ 1 ]); break;
						case StringType: [[fallthrough]];
						case CharPointerType: [[fallthrough]];
						case StringViewType: throw std::runtime_error(TypeFieldMessages[ 2 ]); break;
						case BoolType: throw std::runtime_error(TypeFieldMessages[ 3 ]); break;
						case CharType: throw std::runtime_error(TypeFieldMessages[ 4 ]); break;
						case ConstVoidPtrType: [[fallthrough]];
						case VoidPtrType: throw std::runtime_error(TypeFieldMessages[ 5 ]); break;
					}
		}
		specValues.typeSpec = ch;
		if( !specValues.hasAlt ) return;
		switch( argType ) {
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType: [[fallthrough]];
				case BoolType: [[fallthrough]];
				case CharType:
					{
						switch( ch ) {
								case 'b': specValues.preAltForm = "0b"sv; return;
								case 'B': specValues.preAltForm = "0B"sv; return;
								case 'o': specValues.preAltForm = "0"sv; return;
								case 'x': specValues.preAltForm = "0x"sv; return;
								case 'X': specValues.preAltForm = "0X"sv; return;
								default: return; break;
							}
					}
				default: return; break;
			}
	}

	void ArgFormatter::VerifyArgumentBracket(std::string_view sv, size_t& start, const msg_details::SpecType& argType) {
		VerifyFillAlignField(sv, start, argType);
		VerifySignField(sv[ start ], start);
		if( sv[ start ] == '#' ) {
				VerifyAltField(sv, argType);
				++start;
		}
		if( sv[ start ] == '0' ) {
				if( specValues.fillCharacter == '\0' ) {
						specValues.fillCharacter = '0';
				}
				++start;
		}
		if( (sv[ start ] == '{') || (sv[ start ] >= '1' && sv[ start ] <= '9') ) {
				VerifyWidthField(sv, start);
		}
		if( sv[ start ] == '.' ) {
				VerifyPrecisionField(sv, start, argType);
		}
		if( sv[ start ] == 'L' ) {
				VerifyLocaleField(sv, start, argType);
		}
		// clang-format off
        sv[start] != '}' ? HandlePotentialTypeField(sv[start], argType)
                                    : (IsAlpha(sv[ --start ]) && sv[ start ] != 'L')   ? HandlePotentialTypeField(sv[ start ], argType)
                                                                                                                           : VerifyEscapedBracket(sv, start);
		// clang-format on
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

	void serenity::arg_formatter::ArgFormatter::FormatRawValueToStr(int precision, const msg_details::SpecType& type) {
		using enum msg_details::SpecType;
		switch( type ) {
				case StringType:
					{
						return AppendByPrecision(std::move(argStorage.string_state(specValues.argPosition)), precision);
					}
				case CharPointerType:
					{
						return AppendByPrecision(std::move(argStorage.c_string_state(specValues.argPosition)), precision);
					}
				case StringViewType:
					{
						return AppendByPrecision(std::move(argStorage.string_view_state(specValues.argPosition)), precision);
					}
				case IntType:
					{
						return FormatIntTypeArg(argStorage.int_state(specValues.argPosition));
					}
				case U_IntType:
					{
						return FormatIntTypeArg(argStorage.uint_state(specValues.argPosition));
					}
				case LongLongType:
					{
						return FormatIntTypeArg(argStorage.long_long_state(specValues.argPosition));
					}
				case U_LongLongType:
					{
						return FormatIntTypeArg(argStorage.u_long_long_state(specValues.argPosition));
					}
				case BoolType:
					{
						if( specValues.typeSpec != '\0' && specValues.typeSpec != 's' ) {
								FormatIntTypeArg(static_cast<unsigned char>(argStorage.bool_state(specValues.argPosition)));
						} else {
								rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
							}
						return;
					}
				case CharType:
					{
						if( specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ) {
								FormatIntTypeArg(static_cast<int>(argStorage.char_state(specValues.argPosition)));
						} else {
								rawValueTemp += argStorage.char_state(specValues.argPosition);
							}
						return;
					}
				case FloatType:
					{
						return FormatFloatTypeArg(argStorage.float_state(specValues.argPosition), precision);
					}
				case DoubleType:
					{
						return FormatFloatTypeArg(argStorage.double_state(specValues.argPosition), precision);
					}
				case LongDoubleType:
					{
						return FormatFloatTypeArg(argStorage.long_double_state(specValues.argPosition), precision);
					}
				case ConstVoidPtrType:
					{
						auto data { buffer.data() };
						charsResult =
						std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16);
						rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
						return;
					}
				case VoidPtrType:
					{
						auto data { buffer.data() };
						charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16);
						rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
						return;
					}
				default: return; break;
			}
	}
	void serenity::arg_formatter::ArgFormatter::AppendByPrecision(std::string_view val, int precision) {
		int size { static_cast<int>(val.size()) };
		precision = precision > 0 ? precision > size ? size : precision : size;
		rawValueTemp.reserve(size);
		rawValueTemp.append(val.data(), precision);
	}

}    // namespace serenity::arg_formatter
