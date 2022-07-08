#pragma once
// Copyright from <format> header extending to libfmt

// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sub-license, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code, portions
// of this Software are embedded into a machine-executable object form of such
// source code, you may redistribute such embedded portions in such object form
// without including the above copyright and permission notices.

#include <serenity/MessageDetails/ArgContainer.h>
#include <serenity/MessageDetails/ArgFormatter.h>

// TODO: Address the notes scattered throughout and any other todo statements -> Other than possibly reworking Format for alignment cases, I believe
//              I'm done with the the current cases. All that's left here is to add std::tm inclusion and the custom formatters for the custom flags from the logger
//              side. After which, I can go ahead and format everything into the file buffer being used directly and hopefully streamline the entire logging pipeline.

constexpr auto fillBuffDefaultCapacity { 256 };
constexpr serenity::arg_formatter::ArgFormatter::ArgFormatter()
	: argCounter(0), m_indexMode(IndexMode::automatic), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}), argStorage(ArgContainer {}),
	  buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}), valueSize(size_t {}), fillBuffer(std::vector<char> {}) {
	fillBuffer.reserve(fillBuffDefaultCapacity);
}

constexpr void serenity::arg_formatter::BracketSearchResults::Reset() {
	if( !std::is_constant_evaluated() ) {
			std::memset(this, 0, sizeof(BracketSearchResults));
	} else {
			beginPos = endPos = 0;
		}
}

constexpr void serenity::arg_formatter::SpecFormatting::ResetSpecs() {
	if( !std::is_constant_evaluated() ) {
			std::memset(this, 0, sizeof(SpecFormatting));
	} else {
			argPosition = nestedWidthArgPos = nestedPrecArgPos = 0;
			localize = hasAlt = hasClosingBrace = false;
			alignmentPadding = precision = 0;
			fillCharacter = typeSpec = '\0';
			align                    = Alignment::Empty;
			signType                 = Sign::Empty;
			preAltForm               = "";
		}
}

template<typename Iter, typename... Args> constexpr auto serenity::arg_formatter::ArgFormatter::CaptureArgs(Iter&& iter, Args&&... args) -> decltype(iter) {
	return std::move(argStorage.CaptureArgs(std::move(iter), std::forward<Args>(args)...));
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	ParseFormatString(std::move(CaptureArgs(std::move(Iter), std::forward<Args>(args)...)), sv);
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, const std::locale& loc, std::string_view sv, Args&&... args) {
	ParseFormatString(std::move(CaptureArgs(std::move(Iter), std::forward<Args>(args)...)), loc, sv);
}

template<typename... Args> std::string serenity::arg_formatter::ArgFormatter::se_format(std::string_view sv, Args&&... args) {
	std::string tmp;
	tmp.reserve(ReserveCapacity(std::forward<Args>(args)...));
	se_format_to(std::back_inserter(tmp), sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename... Args> std::string serenity::arg_formatter::ArgFormatter::se_format(const std::locale& loc, std::string_view sv, Args&&... args) {
	std::string tmp;
	tmp.reserve(ReserveCapacity(std::forward<Args>(args)...));
	se_format_to(std::back_inserter(tmp), loc, sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedLeft(T&& container, const int& totalWidth) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData, buffer.data(), valueSize);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedLeft(T&& container, std::string_view val, const int& precision, const int& totalWidth) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData, val.data(), precision);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedRight(T&& container, const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, buffer.data(), valueSize);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedRight(T&& container, std::string_view val, const int& precision, const int& totalWidth,
                                                                        const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, val.data(), precision);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedCenter(T&& container, const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, buffer.data(), valueSize);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedCenter(T&& container, std::string_view val, const int& precision, const int& totalWidth,
                                                                         const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, val.data(), precision);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteNonAligned(T&& container) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(buffer.data(), valueSize);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), buffer.data(), buffer.data() + valueSize);
	} else {
			std::copy(buffer.data(), buffer.data() + valueSize, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteNonAligned(T&& container, std::string_view val, const int& precision) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(val.data(), precision);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), val.data(), val.data() + precision);
	} else {
			std::copy(val.data(), val.data() + precision, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimplePadding(T&& container, const size_t& fillAmount) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(fillBuffer.data(), fillAmount);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), fillBuffer.data(), fillBuffer.data() + fillAmount);
	} else {
			std::copy(fillBuffer.data(), fillBuffer.data() + fillAmount, std::back_inserter(container));
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::FillBuffWithChar(const int& totalWidth) {
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(fillBuffer.data(), specValues.fillCharacter, totalWidth))
								  : fillBuffer.resize(totalWidth, specValues.fillCharacter);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(T&& container, const int& totalWidth) {
	if( auto fill { (totalWidth > valueSize) ? totalWidth - valueSize : 0 }; fill != 0 ) {
			if( totalWidth > fillBuffDefaultCapacity ) fillBuffer.reserve(totalWidth);
			FillBuffWithChar(totalWidth);
			switch( specValues.align ) {
					case Alignment::AlignLeft: return WriteAlignedLeft(std::forward<FwdRef<T>>(container), totalWidth);
					case Alignment::AlignRight: return WriteAlignedRight(std::forward<FwdRef<T>>(container), totalWidth, fill);
					case Alignment::AlignCenter: return WriteAlignedCenter(std::forward<FwdRef<T>>(container), totalWidth, fill / 2);
					default: return WriteSimplePadding(std::forward<FwdRef<T>>(container), fill);
				}
	} else {
			WriteNonAligned(std::forward<FwdRef<T>>(container));
		}
}

template<typename T>    // string type overload for alignment
constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(T&& container, std::string_view val, const int& totalWidth, int precision) {
	auto size { static_cast<int>(val.size()) };
	precision = precision != 0 ? precision > size ? size : precision : size;
	if( auto fill { totalWidth > size ? totalWidth - size : 0 }; fill != 0 ) {
			if( totalWidth > fillBuffDefaultCapacity ) fillBuffer.reserve(totalWidth);
			FillBuffWithChar(totalWidth);
			switch( specValues.align ) {
					case Alignment::AlignLeft: return WriteAlignedLeft(std::forward<FwdRef<T>>(container), val, precision, totalWidth);
					case Alignment::AlignRight: return WriteAlignedRight(std::forward<FwdRef<T>>(container), val, precision, totalWidth, fill);
					case Alignment::AlignCenter: return WriteAlignedCenter(std::forward<FwdRef<T>>(container), val, precision, totalWidth, fill / 2);
					default: return WriteSimplePadding(std::forward<FwdRef<T>>(container), fill);
				}
	} else {
			WriteNonAligned(std::forward<FwdRef<T>>(container), val, precision);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Format(T&& container, const msg_details::SpecType& argType) {
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 ) {    // use this check to guard from any unnecessary additional checks
			if( IsSimpleSubstitution(argType, precision) ) {
					// Handles The Case Of No Specifiers And No Alignment
					return WriteSimpleValue(std::forward<FwdRef<T>>(container), argType);
			}
			// Handles The Case Of Specifiers And No Alignment
			switch( argType ) {
					default:
						!specValues.localize ? FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, argType)
											 : LocalizeArgument(std::forward<FwdRef<T>>(container), default_locale, precision, totalWidth, argType);
						WriteBufferToContainer(std::forward<FwdRef<T>>(container));
						return;
					case SpecType::StringViewType: [[fallthrough]];
					case SpecType::CharPointerType: [[fallthrough]];
					case SpecType::StringType: WriteFormattedString(std::forward<FwdRef<T>>(container), argType, precision); return;
				}
	}
	// Handles The Case Of Specifiers WITH Alignment
	switch( argType ) {
			default:
				!specValues.localize ? FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, argType)
									 : LocalizeArgument(std::forward<FwdRef<T>>(container), default_locale, precision, totalWidth, argType);
				FormatAlignment(std::forward<FwdRef<T>>(container), totalWidth);
				return;
			case SpecType::StringViewType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.string_view_state(specValues.argPosition), totalWidth, precision);
				return;
			case SpecType::CharPointerType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.c_string_state(specValues.argPosition), totalWidth, precision);
				return;
			case SpecType::StringType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.string_state(specValues.argPosition), totalWidth, precision);
				return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteBufferToContainer(T&& container) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(buffer.data(), valueSize);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), buffer.data(), buffer.data() + valueSize);
	} else {
			std::copy(buffer.data(), buffer.data() + valueSize, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Format(T&& container, const std::locale& loc, const msg_details::SpecType& argType) {
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 ) {    // use this check to guard from any unnecessary additional checks
			if( IsSimpleSubstitution(argType, precision) ) {
					// Handles The Case Of No Specifiers And No Alignment
					return WriteSimpleValue(std::forward<FwdRef<T>>(container), argType);
			}
			// Handles The Case Of Specifiers And No Alignment
			switch( argType ) {
					default:
						!specValues.localize ? FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, argType)
											 : LocalizeArgument(std::forward<FwdRef<T>>(container), loc, precision, totalWidth, argType);
						WriteBufferToContainer(std::forward<FwdRef<T>>(container));
						return;
					case SpecType::StringViewType: [[fallthrough]];
					case SpecType::CharPointerType: [[fallthrough]];
					case SpecType::StringType: WriteFormattedString(std::forward<FwdRef<T>>(container), argType, precision); return;
				}
	}
	// Handles The Case Of Specifiers WITH Alignment
	switch( argType ) {
			default:
				!specValues.localize ? FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, argType)
									 : LocalizeArgument(std::forward<FwdRef<T>>(container), loc, precision, totalWidth, argType);
				FormatAlignment(std::forward<FwdRef<T>>(container), totalWidth);
				return;
			case SpecType::StringViewType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.string_view_state(specValues.argPosition), totalWidth, precision);
				return;
			case SpecType::CharPointerType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.c_string_state(specValues.argPosition), totalWidth, precision);
				return;
			case SpecType::StringType:
				FormatAlignment(std::forward<FwdRef<T>>(container), argStorage.string_state(specValues.argPosition), totalWidth, precision);
				return;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::Parse(std::string_view sv, size_t& start, const msg_details::SpecType& argType) {
	auto svSize { sv.size() };
	VerifyFillAlignField(sv, start, argType);
	if( start >= svSize ) return;
	switch( sv[ start ] ) {
			case '+':
				specValues.signType = Sign::Plus;
				if( ++start >= svSize ) return;
				break;
			case '-':
				specValues.signType = Sign::Minus;
				if( ++start >= svSize ) return;
				break;
			case ' ':
				specValues.signType = Sign::Space;
				if( ++start >= svSize ) return;
				break;
			default: break;
		}
	if( sv[ start ] == '#' ) {
			VerifyAltField(sv, argType);
			if( ++start >= svSize ) return;
	}
	if( sv[ start ] == '0' ) {
			if( specValues.fillCharacter == '\0' ) {
					specValues.fillCharacter = '0';
			}
			if( ++start >= svSize ) return;
	}
	if( (sv[ start ] == '{') || (sv[ start ] >= '1' && sv[ start ] <= '9') ) {
			VerifyWidthField(sv, start);
			if( start >= svSize ) return;
	}
	if( sv[ start ] == '.' ) {
			VerifyPrecisionField(sv, start, argType);
			if( start >= svSize ) return;
	}
	if( sv[ start ] == 'L' ) {
			VerifyLocaleField(sv, start, argType);
			if( start >= svSize ) return;
	}
	if( sv[ start ] != '}' ) {
			HandlePotentialTypeField(sv[ start ], argType);
	}
}

// As cluttered as this is, I kind of have to leave it like it is at the moment. When decluttering and abstracting some
// of the common calls into functions, the call site ended up actually making this whole process ~4-5% slower
template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::ParseFormatString(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter  = 0;
	m_indexMode = IndexMode::automatic;
	auto& container { IteratorAccessHelper(Iter).Container() };
	for( ;; ) {
			specValues.ResetSpecs();
			/****************************************************************  NOTE ****************************************************************/
			// This check saw ~24-27% performance gain in most cases, however, if this check is abstracted into a function call, the function call saw
			//  ~60% drop in performance. For reference, on my desktop -> current timings are ~49ns for the custom sandbox case of formatting
			// TestPoint, without this check, it drops to ~67ns, but abstract this into a function and call it from there and it dropped to ~121ns. I have
			// no idea why there is such a hefty overhead for the call site, especially when I was forwarding a reference to the container and just the
			// string view and performing this exact check, but I'll be leaving this as-is here.
			/****************************************************************  NOTE ****************************************************************/
			// Check small sv size conditions and handle the niche cases, otherwise break and continue onward
			switch( sv.size() ) {
					case 0: return;
					case 1:
						if constexpr( std::is_same_v<type<T>, std::string> ) {
								container += sv[ 0 ];
						} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
								container.emplace_back(sv[ 0 ]);
						} else {
								std::copy(sv.data(), sv.data() + 1, std::back_inserter(container));
							}
						return;
					case 2:
						/*******************************************************  NOTE *******************************************************/
						// Simple custom args saw a ~3% gain in performance with this check while native args saw ~1% gain in performance
						/*******************************************************  NOTE *******************************************************/
						// Handle If format string only contains '{}' and no other text
						if( sv[ 0 ] == '{' && sv[ 1 ] == '}' ) {
								auto& argType { argStorage.SpecTypesCaptured()[ 0 ] };
								argType != SpecType::CustomType ? WriteSimpleValue(std::forward<FwdRef<T>>(container), std::move(argType))
																: argStorage.custom_state(0).FormatCallBack(sv);
								return;
						}
						// Otherwise, write out any remaining characters as-is and bypass the check that would have found
						// this case in FindBrackets(). In most cases, ending early here saw ~2-3% gain in performance
						if constexpr( std::is_same_v<type<T>, std::string> ) {
								container.append(sv.data(), sv.data() + 2);
						} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
								container.insert(container.end(), sv.data(), sv.data() + 2);
						} else {
								std::copy(sv.data(), sv.data() + 2, std::back_inserter(container));
							}
						return;
					default: break;
				}
			// If the above wasn't executed, then find the first pair of curly brackets and if none were found, write out the parse string as-is
			if( !FindBrackets(sv) ) {
					if constexpr( std::is_same_v<type<T>, std::string> ) {
							container.append(sv.data(), sv.size());
					} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
							container.insert(container.end(), sv.data(), sv.data() + sv.size());
							if( sv.back() != '\0' ) {
									container.push_back('\0');
							}
					} else {
							std::copy(sv.data(), sv.data() + sv.size(), std::back_inserter(container));
						}
					return;
			}
			// If the position of the first curly bracket found isn't the beginning of the parse string, then write the text as-is up until the bracket position
			auto& begin { bracketResults.beginPos };
			auto& end { bracketResults.endPos };
			if( begin > 0 ) {
					if constexpr( std::is_same_v<type<T>, std::string> ) {
							begin >= 2 ? container.append(sv.data(), begin) : container += sv[ 0 ];
					} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
							container.insert(container.end(), sv.data(), sv.data() + begin);
					} else {
							std::copy(sv.data(), sv.data() + begin, std::back_inserter(container));
						}
					sv.remove_prefix(begin);
					end -= begin;
					begin = 0;
			}
			size_t pos { 0 };
			auto bracketSize { end - begin };
			std::string_view argBracket(sv.data() + 1, sv.data() + bracketSize + 1);
			/* Since we advance the begin position by 1 in the argBracket construction, if the first char is '{' then it was an escaped bracket */
			if( argBracket[ pos ] == '{' ) {
					container.push_back('{');
					++pos;
			}
			// NOTE: Since a well-formed substitution bracket should end with '}' and we can assume it's well formed due to the check in FindBrackets(),
			//                argBracket[ bracketSize - 2 ] is used here to check if the position before the close bracket is another closing bracket instead.
			if( bracketSize > 3 && argBracket[ bracketSize - 2 ] == '}' ) specValues.hasClosingBrace = true;
			/************************************* Handle Positional Args *************************************/
			if( !VerifyPositionalField(argBracket, pos, specValues.argPosition) ) {
					// Nothing Else to Parse- just a simple substitution after position field so write it and continute parsing format string
					auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
					argType != SpecType::CustomType ? WriteSimpleValue(std::forward<FwdRef<T>>(container), argType)
													: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket);
					if( specValues.hasClosingBrace ) {
							container.push_back('}');
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			/****************************** Handle What's Left Of The Bracket ******************************/
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( argType != SpecType::CustomType ) {
					Parse(argBracket, pos, argType);
					Format(std::forward<FwdRef<T>>(container), argType);
			} else {
					argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket);
				}
			if( specValues.hasClosingBrace ) {
					container.push_back('}');
			}
			sv.remove_prefix(bracketSize + 1);
		}
}

// Same Note as the non-localized version of this function:
// As cluttered as this is, I kind of have to leave it like it is at the moment. When decluttering and abstracting some
// of the common calls into functions, the call site ended up actually making this whole process ~4-5% slower
template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::ParseFormatString(std::back_insert_iterator<T>&& Iter, const std::locale& loc, std::string_view sv) {
	argCounter  = 0;
	m_indexMode = IndexMode::automatic;
	auto& container { IteratorAccessHelper(Iter).Container() };
	for( ;; ) {
			specValues.ResetSpecs();
			switch( sv.size() ) {
					case 0: return;
					case 1: Iter = sv[ 0 ]; return;
					case 2:
						if( sv[ 0 ] == '{' && sv[ 1 ] == '}' ) {
								auto& argType { argStorage.SpecTypesCaptured()[ 0 ] };
								argType != SpecType::CustomType ? WriteSimpleValue(std::forward<FwdRef<T>>(container), std::move(argType))
																: argStorage.custom_state(0).FormatCallBack(sv);
								return;
						}
						if constexpr( std::is_same_v<type<T>, std::string> ) {
								container.append(sv.data(), sv.data() + 2);
						} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
								container.insert(container.end(), sv.data(), sv.data() + 2);
						} else {
								std::copy(sv.data(), sv.data() + sv.size(), std::back_inserter(container));
							}
						return;
					default: break;
				}
			if( !FindBrackets(sv) ) {
					if constexpr( std::is_same_v<type<T>, std::string> ) {
							container.append(sv.data(), sv.size());
					} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
							container.insert(container.end(), sv.data(), sv.data() + sv.size());
							if( sv.back() != '\0' ) {
									container.push_back('\0');
							}
					} else {
							std::copy(sv.data(), sv.data() + sv.size(), std::back_inserter(container));
						}
					return;
			}
			auto& begin { bracketResults.beginPos };
			auto& end { bracketResults.endPos };
			if( begin > 0 ) {
					if constexpr( std::is_same_v<type<T>, std::string> ) {
							begin >= 2 ? container.append(sv.data(), begin) : container += sv[ 0 ];
					} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
							container.insert(container.end(), sv.data(), sv.data() + begin);
					} else {
							std::copy(sv.data(), sv.data() + begin, std::back_inserter(container));
						}
					sv.remove_prefix(begin);
					end -= begin;
					begin = 0;
			}
			size_t pos { 0 };
			std::string_view argBracket(sv.data() + (++begin), sv.data() + (++end));
			if( argBracket[ pos ] == '{' ) {
					container.push_back('{');
					++pos;
			}
			auto bracketSize { end - begin };
			if( bracketSize > 3 && argBracket[ bracketSize - 2 ] == '}' ) specValues.hasClosingBrace = true;
			if( !VerifyPositionalField(argBracket, pos, specValues.argPosition) ) {
					auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
					argType != SpecType::CustomType ? WriteSimpleValue(std::forward<FwdRef<T>>(container), argType)
													: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket);
					if( specValues.hasClosingBrace ) {
							container.push_back('}');
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( argType != SpecType::CustomType ) {
					Parse(argBracket, pos, argType);
					Format(std::forward<FwdRef<T>>(container), loc, argType);
			} else {
					argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket);
				}
			if( specValues.hasClosingBrace ) {
					container.push_back('}');
			}
			sv.remove_prefix(bracketSize + 1);
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::FindBrackets(std::string_view sv) {
	const auto svSize { sv.size() };
	if( svSize < 3 ) return false;
	auto& begin { bracketResults.beginPos };
	auto& end { bracketResults.endPos };
	begin = end = 0;
	for( ;; ) {
			if( sv[ begin ] == '{' ) break;
			if( ++begin >= svSize ) return false;
		}
	end = begin;
	for( ;; ) {
			if( ++end >= svSize ) ReportError(ErrorType::missing_bracket);
			switch( sv[ end ] ) {
					case '}': return true; break;
					case '{':
						for( ;; ) {
								if( ++end >= svSize ) ReportError(ErrorType::missing_bracket);
								if( sv[ end ] != '}' ) continue;
								for( ;; ) {
										if( ++end >= svSize ) ReportError(ErrorType::missing_bracket);
										if( sv[ end ] != '}' ) continue;
										return true;
									}
							}
					default: continue;
				}
		}
}

// Only handles a max of two digits and foregoes any real safety checks but is faster than std::from_chars()
// in regards to its usage in VerifyPositionField() by ~38% when compiled with -02. For reference,
// std::from_chars() averaged ~2.1ns and se_from_chars() averages ~1.3ns for this specific use case.
static constexpr int offset { 48 };    // offset used to get the actual value represented in this use case
template<typename IntergralType>
requires std::is_integral_v<std::remove_cvref_t<IntergralType>>
static constexpr size_t se_from_chars(const char* begin, const char* end, IntergralType&& value) {
	for( ;; ) {
			if( !serenity::IsDigit(*begin) ) {
					if( ++begin == end ) return 0;
			}
			value = *begin - offset;
			if( !serenity::IsDigit(*(++begin)) ) return 1;
			(value *= 10) += (*begin - offset);
			return 2;
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::VerifyPositionalField(std::string_view sv, size_t& start, unsigned char& positionValue) {
	if( m_indexMode == IndexMode::automatic ) {
			// we're in automatic mode
			if( const auto& ch { sv[ start ] }; IsDigit(ch) ) {
					m_indexMode = IndexMode::manual;
					return VerifyPositionalField(sv, start, positionValue);
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
							default: ReportError(ErrorType::position_field_spec); break;
						}
			}
	} else {
			// we're in manual mode
			auto data { sv.data() };
			start += se_from_chars(data + start, data + sv.size(), positionValue);
			if( start != 0 ) {
					SE_ASSERT(positionValue <= MAX_ARG_INDEX, "Error In Position Argument Field: Max Position (24) Exceeded.");
					switch( sv[ start ] ) {
							case ':':
								++argCounter;
								++start;
								return true;
							case '}': ++start; return false;
							default: ReportError(ErrorType::position_field_mode); break;
						}
			} else {
					switch( sv[ start ] ) {
							case '}': ReportError(ErrorType::position_field_mode); break;
							case ' ':
								{
									auto size { sv.size() };
									for( ;; ) {
											if( start >= size ) break;
											if( sv[ ++start ] != ' ' ) break;
										}
									return VerifyPositionalField(sv, start, positionValue);
									break;
								}
							case ':': ReportError(ErrorType::position_field_no_position); break;
							default: ReportError(ErrorType::position_field_runon); break;
						}
				}
		}
	ReportError(ErrorType::none);
}

constexpr void serenity::arg_formatter::ArgFormatter::OnAlignLeft(const char& ch, size_t& pos) {
	specValues.align = Alignment::AlignLeft;
	++pos;
	if( ch == ':' ) {
			specValues.fillCharacter = ' ';
	} else if( ch != '{' && ch != '}' ) {
			specValues.fillCharacter = ch;
	} else {
			ReportError(ErrorType::invalid_fill_character);
		}
}
constexpr void serenity::arg_formatter::ArgFormatter::OnAlignRight(const char& ch, size_t& pos) {
	specValues.align = Alignment::AlignRight;
	++pos;
	if( ch == ':' ) {
			specValues.fillCharacter = ' ';
	} else if( ch != '{' && ch != '}' ) {
			specValues.fillCharacter = ch;
	} else {
			ReportError(ErrorType::invalid_fill_character);
		}
}
constexpr void serenity::arg_formatter::ArgFormatter::OnAlignCenter(const char& ch, size_t& pos) {
	specValues.align = Alignment::AlignCenter;
	++pos;
	if( ch == ':' ) {
			specValues.fillCharacter = ' ';
	} else if( ch != '{' && ch != '}' ) {
			specValues.fillCharacter = ch;
	} else {
			ReportError(ErrorType::invalid_fill_character);
		}
}
constexpr void serenity::arg_formatter::ArgFormatter::OnAlignDefault(const SpecType& argType, size_t& pos) {
	using enum msg_details::SpecType;
	--pos;
	switch( argType ) {
			case MonoType: return;
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case FloatType: [[fallthrough]];
			case LongDoubleType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: specValues.align = Alignment::AlignRight; break;
			default: specValues.align = Alignment::AlignLeft; break;
		}
	specValues.fillCharacter = ' ';
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyFillAlignField(std::string_view sv, size_t& currentPos, const msg_details::SpecType& argType) {
	const auto& ch { sv[ currentPos ] };
	// handle padding with and without argument to pad
	/*
	    format("{:*5}")        results in -> *****
	    format("{:*5}", 'a')  results in -> a****
	*/
	if( argType == SpecType::MonoType ) {
			specValues.fillCharacter = ch;
			// for monotype cases, don't have to worry about alignment, so just check if the position needs to be adjusted or not
			switch( ++currentPos >= sv.size() ? sv.back() : sv[ currentPos ] ) {
					default: return;
					case '<': [[fallthrough]];
					case '>': [[fallthrough]];
					case '^': ++currentPos; return;
				}
	}

	switch( ++currentPos >= sv.size() ? '}' : sv[ currentPos ] ) {
			case '<': OnAlignLeft(ch, currentPos); return;
			case '>': OnAlignRight(ch, currentPos); return;
			case '^': OnAlignCenter(ch, currentPos); return;
			default: OnAlignDefault(argType, currentPos); return;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyAltField(std::string_view sv, const msg_details::SpecType& argType) {
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
			default: ReportError(ErrorType::invalid_alt_type); break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyWidthField(std::string_view sv, size_t& currentPosition) {
	if( const auto& ch { sv[ currentPosition ] }; IsDigit(ch) ) {
			auto svData { sv.data() };
			currentPosition += se_from_chars(svData + currentPosition, svData + sv.size(), specValues.alignmentPadding);
			return;
	} else {
			switch( ch ) {
					case '{': VerifyPositionalField(sv, ++currentPosition, specValues.nestedWidthArgPos); return;
					case '}': VerifyPositionalField(sv, currentPosition, specValues.nestedWidthArgPos); return;
					default: ReportError(ErrorType::missing_bracket); return;
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyPrecisionField(std::string_view sv, size_t& currentPosition, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType: [[fallthrough]];
			case StringViewType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: break;
			default: ReportError(ErrorType::invalid_precision_type); break;
		}
	if( const auto& ch { sv[ ++currentPosition ] }; IsDigit(ch) ) {
			auto data { sv.data() };
			currentPosition += se_from_chars(data + currentPosition, data + sv.size(), specValues.precision);
			++argCounter;
			return;
	} else {
			switch( ch ) {
					case '{': VerifyPositionalField(sv, ++currentPosition, specValues.nestedPrecArgPos); return;
					case '}': VerifyPositionalField(sv, currentPosition, specValues.nestedPrecArgPos); return;
					default: ReportError(ErrorType::missing_bracket); return;
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyLocaleField(std::string_view sv, size_t& currentPosition, const msg_details::SpecType& argType) {
	using SpecType = msg_details::SpecType;
	++currentPosition;
	switch( argType ) {
			case SpecType::CharType: [[fallthrough]];
			case SpecType::ConstVoidPtrType: [[fallthrough]];
			case SpecType::MonoType: [[fallthrough]];
			case SpecType::StringType: [[fallthrough]];
			case SpecType::StringViewType: [[fallthrough]];
			case SpecType::VoidPtrType: ReportError(ErrorType::invalid_locale_type); break;
			default: specValues.localize = true; return;
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::IsSimpleSubstitution(const msg_details::SpecType& argType, const int& precision) {
	using enum SpecType;
	switch( argType ) {
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType:
				{
					return precision == 0;
				}
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType:
				{
					return !specValues.hasAlt && specValues.signType == Sign::Empty && !specValues.localize && specValues.typeSpec == '\0';
				}
			case BoolType:
				{
					return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 's');
				}
			case CharType:
				{
					return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c');
				}
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType:
				{
					return !specValues.localize && precision == 0 && specValues.signType == Sign::Empty && !specValues.hasAlt && specValues.typeSpec == '\0';
				}
			// for pointer types, if the width field is 0, there's no fill/alignment to take into account and therefore it's a simple sub
			case ConstVoidPtrType: [[fallthrough]];
			case VoidPtrType:
				{
					return true;
				}
			default: return false; break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::OnValidTypeSpec(const SpecType& type, const char& ch) {
	using namespace std::literals::string_view_literals;
	using enum msg_details::SpecType;
	specValues.typeSpec = ch;
	if( !specValues.hasAlt ) return;
	switch( type ) {
			default: return;
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: [[fallthrough]];
			case BoolType: [[fallthrough]];
			case CharType:
				switch( ch ) {
						case 'b': specValues.preAltForm = "0b"sv; return;
						case 'B': specValues.preAltForm = "0B"sv; return;
						case 'o': specValues.preAltForm = "0"sv; return;
						case 'x': specValues.preAltForm = "0x"sv; return;
						case 'X': specValues.preAltForm = "0X"sv; return;
					}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::OnInvalidTypeSpec(const SpecType& type) {
	using enum msg_details::SpecType;
	switch( type ) {
			case MonoType: return;    // possibly issue warning on a type spec being provided on no argument?
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: ReportError(ErrorType::invalid_int_spec); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: ReportError(ErrorType::invalid_float_spec); break;
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType: ReportError(ErrorType::invalid_string_spec); break;
			case BoolType: ReportError(ErrorType::invalid_bool_spec); break;
			case CharType: ReportError(ErrorType::invalid_char_spec); break;
			case ConstVoidPtrType: [[fallthrough]];
			case VoidPtrType: ReportError(ErrorType::invalid_pointer_spec); break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::HandlePotentialTypeField(const char& ch, const msg_details::SpecType& argType) {
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
			case 'X': OnValidTypeSpec(argType, ch); return;
			default: OnInvalidTypeSpec(argType); return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleString(T&& container) {
	std::string_view sv { std::move(argStorage.string_state(specValues.argPosition)) };
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					container.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleCString(T&& container) {
	std::string_view sv { std::move(argStorage.c_string_state(specValues.argPosition)) };
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					container.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleStringView(T&& container) {
	std::string_view sv { std::move(argStorage.string_view_state(specValues.argPosition)) };
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					container.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleInt(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleUInt(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleLongLong(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleULongLong(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleBool(T&& container) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			std::string_view sv { argStorage.bool_state(specValues.argPosition) ? "true" : "false" };
			container.insert(container.end(), sv.begin(), sv.end());
	} else {
			std::string_view sv { argStorage.bool_state(specValues.argPosition) ? "true" : "false" };
			std::copy(sv.begin(), sv.end(), std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleFloat(T&& container) {
	auto data { buffer.data() };
	std::memset(data, 0, buffer.size());
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleDouble(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleLongDouble(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr, std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleConstVoidPtr(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append("0x").append(
			data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			constexpr std::string_view sv { "0x" };
			container.insert(container.end(), sv.begin(), sv.end());
			container.insert(container.end(), data,
			                 std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr);
	} else {
			constexpr std::string_view sv { "0x" };
			auto& iter { std::back_inserter(container) };
			std::copy(sv.data(), sv.data() + sv.size(), iter);
			std::move(data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr, iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleVoidPtr(T&& container) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append("0x").append(
			data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr - data);
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			constexpr std::string_view sv { "0x" };
			container.insert(container.end(), sv.begin(), sv.end());
			container.insert(container.end(), data,
			                 std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr);
	} else {
			constexpr std::string_view sv { "0x" };
			auto& iter { std::back_inserter(container) };
			std::copy(sv.data(), sv.data() + sv.size(), iter);
			std::move(data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr, iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleValue(T&& container, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType: WriteSimpleString(std::forward<FwdRef<T>>(container)); return;
			case CharPointerType: WriteSimpleCString(std::forward<FwdRef<T>>(container)); return;
			case StringViewType: WriteSimpleStringView(std::forward<FwdRef<T>>(container)); return;
			case IntType: WriteSimpleInt(std::forward<FwdRef<T>>(container)); break;
			case U_IntType: WriteSimpleUInt(std::forward<FwdRef<T>>(container)); break;
			case LongLongType: WriteSimpleLongLong(std::forward<FwdRef<T>>(container)); break;
			case U_LongLongType: WriteSimpleULongLong(std::forward<FwdRef<T>>(container)); break;
			case BoolType: WriteSimpleBool(std::forward<FwdRef<T>>(container)); break;
			case CharType: container.insert(container.end(), argStorage.char_state(specValues.argPosition)); break;
			case FloatType: WriteSimpleFloat(std::forward<FwdRef<T>>(container)); break;
			case DoubleType: WriteSimpleDouble(std::forward<FwdRef<T>>(container)); break;
			case LongDoubleType: WriteSimpleLongDouble(std::forward<FwdRef<T>>(container)); break;
			case ConstVoidPtrType: WriteSimpleConstVoidPtr(std::forward<FwdRef<T>>(container)); break;
			case VoidPtrType: WriteSimpleVoidPtr(std::forward<FwdRef<T>>(container)); break;
			default: break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatCharType(char& value) {
	specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ? FormatIntegerType(static_cast<int>(value)) : WriteChar(value);
}

constexpr void serenity::arg_formatter::ArgFormatter::WriteBool(const bool& value) {
	using namespace std::string_view_literals;
	auto sv { value ? "true"sv : "false"sv };
	std::copy(sv.data(), sv.data() + sv.size(), buffer.begin());
	valueSize = sv.size();
}
constexpr void serenity::arg_formatter::ArgFormatter::FormatBoolType(bool& value) {
	specValues.typeSpec != '\0' && specValues.typeSpec != 's' ? FormatIntegerType(static_cast<unsigned char>(value)) : WriteBool(value);
}

template<typename T>
requires std::is_pointer_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::FormatPointerType(T&& value, const msg_details::SpecType& type) {
	using enum msg_details::SpecType;
	static constexpr std::string_view sv { "0x" };
	switch( type ) {
			case ConstVoidPtrType:
				{
					auto data { buffer.data() };
					std::memcpy(data, sv.data(), 2);
					valueSize = std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(std::forward<T>(value)), 16).ptr - data;
					return;
				}
			case VoidPtrType:
				{
					auto data { buffer.data() };
					std::memcpy(data, sv.data(), 2);
					valueSize = std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(std::forward<T>(value)), 16).ptr - data;
					return;
				}
			default: return;
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::FormatArgument(T&& container, const int& precision, const int& totalWidth, const msg_details::SpecType& type) {
	using enum msg_details::SpecType;
	switch( type ) {
			case IntType: FormatIntegerType(argStorage.int_state(specValues.argPosition)); return;
			case U_IntType: FormatIntegerType(argStorage.uint_state(specValues.argPosition)); return;
			case LongLongType: FormatIntegerType(argStorage.long_long_state(specValues.argPosition)); return;
			case U_LongLongType: FormatIntegerType(argStorage.u_long_long_state(specValues.argPosition)); return;
			case BoolType: FormatBoolType(argStorage.bool_state(specValues.argPosition)); return;
			case CharType: FormatCharType(argStorage.char_state(specValues.argPosition)); return;
			case FloatType: FormatFloatType(argStorage.float_state(specValues.argPosition), precision); return;
			case DoubleType: FormatFloatType(argStorage.double_state(specValues.argPosition), precision); return;
			case LongDoubleType: FormatFloatType(argStorage.long_double_state(specValues.argPosition), precision); return;
			case ConstVoidPtrType: FormatPointerType(argStorage.const_void_ptr_state(specValues.argPosition), type); return;
			case VoidPtrType: FormatPointerType(argStorage.void_ptr_state(specValues.argPosition), type); return;
			default: return;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::BufferToUpper(char* begin, const char* end) {
	// Bypassing a check on every char and only matching these cases is slightly faster and uses considerably less CPU cycles.
	// Since this is being used explicitly to convert hex values and the case 'p' (for floating point cases), these are the only
	// values that are of any concern to check here.
	for( ;; ) {
			if( begin == end ) return;
			switch( *begin ) {
					case 'a': [[fallthrough]];
					case 'b': [[fallthrough]];
					case 'c': [[fallthrough]];
					case 'd': [[fallthrough]];
					case 'e': [[fallthrough]];
					case 'f': [[fallthrough]];
					case 'p': [[fallthrough]];
					case 'x': *begin++ -= 32; continue;
					default: ++begin; continue;
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::SetFloatingFormat(std::chars_format& format, int& precision, bool& isUpper) {
	switch( specValues.typeSpec ) {
			case '\0':
				// default behaviors
				if( specValues.hasAlt && precision == 0 ) {
						format = std::chars_format::scientific;
				} else if( !specValues.localize && specValues.alignmentPadding == 0 && precision == 0 && specValues.signType == Sign::Empty ) {
						format = std::chars_format::fixed;
				} else {
						format = std::chars_format::general;
					}
				break;
			case 'a':
				precision = precision > 0 ? precision : 0;
				format    = std::chars_format::hex;
				break;
			case 'A':
				isUpper   = true;
				precision = precision > 0 ? precision : 0;
				format    = std::chars_format::hex;
				break;
			case 'e':
				format    = std::chars_format::scientific;
				precision = precision > 0 ? precision : 6;
				break;
			case 'E':
				isUpper   = true;
				format    = std::chars_format::scientific;
				precision = precision > 0 ? precision : 6;
				break;
			case 'f':
				format    = std::chars_format::fixed;
				precision = precision > 0 ? precision : 6;
				break;
			case 'F':
				isUpper   = true;
				format    = std::chars_format::fixed;
				precision = precision > 0 ? precision : 6;
				break;
			case 'g':
				format    = std::chars_format::general;
				precision = precision > 0 ? precision : 6;
				break;
			case 'G':
				isUpper   = true;
				format    = std::chars_format::general;
				precision = precision > 0 ? precision : 6;
				break;
			default: break;
		}
}

template<typename T>
requires std::is_arithmetic_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::WriteSign(T&& value, int& pos) {
	switch( specValues.signType == Sign::Space ? value < 0 ? Sign::Minus : Sign::Space : specValues.signType ) {
			case Sign::Space: buffer[ pos++ ] = ' '; return;
			case Sign::Plus: buffer[ pos++ ] = '+'; return;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: return;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::WriteChar(const char& value) {
	buffer[ 0 ] = value;
	valueSize   = 1;
}

constexpr void serenity::arg_formatter::ArgFormatter::SetIntegralFormat(int& base, bool& isUpper) {
	// spec 'c' is handled in FormatArgument() By direct write to buffer
	switch( specValues.typeSpec ) {
			case '\0': base = 10; return;
			case 'b': base = 2; return;
			case 'B':
				base    = 2;
				isUpper = true;
				return;
			case 'o': base = 8; return;
			case 'x': base = 16; return;
			case 'X':
				base    = 16;
				isUpper = true;
				return;
			default: base = 10; return;
		}
}

template<typename T>
requires std::is_floating_point_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::FormatFloatType(T&& value, int precision) {
	int pos {};
	bool isUpper { false };
	auto data { buffer.data() };
	std::chars_format format {};
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE)) : std::fill(buffer.begin(), buffer.end(), 0);
	if( specValues.signType != Sign::Empty ) WriteSign(std::forward<T>(value), pos);
	SetFloatingFormat(format, precision, isUpper);
	auto end { (precision != 0 ? std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format, precision)
		                       : std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format))
		       .ptr };
	valueSize = end - data;
	if( isUpper ) BufferToUpper(data, end);
}

template<typename T>
requires std::is_integral_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::FormatIntegerType(T&& value) {
	int pos {}, base { 10 };
	bool isUpper { false };
	auto data { buffer.data() };
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE)) : std::fill(buffer.begin(), buffer.end(), 0);
	if( specValues.signType != Sign::Empty ) WriteSign(std::forward<T>(value), pos);
	if( specValues.preAltForm.size() != 0 ) {
			std::memcpy(data + pos, specValues.preAltForm.data(), specValues.preAltForm.size());
			pos += static_cast<int>(specValues.preAltForm.size());    // safe to downcast as it will only ever be positive and max val of 2
	}
	SetIntegralFormat(base, isUpper);
	valueSize = std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, base).ptr - data;
	if( isUpper ) BufferToUpper(data, data + valueSize);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatStringType(T&& container, std::string_view val, const int& precision) {
	int size { static_cast<int>(val.size()) };
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container.append(val.data(), (precision != 0 ? precision > size ? size : precision : size));
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), val.data(), val.data() + (precision != 0 ? precision > size ? size : precision : size));
			if( val.back() != '\0' ) {
					container.push_back('\0');
			}
	} else {
			std::copy(val.data(), val.data() + (precision != 0 ? precision > size ? size : precision : size), std::back_inserter(container));
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteFormattedString(T&& container, const SpecType& type, const int& precision) {
	using enum msg_details::SpecType;
	switch( type ) {
			case StringViewType: return FormatStringType(std::forward<FwdRef<T>>(container), argStorage.string_view_state(specValues.argPosition), precision);
			case StringType: return FormatStringType(std::forward<FwdRef<T>>(container), argStorage.string_state(specValues.argPosition), precision);
			case CharPointerType: return FormatStringType(std::forward<FwdRef<T>>(container), argStorage.c_string_state(specValues.argPosition), precision);
			default: return;
		}
}

// Now that the runtime errors are organized in a neater fashion, would really love to figure out
// how libfmt does compile-time checking.  A lot of what is being used to verify things are all
// runtime-access stuff so I'm assuming achieving this won't be easy at all =/
constexpr void serenity::arg_formatter::ArgFormatter::ReportError(ErrorType err) {
	using enum ErrorType;
	switch( err ) {
			case missing_bracket: throw format_error(format_error_messages[ 1 ]); break;
			case position_field_spec: throw format_error(format_error_messages[ 2 ]); break;
			case position_field_mode: throw format_error(format_error_messages[ 3 ]); break;
			case position_field_no_position: throw format_error(format_error_messages[ 4 ]); break;
			case position_field_runon: throw format_error(format_error_messages[ 5 ]); break;
			case max_args_exceeded: throw format_error(format_error_messages[ 6 ]); break;
			case invalid_fill_character: throw format_error(format_error_messages[ 7 ]); break;
			case invalid_alt_type: throw format_error(format_error_messages[ 8 ]); break;
			case invalid_precision_type: throw format_error(format_error_messages[ 9 ]); break;
			case invalid_locale_type: throw format_error(format_error_messages[ 10 ]); break;
			case invalid_int_spec: throw format_error(format_error_messages[ 11 ]); break;
			case invalid_float_spec: throw format_error(format_error_messages[ 12 ]); break;
			case invalid_string_spec: throw format_error(format_error_messages[ 13 ]); break;
			case invalid_bool_spec: throw format_error(format_error_messages[ 14 ]); break;
			case invalid_char_spec: throw format_error(format_error_messages[ 15 ]); break;
			case invalid_pointer_spec: throw format_error(format_error_messages[ 16 ]); break;
			default: throw format_error(format_error_messages[ 0 ]); break;
		}
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeArgument(T&& container, const std::locale& loc, const int& precision, const int& totalWidth,
                                                             const msg_details::SpecType& type) {
	using enum serenity::msg_details::SpecType;
	// NOTE: The following types should have been caught in the verification process:  monostate, string, c-string, string view, const void*, void *
	switch( type ) {
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: LocalizeIntegral(std::forward<FwdRef<T>>(container), loc, precision, totalWidth, type); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: [[fallthrough]];
			case U_LongLongType: LocalizeFloatingPoint(std::forward<FwdRef<T>>(container), loc, precision, totalWidth, type); break;
			case BoolType: LocalizeBool(std::forward<FwdRef<T>>(container), loc); break;
		}
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeIntegral(T&& container, const std::locale& loc, const int& precision, const int& totalWidth,
                                                             const msg_details::SpecType& type) {
	FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, type);
	FormatIntegralGrouping(loc, valueSize);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeFloatingPoint(T&& container, const std::locale& loc, const int& precision, const int& totalWidth,
                                                                  const msg_details::SpecType& type) {
	FormatArgument(std::forward<FwdRef<T>>(container), precision, totalWidth, type);
	size_t pos { 0 };
	bool hasMantissa { false };
	for( ;; ) {
			if( pos >= valueSize ) break;
			if( buffer[ pos ] == '.' ) {
					hasMantissa = true;
					FormatIntegralGrouping(loc, pos);
					buffer[ pos++ ] = std::use_facet<std::numpunct<char>>(loc).decimal_point();
					break;
			}
			++pos;
		}
	if( !hasMantissa ) {
			FormatIntegralGrouping(loc, valueSize);
	}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::LocalizeBool(T&& container, const std::locale& loc) {
	std::string_view sv { argStorage.bool_state(specValues.argPosition) ? std::use_facet<std::numpunct<char>>(loc).truename()
		                                                                : std::use_facet<std::numpunct<char>>(loc).falsename() };
	valueSize = sv.size();
	std::copy(sv.data(), sv.data() + valueSize, buffer.begin());
}
