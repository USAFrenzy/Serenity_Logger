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
// EDIT: It now seems that MSVC build  192930145 fixes the performance issues among other things with the <format> lib;
// however, the performance times of serenity is STILL faster than the MSVC's implementation - the consistency of their
// performance is now a non-issue though (same performance with or without the UTF-8 flag)
/**********************************************************************************************************************************/

#include "ArgFormatter.h"
#include <string_view>

template<typename T, typename... Args>
void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename... Args> std::string serenity::arg_formatter::ArgFormatter::se_format(std::string_view sv, Args&&... args) {
	std::string tmp;
	se_format_to(std::back_inserter(tmp), sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename T>
inline bool serenity::arg_formatter::ArgFormatter::HandleIfEndOrWhiteSpace(std::back_insert_iterator<T>&& Iter, std::string_view sv, size_t& currentPosition,
                                                                           const size_t& bracketSize) {
	char ch;
	if( bracketSize > 1 ) {
			ch = sv[ currentPosition + static_cast<size_t>(1) ];
	} else {
			ch = sv[ currentPosition ];
		}
	if( ch == '}' ) {
			ParsePositionalField(sv, argCounter, currentPosition);
			FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
			if( specValues.hasClosingBrace ) {
					'}';
			}
			return true;
	} else if( ch == ' ' ) {
			// handle appropriately if it's only one space as a Sign spec,
			// otherwise, remove any extra spaces past the first one
			if( currentPosition >= 0 ) {
					if( sv[ currentPosition ] == ':' ) {
							specValues.signType = Sign::Space;
							currentPosition += 2;    // move past ':' & ' '
							return false;
					}
			}
			for( ;; ) {
					if( currentPosition > bracketSize ) break;
					if( ch = sv[ ++currentPosition ] != ' ' ) break;
				}
			if( ch == '}' ) {
					FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
					++argCounter;
					return true;
			} else {
					++currentPosition;
				}
	}
	return false;
}

template<typename T> void serenity::arg_formatter::ArgFormatter::Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter = 0;
	for( ;; ) {
			if( sv.size() < 2 ) break;
			size_t pos { 0 };
			result.remainder = sv;
			specValues.ResetSpecs();
			FindBrackets(result.remainder);
			if( !bracketResults.isValid ) {
					std::copy(result.remainder.begin(), result.remainder.end(), Iter);
					break;
			}
			std::string_view preToken { sv.substr(0, bracketResults.beginPos) };
			if( preToken.size() != 0 ) {
					std::copy(preToken.begin(), preToken.end(), Iter);
			}

			auto argBracket { sv.substr(bracketResults.beginPos + 1, (bracketResults.endPos - bracketResults.beginPos)) };
			const size_t& bracketSize { argBracket.size() };
			/*Handle Escaped Bracket*/
			if( argBracket[ pos ] == '{' ) {
					Iter = '{';
					++pos;
			}

			/* Handle If Bracket Contained No Specs */
			if( HandleIfEndOrWhiteSpace(std::forward<std::back_insert_iterator<T>>(Iter), argBracket, pos, bracketSize) ) {
					sv.remove_prefix(argBracket.size() + preToken.size() + 1);
					continue;
			}

			/*Handle Positional Args*/
			if( !ParsePositionalField(argBracket, argCounter, pos) ) {
					// Nothing to Parse - just a simple substitution
					FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
					if( specValues.hasClosingBrace ) {
							Iter = '}';
					}
					sv.remove_prefix(argBracket.size() + preToken.size() + 1);
					++argCounter;
					continue;
			}
			/* Handle What's Left Of The Bracket */
			VerifyArgumentBracket(argBracket, pos, bracketSize);
			FormatTokens(std::forward<std::back_insert_iterator<T>>(Iter));
			if( specValues.hasClosingBrace ) {
					Iter = '}';
			}
			sv.remove_prefix(argBracket.size() + preToken.size() + 1);
		}
	if( sv.size() != 0 ) {
			std::copy(sv.begin(), sv.end(), Iter);
	}
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::AppendDirectly(std::back_insert_iterator<T>&& Iter, msg_details::SpecType type, bool isSimpleSub) {
	using SpecType = msg_details::SpecType;
	std::string_view sv;
	switch( type ) {
			case SpecType::StringType: sv = std::move(argStorage.string_state(specValues.argPosition)); break;
			case SpecType::CharPointerType: sv = std::move(argStorage.c_string_state(specValues.argPosition)); break;
			case SpecType::StringViewType: sv = std::move(argStorage.string_view_state(specValues.argPosition)); break;
			default: break;
		}

	std::copy(sv.begin(), sv.end(), Iter);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::FormatTokens(std::back_insert_iterator<T>&& Iter) {
	rawValueTemp.clear();
	size_t fillAmount {}, totalWidth {}, i { 0 };
	// clang-format off
	auto precision{ specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
																									: specValues.precision != 0 ? specValues.precision : 0
	};
	// clang-format on
	if( specValues.nestedWidthArgPos != 0 ) {
			totalWidth = argStorage.int_state(specValues.nestedWidthArgPos);
	} else {
			totalWidth = specValues.alignmentPadding != 0 ? specValues.alignmentPadding : 0;
		}

	using SpecType = msg_details::SpecType;
	auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
	bool simpleSub { totalWidth == 0 };

	if( totalWidth == 0 && precision == 0 ) {
			switch( argType ) {
					case SpecType::StringType: [[fallthrough]];
					case SpecType::CharPointerType: [[fallthrough]];
					case SpecType::StringViewType:
						AppendDirectly(std::forward<std::back_insert_iterator<T>>(Iter), argType, simpleSub);
						return;
						break;
					default: break;
				}
	}
	if( specValues.localize ) {
			LocalizeArgument(std::forward<std::back_insert_iterator<T>>(Iter), precision, argType, simpleSub);
	} else {
			FormatRawValueToStr(std::forward<std::back_insert_iterator<T>>(Iter), precision, argType, simpleSub);
		}
	if( simpleSub ) return;

	auto size(rawValueTemp.size());
	fillAmount = (totalWidth > size) ? totalWidth - size : 0;
	if( fillAmount == 0 ) {
			std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
			return;
	}

	std::string_view fillChar { &specValues.fillCharacter, 1 };

	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillChar.begin(), fillChar.end(), Iter);
							++i;
						}
				}
				break;
			case Alignment::AlignRight:
				{
					for( ;; ) {
							if( i >= fillAmount ) break;
							std::copy(fillChar.begin(), fillChar.end(), Iter);
							++i;
						}
					std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
				}
				break;
			case Alignment::AlignCenter:
				fillAmount /= 2;
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillChar.begin(), fillChar.end(), Iter);
						++i;
					}
				*Iter      = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
				fillAmount = (totalWidth - size - fillAmount);
				i          = 0;
				for( ;; ) {
						if( i >= fillAmount ) break;
						std::copy(fillChar.begin(), fillChar.end(), Iter);
						++i;
					}
				break;
			default: break;
		}
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

template<typename T, typename U>
void serenity::arg_formatter::ArgFormatter::FormatFloatTypeArg(std::back_insert_iterator<T>&& Iter, U&& value, int precision, bool isSimpleSub) {
	std::chars_format format {};
	auto data { buffer.data() };
	std::fill(data, data + buffer.size(), 0);
	int pos { 0 };

	if( specValues.signType == Sign::Space ) {
			specValues.signType = value < 0 ? Sign::Minus : Sign::Space;
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

	// default behaviors
	if( specValues.hasAlt && precision == 0 ) {
			format = std::chars_format::scientific;
	} else if( !specValues.localize && specValues.alignmentPadding == 0 && precision == 0 && specValues.signType == Sign::Empty ) {
			format = std::chars_format::fixed;
	} else {
			format = std::chars_format::general;
		}
	// default behavior skipped, so process the type spec that was found
	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'a': [[fallthrough]];
			case 'A':
				precision = precision > 0 ? precision : 0;
				format    = std::chars_format::hex;
				break;
			case 'e': [[fallthrough]];
			case 'E':
				format    = std::chars_format::scientific;
				precision = precision > 0 ? precision : 6;
				break;
			case 'f': [[fallthrough]];
			case 'F':
				format    = std::chars_format::fixed;
				precision = precision > 0 ? precision : 6;
				break;
			case 'g': [[fallthrough]];
			case 'G':
				format    = std::chars_format::general;
				precision = precision > 0 ? precision : 6;
				break;
			default: break;
		}

	if( precision != 0 ) {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<U>(value), format, precision);
	} else {
			charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<U>(value), format);
		}
	switch( specValues.typeSpec ) {
			case '\0': break;
			case 'A': [[fallthrough]];
			case 'E': [[fallthrough]];
			case 'G':
				for( auto& ch: buffer ) {
						if( ch == *charsResult.ptr ) break;
						if( ch >= 'a' && ch <= 'z' ) {
								ch -= 32;
						}
					}
				break;
			default: break;
		}    // capitilization

	if( isSimpleSub && !specValues.localize ) {
			for( data; data != charsResult.ptr; ++data ) {
					*Iter = *data;
				}
			return;
	}
	rawValueTemp.append(data, charsResult.ptr);
}

template<typename T, typename U>
void serenity::arg_formatter::ArgFormatter::FormatIntTypeArg(std::back_insert_iterator<T>&& Iter, U&& value, bool isSimpleSub) {
	int base { 10 };
	auto data { buffer.data() };
	std::fill(data, data + buffer.size(), 0);
	int pos { 0 };

	if( specValues.signType == Sign::Space ) {
			specValues.signType = value < 0 ? Sign::Minus : Sign::Space;
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
			case '\0': break;
			case 'b': [[fallthrough]];
			case 'B': base = 2; break;
			case 'c':
				if( isSimpleSub ) {
						Iter = static_cast<char>(value);
				} else {
						rawValueTemp += static_cast<char>(value);
					}
				return;
			case 'o': base = 8; break;
			case 'x': [[fallthrough]];
			case 'X': base = 16; break;
			default: break;
		}

	charsResult = std::to_chars(data + pos, data + buffer.size(), std::forward<U>(value), base);

	switch( specValues.typeSpec ) {
			case '\0': break;
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

	if( isSimpleSub && !specValues.localize ) {
			for( data; data != charsResult.ptr; ++data ) {
					*Iter = *data;
				}
			return;
	}
	rawValueTemp.append(data, charsResult.ptr);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::AppendByPrecision(std::back_insert_iterator<T>&& Iter, std::string_view val, int precision, bool isSimpleSub) {
	int size { static_cast<int>(val.size()) };
	precision = precision > 0 ? precision > size ? size : precision : size;
	if( isSimpleSub ) {
			size_t pos { 0 };
			for( ;; ) {
					if( pos == precision ) break;
					*Iter = val[ pos ];
					++pos;
				}
			return;
	}
	rawValueTemp.reserve(size);
	rawValueTemp.append(val.data(), precision);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::FormatRawValueToStr(std::back_insert_iterator<T>&& Iter, int& precision, msg_details::SpecType type, bool isSimpleSub) {
	using SpecType = msg_details::SpecType;
	switch( type ) {
			case SpecType::StringType:
				AppendByPrecision(std::forward<std::back_insert_iterator<T>>(Iter), std::move(argStorage.string_state(specValues.argPosition)), precision,
				                  isSimpleSub);
				break;
			case SpecType::CharPointerType:
				AppendByPrecision(std::forward<std::back_insert_iterator<T>>(Iter), std::move(argStorage.c_string_state(specValues.argPosition)), precision,
				                  isSimpleSub);
				break;
			case SpecType::StringViewType:
				AppendByPrecision(std::forward<std::back_insert_iterator<T>>(Iter), std::move(argStorage.string_view_state(specValues.argPosition)), precision,
				                  isSimpleSub);
				break;
			case SpecType::IntType:
				FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.int_state(specValues.argPosition), isSimpleSub);
				break;
			case SpecType::U_IntType:
				FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.uint_state(specValues.argPosition), isSimpleSub);
				break;
			case SpecType::LongLongType:
				FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.long_long_state(specValues.argPosition), isSimpleSub);
				break;
			case SpecType::U_LongLongType:
				FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.u_long_long_state(specValues.argPosition), isSimpleSub);
				break;
			case SpecType::BoolType:
				if( isSimpleSub && specValues.typeSpec == '\0' ) {
						std::string_view sv { (argStorage.bool_state(specValues.argPosition) ? "true" : "false") };
						for( auto& ch: sv ) {
								*Iter = ch;
							}
						return;
				}
				if( specValues.typeSpec != '\0' && specValues.typeSpec != 's' ) {
						FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter),
						                 static_cast<unsigned char>(argStorage.bool_state(specValues.argPosition)), isSimpleSub);
				} else {
						rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
					}
				break;
			case SpecType::CharType:
				if( isSimpleSub && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c') ) {
						*Iter = argStorage.char_state(specValues.argPosition);
						return;
				}
				if( specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ) {
						FormatIntTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), static_cast<int>(argStorage.char_state(specValues.argPosition)),
						                 isSimpleSub);
				} else {
						rawValueTemp += argStorage.char_state(specValues.argPosition);
					}
				break;
			case SpecType::FloatType:
				FormatFloatTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.float_state(specValues.argPosition), precision, isSimpleSub);
				break;
			case SpecType::DoubleType:
				FormatFloatTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.double_state(specValues.argPosition), precision, isSimpleSub);
				break;
			case SpecType::LongDoubleType:
				FormatFloatTypeArg(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.long_double_state(specValues.argPosition), precision, isSimpleSub);
				break;
			case SpecType::ConstVoidPtrType:
				{
					auto data { buffer.data() };
					charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16);
					if( isSimpleSub ) {
							Iter = '0';
							Iter = 'x';
							std::copy(data, charsResult.ptr, Iter);
							return;
					}
					rawValueTemp.reserve(charsResult.ptr - buffer.data() + 2);
					rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
				}
				break;
			case SpecType::VoidPtrType:
				{
					auto data { buffer.data() };
					charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16);
					if( isSimpleSub ) {
							Iter = '0';
							Iter = 'x';
							std::copy(data, charsResult.ptr, Iter);
							return;
					}
					rawValueTemp.reserve(charsResult.ptr - buffer.data() + 2);
					rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
				}
				break;
			default: break;
		}
}
template<typename T>
void serenity::arg_formatter::ArgFormatter::FormatIntegralGrouping(std::back_insert_iterator<T>&& Iter, std::string& section, char separator, bool isSimpleSub) {
	size_t groups { 0 };
	auto groupBegin { groupings.begin() };
	int groupGap { *groupBegin };
	auto end { section.size() };
	if( section.size() < groupGap ) {
			if( isSimpleSub ) {
					std::move(section.begin(), section.end(), Iter);
			}
			return;
	}
	localeTemp.clear();
	localeTemp.reserve(section.size() + groupGap);
	std::string_view sv { section };
	if( specValues.hasAlt ) {
			sv.remove_prefix(2);
	}

	// TODO: Rework this to avoid using insert (Used insert to just get this working, but it's very slow)
	if( groupings.size() > 1 ) {
			/********************* grouping is unique *********************/
			if( groupings.size() == 3 ) {
					localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
					sv.remove_suffix(groupGap);
					localeTemp.insert(0, 1, separator);

					groupGap = *(++groupBegin);
					localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
					sv.remove_suffix(groupGap);
					localeTemp.insert(0, 1, separator);

					groupGap = *(++groupBegin);
					groups   = end / groupGap - end % groupGap;
					for( ; groups; --groups ) {
							if( groups > 0 ) {
									localeTemp.insert(0, 1, separator);
							}
							if( sv.size() > groupGap ) {
									localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
							} else {
									localeTemp.insert(0, sv);
									break;
								}
							if( sv.size() >= groupGap ) {
									sv.remove_suffix(groupGap);
							}
						}
			} else {
					// grouping is one group and then uniform
					localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
					sv.remove_suffix(groupGap);

					groupGap = *(++groupBegin);
					groups   = end / groupGap - end % groupGap;

					for( ; groups; --groups ) {
							if( groups > 0 ) {
									localeTemp.insert(0, 1, separator);
							}
							if( sv.size() > groupGap ) {
									localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
							} else {
									localeTemp.insert(0, sv);
									break;
								}
							if( sv.size() >= groupGap ) {
									sv.remove_suffix(groupGap);
							}
						}
				}
	} else {
			/********************* grouping is uniform *********************/
			groups = end / groupGap + end % groupGap;
			for( ; groups; --groups ) {
					if( sv.size() > groupGap ) {
							localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
					} else {
							localeTemp.insert(0, sv);
							break;
						}
					if( groups > 0 ) {
							localeTemp.insert(0, 1, separator);
					}
					if( sv.size() >= groupGap ) {
							sv.remove_suffix(groupGap);
					}
				}
		}
	if( specValues.hasAlt ) {
			localeTemp.insert(0, section.substr(0, 2));
	}
	if( isSimpleSub ) {
			std::move(localeTemp.begin(), localeTemp.end(), Iter);
			return;
	}
	section.clear();
	section.reserve(localeTemp.size());
	section.append(std::move(localeTemp));
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeIntegral(std::back_insert_iterator<T>&& Iter, int precision, msg_details::SpecType type, bool isSimpleSub) {
	FormatRawValueToStr(std::forward<std::back_insert_iterator<T>>(Iter), precision, type, isSimpleSub);
	FormatIntegralGrouping(std::forward<std::back_insert_iterator<T>>(Iter), rawValueTemp, separator, isSimpleSub);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeFloatingPoint(std::back_insert_iterator<T>&& Iter, int precision, msg_details::SpecType type, bool isSimpleSub) {
	FormatRawValueToStr(std::forward<std::back_insert_iterator<T>>(Iter), precision, type, isSimpleSub);
	size_t pos { 0 };
	auto size { rawValueTemp.size() };
	auto data { rawValueTemp.begin() };
	std::string_view sv { rawValueTemp };
	localeTemp.clear();
	for( ;; ) {
			if( pos >= size ) break;
			if( sv[ pos ] == '.' ) {
					localeTemp.append(sv.substr(0, pos));
					FormatIntegralGrouping(std::forward<std::back_insert_iterator<T>>(Iter), localeTemp, separator, isSimpleSub);
					if( isSimpleSub ) {
							*Iter = decimal;
							auto subSV { sv.substr(pos + 1, sv.size()) };
							std::copy(subSV.begin(), subSV.end(), Iter);
							return;
					}
					localeTemp += decimal;
					localeTemp.append(sv.substr(pos + 1, sv.size()));
					break;
			}
			++pos;
		}
	if( localeTemp.size() != 0 ) {
			rawValueTemp.clear();
			rawValueTemp.append(std::move(localeTemp));
			return;
	}
	FormatIntegralGrouping(std::forward<std::back_insert_iterator<T>>(Iter), rawValueTemp, separator, isSimpleSub);
}

template<typename T> void serenity::arg_formatter::ArgFormatter::LocalizeBool(std::back_insert_iterator<T>&& Iter, bool isSimpleSub) {
	if( isSimpleSub ) {
			std::string_view sv { argStorage.bool_state(specValues.argPosition) ? trueStr : falseStr };
			std::copy(sv.begin(), sv.end(), Iter);
			return;
	}
	rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? trueStr : falseStr);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeArgument(std::back_insert_iterator<T>&& Iter, int precision, msg_details::SpecType type, bool isSimpleSub) {
	using enum serenity::msg_details::SpecType;
	// NOTE: The following types should have been caught in the verification process:
	//       monostate, string, c-string, string view, const void*, void *
	switch( type ) {
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: LocalizeIntegral(std::forward<std::back_insert_iterator<T>>(Iter), precision, type, isSimpleSub); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: [[fallthrough]];
			case U_LongLongType: LocalizeFloatingPoint(std::forward<std::back_insert_iterator<T>>(Iter), precision, type, isSimpleSub); break;
			case BoolType: LocalizeBool(std::forward<std::back_insert_iterator<T>>(Iter), isSimpleSub); break;
			default: break;
		}
}