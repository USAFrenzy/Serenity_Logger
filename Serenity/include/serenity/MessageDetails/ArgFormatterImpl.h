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

// Copy-pasta from Common.h -> avoids the cyclic include from Common.h when USE_STD_FORMAT and USE_FMTLIB aren't defined
static constexpr bool IsDigit(const char& ch) {
	return ((ch >= '0') && (ch <= '9'));
}

static constexpr bool IsAlpha(const char& ch) {
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}
// TODO: Address the notes scattered throughout and any other todo statements -> Other than possibly reworking Format for alignment cases, I believe
//              I'm done with the the current cases. All that's left here is to add std::tm inclusion and the custom formatters for the custom flags from the logger
//              side. After which, I can go ahead and format everything into the file buffer being used directly and hopefully streamline the entire logging pipeline.

static constexpr std::array<const char*, 12> short_months  = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static constexpr std::array<const char*, 7> short_weekdays = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static constexpr std::array<const char*, 7> long_weekdays  = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static constexpr std::array<const char*, 12> long_months   = { "January", "February", "March",     "April",   "May",      "June",
	                                                           "July",    "August",   "September", "October", "November", "December" };
// Only handles a max of two digits and foregoes any real safety checks but is faster than std::from_chars()
// in regards to its usage in VerifyPositionField() by ~38% when compiled with -02. For reference,
// std::from_chars() averaged ~2.1ns and se_from_chars() averages ~1.3ns for this specific use case.
static constexpr int offset { 48 };    // offset used to get the actual value represented in this use case
template<typename IntergralType>
requires std::is_integral_v<std::remove_cvref_t<IntergralType>>
static constexpr size_t se_from_chars(const char* begin, const char* end, IntergralType&& value) {
	for( ;; ) {
			if( !IsDigit(*begin) ) {
					if( ++begin == end ) return 0;
			}
			value = *begin - offset;
			if( !IsDigit(*(++begin)) ) return 1;
			(value *= 10) += (*begin - offset);
			return 2;
		}
}

constexpr auto fillBuffDefaultCapacity { 256 };
constexpr serenity::arg_formatter::ArgFormatter::ArgFormatter()
	: argCounter(0), m_indexMode(IndexMode::automatic), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}), argStorage(ArgContainer {}),
	  buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}), valueSize(size_t {}), fillBuffer(std::vector<char> {}) {
	// Initialize now to lower the initial cost when formatting (brings initial cost from ~33us down to ~11us). The Call To UtcOffset() will initialize
	// TimeZoneInstance() via TimeZone() via TZInfo() -> thereby initializing  all function statics.
	// NOTE: would still love a constexpr friendly version of this but I'm not finding anything online that says that might be remotely possible using the standard
	if( !std::is_constant_evaluated() ) serenity::globals::UtcOffset();
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

static constexpr std::vector<char> ConvBuffToSigned(std::vector<unsigned char>& buff, size_t endPos) {
	std::vector<char> signedTemp(endPos);
	auto pos { -1 };
	for( ;; ) {
			if( ++pos >= endPos ) break;
			signedTemp[ pos ] = static_cast<signed char>(buff[ pos ]);
		}
	return std::move(signedTemp);
}

template<typename T> struct is_basic_char_buff;
template<typename T>
struct is_basic_char_buff: std::bool_constant<std::is_same_v<type<T>, std::array<char, serenity::arg_formatter::SERENITY_ARG_BUFFER_SIZE>> ||
                                              std::is_same_v<type<T>, std::vector<unsigned char>> || std::is_same_v<type<T>, std::vector<char>>>
{
};
template<typename T> inline constexpr bool is_basic_char_buff_v = is_basic_char_buff<T>::value;

template<typename T, typename U> static constexpr void FlushBuffer(T&& buff, size_t endPos, U&& container) requires is_basic_char_buff_v<serenity::type<T>> {
	if constexpr( std::is_same_v<type<U>, std::string> ) {
			container.append(buff.data(), endPos);
	} else if constexpr( std::is_same_v<type<U>, std::vector<typename type<U>::value_type>> ) {
			container.insert(container.end(), buff.data(), buff.data() + endPos);
	} else {
			auto iter { std::back_inserter(container) };
			std::copy(buff.data(), buff.data() + endPos, iter);
		}
}

// I'll wait until I get back home, but may just go ahead and reduce the buffers down to one buffer and resize when needed
template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteBufferToContainer(T&& container) {
	if( !specValues.localize ) {
			using BuffRef = FwdRef<std::array<char, SERENITY_ARG_BUFFER_SIZE>>;
			FlushBuffer(std::forward<BuffRef>(buffer), valueSize, std::forward<T>(container));
	} else {
			using BuffRef = FwdRef<std::vector<unsigned char>>;
			auto& localeBuff { specValues.localizationBuff };
			if constexpr( std::is_signed_v<char> ) {
					// Since localeBuff is of unsigned char tyoe, need to convert to signed char for systems that use signed char as the basic char type
					FlushBuffer(std::move(ConvBuffToSigned(localeBuff, valueSize)), valueSize, std::forward<T>(container));
			} else {
					FlushBuffer(std::forward<BuffRef>(localeBuff), valueSize, std::forward<T>(container));
				}
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
						!specValues.localize ? FormatArgument(precision, totalWidth, argType) : LocalizeArgument(default_locale, precision, totalWidth, argType);
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
				!specValues.localize ? FormatArgument(precision, totalWidth, argType) : LocalizeArgument(default_locale, precision, totalWidth, argType);
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
						!specValues.localize ? FormatArgument(precision, totalWidth, argType) : LocalizeArgument(loc, precision, totalWidth, argType);
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
				!specValues.localize ? FormatArgument(precision, totalWidth, argType) : LocalizeArgument(loc, precision, totalWidth, argType);
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

static constexpr void ParseChronoSpec(const char& ch, const char& next) {
	// this will be for chrono types
	switch( ch ) {
			case '%': break;
			case 'n': break;
			case 'p': break;
			case 'q': break;
			case 'r': break;
			case 't': break;
			case 'E':
				if( next == 'X' ) /*do something*/
					break;
			case 'H': break;
			case 'I': break;
			case 'M': break;
			case '0':
				switch( next ) {
						case 'H': break;
						case 'I': break;
						case 'M': break;
						case 'S': break;
						default: break;
					}
				break;
			case 'Q': break;
			case 'R': break;
			case 'S': break;
			case 'T': break;
			case 'X': break;
			default: /*ReportError(ErrorType::invalid_ctime_spec);*/ break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyTimeSpec(std::string_view sv, size_t& pos) {
	// this is for c-time formatting
	auto size { sv.size() };
	for( ;; ) {
			switch( sv[ pos ] ) {
					case 'E':
						switch( ++pos < sv.size() ? sv[ pos ] : '}' ) {
								case 'c': [[fallthrough]];
								case 'x': [[fallthrough]];
								case 'y': [[fallthrough]];
								case 'C': [[fallthrough]];
								case 'X': [[fallthrough]];
								case 'Y':
									specValues.timeSpecFormat[ specValues.timeSpecCounter ]      = LocaleFormat::localized;
									specValues.timeSpecContainer[ specValues.timeSpecCounter++ ] = sv[ pos ];
									break;
								default: errHandle.ReportError(ErrorType::invalid_ctime_spec);
							}
						break;
					case 'O':
						switch( ++pos < sv.size() ? sv[ pos ] : '}' ) {
								case 'd': [[fallthrough]];
								case 'e': [[fallthrough]];
								case 'm': [[fallthrough]];
								case 'u': [[fallthrough]];
								case 'w': [[fallthrough]];
								case 'y': [[fallthrough]];
								case 'H': [[fallthrough]];
								case 'I': [[fallthrough]];
								case 'M': [[fallthrough]];
								case 'S': [[fallthrough]];
								case 'U': [[fallthrough]];
								case 'V': [[fallthrough]];
								case 'W':
									specValues.timeSpecFormat[ specValues.timeSpecCounter ]      = LocaleFormat::localized;
									specValues.timeSpecContainer[ specValues.timeSpecCounter++ ] = sv[ pos ];
									break;
								default: errHandle.ReportError(ErrorType::invalid_ctime_spec);
							}
						break;
					case 'a': [[fallthrough]];
					case 'b': [[fallthrough]];
					case 'c': [[fallthrough]];
					case 'd': [[fallthrough]];
					case 'e': [[fallthrough]];
					case 'g': [[fallthrough]];
					case 'h': [[fallthrough]];
					case 'j': [[fallthrough]];
					case 'm': [[fallthrough]];
					case 'n': [[fallthrough]];
					case 'p': [[fallthrough]];
					case 'r': [[fallthrough]];
					case 't': [[fallthrough]];
					case 'u': [[fallthrough]];
					case 'w': [[fallthrough]];
					case 'x': [[fallthrough]];
					case 'y': [[fallthrough]];
					case 'z': [[fallthrough]];
					case 'A': [[fallthrough]];
					case 'B': [[fallthrough]];
					case 'C': [[fallthrough]];
					case 'D': [[fallthrough]];
					case 'F': [[fallthrough]];
					case 'G': [[fallthrough]];
					case 'H': [[fallthrough]];
					case 'I': [[fallthrough]];
					case 'M': [[fallthrough]];
					case 'R': [[fallthrough]];
					case 'S': [[fallthrough]];
					case 'T': [[fallthrough]];
					case 'U': [[fallthrough]];
					case 'V': [[fallthrough]];
					case 'W': [[fallthrough]];
					case 'X': [[fallthrough]];
					case 'Y': [[fallthrough]];
					case 'Z': [[fallthrough]];
					case '%': [[fallthrough]];
					default:
						specValues.timeSpecFormat[ specValues.timeSpecCounter ]      = LocaleFormat::standard;
						specValues.timeSpecContainer[ specValues.timeSpecCounter++ ] = sv[ pos ];
						break;
				}
			if( ++pos >= size ) return;
			switch( sv[ pos ] ) {
					case '%': ++pos >= size ? errHandle.ReportError(ErrorType::missing_ctime_spec) : void(0); continue;
					case '}': return;
					default: continue;
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyFillAlignTimeField(std::string_view sv, size_t& currentPos) {
	const auto& ch { sv[ currentPos ] };
	switch( ++currentPos >= sv.size() ? '}' : sv[ currentPos ] ) {
			case '<': OnAlignLeft(ch, currentPos); return;
			case '>': OnAlignRight(ch, currentPos); return;
			case '^': OnAlignCenter(ch, currentPos); return;
			default:
				--currentPos;
				specValues.fillCharacter = ' ';
				break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyTimePrecisionField(std::string_view sv, size_t& currentPosition) {
	using enum msg_details::SpecType;
	if( const auto& ch { sv[ ++currentPosition ] }; IsDigit(ch) ) {
			auto data { sv.data() };
			currentPosition += se_from_chars(data + currentPosition, data + sv.size(), specValues.precision);
			++argCounter;
			return;
	} else {
			switch( ch ) {
					case '{': VerifyPositionalField(sv, ++currentPosition, specValues.nestedPrecArgPos); return;
					case '}': VerifyPositionalField(sv, currentPosition, specValues.nestedPrecArgPos); return;
					default: errHandle.ReportError(ErrorType::missing_bracket); return;
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::ParseTimeField(std::string_view sv, size_t& start) {
	auto svSize { sv.size() };
	if( sv[ start ] != '%' ) {
			VerifyFillAlignTimeField(sv, start);
			if( start >= svSize ) return;
	}
	if( (sv[ start ] == '{') || (sv[ start ] >= '1' && sv[ start ] <= '9') ) {
			VerifyWidthField(sv, start);
			if( start >= svSize ) return;
	}
	if( sv[ start ] == '.' ) {
			VerifyTimePrecisionField(sv, start);
			if( start >= svSize ) return;
	}
	if( sv[ start ] == 'L' ) {
			specValues.localize = true;
			if( ++start >= svSize ) return;
	}
	// assume the next ch is '%' and advance past it
	VerifyTimeSpec(sv, ++start);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatTimeField(T&& container) {
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 && precision == 0 && specValues.timeSpecCounter < 2 ) {
			return WriteSimpleCTime(std::forward<FwdRef<T>>(container));
	} else if( totalWidth == 0 ) {
			!specValues.localize ? FormatCTime(argStorage.c_time_state(specValues.argPosition), precision, 0, specValues.timeSpecCounter)
								 : LocalizeCTime(default_locale, argStorage.c_time_state(specValues.argPosition), precision);
			return WriteBufferToContainer(std::forward<FwdRef<T>>(container));
	} else {
			!specValues.localize ? FormatCTime(argStorage.c_time_state(specValues.argPosition), precision, 0, specValues.timeSpecCounter)
								 : LocalizeCTime(default_locale, argStorage.c_time_state(specValues.argPosition), precision);
			FormatAlignment(std::forward<FwdRef<T>>(container), totalWidth);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatTimeField(T&& container, const std::locale& loc) {
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 && precision == 0 && specValues.timeSpecCounter < 2 ) {
			return WriteSimpleCTime(std::forward<FwdRef<T>>(container));
	} else if( totalWidth == 0 ) {
			!specValues.localize ? FormatCTime(argStorage.c_time_state(specValues.argPosition), precision, 0, specValues.timeSpecCounter)
								 : LocalizeCTime(loc, argStorage.c_time_state(specValues.argPosition), precision);
			return WriteBufferToContainer(std::forward<FwdRef<T>>(container));
	} else {
			!specValues.localize ? FormatCTime(argStorage.c_time_state(specValues.argPosition), precision, 0, specValues.timeSpecCounter)
								 : LocalizeCTime(loc, argStorage.c_time_state(specValues.argPosition), precision);
			FormatAlignment(std::forward<FwdRef<T>>(container), totalWidth);
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
	valueSize   = 0;
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
								switch( argType ) {
										case SpecType::CustomType: argStorage.custom_state(0).FormatCallBack(sv); return;
										default: WriteSimpleValue(std::forward<FwdRef<T>>(container), std::move(argType)); return;
									}
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
					switch( argType ) {
							case SpecType::CustomType: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket); break;
							case SpecType::CTimeType: WriteSimpleCTime(std::forward<FwdRef<T>>(container)); break;
							default: WriteSimpleValue(std::forward<FwdRef<T>>(container), argType); break;
						}
					if( specValues.hasClosingBrace ) {
							container.push_back('}');
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			/****************************** Handle What's Left Of The Bracket ******************************/
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			switch( argType ) {
					case SpecType::CustomType: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket); break;
					case SpecType::CTimeType:
						ParseTimeField(argBracket, pos);
						FormatTimeField(std::forward<FwdRef<T>>(container));
						break;
					default:
						Parse(argBracket, pos, argType);
						Format(std::forward<FwdRef<T>>(container), argType);
						break;
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
	valueSize   = 0;
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
								switch( argType ) {
										case SpecType::CustomType: argStorage.custom_state(0).FormatCallBack(sv); return;
										default: WriteSimpleValue(std::forward<FwdRef<T>>(container), std::move(argType)); return;
									}
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
					switch( argType ) {
							case SpecType::CustomType: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket); break;
							case SpecType::CTimeType: WriteSimpleCTime(std::forward<FwdRef<T>>(container)); break;
							default: WriteSimpleValue(std::forward<FwdRef<T>>(container), argType); break;
						}
					if( specValues.hasClosingBrace ) {
							container.push_back('}');
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			/****************************** Handle What's Left Of The Bracket ******************************/
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			switch( argType ) {
					case SpecType::CustomType: argStorage.custom_state(specValues.argPosition).FormatCallBack(argBracket); break;
					case SpecType::CTimeType:
						ParseTimeField(argBracket, pos);
						FormatTimeField(std::forward<FwdRef<T>>(container), loc);
						break;
					default:
						Parse(argBracket, pos, argType);
						Format(std::forward<FwdRef<T>>(container), loc, argType);
						break;
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
			if( ++end >= svSize ) errHandle.ReportError(ErrorType::missing_bracket);
			switch( sv[ end ] ) {
					case '}': return true; break;
					case '{':
						for( ;; ) {
								if( ++end >= svSize ) errHandle.ReportError(ErrorType::missing_bracket);
								if( sv[ end ] != '}' ) continue;
								for( ;; ) {
										if( ++end >= svSize ) errHandle.ReportError(ErrorType::missing_bracket);
										if( sv[ end ] != '}' ) continue;
										return true;
									}
							}
					default: continue;
				}
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::VerifyPositionalField(std::string_view sv, size_t& start, unsigned char& positionValue) {
	if( m_indexMode == IndexMode::automatic ) {
			// we're in automatic mode
			if( const auto& ch { sv[ start ] }; IsDigit(ch) ) {
					m_indexMode = IndexMode::manual;
					return VerifyPositionalField(sv, start, positionValue);
			} else if( ch == '}' ) {
					positionValue = argCounter;
					++argCounter;
					return false;
			} else if( ch == ':' ) {
					positionValue = argCounter;
					++argCounter;
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
								positionValue = argCounter;
								++argCounter;
								++start;
								return true;
								break;
							default: errHandle.ReportError(ErrorType::position_field_spec); break;
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
							default: errHandle.ReportError(ErrorType::position_field_mode); break;
						}
			} else {
					switch( sv[ start ] ) {
							case '}': errHandle.ReportError(ErrorType::position_field_mode); break;
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
							case ':': errHandle.ReportError(ErrorType::position_field_no_position); break;
							default: errHandle.ReportError(ErrorType::position_field_runon); break;
						}
				}
		}
	errHandle.ReportError(ErrorType::none);
}

constexpr void serenity::arg_formatter::ArgFormatter::OnAlignLeft(const char& ch, size_t& pos) {
	specValues.align = Alignment::AlignLeft;
	++pos;
	if( ch == ':' ) {
			specValues.fillCharacter = ' ';
	} else if( ch != '{' && ch != '}' ) {
			specValues.fillCharacter = ch;
	} else {
			errHandle.ReportError(ErrorType::invalid_fill_character);
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
			errHandle.ReportError(ErrorType::invalid_fill_character);
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
			errHandle.ReportError(ErrorType::invalid_fill_character);
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
			default: errHandle.ReportError(ErrorType::invalid_alt_type); break;
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
					default: errHandle.ReportError(ErrorType::missing_bracket); return;
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
			default: errHandle.ReportError(ErrorType::invalid_precision_type); break;
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
					default: errHandle.ReportError(ErrorType::missing_bracket); return;
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
			case SpecType::VoidPtrType: errHandle.ReportError(ErrorType::invalid_locale_type); break;
			default: specValues.localize = true; return;
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::IsSimpleSubstitution(const msg_details::SpecType& argType, const int& prec) {
	using enum SpecType;
	switch( argType ) {
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType: return prec == 0;
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: return !specValues.hasAlt && specValues.signType == Sign::Empty && !specValues.localize && specValues.typeSpec == '\0';
			case BoolType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 's');
			case CharType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c');
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: return !specValues.localize && prec == 0 && specValues.signType == Sign::Empty && !specValues.hasAlt && specValues.typeSpec == '\0';
			// for pointer types, if the width field is 0, there's no fill/alignment to take into account and therefore it's a simple sub
			case ConstVoidPtrType: [[fallthrough]];
			case VoidPtrType: return true;
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
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: errHandle.ReportError(ErrorType::invalid_int_spec); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: errHandle.ReportError(ErrorType::invalid_float_spec); break;
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType: errHandle.ReportError(ErrorType::invalid_string_spec); break;
			case BoolType: errHandle.ReportError(ErrorType::invalid_bool_spec); break;
			case CharType: errHandle.ReportError(ErrorType::invalid_char_spec); break;
			case ConstVoidPtrType: [[fallthrough]];
			case VoidPtrType: errHandle.ReportError(ErrorType::invalid_pointer_spec); break;
			case MonoType: [[fallthrough]];
			case CustomType: [[fallthrough]];
			case CTimeType: return;    // possibly issue warning on a type spec being provided on no argument?
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

template<typename T>
requires std::is_integral_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::TwoDigitToBuff(T val) {
	buffer[ valueSize++ ] = val > 9 ? static_cast<char>((val / 10) + offset) : '0';
	buffer[ valueSize++ ] = static_cast<char>((val % 10) + offset);
}

constexpr void serenity::arg_formatter::ArgFormatter::Format24HourTime(int hour, int min, int sec, int precision) {
	Format24HM(hour, min);
	buffer[ valueSize++ ] = ':';
	TwoDigitToBuff(sec);
	if( precision != 0 ) FormatSubseconds(precision);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Write24HourTime(T&& container, const int& hour, const int& min, const int& sec) {
	Format24HourTime(hour, min, sec);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatShortMonth(int mon) {
	auto month { short_months[ mon ] };
	int pos { 0 };
	for( ;; ) {
			buffer[ valueSize++ ] = month[ pos ];
			if( ++pos >= 3 ) return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteShortMonth(T&& container, const int& mon) {
	FormatShortMonth(mon);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatShortWeekday(int wkday) {
	auto wkDay { short_weekdays[ wkday ] };
	int pos { 0 };
	for( ;; ) {
			buffer[ valueSize++ ] = wkDay[ pos ];
			if( ++pos >= 3 ) return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteShortWeekday(T&& container, const int& wkday) {
	FormatShortWeekday(wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatTimeDate(const std::tm& time) {
	FormatShortWeekday(time.tm_wday);
	buffer[ valueSize++ ] = ' ';
	FormatShortMonth(time.tm_mon);
	buffer[ valueSize++ ] = ' ';
	TwoDigitToBuff(time.tm_mday);
	buffer[ valueSize++ ] = ' ';
	Format24HourTime(time.tm_hour, time.tm_min, time.tm_sec);
	buffer[ valueSize++ ] = ' ';
	FormatLongYear(time.tm_year);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteTimeDate(T&& container, const std::tm& time) {
	FormatTimeDate(time);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteShortYear(T&& container, const int& year) {
	FormatShortYear(year);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatShortYear(int year) {
	year %= 100;
	buffer[ valueSize++ ] = static_cast<char>((year / 10) + offset);
	buffer[ valueSize++ ] = static_cast<char>((year % 10) + offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WritePaddedDay(T&& container, const int& day) {
	TwoDigitToBuff(day);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSpacePaddedDay(T&& container, const int& day) {
	FormatSpacePaddedDay(day);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatSpacePaddedDay(int day) {
	buffer[ valueSize++ ] = day > 9 ? static_cast<char>((day / 10) + offset) : ' ';
	buffer[ valueSize++ ] = static_cast<char>((day % 10) + offset);
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteShortIsoWeekYear(T&& container, const int& year, const int& yrday, const int& wkday) {
	FormatShortIsoWeekYear(year, yrday, wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatShortIsoWeekYear(int year, int yrday, int wkday) {
	year += 1900;
	auto w { (10 + yrday - wkday) / 7 };
	if( w < 1 ) return FormatShortYear(year - 1901);    // decrement year
	auto prevYear { year - 1 };
	int weeks { 52 + ((year / 4 - year / 100 - year / 400 == 4) || ((prevYear / 4 - prevYear / 100 - prevYear / 400) == 3) ? 1 : 0) };
	if( w > weeks ) return FormatShortYear(year - 1899);    // increment year
	return FormatShortYear(year - 1900);                    // Use current year
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteDayOfYear(T&& container, const int& day) {
	FormatDayOfYear(day);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatDayOfYear(int day) {
	++day;    // increment due to the inclusion of 0 -> day  0 is day 1 of year
	buffer[ valueSize++ ] = day > 99 ? static_cast<char>((day / 100) + offset) : '0';
	buffer[ valueSize++ ] = day > 9 ? static_cast<char>(day > 99 ? ((day / 10) % 10) + offset : ((day / 10) + offset)) : '0';
	buffer[ valueSize++ ] = static_cast<char>((day % 10) + offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WritePaddedMonth(T&& container, const int& month) {
	TwoDigitToBuff(month + 1);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteLiteral(T&& container, unsigned char lit) {
	if constexpr( std::is_same_v<type<T>, std::string> ) {
			container += lit;
	} else if constexpr( std::is_same_v<type<T>, std::vector<typename type<T>::value_type>> ) {
			container.insert(container.end(), &lit, &lit + 1);
	} else {
			std::copy(&lit, &lit + 1, std::back_inserter(container));
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatLiteral(unsigned char lit) {
	buffer[ valueSize++ ] = lit;
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteAMPM(T&& container, const int& hour) {
	FormatAMPM(hour);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatAMPM(int hour) {
	buffer[ valueSize++ ] = hour >= 12 ? 'P' : 'A';
	buffer[ valueSize++ ] = 'M';
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Write12HourTime(T&& container, const int& hour, const int& min, const int& sec) {
	Format24HourTime(hour > 12 ? hour - 12 : hour, min, sec);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::Format12HourTime(int hour, int min, int sec, int precision) {
	Format24HourTime(hour > 12 ? hour - 12 : hour, min, sec, precision);
	FormatAMPM(hour);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteWeekdayDec(T&& container, const int& wkday) {
	FormatWeekdayDec(wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatWeekdayDec(int wkday) {
	buffer[ valueSize++ ] = static_cast<char>(wkday += offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteMMDDYY(T&& container, const int& month, const int& day, const int& year) {
	FormatMMDDYY(month, day, year);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatMMDDYY(int month, int day, int year) {
	year %= 100;
	++month;
	FormatShortMonth(month);
	buffer[ valueSize++ ] = '/';
	TwoDigitToBuff(day);
	buffer[ valueSize++ ] = '/';
	FormatShortYear(year);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteIsoWeekDec(T&& container, const int& wkday) {
	FormatIsoWeekDec(wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatIsoWeekDec(int wkday) {
	buffer[ valueSize++ ] = static_cast<char>((wkday != 0 ? wkday : 7) + offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteUtcOffset(T&& container) {
	FormatUtcOffset();
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteLongWeekday(T&& container, const int& wkday) {
	FormatLongWeekday(wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatLongWeekday(int wkday) {
	std::string_view weekday { long_weekdays[ wkday ] };
	int pos { 0 };
	auto size { weekday.size() };
	for( ;; ) {
			buffer[ valueSize++ ] = weekday[ pos ];
			if( ++pos >= size ) return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteLongMonth(T&& container, const int& mon) {
	FormatLongMonth(mon);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatLongMonth(int mon) {
	std::string_view month { long_months[ mon ] };
	int pos { 0 };
	auto size { month.size() };
	for( ;; ) {
			buffer[ valueSize++ ] = month[ pos ];
			if( ++pos >= size ) return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteYYYYMMDD(T&& container, const int& year, const int& mon, const int& day) {
	FormatYYYYMMDD(year, mon, day);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatYYYYMMDD(int year, int mon, int day) {
	year += 1900;
	++mon;
	FormatLongYear(year);
	buffer[ valueSize++ ] = '-';
	FormatShortMonth(mon);
	buffer[ valueSize++ ] = '-';
	TwoDigitToBuff(day);
}
template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteLongYear(T&& container, int year) {
	FormatLongYear(year);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatLongYear(int year) {
	year += 1900;
	buffer[ valueSize++ ] = static_cast<char>(year / 1000 + offset);
	buffer[ valueSize++ ] = static_cast<char>((year / 100) % 10 + offset);
	buffer[ valueSize++ ] = static_cast<char>((year % 100) / 10 + offset);
	buffer[ valueSize++ ] = static_cast<char>((year % 100) % 10 + offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteLongIsoWeekYear(T&& container, const int& year, const int& yrday, const int& wkday) {
	FormatLongIsoWeekYear(year, yrday, wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatLongIsoWeekYear(int year, int yrday, int wkday) {
	year += 1900;
	auto w { (10 + yrday - wkday) / 7 };
	if( w < 1 ) return FormatLongYear(year - 1901);    // decrement year
	auto prevYear { year - 1 };
	int weeks { 52 + ((year / 4 - year / 100 - year / 400 == 4) || ((prevYear / 4 - prevYear / 100 - prevYear / 400) == 3) ? 1 : 0) };
	if( w > weeks ) return FormatLongYear(year - 1899);    // increment year
	return FormatLongYear(year - 1900);                    // Use current year
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteTruncatedYear(T&& container, const int& year) {
	FormatTruncatedYear(year);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatTruncatedYear(int year) {
	(year += 1900) /= 100;
	buffer[ valueSize++ ] = static_cast<char>((year / 10) + offset);
	buffer[ valueSize++ ] = static_cast<char>((year % 10) + offset);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Write24Hour(T&& container, const int& hour) {
	TwoDigitToBuff(hour);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Write12Hour(T&& container, const int& hour) {
	TwoDigitToBuff(hour > 12 ? hour - 12 : hour);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteMinute(T&& container, const int& min) {
	TwoDigitToBuff(min);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Write24HM(T&& container, const int& hour, const int& min) {
	Format24HM(hour, min);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::Format24HM(int hour, int min) {
	TwoDigitToBuff(hour);
	buffer[ valueSize++ ] = ':';
	TwoDigitToBuff(min);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSecond(T&& container, const int& sec) {
	TwoDigitToBuff(sec);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteTime(T&& container, const int& hour, const int& min, const int& sec) {
	Format24HourTime(hour, min, sec);
	(hour, min, sec);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteTZName(T&& container) {
	FormatTZName();
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteWeek(T&& container, const int& yrday, const int& wkday) {
	TwoDigitToBuff((10 + yrday - wkday) / 7);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteIsoWeek(T&& container, const int& yrday, const int& wkday) {
	TwoDigitToBuff((yrday + 7 - (wkday == 0 ? 6 : wkday - 1)) / 7);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteIsoWeekNumber(T&& container, const int& year, const int& yrday, const int& wkday) {
	FormatIsoWeekNumber(year, yrday, wkday);
	WriteBufferToContainer(std::forward<FwdRef<T>>(container));
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatIsoWeekNumber(int year, int yrday, int wkday) {
	auto w = (10 + yrday - wkday) / 7;
	if( w < 1 ) {
			--year;
			auto prevYear = year - 1;
			int weeks { 52 + ((year / 4 - year / 100 - year / 400 == 4) || ((prevYear / 4 - prevYear / 100 - prevYear / 400) == 3) ? 1 : 0) };
			TwoDigitToBuff(weeks);
			return;
	}
	auto prevYear = year - 1;
	int weeks { 52 + ((year / 4 - year / 100 - year / 400 == 4) || ((prevYear / 4 - prevYear / 100 - prevYear / 400) == 3) ? 1 : 0) };
	if( w > weeks ) {
			buffer[ valueSize++ ] = 1;
			return;
	}
	TwoDigitToBuff(w);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleCTime(T&& container) {
	const auto& tm { argStorage.c_time_state(specValues.argPosition) };
	switch( specValues.timeSpecContainer[ 0 ] ) {
			case 'a': WriteShortWeekday(std::forward<FwdRef<T>>(container), tm.tm_wday); return;
			case 'h': [[fallthrough]];
			case 'b': WriteShortMonth(std::forward<FwdRef<T>>(container), tm.tm_mon); return;
			case 'c': WriteTimeDate(std::forward<FwdRef<T>>(container), tm); return;
			case 'd': WritePaddedDay(std::forward<FwdRef<T>>(container), tm.tm_mday); return;
			case 'e': WriteSpacePaddedDay(std::forward<FwdRef<T>>(container), tm.tm_mday); return;
			case 'g': WriteShortIsoWeekYear(std::forward<FwdRef<T>>(container), tm.tm_year, tm.tm_yday, tm.tm_wday); return;
			case 'j': WriteDayOfYear(std::forward<FwdRef<T>>(container), tm.tm_yday); return;
			case 'm': WritePaddedMonth(std::forward<FwdRef<T>>(container), tm.tm_mon); return;
			case 'p': WriteAMPM(std::forward<FwdRef<T>>(container), tm.tm_hour); return;
			case 'r': Write12HourTime(std::forward<FwdRef<T>>(container), tm.tm_hour, tm.tm_min, tm.tm_sec); return;
			case 'u': WriteIsoWeekDec(std::forward<FwdRef<T>>(container), tm.tm_wday); return;
			case 'w': WriteWeekdayDec(std::forward<FwdRef<T>>(container), tm.tm_wday); return;
			case 'D': [[fallthrough]];
			case 'x': WriteMMDDYY(std::forward<FwdRef<T>>(container), tm.tm_mon, tm.tm_mday, tm.tm_year); return;
			case 'y': WriteShortYear(std::forward<FwdRef<T>>(container), tm.tm_year); return;
			case 'z': WriteUtcOffset(std::forward<FwdRef<T>>(container)); return;
			case 'A': WriteLongWeekday(std::forward<FwdRef<T>>(container), tm.tm_wday); return;
			case 'B': WriteLongMonth(std::forward<FwdRef<T>>(container), tm.tm_mon); return;
			case 'C': WriteTruncatedYear(std::forward<FwdRef<T>>(container), tm.tm_year); return;
			case 'F': WriteYYYYMMDD(std::forward<FwdRef<T>>(container), tm.tm_year, tm.tm_mon, tm.tm_mday); return;
			case 'G': WriteLongIsoWeekYear(std::forward<FwdRef<T>>(container), tm.tm_year, tm.tm_yday, tm.tm_wday); return;
			case 'H': Write24Hour(std::forward<FwdRef<T>>(container), tm.tm_hour); return;
			case 'I': Write12Hour(std::forward<FwdRef<T>>(container), tm.tm_hour); return;
			case 'M': WriteMinute(std::forward<FwdRef<T>>(container), tm.tm_min); return;
			case 'R': Write24HM(std::forward<FwdRef<T>>(container), tm.tm_hour, tm.tm_min); return;
			case 'S': WriteSecond(std::forward<FwdRef<T>>(container), tm.tm_sec); return;
			case 'T': Write24HourTime(std::forward<FwdRef<T>>(container), tm.tm_hour, tm.tm_min, tm.tm_sec); return;
			case 'U': WriteWeek(std::forward<FwdRef<T>>(container), tm.tm_yday, tm.tm_wday); return;
			case 'V': WriteIsoWeekNumber(std::forward<FwdRef<T>>(container), tm.tm_year, tm.tm_yday, tm.tm_wday); return;
			case 'W': WriteIsoWeek(std::forward<FwdRef<T>>(container), tm.tm_yday, tm.tm_wday); return;
			case 'X': WriteTime(std::forward<FwdRef<T>>(container), tm.tm_hour, tm.tm_min, tm.tm_sec); return;
			case 'Y': WriteLongYear(std::forward<FwdRef<T>>(container), tm.tm_year); return;
			case 'Z': WriteTZName(std::forward<FwdRef<T>>(container)); return;
			case 'n': [[fallthrough]];
			case 't': [[fallthrough]];
			case '%': [[fallthrough]];
			default: WriteLiteral(std::forward<FwdRef<T>>(container), specValues.timeSpecContainer[ 0 ]); return;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatCTime(const std::tm& time, const int& precision, int startPos, int endPos) {
	for( ;; ) {
			switch( specValues.timeSpecContainer[ startPos ] ) {
					case 'a': FormatShortWeekday(time.tm_wday); break;
					case 'h': [[fallthrough]];
					case 'b': FormatShortMonth(time.tm_mon); break;
					case 'c': FormatTimeDate(time); break;
					case 'd': TwoDigitToBuff(time.tm_mday); break;
					case 'e': FormatSpacePaddedDay(time.tm_mday); break;
					case 'g': FormatShortIsoWeekYear(time.tm_year, time.tm_yday, time.tm_wday); break;
					case 'j': FormatDayOfYear(time.tm_yday); break;
					case 'm': TwoDigitToBuff(time.tm_mon + 1); break;
					case 'p': FormatAMPM(time.tm_hour); break;
					case 'r': Format12HourTime(time.tm_hour, time.tm_min, time.tm_sec, precision); break;
					case 'w': FormatWeekdayDec(time.tm_wday); break;
					case 'u': FormatIsoWeekDec(time.tm_wday); break;
					case 'D': [[fallthrough]];
					case 'x': FormatMMDDYY(time.tm_mon, time.tm_mday, time.tm_year); break;
					case 'y': FormatShortYear(time.tm_year); break;
					case 'z': FormatUtcOffset(); break;
					case 'A': FormatLongWeekday(time.tm_wday); break;
					case 'B': FormatLongMonth(time.tm_mon); break;
					case 'C': FormatTruncatedYear(time.tm_year); break;
					case 'F': FormatYYYYMMDD(time.tm_year, time.tm_mon, time.tm_mday); break;
					case 'G': FormatLongIsoWeekYear(time.tm_year, time.tm_yday, time.tm_wday); break;
					case 'H': TwoDigitToBuff(time.tm_hour); break;
					case 'I': TwoDigitToBuff(time.tm_hour > 12 ? time.tm_hour - 12 : time.tm_hour); break;
					case 'M': TwoDigitToBuff(time.tm_min); break;
					case 'R': Format24HM(time.tm_hour, time.tm_min); break;
					case 'S': TwoDigitToBuff(time.tm_sec); break;
					case 'T': Format24HourTime(time.tm_hour, time.tm_min, time.tm_sec, precision); break;
					case 'U': TwoDigitToBuff((10 + time.tm_yday - time.tm_wday) / 7); break;
					case 'V': FormatIsoWeekNumber(time.tm_year, time.tm_yday, time.tm_wday); break;
					case 'W': TwoDigitToBuff((time.tm_yday + 7 - (time.tm_wday == 0 ? 6 : time.tm_wday - 1)) / 7); break;
					case 'X': Format24HourTime(time.tm_hour, time.tm_min, time.tm_sec, precision); break;
					case 'Y': FormatLongYear(time.tm_year); break;
					case 'Z': FormatTZName(); break;
					case 'n': [[fallthrough]];
					case 't': [[fallthrough]];
					case '%': [[fallthrough]];
					default: FormatLiteral(specValues.timeSpecContainer[ startPos ]); break;
				}
			if( ++startPos >= endPos ) return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleValue(T&& container, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType: WriteSimpleString(std::forward<FwdRef<T>>(container)); return;
			case CharPointerType: WriteSimpleCString(std::forward<FwdRef<T>>(container)); return;
			case StringViewType: WriteSimpleStringView(std::forward<FwdRef<T>>(container)); return;
			case IntType: WriteSimpleInt(std::forward<FwdRef<T>>(container)); return;
			case U_IntType: WriteSimpleUInt(std::forward<FwdRef<T>>(container)); return;
			case LongLongType: WriteSimpleLongLong(std::forward<FwdRef<T>>(container)); return;
			case U_LongLongType: WriteSimpleULongLong(std::forward<FwdRef<T>>(container)); return;
			case BoolType: WriteSimpleBool(std::forward<FwdRef<T>>(container)); return;
			case CharType: container.insert(container.end(), argStorage.char_state(specValues.argPosition)); return;
			case FloatType: WriteSimpleFloat(std::forward<FwdRef<T>>(container)); return;
			case DoubleType: WriteSimpleDouble(std::forward<FwdRef<T>>(container)); return;
			case LongDoubleType: WriteSimpleLongDouble(std::forward<FwdRef<T>>(container)); return;
			case ConstVoidPtrType: WriteSimpleConstVoidPtr(std::forward<FwdRef<T>>(container)); return;
			case VoidPtrType: WriteSimpleVoidPtr(std::forward<FwdRef<T>>(container)); return;
			default: return;
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
	constexpr std::string_view sv { "0x" };
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

constexpr void serenity::arg_formatter::ArgFormatter::FormatArgument(const int& precision, const int& totalWidth, const msg_details::SpecType& type) {
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
	int pos { 0 };
	bool isUpper { false };
	auto data { buffer.data() };
	std::chars_format format {};
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE)) : std::fill(buffer.begin(), buffer.end(), 0);
	if( specValues.signType != Sign::Empty ) WriteSign(std::forward<T>(value), pos);
	SetFloatingFormat(format, precision, isUpper);
	auto end { precision != 0 ? std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format, precision).ptr
		                      : std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format).ptr };
	valueSize = end - data;
	if( isUpper ) BufferToUpper(data, end);
}

template<typename T>
requires std::is_integral_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::FormatIntegerType(T&& value) {
	int pos { 0 }, base { 10 };
	bool isUpper { false };
	auto data { buffer.data() };
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE)) : std::fill(buffer.begin(), buffer.end(), 0);
	if( specValues.signType != Sign::Empty ) WriteSign(std::forward<T>(value), pos);
	if( specValues.preAltForm.size() != 0 ) {
			std::memcpy(data + pos, specValues.preAltForm.data(), specValues.preAltForm.size());
			pos += static_cast<int>(specValues.preAltForm.size());    // safe to downcast as it will only ever be positive and max val of 2
	}
	SetIntegralFormat(base, isUpper);
	auto end { std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, base).ptr };
	valueSize = end - data;
	if( isUpper ) BufferToUpper(data, end);
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
