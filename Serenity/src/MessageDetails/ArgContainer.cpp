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
		// GetArgValue(container, index, fillAlignValues.additionalSpec));
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) };
		// auto data{ buff.data() };
		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');

		// if (fillSpec == '\0') fillSpec = ' ';
		// size_t i{ 0 };
		// for (auto& ch : container) {
		//	buff[i] = ch);
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
		// GetArgValue(container, index, fillAlignValues.additionalSpec));
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) };
		// auto data{ buff.data() };
		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');

		// if (fillSpec == '\0') fillSpec = ' ';
		// std::fill(data, data + fillAmount, fillSpec);
		// size_t i{ fillAmount };
		// for (auto& ch : container) {
		//	buff[i] = ch);
		//	++i;
		// }

		// container.clear();
		// container.append(data, data + i);
	}

	void ArgContainer::AlignCenter(size_t index, std::string& container) {
		// auto& fillSpec{ fillAlignValues.fillSpec };
		// auto& buff{ fillAlignValues.buff };
		// container.clear();
		// GetArgValue(container, index, fillAlignValues.additionalSpec));
		// auto fillLeftover{ (fillAlignValues.digitSpec - container.size()) % 2 };
		// auto fillAmount{ (fillAlignValues.digitSpec - container.size()) / 2 };
		// auto data{ buff.data() };

		// std::fill(data, data + fillAlignValues.digitSpec + 1, '\0');
		// if (fillSpec == '\0') fillSpec = ' ';
		// std::fill(data, data + fillAmount, fillSpec);
		// size_t i{ fillAmount };
		// for (auto& ch : container) {
		//	buff[i] = ch);
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

	size_t ArgContainer::FindDigitEnd(std::string_view sv) {
		size_t size { sv.size() }, pos { 0 };
		for( ;; ) {
				if( pos >= size ) break;
				if( sv[ pos ] != '\0' ) {
						++pos;
						continue;
				}
				break;
			}
		return pos;
	}

	/*************************************** Variant Order ********************************************
	 * [0] std::monostate, [1] std::string, [2] const char*, [3] std::string_view, [4] int,
	 * [5] unsigned int, [6] long long, [7] unsigned long long, [8] bool, [9] char, [10] float,
	 * [11] double, [12] long double, [13] const void* [14] void*
	 *************************************************************************************************/
	void ArgContainer::GetArgValueAsStr(std::string& container, size_t positionIndex, char&& additionalSpec, int precision) {
		auto& arg { argContainer[ positionIndex ] };
		auto data { buffer.data() };
		std::fill(data, data + buffer.size(), '\0');
		std::string_view tmp;
		switch( arg.index() ) {
				case 0: break;
				case 1:
					tmp       = std::get<1>(arg);
					precision = precision > 0 ? precision : static_cast<int>(tmp.size());
					container.append(tmp.data(), tmp.data() + precision);
					break;
				case 2:
					tmp       = std::get<2>(arg);
					precision = precision > 0 ? precision : static_cast<int>(tmp.size());
					container.append(tmp.data(), tmp.data() + precision);
					break;
				case 3:
					tmp       = std::get<3>(arg);
					precision = precision > 0 ? precision : static_cast<int>(tmp.size());
					container.append(tmp.data(), tmp.data() + precision);
					break;
				case 4:
					result = std::to_chars(data, data + buffer.size(), std::get<4>(arg));
					container.append(data, result.ptr);
					break;
				case 5:
					result = std::to_chars(data, data + buffer.size(), std::get<5>(arg));
					container.append(data, result.ptr);
					break;
				case 6:
					result = std::to_chars(data, data + buffer.size(), std::get<6>(arg));
					container.append(data, result.ptr);
					break;
				case 7:
					result = std::to_chars(data, data + buffer.size(), std::get<7>(arg));
					container.append(data, result.ptr);
					break;
				case 8: container.append(std::get<8>(arg) == true ? "true" : "false"); break;
				case 9: container.append(1, std::get<9>(arg)); break;
				case 10: FormatFloatTypeArg(container, std::move(additionalSpec), std::get<10>(arg), precision); break;
				case 11: FormatFloatTypeArg(container, std::move(additionalSpec), std::get<11>(arg), precision); break;
				case 12: FormatFloatTypeArg(container, std::move(additionalSpec), std::get<12>(arg), precision); break;
				case 13:
					result = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(std::get<13>(arg)), 16);
					container.append("0x").append(data, result.ptr);
					break;
				case 14:
					result = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(std::get<14>(arg)), 16);
					container.append("0x").append(data, result.ptr);
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