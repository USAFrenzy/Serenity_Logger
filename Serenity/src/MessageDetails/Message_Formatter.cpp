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

	void ArgContainer::AdvanceToNextArg() {
		if( argIndex < maxIndex ) {
				++argIndex;
		} else {
				endReached = true;
			}
	}

	static constexpr bool AreSpecsSupported(std::string_view argBracket) {
		size_t pos { 1 };    // ignore '{'
		for( ;; ) {
				if( pos >= argBracket.size() ) break;
				auto ch { argBracket.at(pos) };
				// don't currently positional specs
				if( std::isdigit(ch) ) return false;
				if( ch == ':' ) {
						// Not a valid specifier
						if( argBracket.size() < pos + 1 ) return false;
						// don't currently support width specs
						if( std::isdigit(argBracket.at(pos + 1)) ) return false;
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
								case '<': [[fallthrough]];
								case '>': [[fallthrough]];
								case '^': [[fallthrough]];
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

	void ArgContainer::HandleStringSpec(char spec) {
		if( spec != 's' ) {
				// clang-format off
			std::cerr << "Warning: Arg Specifier Being Used On String Type Argument Is A Non-String Type Specifier\n";
				// clang-format on
		} else {
				argSpecValue.emplace_back(SpecValue::s);
			}
	}

	void ArgContainer::HandleIntSpec(char spec) {
		switch( spec ) {
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default: std::cerr << "Warning: Arg Specifier For Int Is Not A Valid Spec Argument\n"; break;
			}
	}

	void ArgContainer::HandleBoolSpec(char spec) {
		switch( spec ) {
				case 's': argSpecValue.emplace_back(SpecValue::s); break;
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default: std::cerr << "Warning: Arg Specifier For Bool Is Not A Valid Spec Argument\n"; break;
			}
	}

	void ArgContainer::HandleFloatingPointSpec(char spec) {
		switch( spec ) {
				case 'a': argSpecValue.emplace_back(SpecValue::a); break;
				case 'A': argSpecValue.emplace_back(SpecValue::A); break;
				case 'e': argSpecValue.emplace_back(SpecValue::e); break;
				case 'E': argSpecValue.emplace_back(SpecValue::E); break;
				case 'f': argSpecValue.emplace_back(SpecValue::f); break;
				case 'F': argSpecValue.emplace_back(SpecValue::F); break;
				case 'g': argSpecValue.emplace_back(SpecValue::g); break;
				case 'G': argSpecValue.emplace_back(SpecValue::G); break;
				default: break;
			}
	}

	void ArgContainer::HandleCharSpec(char spec) {
		switch( spec ) {
				case 'b': argSpecValue.emplace_back(SpecValue::b); break;
				case 'B': argSpecValue.emplace_back(SpecValue::B); break;
				case 'c': argSpecValue.emplace_back(SpecValue::c); break;
				case 'd': argSpecValue.emplace_back(SpecValue::d); break;
				case 'o': argSpecValue.emplace_back(SpecValue::o); break;
				case 'x': argSpecValue.emplace_back(SpecValue::x); break;
				case 'X': argSpecValue.emplace_back(SpecValue::X); break;
				default: std::cerr << "Warning: Arg Specifier For Char Is Not A Valid Spec Argument\n"; break;
			}
	}

	void Message_Formatter::EnableFallbackToStd(bool enable) {
		argStorage.EnableFallbackToStd(enable);
	}

	void ArgContainer::EnableFallbackToStd(bool enable) {
		isStdFallbackEnabled = enable;
	}

	std::tuple<std::string, char> ArgContainer::SplitPrecisionAndSpec(SpecType type, std::string_view spec) {
		std::string prec;
		char actualSpec { '\0' };
		for( ;; ) {
				if( spec.size() == 0 ) break;
				if( std::isdigit(spec.front()) ) {
						prec += spec.front();
						spec.remove_prefix(1);
						continue;
				}
				if( std::isalpha(spec.front()) ) {
						using enum SpecType;
						if( type == StringType || type == CharPointerType || type == StringViewType ) break;
						actualSpec = spec.front();
						break;
				}
				if( prec.size() == 0 ) {
						std::cerr << "Warning: Nothing Found For The Precision Field After\".\"\n";
				}
				break;
			}
		return std::make_tuple(prec, actualSpec);
	}

	void ArgContainer::VerifySpecWithPrecision(SpecType type, std::string_view spec) {
		// If !isStdFallbackEnabled for string types, just issue the warning,
		// return from this function, and proceed as if spec field was empty.
		// Otherwise, Follow through with SplitPrecisionAndSpec().
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

		auto [ precStr, chSpec ] = SplitPrecisionAndSpec(type, spec);
		// Now just need a way to store and somehow map the precision with the argument
		switch( type ) {
				case SpecType::CharPointerType: [[fallthrough]];
				case SpecType::StringViewType: [[fallthrough]];
				case SpecType::StringType:
					// argSpecValue.emplace_back(SpecValue::std_fallback);
					// throw std::runtime_error("Fallback Not Yet Implemented\n");
					break;                           // Still Need To Implement
				case SpecType::FloatType: break;         // Still Need To Implement
				case SpecType::DoubleType: break;        // Still Need To Implement
				case SpecType::LongDoubleType: break;    // Still Need To Implement
				default: break;
			}
	}

	void ArgContainer::VerifySpec(SpecType type, char spec) {
		using enum SpecType;
		if( spec == '\0' ) return;
		switch( type ) {
				case MonoType: break;
				case StringType: [[fallthrough]];
				case CharPointerType: [[fallthrough]];
				case StringViewType: HandleStringSpec(spec); break;
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: [[fallthrough]];
				case U_LongLongType: HandleIntSpec(spec); break;
				case BoolType: HandleBoolSpec(spec); break;
				case CharType: HandleCharSpec(spec); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType: HandleFloatingPointSpec(spec); break;
				case ConstVoidPtrType: [[fallthrough]];
				case VoidPtrType: argSpecValue.emplace_back(SpecValue::p); break;
				default: break;
			}
	}

	void ArgContainer::HandleArgBracket(std::string_view argBracket, int counterToIndex) {
		size_t pos { 1 };    // start at 1 to ignore '{'
		SpecType argType;
		// clang-format off
		argSpecTypes.size() >= counterToIndex ? argType = argSpecTypes.at(counterToIndex) 
			                                                                            : argType = SpecType::MonoType;
		// clang-format on
		if( argType == SpecType::MonoType ) return;
		// Setting up for future expansion for support with std::format specifiers
		// i.e. (fill & align, width, positional specs, etc)
		for( ;; ) {
				switch( argBracket[ pos ] ) {
						case ':':
							++pos;
							continue;
							break;
						case '.':
							++pos;
							VerifySpecWithPrecision(argType, std::move(argBracket.substr(pos, argBracket.size())));
							break;
						default: VerifySpec(argType, argBracket[ pos ]); break;
					}
				return;
			}
	}

	// This doesn't break with an arbitrary amount of spaces in brackets
	// (surprisingly, spdlog didn't handle an arbitrary amount of spaces,
	//  though that might be a fmtlib limitation instead)
	void ArgContainer::ParseForSpecifiers(std::string_view fmt) {
		using B_Type = LazyParseHelper::bracket_type;
		std::string_view argBracket;
		int argCounter { 0 };    // used to map our current arg to its type in HandleArgBracket()
		for( ;; ) {
				if( fmt.size() <= 2 ) return;    // Definitely don't have another '{}'
				argBracket = "";                 // reset for proper fmt prefix removal
				auto specFieldPresent { fmt[ 1 ] == ':' };
				if( fmt[ 0 ] == '{' && !specFieldPresent ) {
						argSpecValue.emplace_back(SpecValue::none);
						fmt.remove_prefix(1);
						++argCounter;
						continue;
				}
				if( specFieldPresent ) {
						parseHelper.SetBracketPosition(B_Type::open, fmt.find_first_of('{'));
						parseHelper.SetBracketPosition(B_Type::close, fmt.find_first_of('}'));

						if( (parseHelper.BracketPosition(B_Type::open) != std::string_view::npos) &&
						    (parseHelper.BracketPosition(B_Type::close) != std::string_view::npos) )
							{
								argBracket = std::move(fmt.substr(parseHelper.BracketPosition(B_Type::open),
								                                  parseHelper.BracketPosition(B_Type::close) + 1));
						} else {
								// return if we don't have a closing bracket position
								return;
							}
						if( argBracket.size() == 3 ) {
								// specs need a ':' and a specifier
								throw std::runtime_error("Not A Valid Specifier");
						} else {
								HandleArgBracket(argBracket, argCounter);
								++argCounter;
							}
				}
				argBracket.size() == 0 ? fmt.remove_prefix(1) : fmt.remove_prefix(argBracket.size());
			}
	}

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
	std::string&& ArgContainer::GetArgValue() {
		auto& strRef { parseHelper.StringBuffer() };
		strRef.clear();
		parseHelper.ClearBuffer();
		auto& arg { argContainer[ argIndex ] };
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
