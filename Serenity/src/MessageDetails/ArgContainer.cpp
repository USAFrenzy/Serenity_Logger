#include <serenity/MessageDetails/ArgContainer.h>
#include <iostream>

namespace serenity::experimental::msg_details {

	const std::vector<ArgContainer::LazilySupportedTypes>& ArgContainer::ArgStorage() const {
		return argContainer;
	}

	void ArgContainer::AdvanceToNextArg() {
		if( argIndex < maxIndex ) {
				++argIndex;
		} else {
				endReached = true;
			}
	}

	bool ArgContainer::IsValidStringSpec(const char& spec) {
		if( spec == 's' ) {
				return true;
		} else {
				std::string throwMessage { "Arg Specifier '" };
				throwMessage += spec;
				throwMessage.append("' For A String Is Not A Valid Spec Argument\n");
				throw(std::move(throwMessage));
			}
	}

	bool ArgContainer::IsValidIntSpec(const char& spec) {
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

	bool ArgContainer::IsValidBoolSpec(const char& spec) {
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

	bool ArgContainer::IsValidFloatingPointSpec(const char& spec) {
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

	bool ArgContainer::IsValidCharSpec(const char& spec) {
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

	void ArgContainer::EnableFallbackToStd(bool enable) {
		isStdFallbackEnabled = enable;
	}

	bool ArgContainer::VerifySpec(SpecType type, const char& spec) {
		using enum SpecType;
		if( spec == '\0' ) return true;
		if( !IsAlpha(spec) ) {
				std::string throwMsg { "Error In Verifying Argument Specifier \"" };
				throwMsg += spec;
				throwMsg.append("\": Invalid Argument Specifier Was Provided Or Specifier Format Is Incorrect.\n");
				std::cout << throwMsg;
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

	// Align Left And Align Right Still Need To Add Default Behaviors
	void ArgContainer::AlignLeft(size_t index, std::string& container) {
		// auto& fillSpec{ fillAlignValues.fillSpec };
		// auto& buff{ fillAlignValues.buff };
		// container.clear();
		// GetArgValue(container, index, std::move(fillAlignValues.additionalSpec));
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) };
		// auto data{ buff.data() };
		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');

		// if (fillSpec == '\0') fillSpec = ' ';
		// size_t i{ 0 };
		// for (auto& ch : container) {
		//	buff[i] = std::move(ch);
		//	++i;
		// }
		// std::fill(data + i, data + fillAlignValues.digitSpec, fillSpec);
		// container.clear();
		// container.append(data, fillAlignValues.digitSpec);
	}

	void ArgContainer::AlignRight(size_t index, std::string& container) {
		// auto& fillSpec{ fillAlignValues.fillSpec };
		// auto& buff{ fillAlignValues.buff };
		// container.clear();
		// GetArgValue(container, index, std::move(fillAlignValues.additionalSpec));
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) };
		// auto data{ buff.data() };
		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');

		// if (fillSpec == '\0') fillSpec = ' ';
		// std::fill(data, data + fillAmount, fillSpec);
		// size_t i{ fillAmount };
		// for (auto& ch : container) {
		//	buff[i] = std::move(ch);
		//	++i;
		// }

		// container.clear();
		// container.append(data, data + i);
	}

	void ArgContainer::AlignCenter(size_t index, std::string& container) {
		// auto& fillSpec{ fillAlignValues.fillSpec };
		// auto& buff{ fillAlignValues.buff };
		// container.clear();
		// GetArgValue(container, index, std::move(fillAlignValues.additionalSpec));
		// auto fillLeftover{ (fillAlignValues.digitSpec - container.size()) % 2 };
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) / 2 };
		// auto data{ buff.data() };

		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');
		// if (fillSpec == '\0') fillSpec = ' ';
		// std::fill(data, data + fillAmount, fillSpec);
		// size_t i{ fillAmount };
		// for (auto& ch : container) {
		//	buff[i] = std::move(ch);
		//	++i;
		// }
		// auto endPos{ i + fillAmount + fillLeftover };
		// std::fill(data + i, data + endPos, fillSpec);

		// container.clear();
		// container.append(data, data + endPos);
	}

	void ArgContainer::CountNumberOfBrackets(std::string_view fmt) {
		for( ;; ) {
				if( fmt.size() == 0 ) break;
				auto startPos { fmt.find_first_of('{') };
				auto endPos { fmt.find_first_of('}') };
				if( startPos != std::string_view::npos ) {
						if( endPos != std::string_view::npos ) {
								auto copy { fmt };
								copy.remove_prefix(startPos + 1);
								copy.remove_suffix(fmt.size() - endPos - 1);
								if( copy.find_first_of('{') != std::string_view::npos ) {
										if( copy.find_first_of('}') != std::string_view::npos ) {
												++remainingArgs;
										}
								}
								++remainingArgs;
								fmt.remove_prefix(endPos + 1);
						}
				} else {
						break;
					}
			}
	}

	size_t ArgContainer::FindDigitEnd(std::string_view sv, size_t start) {
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

	/*************************************** Variant Order ********************************************
	 * [0] std::monostate, [1] std::string, [2] const char*, [3] std::string_view, [4] int,
	 * [5] unsigned int, [6] long long, [7] unsigned long long, [8] bool, [9] char, [10] float,
	 * [11] double, [12] long double, [13] const void* [14] void*
	 *************************************************************************************************/
	void ArgContainer::GetArgValue(std::string& container, size_t positionIndex, char&& additionalSpec) {
		container.clear();
		auto& arg { argContainer[ positionIndex ] };
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};

		switch( arg.index() ) {
				case 0: break;
				case 1: container.append(std::move(std::get<1>(std::move(arg)))); break;
				case 2: container.append(std::move(std::get<2>(std::move(arg)))); break;
				case 3: container.append(std::move(std::get<3>(std::move(arg)))); break;
				case 4:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<4>(std::move(arg))));
					container.append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				case 5:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<5>(std::move(arg))));
					container.append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				case 6:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<6>(std::move(arg))));
					container.append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				case 7:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::move(std::get<7>(std::move(arg))));
					container.append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				case 8: container.append(std::move(std::get<8>(std::move(arg))) == true ? "true" : "false"); break;
				case 9: container += std::move(std::get<9>(std::move(arg))); break;
				case 10:
					FormatFloatTypeArg(container, std::move(additionalSpec), std::move(std::get<10>(std::move(arg))), buffer);
					break;
				case 11:
					FormatFloatTypeArg(container, std::move(additionalSpec), std::move(std::get<11>(std::move(arg))), buffer);
					break;
				case 12:
					FormatFloatTypeArg(container, std::move(additionalSpec), std::move(std::get<12>(std::move(arg))), buffer);
					break;
				case 13:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					              reinterpret_cast<size_t>(std::move(std::get<13>(std::move(arg)))), 16);
					container.append("0x").append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				case 14:
					std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					              reinterpret_cast<size_t>(std::move(std::get<14>(std::move(arg)))), 16);
					container.append("0x").append(buffer.data(), buffer.data() + FindDigitEnd(buffer.data(), buffer.size()));
					break;
				default: break;
			}
	}

	bool ArgContainer::EndReached() const {
		return endReached;
	}

	bool ArgContainer::ContainsUnsupportedType() const {
		return containsUnknownType;
	}

}    // namespace serenity::experimental::msg_details