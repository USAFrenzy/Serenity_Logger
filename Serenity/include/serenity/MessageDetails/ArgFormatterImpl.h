#pragma once
// Copyright from <format> header extending to libfmt

// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
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

/************************************** What The ArgFormatter And ArgContainer Classes Offer **************************************/
// This work is a very simple reimplementation with limititations on my end of Victor Zverovich's fmt library.
// Currently the standard's implementation of his work is still underway although for the most part, it's feature
// complete with Victor's library - there are some huge performance drops when it's not compiled under the /utf-8
// flag on MSVC though.
//
// The ArgFormatter and ArgContainer classes work in tandem to deliver a very bare-bones version of what the fmt,
// and MSVC's implementation of fmt, libraries provides and is only intended for usage until MSVC's code is as performant
// as it is when compiled with /utf-8 for compilation without the need for this flag.
//
// With that being said, these classes provide the functionality of formatting to a container with a back insert iterator
// object which mirrors the std::format_to()/std::vformat_to() via the se_format_to() function, as well as a way to recieve
// a string with the formatted result via the se_format() function, mirroring std::format()/std::vformat().
// Unlike MSVC's implementations, however, the default locale used when the locale specifier is present will refer to the
// default locale created on construction of this class. The downside is that this will not reflect any changes when the
// locale is changed globally; but the benefit of this approach is reducing the construction of an empty locale on every
// format, as well as the ability to change the locale with "SetLocale(const std::locale &)" function without affecting
// the locale of the rest of the program. All formatting specifiers and manual/automatic indexing from the fmt library
// are available and supported.
//
// EDIT: It now seems that MSVC build  192930145 fixes the performance issues among other things with the <format> lib; however,
//             the performance times of serenity is STILL faster than the MSVC's implementation (for most cases) - the consistency of their
//             performance is now a non-issue though (same performance with or without the UTF-8 flag)
/**********************************************************************************************************************************/

#include <serenity/MessageDetails/ArgFormatter.h>
#include <string_view>

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename... Args> std::string serenity::arg_formatter::ArgFormatter::se_format(std::string_view sv, Args&&... args) {
	std::string tmp;
	tmp.reserve(ReserveCapacity(std::forward<Args>(args)...));
	se_format_to(std::back_inserter(tmp), sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter = 0;
	for( ;; ) {
			if( sv.size() < 2 ) break;
			size_t pos { 0 };
			specValues.ResetSpecs();
			if( !FindBrackets(sv) ) {
					std::copy(sv.data(), sv.data() + sv.size(), Iter);
					return;
			}
			if( bracketResults.beginPos != 0 ) {
					std::copy(sv.data(), sv.data() + bracketResults.beginPos, Iter);
					sv.remove_prefix(bracketResults.beginPos);
					bracketResults.endPos -= bracketResults.beginPos;
					bracketResults.beginPos = 0;
			}
			std::string_view argBracket(sv.data() + (++bracketResults.beginPos), sv.data() + (++bracketResults.endPos));
			/*Handle Escaped Bracket*/
			if( argBracket[ pos ] == '{' ) {
					Iter = '{';
					++pos;
			}
			/*Handle Positional Args*/
			if( !ParsePositionalField(argBracket, pos, specValues.argPosition) ) {
					// Nothing Else to Parse - just a simple substitution after position field
					auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
					Format(std::forward<std::back_insert_iterator<T>>(Iter), std::move(argType));
					sv.remove_prefix(argBracket.size() + 1);
					continue;
			}
			/* Handle What's Left Of The Bracket */
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( pos < argBracket.size() ) {
					VerifyArgumentBracket(argBracket, pos, argType);
			}
			Format(std::forward<std::back_insert_iterator<T>>(Iter), std::move(argType));
			if( specValues.hasClosingBrace ) {
					Iter = '}';
			}
			sv.remove_prefix(argBracket.size() + 1);
		}
	if( sv.size() != 0 ) {
			std::copy(sv.data(), sv.data() + sv.size(), Iter);
	}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::WriteSimpleValue(std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	auto& container { *IteratorContainer(Iter).Container() };
	switch( argType ) {
			case StringType:
				{
					std::string_view sv { std::move(argStorage.string_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(sv.data(), sv.size());
					} else {
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case CharPointerType:
				{
					std::string_view sv { std::move(argStorage.c_string_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(sv.data(), sv.size());
					} else {
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case StringViewType:
				{
					std::string_view sv { std::move(argStorage.string_view_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(sv.data(), sv.size());
					} else {
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case IntType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case U_IntType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case LongLongType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case U_LongLongType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case BoolType:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
					} else {
							std::string_view sv { argStorage.bool_state(specValues.argPosition) ? "true" : "false" };
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case CharType:
				{
					Iter = argStorage.char_state(specValues.argPosition);
					return;
				}
			case FloatType:
				{
					auto data { buffer.data() };
					std::memset(data, 0, buffer.size());
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case DoubleType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case LongDoubleType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case ConstVoidPtrType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append("0x").append(
							data,
							std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr -
							data);
					} else {
							Iter = '0';
							Iter = 'x';
							std::move(
							data,
							std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr,
							Iter);
						}
					return;
				}
			case VoidPtrType:
				{
					auto data { buffer.data() };
					auto size { buffer.size() };
					std::memset(data, 0, size);
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append("0x").append(
							data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr -
								  data);
					} else {
							Iter = '0';
							Iter = 'x';
							std::move(
							data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr,
							Iter);
						}
					return;
				}
			default: return; break;
		}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::Format(std::back_insert_iterator<T>&& Iter, msg_details::SpecType&& argType) {
	using enum msg_details::SpecType;
	int precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
		            : specValues.precision != 0      ? specValues.precision
		                                             : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 ) {    // use this check to guard from any unnecessary additional checks
			if( IsSimpleSubstitution(argType, precision) ) {
					return WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argType);
			}
	}
	rawValueTemp.clear();
	if( !specValues.localize ) {
			FormatRawValueToStr(precision, argType);
	} else {
			LocalizeArgument(precision, argType);
		}
	const auto& size(rawValueTemp.size());
	size_t fillAmount { (totalWidth > size) ? totalWidth - size : 0 };
	auto& container { *IteratorContainer(Iter).Container() };
	if( fillAmount == 0 ) {
			if constexpr( std::is_same_v<T, std::string> ) {
					container.append(std::move(rawValueTemp).data(), size);
			} else {
					Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
				}
			return;
	}
	// Reuse the buffer being used for formatting arithmetic to chars (leads to a massive performance gain vs a copying push_back/append call for a char).
	// CPU cycles went from spending ~%18 per align loop with the fill char down to ~%2 with this change (std::fill() takes ~%2.3 so in the case of
	// Align Center; this takes the cycles spent here down from ~%36 down to ~%6-%7).
	auto data { buffer.data() };
	std::memset(data, specValues.fillCharacter, buffer.size());
	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(std::move(rawValueTemp).data(), size).append(data, fillAmount);
					} else {
							Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
							Iter = std::move(data, data + fillAmount, Iter);
						}
					return;
				}
			case Alignment::AlignRight:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, fillAmount).append(std::move(rawValueTemp).data(), size);
					} else {
							Iter = std::move(data, data + fillAmount, Iter);
							Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
						}
					return;
				}
			case Alignment::AlignCenter:
				{
					fillAmount /= 2;
					if constexpr( std::is_same_v<T, std::string> ) {
							container.append(data, fillAmount).append(std::move(rawValueTemp).data(), size).append(data, (totalWidth - size - fillAmount));
					} else {
							Iter = std::move(data, data + fillAmount, Iter);
							Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
							Iter = std::move(data, data + (totalWidth - size - fillAmount), Iter);
						}
					return;
				}
			default: return; break;
		}
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatFloatTypeArg(T&& value, int& precision) {
	int pos { 0 };
	auto data { buffer.data() };
	std::memset(data, 0, buffer.size());
	switch( specValues.signType == Sign::Space ? value < 0 ? Sign::Minus : Sign::Space : specValues.signType ) {
			case Sign::Space: data[ pos++ ] = ' '; break;
			case Sign::Plus: data[ pos++ ] = '+'; break;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: break;
		}
	bool isUpper { false };
	std::chars_format format {};
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
	// clang-format off
	rawValueTemp.append(data, (precision != 0 ? std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format, precision)
	                                          : std::to_chars(data + pos, data + buffer.size(), std::forward<T>(value), format)).ptr -data);
	// clang-format on
	if( !isUpper ) return;
	for( auto& ch: rawValueTemp ) {
			if( ch >= 'a' && ch <= 'z' ) {
					ch -= 32;
			}
		}
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatIntTypeArg(T&& value) {
	int pos {};
	int base {};
	auto data { buffer.data() };
	std::memset(data, 0, buffer.size());
	switch( specValues.signType == Sign::Space ? value < 0 ? Sign::Minus : Sign::Space : specValues.signType ) {
			case Sign::Space: data[ pos++ ] = ' '; break;
			case Sign::Plus: data[ pos++ ] = '+'; break;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: break;
		}
	// size is only ever 0, 1, or 2
	switch( specValues.preAltForm.size() ) {
			case 1: data[ pos++ ] = specValues.preAltForm[ 0 ]; break;
			case 2:
				data[ pos++ ] = specValues.preAltForm[ 0 ];
				data[ pos++ ] = specValues.preAltForm[ 1 ];
				break;
			default: break;
		}
	bool isUpper { false };
	switch( specValues.typeSpec ) {
			case '\0': base = 10; break;
			case 'b': base = 2; break;
			case 'B':
				base    = 2;
				isUpper = true;
				break;
			case 'c': rawValueTemp += static_cast<char>(value); return;
			case 'o': base = 8; break;
			case 'x': base = 16; break;
			case 'X':
				base    = 16;
				isUpper = true;
				break;
			default: base = 10; break;
		}
	rawValueTemp.append(data, std::to_chars(data + pos, data + buffer.size(), value, base).ptr - data);
	if( !isUpper ) return;
	for( auto& ch: rawValueTemp ) {
			if( ch >= 'a' && ch <= 'z' ) {
					ch -= 32;
			}
		}
}