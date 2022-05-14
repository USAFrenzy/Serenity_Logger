#include <serenity/Common.h>
#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	bool ArgFormatter::ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start) {
		auto ch { sv[ start ] };

		// we're in automatic mode
		if( m_indexMode == IndexMode::automatic ) {
				if( ch == '}' ) {
						specValues.argPosition = argIndex++;
						return false;
				} else if( ch == ' ' ) {
						for( ;; ) {
								if( start >= sv.size() ) break;
								if( ch = sv[ start++ ] != ' ' ) break;
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
				++start;
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
		char nextCh {};
		if( currentPos + 1 < bracketSize ) {
				nextCh = sv[ currentPos + 1 ];
		}
		switch( nextCh ) {
				case '<': specValues.align = Alignment::AlignLeft; break;
				case '>': specValues.align = Alignment::AlignRight; break;
				case '^': specValues.align = Alignment::AlignCenter; break;
				default: specValues.align = Alignment::Empty; break;
			}
		if( specValues.align != Alignment::Empty ) {
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

	static bool IsValidAltTypeSpec(experimental::msg_details::SpecType type, const char& spec) {
		using SpecType = experimental::msg_details::SpecType;
		if( type == SpecType::IntType || type == SpecType::U_IntType ) {
				switch( spec ) {
						case 'b': [[fallthrough]];
						case 'B': [[fallthrough]];
						case 'o': [[fallthrough]];
						case 'x': [[fallthrough]];
						case 'X': return true; break;
						default: return false; break;
					}
		} else {
				switch( spec ) {
						case 'a': [[fallthrough]];
						case 'A': return true; break;
						default: return false; break;
					}
			}
	}

	static const char* AlternatePreFormatChars(experimental::msg_details::SpecType type, const char& spec) {
		using SpecType = experimental::msg_details::SpecType;
		if( type == SpecType::IntType || type == SpecType::U_IntType ) {
				switch( spec ) {
						case 'b': return "0b"; break;
						case 'B': return "0B"; break;
						case 'o': return "0"; break;
						case 'x': return "0x"; break;
						case 'X': return "0X"; break;
						default: break;
					}
		} else {
				switch( spec ) {
						case 'a': return "0x"; break;
						case 'A': return "0X"; break;
						default: break;
					}
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

	void ArgFormatter::VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type) {
		auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;

		auto ch { sv[ ++currentPosition ] };
		if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						if( type == NestedFieldType::Prec ) {
								SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
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
						VerifyNestedBracket(sv, --currentPosition, bracketSize, type);
					}
		}
		++currentPosition;
	}

	void ArgFormatter::VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
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
		} else if( ch != '}' ) {
				throw std::runtime_error("Error In Width Field: Invalid Format Detected.\n");
		}
	}

	void ArgFormatter::VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize) {
		auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;

		auto ch { sv[ ++currentPosition ] };
		if( ch == '{' ) {
				VerifyNestedBracket(sv, currentPosition, bracketSize, NestedFieldType::Prec);
				return;
		} else if( ch == '}' ) {
				if( m_indexMode == IndexMode::automatic ) {
						SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
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
		SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
		if( argType == SpecType::IntType ) {
				throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n");
		}
		if( IsDigit(ch) ) {
				auto& precision { specValues.precision };
				std::from_chars(sv.data(), sv.data() + sv.size(), precision);
				currentPosition += precision > 9 ? precision >= 100 ? 3 : 2 : 1;
				++argCounter;
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

	void ArgFormatter::VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec) {
		const auto& specStorage { argStorage.SpecTypesCaptured() };
		const auto& cSize { specStorage.size() };
		using SpecType = experimental::msg_details::SpecType;
		SpecType argType { specValues.argPosition <= cSize ? specStorage[ specValues.argPosition ] : SpecType::MonoType };
		if( VerifySpec(argType, spec) ) {
				++currentPosition;
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
		// If this case returns true, then we've already handled the type spec in the alternate form field
		if( specValues.typeSpec != '\0' ) {
				++currentPosition;
				return;
		}
		if( currentPosition >= bracketSize ) return;
		auto ch { sv[ currentPosition ] };
		switch( sv[ currentPosition + 1 ] ) {
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
				default: throw std::runtime_error("Error In Argument Format Field: Unable To Verify Potential Type Specifier - No Closing '}' Found \n"); break;
			}
		++currentPosition;
		return;
	}

	void ArgFormatter::VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize) {
		if( (bracketSize <= 1) || (start >= (bracketSize - 1)) ) return;
		VerifyFillAlignField(sv, start, bracketSize);
		VerifySignField(sv, start, bracketSize);
		if( sv[ start ] == '#' ) {
				VerifyAltField(sv, start, bracketSize);
		}
		if( sv[ start ] == '0' ) {
				if( specValues.fillCharacter == '\0' ) {
						specValues.fillCharacter = '0';
				}
				++start;
		}
		if( (sv[ start ] >= '1' && sv[ start ] <= '9') || (sv[ start ] == '{') ) {
				VerifyWidthField(sv, start, bracketSize);
		}
		if( sv[ start ] == '.' ) {
				VerifyPrecisionField(sv, start, bracketSize);
		}
		if( sv[ start ] == 'L' ) {
				VerifyLocaleField(sv, start, bracketSize);
		}
		if( sv[ start ] == '}' ) {
				VerifyEscapedBracket(sv, start, bracketSize);
		}
		HandlePotentialTypeField(sv, start, bracketSize);
	}

	void ArgFormatter::FormatRawValueToStr(int& precision) {
		using SpecType = experimental::msg_details::SpecType;
		switch( argStorage.SpecTypesCaptured()[ specValues.argPosition ] ) {
				case SpecType::StringType: AppendByPrecision(argStorage.string_state(specValues.argPosition), precision); break;
				case SpecType::CharPointerType: AppendByPrecision(argStorage.c_string_state(specValues.argPosition), precision); break;
				case SpecType::StringViewType: AppendByPrecision(argStorage.string_view_state(specValues.argPosition), precision); break;
				case SpecType::IntType: FormatIntTypeArg(argStorage.int_state(specValues.argPosition)); break;
				case SpecType::U_IntType: FormatIntTypeArg(argStorage.uint_state(specValues.argPosition)); break;
				case SpecType::LongLongType:
					charsResult = std::to_chars(buffer.data(), buffer.data() + buffer.size(), argStorage.long_long_state(specValues.argPosition));
					rawValueTemp.append(buffer.data(), charsResult.ptr);
					break;
				case SpecType::U_LongLongType:
					charsResult = std::to_chars(buffer.data(), buffer.data() + buffer.size(), argStorage.u_long_long_state(specValues.argPosition));
					rawValueTemp.append(buffer.data(), charsResult.ptr);
					break;
				case SpecType::BoolType: rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false"); break;
				case SpecType::CharType: rawValueTemp.append(argStorage.char_state(specValues.argPosition), 1); break;
				case SpecType::FloatType: FormatFloatTypeArg(argStorage.float_state(specValues.argPosition), precision); break;
				case SpecType::DoubleType: FormatFloatTypeArg(argStorage.double_state(specValues.argPosition), precision); break;
				case SpecType::LongDoubleType: FormatFloatTypeArg(argStorage.long_double_state(specValues.argPosition), precision); break;
				case SpecType::ConstVoidPtrType: break;
				case SpecType::VoidPtrType: break;
				default: break;
			}
	}

	void ArgFormatter::FormatIntTypeArg(int&& value) {
		int base { 10 };
		auto data { buffer.data() };
		int pos { 0 };

		if( specValues.signType == Sign::Space ) {
				specValues.signType = value < 0 ? Sign::Minus : Sign::Plus;
		}
		switch( specValues.signType ) {
				case Sign::Space:
					data[ pos ] = ' ';
					++pos;
					break;
				case Sign::Plus:
					data[ pos ] = '+';
					++pos;
					break;
				case Sign::Empty: [[fallthrough]];
				case Sign::Minus: break;
			}

		if( specValues.preAltForm != "\0" ) {
				size_t i { 0 };
				auto size { specValues.preAltForm.size() };
				for( auto& ch: specValues.preAltForm ) {
						data[ pos ] = ch;
						++pos;
					}
		}

		switch( specValues.typeSpec ) {
				case 'b': [[fallthrough]];
				case 'B': base = 2; break;
				case 'o': base = 8; break;
				case 'x': [[fallthrough]];
				case 'X': base = 16; break;
				default: break;
			}

		charsResult = std::to_chars(data + pos, data + buffer.size(), value, base);

		switch( specValues.typeSpec ) {
				case 'B': [[fallthrough]];
				case 'X':
					for( auto& ch: buffer ) {
							if( ch == *charsResult.ptr ) break;
							if( ch >= 'a' && ch <= 'z' ) {
									ch -= 32;
							}
						}
					break;
				default: break;
			}    // capitilization
		rawValueTemp.append(data, charsResult.ptr);
	}

	void ArgFormatter::AppendByPrecision(std::string_view val, int precision) {
		int size { static_cast<int>(val.size()) };
		precision = precision > 0 ? precision > size ? size : precision : size;
		rawValueTemp.append(val.data(), precision);
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
				case 'c': break;
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

	bool ArgFormatter::VerifySpec(experimental::msg_details::SpecType type, const char& spec) {
		using enum experimental::msg_details::SpecType;
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
		groupings = std::use_facet<std::numpunct<char>>(*loc.get()).grouping();
		separator = std::use_facet<std::numpunct<char>>(*loc.get()).thousands_sep();
	}

	void serenity::arg_formatter::ArgFormatter::LocalizeArgument(int precision) {
		// TODO: Implement Logic To Decide HOW an argument is supposed to be localized:
		//       whether it's a numerical localization, a boolean localization, etc
		using enum serenity::experimental::msg_details::SpecType;
		auto argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
		// NOTE: The following types should have been caught in the verification process:
		//       monostate, string, c-string, string view, const void*, void *
		switch( argType ) {
				case IntType: break;
				case U_IntType: break;
				case LongLongType: break;
				case U_LongLongType: break;
				case BoolType: break;
				case CharType: break;
				case FloatType: break;
				case DoubleType: break;
				case LongDoubleType: break;
				default: break;
			}

		FormatRawValueToStr(precision);

		// integer localization
		size_t groupGap { 0 };
		size_t groups { 0 };
		auto end { rawValueTemp.size() };
		if( groupings.size() > 1 ) {
				/********************* grouping is unique *********************/
				// TODO: FINISH IMPLEMENTING LOGIC HERE BASED ON UNIQUE GROUPINGS
				groupGap = *groupings.begin();
		} else {
				/********************* grouping is uniform *********************/
				// NOTE: this could have been simplified if taking a string_view of the rawValueTemp after FormatRawValueToStr() was called hadn't resulted in
				// the random replacement of the first char with a null character when moving the string to the sv before clearing the string. The thought was
				// to just directly append the values in the grouping range to the string and call sv.remove_prefix(groupGap); This would have eliminated the
				// additional sv constructions in the loop and the additional string allocation for 'tmp'. Would still like to find a better way to achieve this.
				groupGap = *groupings.begin();
				groups   = (end / groupGap);
				std::string tmp;
				for( int i { 0 }; i < groups; ++i ) {
						std::string_view sv { rawValueTemp };
						tmp.append(sv.data(), sv.data() + groupGap);
						if( sv.size() > groupGap ) {
								tmp += separator;
						}
						rawValueTemp.erase(groupGap);
					}
				rawValueTemp = std::move(tmp);
			}
	}
}    // namespace serenity::arg_formatter
