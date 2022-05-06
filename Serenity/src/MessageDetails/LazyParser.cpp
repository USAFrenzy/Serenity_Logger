#include <serenity/Common.h>
#include <serenity/MessageDetails/LazyParser.h>

namespace serenity::lazy_parser::helper {
	void LazyParseHelper::ClearBuffer() {
		std::fill(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), 0);
	}

	void LazyParseHelper::ClearPartitions() {
		openBracketPos = closeBracketPos = 0;
		partitionUpToArg.clear();
		remainder.clear();
	}

	void LazyParseHelper::SetBracketPosition(bracket_type bracket, size_t pos) {
		switch( bracket ) {
				case bracket_type::open: openBracketPos = pos; break;
				case bracket_type::close: closeBracketPos = pos; break;
				default: std::string::npos; break;
			}
	}

	size_t LazyParseHelper::BracketPosition(bracket_type bracket) const {
		switch( bracket ) {
				case bracket_type::open: return openBracketPos; break;
				case bracket_type::close: return closeBracketPos; break;
				default: return std::string::npos; break;
			}
	}

	std::array<char, SERENITY_ARG_BUFFER_SIZE>& LazyParseHelper::ConversionResultBuffer() {
		return resultBuffer;
	}

	std::string& LazyParseHelper::StringBuffer() {
		return temp;
	}

	void LazyParseHelper::SetPartition(partition_type pType, std::string_view sv) {
		switch( pType ) {
				case partition_type::primary:
					partitionUpToArg.clear();
					partitionUpToArg.append(sv.data(), sv.size());
					break;
				case partition_type::remainder:
					remainder.clear();
					remainder.append(sv.data(), sv.size());
					break;
			}
	}

	std::string& LazyParseHelper::PartitionString(partition_type pType) {
		switch( pType ) {
				case partition_type::primary: return partitionUpToArg; break;
				case partition_type::remainder: return remainder; break;
			}
		unreachable();
	}

	size_t LazyParseHelper::FindEndPos() {
		size_t pos {};
		for( ;; ) {
				if( resultBuffer[ pos ] == '\0' ) return pos;
				++pos;
			}
	}

	void LazyParseHelper::ResetBracketPositions() {
		closeBracketPos = openBracketPos = std::string_view::npos;
	}

}    // namespace serenity::lazy_parser::helper

namespace serenity::lazy_parser {

	/********************************************* BaseToken Class Functions *********************************************/
	BaseToken::BaseToken(std::string_view sv, TokenType tType, int argindex)
		: m_tokenType(tType), parseHelper(helper::LazyParseHelper {}), initialString(sv.data(), sv.size()), formattedToken(),
		  argToCapture(argindex) { }
	BaseToken::BaseToken(int argIndex)
		: m_tokenType(TokenType::Empty), parseHelper(helper::LazyParseHelper {}), initialString(), formattedToken(),
		  argToCapture(argIndex) { }
	void BaseToken::FormatToken() { }

	/******************************************** EmptyToken Class Functions ********************************************/
	EmptyToken::EmptyToken(int argIndex): BaseToken(argIndex) { }
	void EmptyToken::FormatToken() { }

	/****************************************** FillAlignToken Class Functions ******************************************/
	FillAlignToken::FillAlignToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void FillAlignToken::FormatToken() { }

	/********************************************* SignToken Class Functions *********************************************/
	SignToken::SignToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void SignToken::FormatToken() { }

	/****************************************** AlternateToken Class Functions ******************************************/
	AlternateToken::AlternateToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void AlternateToken::FormatToken() { }

	/******************************************* ZeroPadToken Class Functions *******************************************/
	ZeroPadToken::ZeroPadToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void ZeroPadToken::FormatToken() { }

	/******************************************** LocalToken Class Functions ********************************************/
	LocaleToken::LocaleToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void LocaleToken::FormatToken() { }

	/******************************************** WidthToken Class Functions ********************************************/
	WidthToken::WidthToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void WidthToken::FormatToken() { }

	/****************************************** PrecisionToken Class Functions ******************************************/
	PrecisionToken::PrecisionToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void PrecisionToken::FormatToken() { }

	/******************************************** TypeToken Class Functions ********************************************/
	TypeToken::TypeToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void TypeToken::FormatToken() { }

	/******************************************* CustomToken Class Functions *******************************************/
	CustomToken::CustomToken(std::string_view sv, TokenType tType, int argIndex): BaseToken(sv, tType, argIndex) { }
	void CustomToken::FormatToken() { }

	/**************************************** CharAggregateToken Class Functions ****************************************/
	CharAggregateToken::CharAggregateToken(std::string_view sv, TokenType tType): BaseToken(sv, tType, -1) { }
	void CharAggregateToken::FormatToken() { }

	/******************************************* Lazy Parser Class Functions *******************************************/

	bool LazyParser::IsFlagSet(TokenType& tokenFlags, TokenType checkValue) {
		return (tokenFlags & checkValue) == checkValue;
	}

	size_t LazyParser::FindDigitEnd(std::string_view sv) {
		size_t pos { 0 };
		for( ;; ) {
				auto ch { sv[ pos ] };
				if( IsDigit(ch) ) {
						++pos;
						continue;
				}
				break;
			}
		return pos;
	}

	int LazyParser::TwoDigitFromChars(std::string_view sv) {
		if( sv[ 0 ] == '-' || sv[ 0 ] == '+' ) {
				// ignore any signed designation
				sv.remove_prefix(1);
		}

		int pos { 0 }, finalValue { 0 };
		switch( sv.size() ) {
				case 0: break;
				case 1:
					for( ;; ) {
							auto ch { charDigits[ pos ] };
							if( ch == sv[ 0 ] ) break;
							if( ch > sv[ 0 ] ) {
									--pos;
									break;
							}
							pos = ((pos + 2) % static_cast<int>(charDigits.size()));
						}
					finalValue = pos;
					break;
				case 2:
					for( ;; ) {
							auto ch { charDigits[ pos ] };
							if( ch == sv[ 0 ] ) break;
							if( ch > sv[ 0 ] ) {
									--pos;
									break;
							}
							pos = ((pos + 2) % static_cast<int>(charDigits.size()));
						}
					finalValue = pos * 10;
					pos        = 0;
					for( ;; ) {
							auto ch { charDigits[ pos ] };
							if( ch == sv[ 1 ] ) break;
							if( ch > sv[ 1 ] ) {
									--pos;
									break;
							}
							pos = ((pos + 2) % static_cast<int>(charDigits.size()));
						}
					finalValue += pos;
					break;
				default:
					std::string throwMsg { "TwoDigitFromChars() Only Handles String Types Of Size 2 Or Less: \"" };
					throwMsg.append(sv.data(), sv.size()).append("\" Doesn't Adhere To This Limitation\n");
					throw std::runtime_error(std::move(throwMsg));
					break;
			}
		return finalValue;
	}

	bool LazyParser::ParsePositionalField(std::string_view& sv, int& argIndex) {
		size_t endPos { FindDigitEnd(sv) };
		if( endPos <= 2 ) {
				argIndex = TwoDigitFromChars(sv.substr(0, endPos));
				sv.remove_prefix(endPos);
		} else {
				throw std::runtime_error("Error In Positional Argument Field: Max Position (99) Exceeded\n");
			}
		endPos = 0;
		for( ;; ) {
				if( endPos >= sv.size() ) {
						endPos = std::string_view::npos;
						break;
				}
				if( sv[ endPos ] == ':' ) break;
				++endPos;
			}
		// probably add a handle to argContainer so that a comparison can
		// be made on the argIndex vs the # of args supplied
		bool moreSpecsToParse { (endPos != std::string_view::npos) && sv[ endPos + 1 ] != '}' };
		if( moreSpecsToParse ) {
				sv.remove_prefix(endPos + 1);
		} else {
				std::string throwMsg { "Error In Argument Field: Valid Argument Index Of '" };
				std::array<char, 2> buff { '\0', '\0' };
				std::to_chars(buff.data(), buff.data() + buff.size(), argIndex);
				throwMsg.append(buff.data(), buff.data() + (buff[ 1 ] == '\0' ? 1 : 2));
				throwMsg.append("' Provided But No Valid Specs Found After ':' \n ");
				throw std::runtime_error(throwMsg);
			}
		return moreSpecsToParse;
	}

	void serenity::lazy_parser::LazyParser::FindNestedBrackets(std::string_view sv, size_t& currentPos) {
		size_t pos { currentPos };

		for( ;; ) {
				// search for any nested fields
				if( pos >= sv.size() ) break;
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
								if( pos >= sv.size() ) break;
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

	bool LazyParser::FindBrackets(std::string_view sv) {
		using B_Type = helper::LazyParseHelper::bracket_type;
		using P_Type = helper::LazyParseHelper::partition_type;
		size_t pos { 0 };
		size_t subPos { 0 };
		parseHelper.ResetBracketPositions();

		// This is mainly when setting the sv to the remainder value in the Parse function
		if( sv.size() != 0 && sv[ 0 ] == '\0' ) {
				sv.remove_prefix(1);
		}
		for( ;; ) {
				if( pos >= sv.size() ) return false;
				if( sv[ pos ] != '{' ) {
						++pos;
						continue;
				}
				parseHelper.SetBracketPosition(B_Type::open, pos);
				subPos = pos + 1;
				for( ;; ) {
						if( subPos >= sv.size() ) return false;
						auto ch { sv[ subPos ] };
						if( (ch == ':' || ch == '.') && sv[ subPos + static_cast<size_t>(1) ] == '{' ) {
								FindNestedBrackets(sv, subPos);
						}
						if( sv[ subPos ] != '}' ) {
								++subPos;
								continue;
						}
						parseHelper.SetBracketPosition(B_Type::close, subPos);
						break;
					}
				break;
			}
		parseHelper.SetPartition(P_Type::primary, sv.substr(0, pos));
		parseHelper.SetPartition(P_Type::remainder, sv.substr(subPos + static_cast<size_t>(1), sv.size()));
		return true;
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

	bool LazyParser::HasValidNestedField(std::string_view& sv, NestedFieldType type, size_t index) {
		size_t primaryPos { 0 }, subPos { 0 }, argIndex { index };
		for( ;; ) {
				auto ch { sv[ primaryPos ] };
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
						std::from_chars(sv.data() + primaryPos, sv.data() + subPos, argIndex);
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
	void LazyParser::Parse(std::string_view sv) {
		using enum TokenType;
		using B_Type = helper::LazyParseHelper::bracket_type;
		using P_Type = helper::LazyParseHelper::partition_type;
		m_tokenStorage.clear();
		int autoargCounter { 0 };

		for( ;; ) {
				if( sv.size() == 0 ) return;
				m_tokenType = Empty;
				auto bracketsFound { FindBrackets(sv) };
				if( !bracketsFound ) {
						m_tokenStorage.emplace_back(std::make_unique<CharAggregateToken>(sv, CharAggregate));
						break;
				}
				auto startPos { parseHelper.BracketPosition(B_Type::open) };
				auto endPos { parseHelper.BracketPosition(B_Type::close) };

				// Found both an opening and closing bracket, so store what was found up to this
				// point and start processing the bracket for specifiers
				m_tokenStorage.emplace_back(
				std::make_unique<CharAggregateToken>(parseHelper.PartitionString(P_Type::primary), CharAggregate));

				auto argBracket { sv.substr(startPos + 1, (endPos - startPos)) };
				// placeholder
				auto originalBracket { argBracket };

				auto firstChar { argBracket[ 0 ] };
				if( firstChar == '}' ) {
						if( m_indexMode == IndexMode::manual ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						m_tokenStorage.emplace_back(std::make_unique<EmptyToken>(autoargCounter));
						++autoargCounter;
						sv = parseHelper.PartitionString(P_Type::remainder);
						continue;
				}

				if( firstChar == '{' ) {
						/*Handle Escaped Bracket*/
						m_tokenStorage.emplace_back(
						std::make_unique<CharAggregateToken>(std::string_view(&firstChar, 1), CharAggregate));
				} else if( IsDigit(firstChar) ) {
						/*Handle Positional Args*/
						if( (m_indexMode == IndexMode::automatic) && (autoargCounter != 0) ) {
								// clang-format off
								throw std::runtime_error("Cannot Mix Manual And Automatic Indexing For Arguments\n");
								// clang-format on
						}
						m_indexMode = IndexMode::manual;
						if( !ParsePositionalField(argBracket, autoargCounter) ) {
								// No more specs found so stop here, store the bracket with valid index, and
								// continue parsing sv
								m_tokenStorage.emplace_back(std::make_unique<EmptyToken>(autoargCounter));
								sv = parseHelper.PartitionString(P_Type::remainder);
								++autoargCounter;
								continue;
						}
				} else if( firstChar == ':' && argBracket.size() > 2 ) {
						// ensuring size includes at least one flag, accounting for closing '}'
						argBracket.remove_prefix(1);
				} else {
						throw std::runtime_error("Error In Argument Field: Incorrect Formatting Detected\n");
					}

				// This is for organizational purposes at the moment to make sure I cover things appropriately before making
				// things more streamlined with the switch statement below given the grammer spec to follow is:
				// [[fill]align][sign]["#"]["0"][width]["." precision]["L"][type] (libfmt/<format>'s spec)

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

				auto nextChar { argBracket[ 0 ] };
				bool isPotentialNestedWidth { argBracket.size() > 2 && nextChar == '{' };
				bool isDigitWidth { argBracket.size() > 2 && IsDigit(nextChar) };
				if( isPotentialNestedWidth ) {
						argBracket.remove_prefix(1);    // remove the '{'
						if( !HasValidNestedField(argBracket, NestedFieldType::Width, autoargCounter) ) {
								// clang-format off
								throw std::runtime_error("Error In Width Field: '{' Found With No Appropriately Matching '}'\n");
								// clang-format on
						}
						m_tokenType |= Width;
				} else if( isDigitWidth ) {
						m_tokenType |= Width;
						argBracket.remove_prefix(1);
				}

				if( argBracket.size() > 2 && argBracket[ 0 ] == '.' ) {
						argBracket.remove_prefix(1);
						bool isPotentialNestedPrecision { argBracket.size() > 2 && argBracket[ 0 ] == '{' };
						if( isPotentialNestedPrecision ) {
								argBracket.remove_prefix(1);    // remove the '{'
								if( !HasValidNestedField(argBracket, NestedFieldType::Precision, autoargCounter) )
								{
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

				// After tokenizing the arguement, need to store it in the appropriate container
				// - Storage is by libfmt/<format>'s grammer spec ordering from left to right
				// If the token contains other set flags:
				// - It should be taken care of when parsing that specific token for formatting
				// NOTE: Should add a struct that contains the values found for that token if it DOES
				//       contain other set flags to avoid re-parsing anyways...
				if( IsFlagSet(m_tokenType, FillAlign) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<FillAlignToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Sign) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<SignToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Alternate) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<AlternateToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, ZeroPad) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<ZeroPadToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Width) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<WidthToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Precision) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<PrecisionToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Locale) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<LocaleToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Type) ) {
						m_tokenStorage.emplace_back(
						std::make_unique<TypeToken>(originalBracket, m_tokenType, autoargCounter));
				} else if( IsFlagSet(m_tokenType, Custom) ) {
						// TODO: Actually implement a way to forward user spec and formatting
						// NOTE: I literally have no idea how fmtlib/<format> handles this, so
						//       this will be saved for last
						m_tokenStorage.emplace_back(
						std::make_unique<CustomToken>(originalBracket, m_tokenType, autoargCounter));
				}
				sv = parseHelper.PartitionString(P_Type::remainder);
				// This is where the above logic will flow into to be more stream-lined
				// size_t bracketPos { 0 };
				// size_t bracketSize { argBracket.size() };
				// for( ;; ) {
				//		if( bracketPos >= bracketSize ) break;
				//		auto ch { argBracket[ bracketPos ] };
				//		switch( ch ) {
				//				case '<': [[fallthrough]];
				//				case '>': [[fallthrough]];
				//				case '^': break;
				//				case '+': [[fallthrough]];
				//				case '-': [[fallthrough]];
				//				case ' ': m_tokenType |= Sign; break;
				//				case '#': m_tokenType |= Alternate; break;
				//				case '0': m_tokenType |= ZeroPad; break;
				//				case '.': m_tokenType |= Precision; break;
				//				case 'L': m_tokenType |= Locale; break;
				//				case '}': break;
				//			}
				//		++bracketPos;

				//	}    // argBracket for(;;) loop

			}    // outtermost for(;;) loop
	}

}    // namespace serenity::lazy_parser
