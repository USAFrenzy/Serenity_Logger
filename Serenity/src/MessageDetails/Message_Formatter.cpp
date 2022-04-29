#include <serenity/MessageDetails/Message_Formatter.h>

#include <iostream>

namespace serenity::msg_details {

	void LazyParseHelper::ClearBuffer() {
		std::fill(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), 0);
	}

	void serenity::msg_details::LazyParseHelper::ClearPartitions() {
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

	static constexpr bool AreSpecsSupported(std::string_view argBracket) {
		size_t pos { 1 };    // ignore '{'
		const size_t argBracketSize { argBracket.size() };
		// don't currently positional specs
		if( argBracketSize >= 3 ) {
				if( argBracket.at(2) >= '0' && argBracket.at(2) <= '9' ) {
						return false;
				}
		}
		for( ;; ) {
				if( pos >= argBracketSize ) break;
				auto ch { argBracket.at(pos) };
				if( ch == ':' ) {
						// Not a valid specifier
						if( argBracket.size() < pos + 1 ) return false;
						// don't currently support width specs
						auto potentialWidthSpec { argBracket.at(pos + 1) };
						if( (potentialWidthSpec >= '0') && (potentialWidthSpec <= '9') ) return false;
						// clang-format off
						switch( ch ) {
								// don't currently support precision yet
								case '.': [[fallthrough]];
								// don't currently support localization 
								case 'L': [[fallthrough]];
									// don't currently support nested fields
								case '{': [[fallthrough]];
									// don't currently support signed specs
								case '+': [[fallthrough]];
								case '-': [[fallthrough]];
								case ' ': [[fallthrough]];
									// don't currently support fill and align specs
							//	case '<': [[fallthrough]];
							//	case '>': [[fallthrough]];
							//	case '^': [[fallthrough]];
									// don't currently support alternative form specs
								case '#': return false; break;
								default:  break;
							}
						// clang-format on
				}
				++pos;
			}
		return true;
	}

	// TODO:**********************************************************************************************************
	// ParseForSpecifiers() And HandleArgBracket() are similar to ContainsUnsupportedSpecs() and AreSpecsSupported()
	// at the moment. What I would Like to do is have the later functions run at compile time (lots of work needs
	// to be done for that) and to have the former match the specs to the args and format accordingly.
	//
	// I could use a counter that increments when AdvanceToNextArg() is called in order to index
	// into the spec type vector to retrieve the type of the next arg and pass that to GetArgValue():
	// x 1.) Bypassing the need in GetArgValue() for the switch since the type will already be known
	// x 2.) Allows directly calling std::get<typeMappedToIndex>(arg) instead (simplfying some upkeep)
	// 3.) This type can then also be used to index into a map to retrieve the valid specifiers
	//     allowed for that type (Probably using ParseForSpecs() in CaptureArgs() to verify that specs
	//     found are actually valid (Using HandleArgBracket() when a bracket is found) and then storing
	//     those specs into a container (still need to create the container for specifiers):
	//     - Would require adding the format string as an argument to CaptureArgs()
	// 4.) THEN when converting the arg to a string, the specifiers found can be indexed by the
	//     same counter and used to format the arg value accordingly
	// - Already storing arg type in specType vector
	// - Still need to validate specifiers found and then add them to some sort of container
	// - Still need to create another map to get index from
	// - EDIT: Points #1 & #2 are a no-go as I didn't realize until after the fact that std::get<>
	//         requires a compile time constant...which kinda sucks for this use case. But I can
	//         still map the index to the type being stored and use that to retrieve only the type
	//         value in HandleArgBracket() in order to check specs against that type - so that's
	//         still a plus in my opinion.
	// - EDIT: After some searching around, using make_index_sequence and index_sequence in a templated
	//         manner might work for points #1 & #2. The thing is, adding more complexity might not be
	//         worth it just for simplfying upkeep if performance doesn't either match or exceed the
	//         current setup
	// TODO:**********************************************************************************************************

	bool ArgContainer::HandleStringSpec(char spec) {
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

	bool ArgContainer::HandleIntSpec(char spec) {
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

	bool ArgContainer::HandleBoolSpec(char spec) {
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

	bool ArgContainer::HandleFloatingPointSpec(char spec) {
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

	bool ArgContainer::HandleCharSpec(char spec) {
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

	void ArgContainer::SplitPrecisionAndSpec(SpecType type, std::string_view spec) {
		precisionSpecHelper.precision.clear();
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
				if( precisionSpecHelper.precision.size() == 0 ) {
						std::cerr << "Warning: Nothing Found For The Precision Field After\".\"\n";
				}
				break;
			}
	}

	void ArgContainer::VerifySpecWithPrecision(SpecType type, std::string_view spec) {
		switch( type ) {
				case SpecType::CharPointerType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::StringType:
					if( !isStdFallbackEnabled ) {
							// clang-format off
							std::cerr <<  "Warning: Precision Field For String Types Not Currently Supported Natively.\n"
					                                   "Set \"EnableFallBackToStd(true)\" To Let The Standard Deal With This Argument.\n";
							// clang-format on
							return;
					}
					break;
				default: break;
			}

		SplitPrecisionAndSpec(type, spec);
		// Now just need a way to store and somehow map the precision with the argument
		switch( type ) {
				case SpecType::CharPointerType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::StringType:
					argSpecValueModifiers.emplace_back(SpecValueModifiers::std_fallback);
					throw std::runtime_error("Fallback Not Yet Implemented\n");
					break;                           // Still Need To Implement
				case SpecType::FloatType: break;         // Still Need To Implement
				case SpecType::DoubleType: break;        // Still Need To Implement
				case SpecType::LongDoubleType: break;    // Still Need To Implement
				default: break;
			}
	}

	bool ArgContainer::VerifySpec(SpecType type, char spec) {
		using enum SpecType;
		if( spec == '\0' ) return false;
		switch( type ) {
				case MonoType: return true; break;
				case StringType: [[fallthrough]];
				case CharPointerType: [[fallthrough]];
				case StringViewType:
					if( HandleStringSpec(spec) ) return true;
					break;
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType:
					if( HandleIntSpec(spec) ) return true;
					break;
				case BoolType:
					if( HandleBoolSpec(spec) ) return true;
					break;
				case CharType:
					if( HandleCharSpec(spec) ) return true;
					break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType:
					if( HandleFloatingPointSpec(spec) ) return true;
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

	void ArgContainer::HandleFillAndAlignSpec(char preSpecChar, char fillAlignSpec) {
		// TODO: implement this fully
		argSpecValueModifiers.emplace_back(SpecValueModifiers::fill_align);
	}

	void ArgContainer::HandleWidthSpec(char spec) {
		// TODO: implement this fully
		argSpecValueModifiers.emplace_back(SpecValueModifiers::width);
	}

	static constexpr std::array<char, 3> faSpecs   = { '<', '>', '^' };
	static constexpr std::array<char, 3> signSpecs = { '+', '-', ' ' };

	// TODO: Update this function to work properly after the parsing function is finished
	// Changing how the parse function currently works, will definitely
	// NEED to look at and update this function afterwards
	bool ArgContainer::VerifyIfFillAndAlignSpec(SpecType type, std::string_view specView) {
		size_t pos { 1 };
		fillAlignValues.Reset();
		auto argBracketSize { specView.size() };

		for( ;; ) {
				if( pos >= argBracketSize - 1 ) break;
				auto ch { specView[ pos ] };

				if( IsDigit(ch) ) {
						std::string digitCount { ch };
						auto tempPos { pos + 1 };
						for( ;; ) {
								if( tempPos >= argBracketSize ) break;
								auto nextCh { specView[ tempPos ] };
								if( IsDigit(nextCh) ) {
										digitCount += nextCh;
										++tempPos;
								} else {
										break;
									}
							}
						if( digitCount.size() > 2 ) {
								// clang-format off
								throw std::runtime_error("Digit Spec For Fill/Align Cannot Be Greater Than Two Digits\n");
								// clang-format on
						}
						std::from_chars(digitCount.data(), digitCount.data() + digitCount.size(),
						                fillAlignValues.digitSpec);
						pos = tempPos;
						continue;
				}

				if( IsAlpha(ch) ) {
						auto tempPos { pos + 1 };
						char nextCh;
						if( tempPos <= argBracketSize ) nextCh = specView[ tempPos ];
						if( !std::any_of(faSpecs.begin(), faSpecs.end(), [ & ](char chSp) { return nextCh == chSp; }) ) {
								if( VerifySpec(type, ch) ) fillAlignValues.additionalSpec = ch;
						} else {
								if( ch != '{' && ch != '}' ) {
										fillAlignValues.fillSpec = ch;
								}
								fillAlignValues.fillAlignSpec = nextCh;
								break;
							}
						++pos;
						continue;
				}

				if( ch != '{' && ch != '}' ) {
						auto tempPos { pos + 1 };
						char nextCh;
						if( tempPos <= argBracketSize ) nextCh = specView[ tempPos ];
						if( std::any_of(faSpecs.begin(), faSpecs.end(), [ & ](char chSp) { return nextCh == chSp; }) ) {
								fillAlignValues.fillSpec      = ch;
								fillAlignValues.fillAlignSpec = nextCh;
								pos += 2;
						} else {
								if( IsAlpha(ch) ) {
										if( VerifySpec(type, ch) ) {
												fillAlignValues.additionalSpec = ch;
										}
										++pos;
								}
							}
						continue;
				}    // for loop
			}
		return (fillAlignValues.digitSpec != 0);
	}

	void ArgContainer::HandleSignSpec(char spec) {
		// TODO: implement this fully
		argSpecValueModifiers.emplace_back(SpecValueModifiers::sign);
	}

	void ArgContainer::HandleHashSpec(char spec) {
		argSpecValueModifiers.emplace_back(SpecValueModifiers::hash);
	}

	// Reading the spec standard, I will need to rework this quite a bit already...
	// (Not as straight-forward as I originally thought)
	// EDIT: This function is now already obsolete due to how the parsing function
	//       is currently being reworked
	bool ArgContainer::HandleArgBracket(std::string_view argBracket, int index) {
		size_t pos { 1 };    // start at 1 to ignore '{'
		SpecType argT { SpecType::MonoType };
		FillAlignValues temp = {};
		auto argBracketSize { argBracket.size() };

		if( argBracket[ 1 ] == ':' ) {
				for( ;; ) {
						if( pos >= argBracketSize - 1 ) break;
						auto ch { argBracket[ pos ] };

						if( IsDigit(ch) ) {
								temp.digitSpec += ch;
								auto tempPos { pos + 1 };
								for( ;; ) {
										if( tempPos >= argBracketSize ) break;
										auto nextCh { argBracket[ tempPos ] };
										if( IsDigit(nextCh) ) {
												temp.digitSpec += nextCh;
												++tempPos;
										} else {
												break;
											}
									}
								pos = tempPos;
								continue;
						}

						if( IsAlpha(ch) ) {
								auto tempPos { pos + 1 };
								char nextCh;
								if( tempPos <= argBracketSize ) nextCh = argBracket[ tempPos ];
								if( !std::any_of(faSpecs.begin(), faSpecs.end(),
								                 [ & ](char chSp) { return nextCh == chSp; }) ) {
										if( VerifySpec(argT, ch) ) temp.additionalSpec = ch;
								} else {
										if( ch != '{' && ch != '}' ) {
												temp.fillSpec = ch;
										}
										temp.fillAlignSpec = nextCh;
										break;
									}
								++pos;
								continue;
						}

						if( ch != '{' && ch != '}' ) {
								auto tempPos { pos + 1 };
								char nextCh;
								if( tempPos <= argBracketSize ) nextCh = argBracket[ tempPos ];
								if( std::any_of(faSpecs.begin(), faSpecs.end(),
								                [ & ](char chSp) { return nextCh == chSp; }) ) {
										temp.fillSpec      = ch;
										temp.fillAlignSpec = nextCh;
										pos += 2;
								} else {
										if( IsAlpha(ch) ) {
												if( VerifySpec(argT, ch) ) {
														temp.additionalSpec = ch;
												}
												++pos;
										}
									}
								continue;
						}    // for loop
					}
		}    // ':' if statement
		return (temp.digitSpec != 0);
	}

	// Instead of in CaptureArgs(), this function should be in FormatMessageArgs()
	// so that I can directly format the args without having to worry about storing
	// the spec types and modifiers around... Kind of sucks to have this thought NOW,
	// but it would probably save on all the enum vector storage as I could then just
	// handle the actual formatting of the arg in the Handle'x'Spec functions and return
	// the formatted value directly to substitute into the message or throw on error

	/********************************************************************************
	        First step is to get the parser up and running, next step is to add in the
	        logic of the parsing function to work with FormatMessageArgs(),then implement
	        fully the Handle'x'Spec() type functions and add those in where appropriate
	        to the parsing function (returning the argument values, whether they needed
	        formatting from the Handle'x'Spec() line of functions or if they were just
	        being directly substituted in as a string representation of their value).
	********************************************************************************/

	/******************************************************************************************
	The function flow goal here is:
	- Parse up until an argument bracket is found
	- Take the log message up until that point and append it to the lazy_message variable
	- Parse the argument bracket:
  - Verify the specs in the bracket accurately correlate to the type of argument
  - Format the argument based on those specs
  - If the specs are valid (throwing a runtime error if they aren't):
    - Erase the format string up until that the end of the first argument processed
    - Append the argument value to the lazy_message variable
    - Repeat until the format string is either empty or no more argument brackets are found.
    - If no more argument bracket are found, should append whatever is left of the
      format string to the lazy_message variable.
	******************************************************************************************/

	// spec standard
	//  [fill - and -align(optional) sign(optional) #(optional)0(optional)width(optional) precision(optional) L(optional) type(optional)]

	std::string ArgContainer::AlignLeft(SpecType argType) {
		std::string temp { GetArgValue(argType, fillAlignValues.additionalSpec) };
		if( fillAlignValues.fillSpec != '\0' ) {
				for( int i { 0 }; i < fillAlignValues.digitSpec; ++i ) {
						temp += fillAlignValues.fillSpec;
					}
		} else {
				for( int i { 0 }; i < fillAlignValues.digitSpec; ++i ) {
						temp += ' ';
					}
			}
		return std::move(temp);
	}

	std::string ArgContainer::AlignRight(SpecType argType) {
		return std::string();
	}

	std::string ArgContainer::AlignCenter(SpecType argType) {
		return std::string();
	}

	void ArgContainer::ParseForSpecifiers(std::string_view fmt) {
		using B_Type = LazyParseHelper::bracket_type;
		size_t argCounter { 0 };    // used to map our current arg to its type

		// clang-format off
		// ******************************************** WIP ********************************************
		// clang-format on

		for( ;; ) {
				if( fmt.size() == 0 ) break;
				parseHelper.SetBracketPosition(B_Type::open, fmt.find_first_of('{'));
				parseHelper.SetBracketPosition(B_Type::close, fmt.find_first_of('}'));
				// no substitution needed so return
				if( (parseHelper.BracketPosition(B_Type::open) == std::string_view::npos) ||
				    (parseHelper.BracketPosition(B_Type::close) == std::string_view::npos) )
					{
						return;
				}
				auto argBracket { fmt.substr(parseHelper.BracketPosition(B_Type::open) + 1,
					                     parseHelper.BracketPosition(B_Type::close) + 1) };

				if( fmt[ parseHelper.BracketPosition(B_Type::open) + 1 ] == '{' &&
				    fmt[ parseHelper.BracketPosition(B_Type::close) + 1 ] == '}' )
					return;    // return for now
				// Handle the case of nested brackets

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
						fmt.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
						++argCounter;
						continue;
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
						if( argT == SpecType::MonoType ) return;
					}

				// May be a valid spec field but user forgot ':'.
				// Don't need to worry if the arg bracket is empty as that should have been caught earlier up and skipped.
				if( argBracket[ 0 ] != ':' ) {
						if( argBracket[ 0 ] == '}' ) break;
						std::string throwMsg { "Missing ':' In Argument Specifier Field For Argument " };
						std::array<char, 2> buff { '\0', '\0' };
						std::to_chars(buff.data(), buff.data() + 2, (argCounter + 1));
						throwMsg.append(buff.data(), buff.size()).append("\n");
						throw std::runtime_error(std::move(throwMsg));
				}

				// Parse the rest of the argument bracket
				for( ;; ) {
						firstToken = argBracket[ 1 ];
						switch( firstToken ) {
								case '+': [[fallthrough]];
								case '-': [[fallthrough]];
								case ' ': HandleSignSpec(firstToken); break;
								case '#': HandleHashSpec(firstToken); break;
								case '0': /*HandleZeroPadding();*/ break;
								case '.': VerifySpecWithPrecision(argT, argBracket); break;
								case 'L': /*LocaleFallBack() */ break;
								default:
									bool isFirstTokenDigit { IsDigit(firstToken) };
									auto nextToken = argBracket[ 2 ];
									bool isFASpec { std::any_of(faSpecs.begin(), faSpecs.end(),
										                    [ & ](auto ch) { return nextToken == ch; }) };
									bool isFAFillSpec { (firstToken != '{') && (firstToken != '}') };
									if( isFAFillSpec && isFASpec ) {
											if( nextToken == '}' ) nextToken = '<';
											if( VerifyIfFillAndAlignSpec(argT, argBracket) ) {
													// clang-format off
												std::string formattedArg;
													switch( fillAlignValues.fillAlignSpec ) {
															case '<':  formattedArg.append(AlignLeft(argT)); break;
															case '>': formattedArg.append(AlignRight(argT)); break;
															case '^': formattedArg.append(AlignCenter(argT)); break;
															default: break;
														}
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
				continue;
			}    // fmt processing loop

	}    // parsing fmt loop

	// clang-format off
		 // ******************************************** WIP ********************************************
	// clang-format on

	bool ArgContainer::ContainsUnsupportedSpecs(const std::string_view fmt) {
		auto size { fmt.size() };
		std::string_view argBracket;
		using B_Type = LazyParseHelper::bracket_type;
		for( size_t i { 0 }; i < size; ++i ) {
				argBracket = "";
				if( fmt.at(i) == '{' ) {
						parseHelper.SetBracketPosition(B_Type::open, fmt.find_first_of('{'));
						parseHelper.SetBracketPosition(B_Type::close, fmt.find_first_of('}'));

						if( (parseHelper.BracketPosition(B_Type::open) != std::string_view::npos) &&
						    (parseHelper.BracketPosition(B_Type::close) != std::string_view::npos) )
							{
								argBracket = std::move(fmt.substr(parseHelper.BracketPosition(B_Type::open),
								                                  parseHelper.BracketPosition(B_Type::close) + 1));
						}
						// clang-format off
						auto argBracketSize { argBracket.size() };
						switch( argBracketSize ) {
								case 0: break;
								case 1: break;
								case 2: break;
								// specs need a ':' and a specifier
								case 3: if( argBracket.at(1) != ' ' ) throw std::runtime_error("Not A Valid Specifier"); break;
								default: if( !AreSpecsSupported(argBracket) ) return true; break;
							}
						// clang-format on
						if( argBracketSize == size ) break;
				}
			}
		return false;
	}

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
	/*************************************************************************************************/
	// TODO: As far as any more optimizations go, this function eats up ~3x more cpu cycles compared
	// TODO: to the very next cpu hungry function given a test of 3 empty specifier arguments (an int,
	// TODO: a float, and a string). Given the timings, this may not be any real issue, but it'd be
	// TODO: cool to see how I may be able to speed this up as well since any gains here are massive
	// TODO: gains everywhere else.
	/*************************************************************************************************/
	std::string&& ArgContainer::GetArgValue(SpecType argType, char additionalSpec) {
		auto& strRef { parseHelper.StringBuffer() };
		strRef.clear();
		parseHelper.ClearBuffer();
		auto& arg { argContainer[ argIndex ] };
		auto& buffer { parseHelper.ConversionResultBuffer() };
		auto& result { parseHelper.ConversionResultInfo() };

		switch( mapTypeToIndex[ argType ] ) {
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
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<10>(arg))));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 11:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<11>(arg))));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.data() + parseHelper.FindEndPos());
					}
					return std::move(strRef);
					break;
				case 12:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<12>(arg))));
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
						FlagFormatter(index, ParseForSpec(temp, index));
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
	void Message_Formatter::ValidateCharSpec(size_t index, std::vector<char> specs) {
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

	void Message_Formatter::ValidatePrecisionSpec(size_t index, size_t& value) {
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

	// currently handling %e, %s, and %t
	size_t Message_Formatter::ParseForSpec(std::string& parseStr, size_t index) {
		size_t tmpValue { 0 };
		if( parseStr.size() == 0 ) return tmpValue;
		std::vector<char> specsOutput {};

		if( parseStr.front() == ':' ) {
				auto ch { parseStr.at(1) };
				if( std::isdigit(ch) ) {
						parseStr.erase(0, 1);
						ParsePrecisionSpec(parseStr, tmpValue);
						ValidatePrecisionSpec(index, tmpValue);
				}
				if( std::isalpha(ch) ) {
						parseStr.erase(0, 1);
						ParseCharSpec(parseStr, specsOutput);
						ValidateCharSpec(index, specsOutput);
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
		bool bracketPositionsValid;
		using B_Type = LazyParseHelper::bracket_type;
		using P_Type = LazyParseHelper::partition_type;

		parseHelper.SetBracketPosition(B_Type::open, temp.find_first_of(static_cast<char>('{')));
		parseHelper.SetBracketPosition(B_Type::close, temp.find_first_of(static_cast<char>(' }')));
		bracketPositionsValid = ((parseHelper.BracketPosition(B_Type::open) != std::string::npos) &&
		                         (parseHelper.BracketPosition(B_Type::close) != std::string::npos));
		if( bracketPositionsValid ) {
				parseHelper.SetPartition(P_Type::primary, temp.substr(0, parseHelper.BracketPosition(B_Type::open)));
				temp.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
				parseHelper.SetPartition(P_Type::remainder, temp);
		}
		msg.clear();
		auto size { arg.size() };
		msg.append(std::move(parseHelper.PartitionString(P_Type::primary))
		           .append(std::move(arg.data()), size)
		           .append(std::move(parseHelper.PartitionString(P_Type::remainder))));
		return;
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
