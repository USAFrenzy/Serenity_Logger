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

constexpr serenity::arg_formatter::ArgFormatter::ArgFormatter()
		: argCounter(0), m_indexMode(IndexMode::automatic), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}),
		  argStorage(serenity::msg_details::ArgContainer {}), rawValueTemp(), buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}),
		  charsResult(std::to_chars_result {}) { }

constexpr void serenity::arg_formatter::BracketSearchResults::Reset() {
	beginPos = endPos = 0;
}

constexpr void serenity::arg_formatter::SpecFormatting::ResetSpecs() {
#ifdef NDEBUG
	std::memset(this, 0, sizeof(SpecFormatting));
#else
	argPosition = nestedWidthArgPos = nestedPrecArgPos = 0;
	localize = hasAlt = hasClosingBrace = false;
	alignmentPadding = precision = 0;
	fillCharacter = typeSpec = '\0';
	Alignment align          = Alignment::Empty;
	signType                 = Sign::Empty;
	preAltForm               = "";
#endif    // DEBUG
}
constexpr bool serenity::arg_formatter::ArgFormatter::FindBrackets(std::string_view sv) {
	bracketResults.Reset();
	const auto svSize { sv.size() };
	for( ;; ) {
			if( bracketResults.beginPos >= svSize ) {
					return false;
			}
			if( sv[ bracketResults.beginPos ] != '{' ) {
					++bracketResults.beginPos;
					continue;
			}
			bracketResults.endPos = bracketResults.beginPos + 1;
			for( ;; ) {
					if( bracketResults.endPos >= svSize ) {
							static_assert("Missing Closing '}' In Argument Spec Field\n");
							throw std::runtime_error("Missing Closing '}' In Argument Spec Field\n");
					}
					switch( sv[ bracketResults.endPos ] ) {
							case '{':
								for( ;; ) {
										if( ++bracketResults.endPos >= svSize ) {
												return false;
										}
										if( sv[ bracketResults.endPos ] == '}' ) break;
									}
								++bracketResults.endPos;
								continue;
								break;
							case '}': return true; break;
							default:
								++bracketResults.endPos;
								continue;
								break;
						}
				}
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::ParsePositionalField(std::string_view sv, size_t& start, size_t& positionValue) {
	// we're in automatic mode
	if( m_indexMode == IndexMode::automatic ) {
			if( const auto& ch { sv[ start ] }; IsDigit(ch) ) {
					m_indexMode = IndexMode::manual;
					return ParsePositionalField(sv, start, positionValue);
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
							default: throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Automatic Indexing Mode\n"); break;
						}
			}
	} else {
			// we're in manual mode
			auto data { sv.data() };
			if( auto offset { std::from_chars(data + start, data + sv.size(), positionValue).ptr - data }; offset != 0 ) {
					start = offset;
					if( positionValue < 24 ) {
							switch( sv[ start ] ) {
									case ':': [[fallthrough]];
									case '}':
										++argCounter;
										++start;
										return true;
										break;
									default: throw std::runtime_error("Error In Position Field: No ':' Or '}' Found While In Manual Indexing Mode\n"); break;
								}
					} else {
							throw std::runtime_error("Error In Position Argument Field: Max Position (24) Exceeded\n");
						}
			} else {
					switch( sv[ start ] ) {
							case '}': throw std::runtime_error("Error In Postion Field: Cannot Mix Manual And Automatic Indexing For Arguments\n"); break;
							case ' ':
								{
									auto size { sv.size() };
									for( ;; ) {
											if( start >= size ) break;
											if( sv[ ++start ] != ' ' ) break;
										}
									return ParsePositionalField(sv, start, positionValue);
									break;
								}
							case ':':
								throw std::runtime_error("Error In Position Field: Missing Positional Argument Before ':' In Manual Indexing Mode\n");
								break;
							default: throw std::runtime_error("Formatting Error Detected: Missing ':' Before Next Specifier\n"); break;
						}
				}
		}
	unreachable();
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyFillAlignField(std::string_view sv, size_t& currentPos, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	auto& ch { sv[ currentPos ] };
	switch( ++currentPos >= sv.size() ? sv.back() : sv[ currentPos ] ) {
			case '<': specValues.align = Alignment::AlignLeft; break;
			case '>': specValues.align = Alignment::AlignRight; break;
			case '^': specValues.align = Alignment::AlignCenter; break;
			default:
				// This sets the default behavior for the case that no alignment option is found
				specValues.fillCharacter = ' ';
				--currentPos;
				switch( argType ) {
						case IntType: [[fallthrough]];
						case U_IntType: [[fallthrough]];
						case DoubleType: [[fallthrough]];
						case FloatType: [[fallthrough]];
						case LongDoubleType: [[fallthrough]];
						case LongLongType: [[fallthrough]];
						case U_LongLongType: specValues.align = Alignment::AlignRight; return;
						default: specValues.align = Alignment::AlignLeft; return;
					}
		}
	if( ch != '{' && ch != '}' ) {
			specValues.fillCharacter = ch;
			++currentPos;
			return;
	}
	if( ch == ':' ) {
			specValues.fillCharacter = ' ';
			return;
	}
	throw std::runtime_error("Error In Fill/Align Field: Invalid Fill Character Provided\n");
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifySignField(const char& ch, size_t& currentPosition) {
	switch( ch ) {
			case '+': specValues.signType = Sign::Plus; break;
			case '-': specValues.signType = Sign::Minus; break;
			case ' ': specValues.signType = Sign::Space; break;
			default: specValues.signType = Sign::Empty; return;
		}
	++currentPosition;
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
			default: throw std::runtime_error("Error In Alternate Field: Argument Type Has No Alternate Form\n"); break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyWidthField(std::string_view sv, size_t& currentPosition) {
	if( const auto& ch { sv[ currentPosition ] }; IsDigit(ch) ) {
			currentPosition = std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.alignmentPadding).ptr - sv.data();
			return;
	} else {
			switch( ch ) {
					case '{':
						{
							ParsePositionalField(sv, ++currentPosition, specValues.nestedWidthArgPos);
							return;
						}
					case '}':
						{
							ParsePositionalField(sv, currentPosition, specValues.nestedWidthArgPos);
							return;
						}
					case ' ':
						{
							auto size { sv.size() };
							for( ;; ) {
									if( currentPosition >= size ) break;
									if( sv[ ++currentPosition ] != ' ' ) break;
								}
							return VerifyWidthField(sv, currentPosition);
						}
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyPrecisionField(std::string_view sv, size_t& currentPosition, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType: break;
			case StringViewType: break;
			case CharPointerType: break;
			case FloatType: break;
			case DoubleType: break;
			case LongDoubleType: break;
			default: throw std::runtime_error("Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field\n"); break;
		}
	if( const auto& ch { sv[ ++currentPosition ] }; IsDigit(ch) ) {
			auto data { sv.data() };
			currentPosition = std::from_chars(data + currentPosition, data + sv.size(), specValues.precision).ptr - data;
			++argCounter;
			return;
	} else {
			switch( ch ) {
					case '{':
						{
							ParsePositionalField(sv, ++currentPosition, specValues.nestedPrecArgPos);
							return;
						}
					case '}':
						{
							ParsePositionalField(sv, currentPosition, specValues.nestedPrecArgPos);
							return;
						}
					case ' ':
						{
							auto size { sv.size() };
							for( ;; ) {
									if( currentPosition >= size ) break;
									if( sv[ ++currentPosition ] != ' ' ) break;
								}
							return VerifyPrecisionField(sv, currentPosition, argType);
						}
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyEscapedBracket(std::string_view sv, size_t& currentPosition) {
	if( (currentPosition + 1) <= (sv.size() - 2) && (sv[ currentPosition + static_cast<size_t>(1) ] == '}') ) {
			specValues.hasClosingBrace = true;
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
			case SpecType::VoidPtrType: throw std::runtime_error("Error In Locale Field: Argument Type Cannot Be Localized\n"); break;
			default: specValues.localize = true; return;
		}
}

constexpr bool serenity::arg_formatter::ArgFormatter::IsSimpleSubstitution(const msg_details::SpecType& argType, const int& precision) {
	using enum serenity::msg_details::SpecType;
	switch( argType ) {
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType: return precision == 0; break;
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType:
				{
					return !specValues.hasAlt && specValues.signType == Sign::Empty && !specValues.localize && specValues.typeSpec == '\0';
				}
			case BoolType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 's'); break;
			case CharType: return !specValues.hasAlt && (specValues.typeSpec == '\0' || specValues.typeSpec == 'c'); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType:
				{
					return !specValues.localize && precision == 0 && specValues.signType == Sign::Empty && !specValues.hasAlt && specValues.typeSpec == '\0';
				}
			// for pointer types, if the width field is 0, there's no fill/alignment to take into account and therefore it's a simple sub
			case ConstVoidPtrType: [[fallthrough]];
			case VoidPtrType: return true; break;
			default: return false; break;
		}
}

static constexpr bool IsAllowedSpec(const char& ch) {
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
			case 'X': return true; break;
			default: return false; break;
		}
}

static constexpr std::array<const char*, 6> TypeFieldMessages = {
	"Error In Format: Invalid Type Specifier For Int Type Argument\n",    "Error In Format: Invalid Type Specifier For Float Type Argument\n",
	"Error In Format: Invalid Type Specifier For String Type Argument\n", "Error In Format: Invalid Type Specifier For Bool Type Argument\n",
	"Error In Format: Invalid Type Specifier For Char Type Argument\n",   "Error In Format: Invalid Type Specifier For Pointer Type Argument\n",
};
constexpr void serenity::arg_formatter::ArgFormatter::HandlePotentialTypeField(const char& ch, const msg_details::SpecType& argType) {
	using namespace std::literals::string_view_literals;
	using enum msg_details::SpecType;
	if( !IsAllowedSpec(ch) ) {
			switch( argType ) {
					case IntType: [[fallthrough]];
					case U_IntType: [[fallthrough]];
					case LongLongType: [[fallthrough]];
					case U_LongLongType: throw std::runtime_error(TypeFieldMessages[ 0 ]); break;
					case FloatType: [[fallthrough]];
					case DoubleType: [[fallthrough]];
					case LongDoubleType: throw std::runtime_error(TypeFieldMessages[ 1 ]); break;
					case StringType: [[fallthrough]];
					case CharPointerType: [[fallthrough]];
					case StringViewType: throw std::runtime_error(TypeFieldMessages[ 2 ]); break;
					case BoolType: throw std::runtime_error(TypeFieldMessages[ 3 ]); break;
					case CharType: throw std::runtime_error(TypeFieldMessages[ 4 ]); break;
					case ConstVoidPtrType: [[fallthrough]];
					case VoidPtrType: throw std::runtime_error(TypeFieldMessages[ 5 ]); break;
				}
	}
	specValues.typeSpec = ch;
	if( !specValues.hasAlt ) return;
	switch( argType ) {
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: [[fallthrough]];
			case U_LongLongType: [[fallthrough]];
			case BoolType: [[fallthrough]];
			case CharType:
				{
					switch( ch ) {
							case 'b': specValues.preAltForm = "0b"sv; return;
							case 'B': specValues.preAltForm = "0B"sv; return;
							case 'o': specValues.preAltForm = "0"sv; return;
							case 'x': specValues.preAltForm = "0x"sv; return;
							case 'X': specValues.preAltForm = "0X"sv; return;
							default: return; break;
						}
				}
			default: return; break;
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyArgumentBracket(std::string_view sv, size_t& start, const msg_details::SpecType& argType) {
	VerifyFillAlignField(sv, start, argType);
	VerifySignField(sv[ start ], start);
	if( sv[ start ] == '#' ) {
			VerifyAltField(sv, argType);
			++start;
	}
	if( sv[ start ] == '0' ) {
			if( specValues.fillCharacter == '\0' ) {
					specValues.fillCharacter = '0';
			}
			++start;
	}
	if( (sv[ start ] == '{') || (sv[ start ] >= '1' && sv[ start ] <= '9') ) {
			VerifyWidthField(sv, start);
	}
	if( sv[ start ] == '.' ) {
			VerifyPrecisionField(sv, start, argType);
	}
	if( sv[ start ] == 'L' ) {
			VerifyLocaleField(sv, start, argType);
	}
	// clang-format off
	 sv[start] != '}' ? HandlePotentialTypeField(sv[start], argType)
		 : (IsAlpha(sv[--start]) && sv[start] != 'L') ? HandlePotentialTypeField(sv[start], argType)
		 : VerifyEscapedBracket(sv, start);
	// clang-format on
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatRawValueToStr(int precision, const msg_details::SpecType& type) {
	using enum msg_details::SpecType;
	switch( type ) {
			case StringType:
				{
					return AppendByPrecision(std::move(argStorage.string_state(specValues.argPosition)), precision);
				}
			case CharPointerType:
				{
					return AppendByPrecision(std::move(argStorage.c_string_state(specValues.argPosition)), precision);
				}
			case StringViewType:
				{
					return AppendByPrecision(std::move(argStorage.string_view_state(specValues.argPosition)), precision);
				}
			case IntType:
				{
					return FormatIntTypeArg(argStorage.int_state(specValues.argPosition));
				}
			case U_IntType:
				{
					return FormatIntTypeArg(argStorage.uint_state(specValues.argPosition));
				}
			case LongLongType:
				{
					return FormatIntTypeArg(argStorage.long_long_state(specValues.argPosition));
				}
			case U_LongLongType:
				{
					return FormatIntTypeArg(argStorage.u_long_long_state(specValues.argPosition));
				}
			case BoolType:
				{
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 's' ) {
							FormatIntTypeArg(static_cast<unsigned char>(argStorage.bool_state(specValues.argPosition)));
					} else {
							rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
						}
					return;
				}
			case CharType:
				{
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ) {
							FormatIntTypeArg(static_cast<int>(argStorage.char_state(specValues.argPosition)));
					} else {
							rawValueTemp += argStorage.char_state(specValues.argPosition);
						}
					return;
				}
			case FloatType:
				{
					return FormatFloatTypeArg(argStorage.float_state(specValues.argPosition), precision);
				}
			case DoubleType:
				{
					return FormatFloatTypeArg(argStorage.double_state(specValues.argPosition), precision);
				}
			case LongDoubleType:
				{
					return FormatFloatTypeArg(argStorage.long_double_state(specValues.argPosition), precision);
				}
			case ConstVoidPtrType:
				{
					auto data { buffer.data() };
					charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16);
					rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
					return;
				}
			case VoidPtrType:
				{
					auto data { buffer.data() };
					charsResult = std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16);
					rawValueTemp.append("0x").append(buffer.data(), charsResult.ptr);
					return;
				}
			default: return; break;
		}
}
constexpr void serenity::arg_formatter::ArgFormatter::AppendByPrecision(std::string_view val, int precision) {
	int size { static_cast<int>(val.size()) };
	precision = precision > 0 ? precision > size ? size : precision : size;
	rawValueTemp.reserve(size);
	rawValueTemp.append(val.data(), precision);
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(const std::locale& loc, std::back_insert_iterator<T>&& Iter, std::string_view sv,
                                                                   Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	Parse(loc, std::forward<std::back_insert_iterator<T>>(Iter), sv);
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
	se_format_to(loc, std::back_inserter(tmp), sv, std::forward<Args>(args)...);
	return tmp;
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter  = 0;
	m_indexMode = IndexMode::automatic;
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

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::Parse(const std::locale& loc, std::back_insert_iterator<T>&& Iter, std::string_view sv) {
	argCounter  = 0;
	m_indexMode = IndexMode::automatic;
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
			Format(loc, std::forward<std::back_insert_iterator<T>>(Iter), std::move(argType));
			if( specValues.hasClosingBrace ) {
					Iter = '}';
			}
			sv.remove_prefix(argBracket.size() + 1);
		}
	if( sv.size() != 0 ) {
			std::copy(sv.data(), sv.data() + sv.size(), Iter);
	}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleValue(std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType:
				{
					std::string_view sv { std::move(argStorage.string_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(sv.data(), sv.size());
					} else {
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case CharPointerType:
				{
					std::string_view sv { std::move(argStorage.c_string_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(sv.data(), sv.size());
					} else {
							std::copy(sv.begin(), sv.end(), Iter);
						}
					return;
				}
			case StringViewType:
				{
					std::string_view sv { std::move(argStorage.string_view_state(specValues.argPosition)) };
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(sv.data(), sv.size());
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
							IteratorContainer(Iter).Container()->append(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr -
							                                                  data);
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr - data);
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr - data);
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr - data);
					} else {
							std::move(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr, Iter);
						}
					return;
				}
			case BoolType:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr - data);
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr - data);
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
							IteratorContainer(Iter).Container()->append(
							data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr - data);
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
							IteratorContainer(Iter).Container()->append("0x").append(
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
							IteratorContainer(Iter).Container()->append("0x").append(
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

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(std::back_insert_iterator<T>&& Iter, int totalWidth, size_t fillAmount) {
	auto data { buffer.data() };
	auto size(rawValueTemp.size());
	std::memset(data, specValues.fillCharacter, buffer.size());

	switch( specValues.align ) {
			case Alignment::AlignLeft:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(std::move(rawValueTemp).data(), size).append(data, fillAmount);
					} else {
							Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
							Iter = std::move(data, data + fillAmount, Iter);
						}
					return;
				}
			case Alignment::AlignRight:
				{
					if constexpr( std::is_same_v<T, std::string> ) {
							IteratorContainer(Iter).Container()->append(data, fillAmount).append(std::move(rawValueTemp).data(), size);
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
							IteratorContainer(Iter)
							.Container()
							->append(data, fillAmount)
							.append(std::move(rawValueTemp).data(), size)
							.append(data, (totalWidth - size - fillAmount));
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

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Format(std::back_insert_iterator<T>&& Iter, msg_details::SpecType&& argType) {
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
	FormatRawValueToStr(precision, argType);
	size_t fillAmount { (totalWidth > rawValueTemp.size()) ? totalWidth - rawValueTemp.size() : 0 };
	if( fillAmount == 0 ) {
			if constexpr( std::is_same_v<T, std::string> ) {
					IteratorContainer(Iter).Container()->append(std::move(rawValueTemp).data(), rawValueTemp.size());
			} else {
					Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
				}
			return;
	}
	if( specValues.align == Alignment::Empty ) return;
	FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth, fillAmount);
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::Format(const std::locale& loc, std::back_insert_iterator<T>&& Iter, msg_details::SpecType&& argType) {
	using enum msg_details::SpecType;
	int precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos)
		            : specValues.precision != 0      ? specValues.precision
		                                             : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	// Compared to the unlocalized version, even if locale is supplied, if the flag isn't set -> don't localize it
	if( totalWidth == 0 ) {
			if( IsSimpleSubstitution(argType, precision) ) {
					return WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argType);
			}
	}
	rawValueTemp.clear();
	LocalizeArgument(loc, precision, argType);
	size_t fillAmount { (totalWidth > rawValueTemp.size()) ? totalWidth - rawValueTemp.size() : 0 };
	if( fillAmount == 0 ) {
			if constexpr( std::is_same_v<T, std::string> ) {
					IteratorContainer(Iter).Container()->append(std::move(rawValueTemp).data(), rawValueTemp.size());
			} else {
					Iter = std::move(rawValueTemp.begin(), rawValueTemp.end(), Iter);
				}
			return;
	}
	if( specValues.align == Alignment::Empty ) return;
	FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth, fillAmount);
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatFloatTypeArg(T&& value, int& precision) {
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

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatIntTypeArg(T&& value) {
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
