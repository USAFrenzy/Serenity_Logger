#include <serenity/MessageDetails/Message_Formatter.h>

#include <iostream>

namespace serenity::msg_details {

	void LazyParseHelper::ClearBuffer() {
		std::fill(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), 0);
	}

	void serenity::msg_details::LazyParseHelper::ClearPartitions() {
		openBracketPos = closeBracketPos = 0;
		partitionUpToArg.clear();
		remainder.clear();
	}

	void LazyParseHelper::SetBracketPosition(bracket_type bracket, const size_t& pos) {
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

	const std::to_chars_result LazyParseHelper::ConversionResultInfo() const {
		return result;
	}

	std::string& LazyParseHelper::StringBuffer() {
		return temp;
	}

	void LazyParseHelper::SetConversionResult(const std::to_chars_result& convResult) {
		result = convResult;
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
				default:
					temp.clear();
					return temp;
					break;
			}
	}

	const std::vector<ArgContainer::LazilySupportedTypes>& ArgContainer::ArgStorage() const {
		return argContainer;
	}

	LazyParseHelper& ArgContainer::ParseHelper() {
		return parseHelper;
	}

	void ArgContainer::Reset() {
		argContainer.clear();
		argSpecTypes.clear();
		argIndex = maxIndex = 0;
		endReached          = false;
	}

	size_t ArgContainer::AdvanceToNextArg() {
		if( argIndex < maxIndex ) {
				++argIndex;
		} else {
				endReached = true;
			}
		return argIndex;
	}

	bool ArgContainer::IsValidStringSpec(char spec) {
		if( spec == 's' ) {
				argSpecValue.emplace_back(SpecValue::s);
				return true;
		} else {
				std::string throwMessage { "Arg Specifier '" };
				throwMessage += spec;
				throwMessage.append("' For A String Is Not A Valid Spec Argument\n");
				throw(std::move(throwMessage));
			}
	}

	bool ArgContainer::IsValidIntSpec(char spec) {
		switch( spec ) {
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'd': argSpecValue.emplace_back(SpecValue::d); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For An Int Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgContainer::IsValidBoolSpec(char spec) {
		switch( spec ) {
				case 's': argSpecValue.emplace_back(SpecValue::s); break;
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Bool Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgContainer::IsValidFloatingPointSpec(char spec) {
		switch( spec ) {
				case 'a': argSpecValue.emplace_back(SpecValue::a); break;
				case 'A': argSpecValue.emplace_back(SpecValue::A); break;
				case 'e': argSpecValue.emplace_back(SpecValue::e); break;
				case 'E': argSpecValue.emplace_back(SpecValue::E); break;
				case 'f': argSpecValue.emplace_back(SpecValue::f); break;
				case 'F': argSpecValue.emplace_back(SpecValue::F); break;
				case 'g': argSpecValue.emplace_back(SpecValue::g); break;
				case 'G': argSpecValue.emplace_back(SpecValue::G); break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Floating Point/Double Type Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	bool ArgContainer::IsValidCharSpec(char spec) {
		switch( spec ) {
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'd': argSpecValue.emplace_back(SpecValue::d); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default:
					std::string throwMessage { "Arg Specifier '" };
					throwMessage += spec;
					throwMessage.append("' For A Char Is Not A Valid Spec Argument\n");
					throw(std::move(throwMessage));
					break;
			}
		return true;
	}

	void Message_Formatter::EnableFallbackToStd(bool enable) {
		argStorage.EnableFallbackToStd(enable);
	}

	void ArgContainer::EnableFallbackToStd(bool enable) {
		isStdFallbackEnabled = enable;
	}

	void ArgContainer::HandlePrecisionSpec(size_t index, SpecType type) {
		finalArgValue.clear();
		switch( type ) {
				case SpecType::CharPointerType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::StringType:
					throw std::runtime_error("Fallback Not Yet Implemented\n");
					break;    // Still Need To Implement
				case SpecType::FloatType: [[fallthrough]];
				case SpecType::DoubleType: [[fallthrough]];
				case SpecType::LongDoubleType:
					finalArgValue.append(std::move(GetArgValue(index, precisionSpecHelper.spec)));
					break;    // Still Need To Implement
				default: break;
			}
	}

	void ArgContainer::SplitPrecisionAndSpec(size_t index, SpecType type, std::string_view spec) {
		precisionSpecHelper.precision.clear();
		precisionSpecHelper.spec = '\0';
		for( ;; ) {
				if( spec.size() == 0 ) break;
				auto ch { spec.front() };
				if( IsDigit(ch) ) {
						precisionSpecHelper.precision += spec.front();
						spec.remove_prefix(1);
						continue;
				}

				if( IsAlpha(ch) ) {
						using enum SpecType;
						if( type == StringType || type == CharPointerType || type == StringViewType ) break;
						precisionSpecHelper.spec = spec.front();
						break;
				}
				// possibly nested field
				if( ch == '{' ) {
						// implement nested field logic here
				}

				spec.remove_prefix(1);
			}

		if( precisionSpecHelper.precision.size() == 0 ) {
				std::cerr << "Error In Precision Field: No Precision Was Found\n";
		}
	}

	bool ArgContainer::VerifySpecWithPrecision(size_t index, SpecType type, std::string_view spec) {
		switch( type ) {
				case SpecType::CharPointerType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::StringType:
					if( !isStdFallbackEnabled ) {
							// clang-format off
							std::cerr <<  "Warning: Precision Field For String Types Not Currently Supported Natively.\n"
					                                   "Set \"EnableFallBackToStd(true)\" To Let The Standard Deal With This Argument.\n";
							// clang-format on
							return false;
					}
					break;
				default: break;
			}
		spec.remove_prefix(1);    // remove '.'
		SplitPrecisionAndSpec(index, type, spec);
		bool isValidSpec { VerifySpec(type, precisionSpecHelper.spec) && (precisionSpecHelper.precision.size() != 0) };
		return isValidSpec;
	}

	bool ArgContainer::VerifySpec(SpecType type, char spec) {
		using enum SpecType;
		if( spec == '\0' ) return true;
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
				case VoidPtrType:
					argSpecValue.emplace_back(SpecValue::p);
					return true;
					break;
				default: return false; break;
			}
		return false;
	}

	void ArgContainer::HandleWidthSpec(char spec) {
		// TODO: implement this fully
	}

	static constexpr std::array<char, 3> faSpecs   = { '<', '>', '^' };
	static constexpr std::array<char, 3> signSpecs = { '+', '-', ' ' };

	bool ArgContainer::VerifyIfFillAndAlignSpec(size_t index, SpecType type, std::string_view specView) {
		size_t pos { 0 };
		fillAlignValues.Reset();
		auto argBracketSize { specView.size() };

		for( ;; ) {
				if( pos >= argBracketSize - 1 ) break;
				auto ch { specView[ pos ] };

				// deal with padding spec
				if( IsDigit(ch) ) {
						fillAlignValues.temp.clear();
						fillAlignValues.temp += ch;
						auto tempPos { pos + 1 };
						for( ;; ) {
								if( tempPos >= argBracketSize ) break;
								auto nextCh { specView[ tempPos ] };
								if( IsDigit(nextCh) ) {
										fillAlignValues.temp += nextCh;
										++tempPos;
								} else {
										break;
									}
							}
						if( fillAlignValues.temp.size() > 2 ) {
								// clang-format off
								throw std::runtime_error("Digit Spec For Fill/Align Cannot Be Greater Than Two "
								                         "Digits\n");
								// clang-format on
						}
						std::from_chars(fillAlignValues.temp.data(),
						                fillAlignValues.temp.data() + fillAlignValues.temp.size(),
						                fillAlignValues.digitSpec);
						pos = tempPos;
						continue;
				}

				// Specifically for additional specifiers in the type-field
				if( IsAlpha(ch) || (ch == '}') ) {
						auto tempPos { pos + 1 };
						char nextCh;
						if( tempPos <= argBracketSize ) nextCh = specView[ tempPos ];
						if( !std::any_of(faSpecs.begin(), faSpecs.end(), [ & ](char chSp) { return nextCh == chSp; }) ) {
								if( ch == '}' ) break;
								if( VerifySpec(type, ch) ) fillAlignValues.additionalSpec = ch;
						}
						if( nextCh == '}' ) break;
						++pos;
						continue;
				}

				if( ch == '.' ) {
						// this is where presision should be handled (WIP)
						if( VerifySpecWithPrecision(index, type, specView.substr(pos, specView.size())) ) {
								fillAlignValues.additionalSpec = precisionSpecHelper.spec;
						}
						pos += (precisionSpecHelper.precision.size());
						if( precisionSpecHelper.spec != '\0' ) {
								++pos;
						}
						++pos;    // this one specifically to account for '.'
						continue;
				}

				// Deal with fill spec and alignment spec
				if( ch != '{' && ch != '}' ) {
						char nextCh { '<' };
						if( (pos + 1) <= argBracketSize ) nextCh = specView[ pos + 1 ];
						if( nextCh == '}' ) nextCh = '<';
						if( std::any_of(faSpecs.begin(), faSpecs.end(), [ & ](char chSp) { return nextCh == chSp; }) ) {
								fillAlignValues.fillSpec      = ch;
								fillAlignValues.fillAlignSpec = nextCh;
								pos += 2;
								continue;
						}
				}

				std::string throwMsg { "Error In Fill And Align Field: Invalid Specifier \"" };
				throwMsg += ch;
				throwMsg.append("\" Found\n");
				throw std::runtime_error(std::move(throwMsg));
			}    // for loop
		return (fillAlignValues.digitSpec != 0);
	}

	void ArgContainer::HandleSignSpec(char spec) {
		// TODO: implement this fully
	}

	void ArgContainer::HandleHashSpec(char spec) {
		// TODO: implement this fully
	}

	// spec standard
	//  [fill - and -align(optional) sign(optional) #(optional)0(optional)width(optional) precision(optional) L(optional) type(optional)]

	std::string&& ArgContainer::AlignLeft(size_t index) {
		fillAlignValues.temp.clear();
		fillAlignValues.temp.append(std::move(GetArgValue(index, fillAlignValues.additionalSpec)));
		auto fillAmount { (fillAlignValues.digitSpec - fillAlignValues.temp.size()) };
		if( fillAlignValues.fillSpec != '\0' ) {
				for( int i { 0 }; i < fillAmount; ++i ) {
						fillAlignValues.temp += fillAlignValues.fillSpec;
					}
		} else {
				for( int i { 0 }; i < fillAmount; ++i ) {
						fillAlignValues.temp += ' ';
					}
			}
		return std::move(fillAlignValues.temp);
	}

	std::string&& ArgContainer::AlignRight(size_t index) {
		auto argValue { std::move(GetArgValue(index, fillAlignValues.additionalSpec)) };
		fillAlignValues.temp.clear();
		if( fillAlignValues.fillSpec != '\0' ) {
				for( int i { 0 }; i < (fillAlignValues.digitSpec - argValue.size()); ++i ) {
						fillAlignValues.temp += fillAlignValues.fillSpec;
					}
		} else {
				for( int i { 0 }; i < (fillAlignValues.digitSpec - argValue.size()); ++i ) {
						fillAlignValues.temp += ' ';
					}
			}
		fillAlignValues.temp.append(std::move(argValue));
		return std::move(fillAlignValues.temp);
	}

	std::string&& ArgContainer::AlignCenter(size_t index) {
		auto argValue { std::move(GetArgValue(index, fillAlignValues.additionalSpec)) };
		fillAlignValues.temp.clear();
		if( fillAlignValues.fillSpec != '\0' ) {
				for( int i { 0 }; i < (fillAlignValues.digitSpec - argValue.size()) / 2; ++i ) {
						fillAlignValues.temp += fillAlignValues.fillSpec;
					}
		} else {
				for( int i { 0 }; i < (fillAlignValues.digitSpec - argValue.size()) / 2; ++i ) {
						fillAlignValues.temp += ' ';
					}
			}
		auto fillPartition { fillAlignValues.temp };
		fillAlignValues.temp.append(std::move(argValue)).append(std::move(fillPartition));
		return std::move(fillAlignValues.temp);
	}

	std::string&& ArgContainer::ParseForSpecifiers(std::string_view& fmt) {
		// clang-format off
		// ******************************************** WIP ********************************************
		// clang-format on
		auto argCounter { argIndex };
		finalArgValue.clear();
		if( fmt.size() == 0 ) {
				return std::move(finalArgValue);
		}

		using B_Type = LazyParseHelper::bracket_type;
		using P_Type = LazyParseHelper::partition_type;
		parseHelper.ClearPartitions();
		parseHelper.SetBracketPosition(B_Type::open, fmt.find_first_of(static_cast<char>('{')));
		parseHelper.SetBracketPosition(B_Type::close, fmt.find_first_of(static_cast<char>(' }')));
		// clang-format off
		// no substitution needed so return
		if( (parseHelper.BracketPosition(B_Type::open) == std::string_view::npos) || (parseHelper.BracketPosition(B_Type::close) == std::string_view::npos) )
		{
			return std::move(finalArgValue);
		}
		// clang-format off
		auto argBracket { std::move(fmt.substr(parseHelper.BracketPosition(B_Type::open) + 1, parseHelper.BracketPosition(B_Type::close) + 1) )};
		// clang-format on

		// Handle the case of nested brackets
		if( fmt[ parseHelper.BracketPosition(B_Type::open) + 1 ] == '{' && fmt[ parseHelper.BracketPosition(B_Type::close) + 1 ] == '}' )
		{
				return std::move(finalArgValue);    // return for now
		}

		// handle empty arg brackets no matter the amount of whitespace,
		// but skip the processing step if it only contains whitespace
		auto emptyArg { true };
		size_t pos { 0 };
		char firstToken;
		for( ;; ) {
				if( argBracket[ pos ] == '}' ) break;
				if( ((pos >= argBracket.size() - 1) || (argBracket[ pos ] != ' ')) ) {
						firstToken = argBracket[ pos ];
						emptyArg   = false;
						break;
				}
				++pos;
			}
		if( emptyArg ) {
				// fmt.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
				finalArgValue.append(std::move(GetArgValue(argCounter)));
				fmt.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
				return std::move(finalArgValue);
		}

		// handle positional arg type here and then continue
		SpecType argT;
		if( IsDigit(firstToken) ) {
				auto initialPos { pos };
				++pos;
				for( ;; ) {
						if( !IsDigit(argBracket[ pos ]) ) break;
						++pos;
					}
				std::from_chars(argBracket.data() + initialPos, argBracket.data() + pos, argCounter);
				if( argCounter <= argSpecTypes.size() ) {
						argT = argSpecTypes.at(argCounter);
						argBracket.remove_prefix(pos);
				} else {
						std::array<char, 2> buff { '\0', '\0' };
						std::string throwMsg { "Positional Argument \"" };
						std::to_chars(buff.data(), buff.data() + buff.size(), argCounter);
						auto endPos { buff[ 1 ] == '\0' ? 1 : 2 };
						throwMsg.append(buff.data(), endPos);
						throwMsg.append("\" Exceeds The Number Of Arguments Supplied. ");
						buff[ 0 ] = buff[ 1 ] = '\0';
						std::to_chars(buff.data(), buff.data() + buff.size(), argContainer.size());
						endPos = buff[ 1 ] == '\0' ? 1 : 2;
						throwMsg.append("Number Of Arguments Supplied: \"");
						throwMsg.append(buff.data(), endPos).append("\"\n");
						throw std::runtime_error(std::move(throwMsg));
					}
		} else {
				argT = argSpecTypes.at(0);
				if( argT == SpecType::MonoType ) {
						finalArgValue.append(std::move(GetArgValue(argCounter)));
						fmt.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
						return std::move(finalArgValue);
				}
			}

		// May be a valid spec field but user forgot ':'.
		// Don't need to worry if the arg bracket is empty as that should have been caught earlier up and skipped.
		if( (argBracket[ 0 ] != ':') && (argBracket[ 0 ] != '}') ) {
				std::string throwMsg { "Missing ':' In Argument Specifier Field For Argument " };
				std::array<char, 2> buff { '\0', '\0' };
				std::to_chars(buff.data(), buff.data() + 2, (argCounter + 1));
				throwMsg.append(buff.data(), buff.size()).append("\n");
				throw std::runtime_error(std::move(throwMsg));
		} else if( argBracket[ 0 ] == '}' ) {
				// Otherwise, if we reached the end, get the supposed value and return
				finalArgValue.append(std::move(GetArgValue(argCounter)));
				fmt.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
				return std::move(finalArgValue);
		} else {
				// else remove the next char and continue onward
				argBracket.remove_prefix(1);
			}

		// Parse the rest of the argument bracket
		for( ;; ) {
				firstToken = argBracket[ 0 ];
				switch( firstToken ) {
						case '+': [[fallthrough]];
						case '-': [[fallthrough]];
						case ' ': HandleSignSpec(firstToken); break;
						case '#': HandleHashSpec(firstToken); break;
						case '0': /*HandleZeroPadding();*/ break;
						case '.':
							if( VerifySpecWithPrecision(argCounter, argT, argBracket) ) {
									HandlePrecisionSpec(argCounter, argT);
									return std::move(finalArgValue);
							}
							break;
						case 'L': /*LocaleFallBack() */ break;
						default:
							bool isFirstTokenDigit { IsDigit(firstToken) };
							auto nextToken = argBracket[ 1 ];
							bool isFASpec { std::any_of(faSpecs.begin(), faSpecs.end(),
								                    [ & ](auto ch) { return nextToken == ch; }) };
							bool isFAFillSpec { (firstToken != '{') && (firstToken != '}') };
							if( isFAFillSpec && isFASpec ) {
									if( nextToken == '}' ) nextToken = '<';
									if( VerifyIfFillAndAlignSpec(argCounter, argT, argBracket) ) {
											// clang-format off
										tempStorage.clear();
													switch( fillAlignValues.fillAlignSpec ) {
															case '<':  tempStorage.append(std::move(AlignLeft(argCounter))); break;
															case '>': tempStorage.append(std::move(AlignRight(argCounter))); break;
															case '^': tempStorage.append(std::move(AlignCenter(argCounter))); break;
															default: break;
														}
													finalArgValue.append(std::move(tempStorage));
													return std::move(finalArgValue);
													// add to formatted string here when set
											// clang-format on
									}
							} else if( isFirstTokenDigit ) {
									HandleWidthSpec(firstToken);
							} else {
									VerifySpec(argT, firstToken);
								}
							break;
					}    // switch statement

				// Remove partition up to the first arg processed
				fmt.remove_prefix(fmt.find_first_of('}') + 1);
				break;
			}    // argument bracket processing for-loop
		finalArgValue.append(std::move(GetArgValue(argCounter)));
		return std::move(finalArgValue);

	}    // parsing fmt loop

	// clang-format off
		 // ******************************************** WIP ********************************************
	// clang-format on

	size_t LazyParseHelper::FindEndPos() {
		size_t pos {};
		for( ;; ) {
				if( resultBuffer[ pos ] == '\0' ) return pos;
				++pos;
			}
	}

	/*************************************** Variant Order *******************************************
	 * [0] std::monostate, [1] std::string, [2] const char*, [3] std::string_view, [4] int,
	 * [5] unsigned int, [6] long long, [7] unsigned long long, [8] bool, [9] char, [10] float,
	 * [11] double, [12] long double, [13] const void* [14] void*
	 ************************************************************************************************/
	std::string&& ArgContainer::GetArgValue(size_t positionIndex, char additionalSpec) {
		auto& strRef { parseHelper.StringBuffer() };
		strRef.clear();
		parseHelper.ClearBuffer();
		// need a proper way to find the type or the spec index that maps to the location in argContainer
		auto& arg { argContainer[ positionIndex ] };
		auto& buffer { parseHelper.ConversionResultBuffer() };
		auto& result { parseHelper.ConversionResultInfo() };

		switch( arg.index() ) {
				case 0: return std::move(strRef); break;
				case 1: return std::move(strRef.append(std::move(std::get<1>(arg)))); break;
				case 2: return std::move(strRef.append(std::move(std::get<2>(arg)))); break;
				case 3: return std::move(strRef.append(std::move(std::get<3>(arg)))); break;
				case 4:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<4>(arg))));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 5:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<5>(arg))));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 6:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<6>(arg))));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 7:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<7>(arg))));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 8:
					strRef.append(std::move(std::get<8>(arg)) == true ? "true" : "false");
					return std::move(strRef);
					break;
				case 9:
					strRef += std::move(std::move(std::get<9>(arg)));
					return std::move(strRef);
					break;
				case 10:
					FormatFloatTypeArg(additionalSpec, std::move(std::get<10>(arg)));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 11:
					FormatFloatTypeArg(additionalSpec, std::move(std::get<11>(arg)));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 12:
					FormatFloatTypeArg(additionalSpec, std::move(std::get<12>(arg)));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 13:
					parseHelper.SetConversionResult(std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					                                              reinterpret_cast<size_t>(std::move(std::get<13>(arg))), 16));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append("0x").append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 14:
					parseHelper.SetConversionResult(std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					                                              reinterpret_cast<size_t>(std::move(std::get<14>(arg))), 16));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append("0x").append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				default: return std::move(strRef); break;
			}
	}

	bool ArgContainer::EndReached() const {
		return endReached;
	}

	bool ArgContainer::ContainsUnsupportedType() const {
		return containsUnknownType;
	}

	void Message_Formatter::SetLocaleReference(std::locale* loc) {
		localeRef = *&loc;
	}

	Message_Formatter::Message_Formatter(std::string_view pattern, Message_Info* details)
		: msgInfo(*&details), localeRef(nullptr), sourceFlag(source_flag::empty) {
		SetPattern(pattern);
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
	}

	void Message_Formatter::FlagFormatter(size_t index, size_t precision) {
		switch( index ) {
				case 0: formatter.Emplace_Back(std::make_unique<Format_Arg_a>(*msgInfo)); break;
				case 1: formatter.Emplace_Back(std::make_unique<Format_Arg_b>(*msgInfo)); break;
				case 2: formatter.Emplace_Back(std::make_unique<Format_Arg_c>(*msgInfo)); break;
				case 3: formatter.Emplace_Back(std::make_unique<Format_Arg_d>(*msgInfo)); break;
				case 4: formatter.Emplace_Back(std::make_unique<Format_Arg_e>(precision)); break;
				case 5: formatter.Emplace_Back(std::make_unique<Format_Arg_b>(*msgInfo)); break;
				case 6: formatter.Emplace_Back(std::make_unique<Format_Arg_l>(*msgInfo)); break;
				case 7: formatter.Emplace_Back(std::make_unique<Format_Arg_m>(*msgInfo)); break;
				case 8: formatter.Emplace_Back(std::make_unique<Format_Arg_n>(*msgInfo)); break;
				case 9: formatter.Emplace_Back(std::make_unique<Format_Arg_p>(*msgInfo)); break;
				case 10: formatter.Emplace_Back(std::make_unique<Format_Arg_r>(*msgInfo)); break;
				case 11: formatter.Emplace_Back(std::make_unique<Format_Arg_s>(*msgInfo, sourceFlag)); break;
				case 12: formatter.Emplace_Back(std::make_unique<Format_Arg_t>(precision)); break;
				case 13: formatter.Emplace_Back(std::make_unique<Format_Arg_w>(*msgInfo)); break;
				case 14: formatter.Emplace_Back(std::make_unique<Format_Arg_D>(*msgInfo)); break;
				case 15: formatter.Emplace_Back(std::make_unique<Format_Arg_y>(*msgInfo)); break;
				case 16: formatter.Emplace_Back(std::make_unique<Format_Arg_z>(*msgInfo)); break;
				case 17: formatter.Emplace_Back(std::make_unique<Format_Arg_A>(*msgInfo)); break;
				case 18: formatter.Emplace_Back(std::make_unique<Format_Arg_B>(*msgInfo)); break;
				case 19: formatter.Emplace_Back(std::make_unique<Format_Arg_C>(*msgInfo)); break;
				case 20: formatter.Emplace_Back(std::make_unique<Format_Arg_D>(*msgInfo)); break;
				case 21: formatter.Emplace_Back(std::make_unique<Format_Arg_F>(*msgInfo)); break;
				case 22: formatter.Emplace_Back(std::make_unique<Format_Arg_H>(*msgInfo)); break;
				case 23: formatter.Emplace_Back(std::make_unique<Format_Arg_I>(*msgInfo)); break;
				case 24: formatter.Emplace_Back(std::make_unique<Format_Arg_L>(*msgInfo)); break;
				case 25: formatter.Emplace_Back(std::make_unique<Format_Arg_M>(*msgInfo)); break;
				case 26: formatter.Emplace_Back(std::make_unique<Format_Arg_N>(*msgInfo)); break;
				case 27: formatter.Emplace_Back(std::make_unique<Format_Arg_R>(*msgInfo)); break;
				case 28: formatter.Emplace_Back(std::make_unique<Format_Arg_S>(*msgInfo)); break;
				case 29: formatter.Emplace_Back(std::make_unique<Format_Arg_T>(*msgInfo)); break;
				case 30: formatter.Emplace_Back(std::make_unique<Format_Arg_Y>(*msgInfo)); break;
				case 31: formatter.Emplace_Back(std::make_unique<Format_Arg_T>(*msgInfo)); break;
				case 32: formatter.Emplace_Back(std::make_unique<Format_Arg_Z>(*msgInfo)); break;
				case 33: formatter.Emplace_Back(std::make_unique<Format_Arg_Message>(*msgInfo)); break;
				default:
					// This function is only accessed if an index is found in allValidFlags array
					break;
			}
	}

	void Message_Formatter::SetPattern(std::string_view pattern) {
		fmtPattern.clear();
		fmtPattern.append(pattern.data(), pattern.size());
		StoreFormat();
	}

	Message_Formatter::Formatters& Message_Formatter::GetFormatters() {
		return formatter;
	}

	void Message_Formatter::Formatters::Clear() {
		m_Formatter.clear();
	}

	void Message_Formatter::StoreFormat() {
		namespace lut = serenity::SERENITY_LUTS;
		temp.clear();
		formatter.Clear();
		temp.append(fmtPattern);
		while( temp.size() != 0 ) {
				if( temp.front() == '%' ) {
						auto flag { temp.substr(0, 2) };
						temp.erase(0, flag.size());
						auto position { std::find(lut::allValidFlags.begin(), lut::allValidFlags.end(), flag) };
						if( position == lut::allValidFlags.end() ) {
								formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(flag));
								continue;
						}
						size_t index { static_cast<size_t>(std::distance(lut::allValidFlags.begin(), position)) };
						FlagFormatter(index, ParseUserPatternForSpec(temp, index));
				} else {
						auto pos { temp.find_first_of('%') };
						if( pos == std::string::npos ) {
								formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(temp));
								temp.clear();
								continue;
						}
						std::string subStr { std::move(temp.substr(0, pos)) };
						formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(subStr));
						temp.erase(0, subStr.size());
					}
			}
	}

	// Currently overkill since it's just validating one token
	void Message_Formatter::ValidateUserCharSpec(size_t index, std::vector<char> specs) {
		switch( index ) {
				case 11:
					if( specs.size() == 0 ) {
							sourceFlag |= source_flag::all;
							return;
					}
					for( auto& spec: specs ) {
							switch( spec ) {
									case 'l': sourceFlag |= source_flag::line; break;
									case 'c': sourceFlag |= source_flag::column; break;
									case 'f': sourceFlag |= source_flag::file; break;
									case 'F': sourceFlag |= source_flag::function; break;
									default: break;
								}
						}
					break;
				default: break;
			}
	}

	void Message_Formatter::ValidateUserPrecisionSpec(size_t index, size_t& value) {
		switch( index ) {
				case 4:
					if( (value > 9) || (value < 0) ) {
							printf("%s : Specifier \"%zu\"", precisionWarningMessage[ 0 ], value);
							value = defaultSubSecondPrecision;
					}
					break;
				case 10:
					if( (value > 10) || (value < 0) ) {
							printf("%s : Specifier \"%zu\"", precisionWarningMessage[ 1 ], value);
							value = defaultThreadIdLength;
					}
					break;
				default: break;
			}
	}

	static constexpr std::array<char, 4> validCharSpecs = { 'l', 'c', 'f', 'F' };

	size_t Message_Formatter::ParseUserPatternForSpec(std::string& parseStr, size_t index) {
		size_t tmpValue { 0 };
		if( parseStr.size() == 0 ) return tmpValue;
		std::vector<char> specsOutput {};

		if( parseStr.front() == ':' ) {
				auto ch { parseStr.at(1) };
				if( IsDigit(ch) ) {
						parseStr.erase(0, 1);
						std::string precision;
						for( ;; ) {
								if( !IsDigit(parseStr.front()) ) break;
								precision += parseStr.front();
								parseStr.erase(0, 1);
							}
						std::from_chars(precision.data(), precision.data() + precision.size(), tmpValue);
						ValidateUserPrecisionSpec(index, tmpValue);
				}
				if( std::isalpha(ch) ) {
						parseStr.erase(0, 1);
						for( ;; ) {
								auto ch { parseStr.front() };
								auto end { validCharSpecs.end() };
								if( !IsAlpha(ch) ) break;
								if( std::find(validCharSpecs.begin(), end, ch) == end ) break;
								specsOutput.emplace_back(ch);
								parseStr.erase(0, 1);
							}
						ValidateUserCharSpec(index, specsOutput);
				}
		}
		if( (specsOutput.size() == 0) && (tmpValue == 0) ) {
				switch( index ) {
						case 4: tmpValue = defaultSubSecondPrecision; break;
						case 10: tmpValue = defaultThreadIdLength; break;
						case 11: sourceFlag = source_flag::all; break;
						default: break;
					}
		}
		return tmpValue;
	}

	void Message_Formatter::LazySubstitute(std::string& msg, std::string&& arg) {
		std::string_view temp { msg };
		auto& parseHelper { argStorage.ParseHelper() };
		parseHelper.ClearPartitions();
		using B_Type = LazyParseHelper::bracket_type;
		using P_Type = LazyParseHelper::partition_type;

		parseHelper.SetBracketPosition(B_Type::open, temp.find_first_of(static_cast<char>('{')));
		parseHelper.SetBracketPosition(B_Type::close, temp.find_first_of(static_cast<char>(' }')));

		if( parseHelper.BracketPosition(B_Type::open) != 0 ) {
				parseHelper.SetPartition(P_Type::primary, temp.substr(0, parseHelper.BracketPosition(B_Type::open)));
		}
		temp.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
		parseHelper.SetPartition(P_Type::remainder, temp);

		msg.clear();
		auto size { arg.size() };
		msg.append(std::move(parseHelper.PartitionString(P_Type::primary))
		           .append(std::move(arg.data()), size)
		           .append(std::move(parseHelper.PartitionString(P_Type::remainder))));
	}

	const Message_Info* Message_Formatter::MessageDetails() {
		return msgInfo;
	}

	// Formatters Functions
	constexpr size_t ESTIMATED_ARG_SIZE = 32;
	Message_Formatter::Formatters::Formatters(std::vector<std::unique_ptr<Formatter>>&& container): m_Formatter(std::move(container)) {
		localBuffer.reserve(m_Formatter.size() * ESTIMATED_ARG_SIZE);
	}

	void Message_Formatter::Formatters::Emplace_Back(std::unique_ptr<Formatter>&& formatter) {
		m_Formatter.emplace_back(std::move(formatter));
		localBuffer.reserve(m_Formatter.size() * ESTIMATED_ARG_SIZE);
	}

	std::string_view Message_Formatter::Formatters::FormatUserPattern() {
		localBuffer.clear();
		for( auto& formatter: m_Formatter ) {
				auto formatted { formatter->FormatUserPattern() };
				localBuffer.append(formatted.data(), formatted.size());
			}
		return localBuffer;
	}

	std::string_view serenity::msg_details::Message_Formatter::LineEnding() const {
		return SERENITY_LUTS::line_ending.at(platformEOL);
	}
}    // namespace serenity::msg_details
