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
		  argStorage(serenity::msg_details::ArgContainer {}), buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}), valueSize(), fillBuffer() { }

constexpr void serenity::arg_formatter::BracketSearchResults::Reset() {
	beginPos = endPos = 0;
}

constexpr void serenity::arg_formatter::SpecFormatting::ResetSpecs() {
	if( !std::is_constant_evaluated() ) {
			std::memset(this, 0, sizeof(SpecFormatting));
	} else {
			argPosition = nestedWidthArgPos = nestedPrecArgPos = 0;
			localize = hasAlt = hasClosingBrace = false;
			alignmentPadding = precision = 0;
			fillCharacter = typeSpec = '\0';
			Alignment align          = Alignment::Empty;
			signType                 = Sign::Empty;
			preAltForm               = "";
		}
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
							ReportError(ErrorType::missing_bracket);
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
							default: ReportError(ErrorType::position_field_spec); break;
						}
			}
	} else {
			// we're in manual mode
			auto data { sv.data() };
			if( auto offset { std::from_chars(data + start, data + sv.size(), positionValue).ptr - data }; offset != 0 ) {
					start = offset;
					if( positionValue <= 24 ) {
							switch( sv[ start ] ) {
									case ':': [[fallthrough]];
									case '}':
										++argCounter;
										++start;
										return true;
										break;
									default: ReportError(ErrorType::position_field_mode); break;
								}
					} else {
							ReportError(ErrorType::max_args_exceeded);
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
									return ParsePositionalField(sv, start, positionValue);
									break;
								}
							case ':': ReportError(ErrorType::position_field_no_position); break;
							default: ReportError(ErrorType::position_field_runon); break;
						}
				}
		}
	unreachable();
}

constexpr void serenity::arg_formatter::ArgFormatter::VerifyFillAlignField(std::string_view sv, size_t& currentPos, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	if( sv[ currentPos ] == ':' ) {
			specValues.fillCharacter = ' ';
	} else if( sv[ currentPos ] != '{' && sv[ currentPos ] != '}' ) {
			specValues.fillCharacter = sv[ currentPos ];
	} else {
			ReportError(ErrorType::invalid_fill_character);
		}
	switch( ++currentPos >= sv.size() ? sv.back() : sv[ currentPos ] ) {
			case '<':
				specValues.align = Alignment::AlignLeft;
				++currentPos;
				return;
			case '>':
				specValues.align = Alignment::AlignRight;
				++currentPos;
				return;
			case '^':
				specValues.align = Alignment::AlignCenter;
				++currentPos;
				return;
			default: --currentPos; break;
		}
	// This sets the default behavior for the case that no alignment option is found
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
			default: ReportError(ErrorType::invalid_alt_type); break;
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
			default: ReportError(ErrorType::invalid_precision_type); break;
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
			case SpecType::VoidPtrType: ReportError(ErrorType::invalid_locale_type); break;
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

constexpr void serenity::arg_formatter::ArgFormatter::HandlePotentialTypeField(const char& ch, const msg_details::SpecType& argType) {
	using namespace std::literals::string_view_literals;
	using enum msg_details::SpecType;
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
			case 'X':
				// if valid spec, set the spec and check for alternate form to prepend
				specValues.typeSpec = ch;
				if( !specValues.hasAlt ) return;
				switch( argType ) {
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
									default: return; break;
								}
							break;
						default: return; break;
					}
				break;
				// If not a valid spec, throw an error pertaining to the spec being invalid for that argument type
			default:
				switch( argType ) {
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
				break;
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
	 sv[start] != '}' ? HandlePotentialTypeField(sv[start], argType) : (IsAlpha(sv[--start]) && sv[start] != 'L') ? 
		                            HandlePotentialTypeField(sv[start], argType) : VerifyEscapedBracket(sv, start);
	// clang-format on
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
			size_t pos { 0 };
			/*Handle Escaped Bracket*/
			if( argBracket[ pos ] == '{' ) {
					Iter = '{';
					++pos;
			}
			/*Handle Positional Args*/
			if( !ParsePositionalField(argBracket, pos, specValues.argPosition) ) {
					// Nothing Else to Parse - just a simple substitution after position field
					auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
					Format(std::forward<std::back_insert_iterator<T>>(Iter), argType);
					sv.remove_prefix(argBracket.size() + 1);
					continue;
			}
			/* Handle What's Left Of The Bracket */
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( pos < argBracket.size() ) {
					VerifyArgumentBracket(argBracket, pos, argType);
			}
			Format(std::forward<std::back_insert_iterator<T>>(Iter), argType);
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
					Format(std::forward<std::back_insert_iterator<T>>(Iter), argType);
					sv.remove_prefix(argBracket.size() + 1);
					continue;
			}
			/* Handle What's Left Of The Bracket */
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( pos < argBracket.size() ) {
					VerifyArgumentBracket(argBracket, pos, argType);
			}
			Format(loc, std::forward<std::back_insert_iterator<T>>(Iter), argType);
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

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(std::back_insert_iterator<T>&& Iter, int totalWidth) {
	auto valueData { buffer.data() };
	if( size_t fillAmount { (totalWidth > valueSize) ? totalWidth - valueSize : 0 }; fillAmount != 0 ) {
			// swapping out the reuse of the buffer array for its own dedicated vector; using vector so there's no hard limit on fill amount -> in this way, the
			// limit is pretty much just dictated by the max size of the container being formatted to. It's sliightly slower than, say, using memset on an array
			// (only ~20ns-~30ns slower for smaller values up to 128 ) but it adds a lot more flexibility so it's definitely worth it I think
			fillBuffer.reserve(fillAmount + 1);
			fillBuffer.resize(fillAmount + 1, specValues.fillCharacter);
			auto data { fillBuffer.data() };
			switch( specValues.align ) {
					case Alignment::AlignLeft:
						{
							if constexpr( std::is_same_v<T, std::string> ) {
									IteratorContainer(Iter).Container()->append(valueData, valueSize).append(data, fillAmount);
							} else {
									Iter = std::move(valueData, valueData + valueSize, Iter);
									Iter = std::move(data, data + fillAmount, Iter);
								}
							return;
						}
					case Alignment::AlignRight:
						{
							if constexpr( std::is_same_v<T, std::string> ) {
									IteratorContainer(Iter).Container()->append(data, fillAmount).append(valueData, valueSize);
							} else {
									Iter = std::move(data, data + fillAmount, Iter);
									Iter = std::move(valueData, valueData + valueSize, Iter);
								}
							return;
						}
					case Alignment::AlignCenter:
						{
							fillAmount /= 2;
							if constexpr( std::is_same_v<T, std::string> ) {
									// clang-format off
									IteratorContainer(Iter).Container()->append(data, fillAmount)
									.append(valueData, valueSize).append(data, (totalWidth - valueSize - fillAmount));
									// clang - format on
							} else {
									Iter = std::move(data, data + fillAmount, Iter);
									Iter = std::move(valueData, valueData + valueSize, Iter);
									Iter = std::move(data, data + (totalWidth - valueSize - fillAmount), Iter);
								}
							return;
						}
					default: return; break;
				}
	} else {
			if constexpr( std::is_same_v<T, std::string> ) {
					IteratorContainer(Iter).Container()->append(valueData, valueSize);
			} else {
					Iter = std::move(valueData, valueData + valueSize, Iter);
				}
		}
}

template<typename T>    // string type overload for alignment
constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(std::back_insert_iterator<T>&& Iter, std::string_view val, int totalWidth, int precision) {
	auto size { static_cast<int>(val.size()) };
	precision = precision != 0 ? precision > size ? size : precision : size;
	if( auto fillAmount { totalWidth > size ? totalWidth - size : 0 }; fillAmount != 0 ) {
			fillBuffer.reserve(fillAmount + 1);
			fillBuffer.resize(fillAmount + 1, specValues.fillCharacter);
			auto data { fillBuffer.data() };
			switch( specValues.align ) {
					case Alignment::AlignLeft:
						{
							if constexpr( std::is_same_v<T, std::string> ) {
									IteratorContainer(Iter).Container()->append(val.data(), precision).append(data, fillAmount);
							} else {
									Iter = std::copy(val.data(), val.data() + precision, Iter);
									Iter = std::copy(data, data + fillAmount, Iter);
								}
							return;
						}
					case Alignment::AlignRight:
						{
							if constexpr( std::is_same_v<T, std::string> ) {
									IteratorContainer(Iter).Container()->append(data, fillAmount).append(val.data(), precision);
							} else {
									Iter = std::copy(data, data + fillAmount, Iter);
									Iter = std::copy(val.data(), val.data() + precision, Iter);
								}
							return;
						}
					case Alignment::AlignCenter:
						{
							fillAmount /= 2;
							if constexpr( std::is_same_v<T, std::string> ) {
									IteratorContainer(Iter).Container()->append(data, fillAmount).append(val.data(), precision).append(data, (totalWidth - size - fillAmount));
							} else {
									Iter = std::copy(data, data + fillAmount, Iter);
									Iter = std::copy(val.data(), val.data() + precision, Iter);
									Iter = std::copy(data, data + (totalWidth - size - fillAmount), Iter);
								}
							return;
						}
					default: return; break;
				}
	} else {
			if constexpr( std::is_same_v<T, std::string> ) {
					IteratorContainer(Iter).Container()->append(val.data(), precision);
			} else {
					Iter = std::copy(val.data(), val.data() + precision, Iter);
				}
		}
}

constexpr void serenity::arg_formatter::ArgFormatter::FormatArgument(int precision, const msg_details::SpecType& type) {
	using enum msg_details::SpecType;
	switch( type ) {
			case IntType:
				{
					return FormatInt(argStorage.int_state(specValues.argPosition));
					break;
				}
			case U_IntType:
				{
					return FormatInt(argStorage.uint_state(specValues.argPosition));
					break;
				}
			case LongLongType:
				{
					return FormatInt(argStorage.long_long_state(specValues.argPosition));
					break;
				}
			case U_LongLongType:
				{
					return FormatInt(argStorage.u_long_long_state(specValues.argPosition));
					break;
				}
			case BoolType:
				{
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 's' ) {
							FormatInt(static_cast<unsigned char>(argStorage.bool_state(specValues.argPosition)));
					} else {
							using namespace std::string_view_literals;
							auto sv { argStorage.bool_state(specValues.argPosition) ? "true"sv : "false"sv };
							std::copy(sv.data(), sv.data() + sv.size(), buffer.begin());
							valueSize = sv.size();
						}
					return;
					break;
				}
			case CharType:
				{
					if( specValues.typeSpec != '\0' && specValues.typeSpec != 'c' ) {
							FormatInt(static_cast<int>(argStorage.char_state(specValues.argPosition)));
					} else {
							valueSize   = 1;
							buffer[ 0 ] = argStorage.char_state(specValues.argPosition);
						}
					return;
					break;
				}
			case FloatType:
				{
					return FormatFloat(argStorage.float_state(specValues.argPosition), precision);
					break;
				}
			case DoubleType:
				{
					return FormatFloat(argStorage.double_state(specValues.argPosition), precision);
					break;
				}
			case LongDoubleType:
				{
					return FormatFloat(argStorage.long_double_state(specValues.argPosition), precision);
					break;
				}
			case ConstVoidPtrType:
				{
					buffer[ 0 ] = '0';
					buffer[ 1 ] = 'x';
					auto data { buffer.data() };
					valueSize =
					std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr - data;
					return;
					break;
				}
			case VoidPtrType:
				{
					;
					buffer[ 0 ] = '0';
					buffer[ 1 ] = 'x';
					auto data { buffer.data() };
					valueSize =
					std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr - data;
					return;
					break;
				}
			default: return; break;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatFloat(T&& value, int precision) {
	int pos { 0 };
	auto data { buffer.data() };
	if( !std::is_constant_evaluated() ) {
			std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE);
	} else {
			std::fill(buffer.begin(), buffer.end(), '\0');
		}
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
	auto end { (precision != 0 ? std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format, precision)
		                       : std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format))
		       .ptr };
	valueSize = end - data;
	if( !isUpper ) return;
	for( auto& ch: buffer ) {
			if( ch == *end ) return;
			if( ch >= 'a' && ch <= 'z' ) {
					ch -= 32;
			}
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatInt(T&& value) {
	int pos {};
	int base {};
	auto data { buffer.data() };
	if( !std::is_constant_evaluated() ) {
			std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE);
	} else {
			std::fill(buffer.begin(), buffer.end(), '\0');
		}
	switch( specValues.signType == Sign::Space ? value < 0 ? Sign::Minus : Sign::Space : specValues.signType ) {
			case Sign::Space: data[ pos++ ] = ' '; break;
			case Sign::Plus: data[ pos++ ] = '+'; break;
			case Sign::Empty: [[fallthrough]];
			case Sign::Minus: break;
		}
	// size is only ever 0, 1, or 2
	switch( specValues.preAltForm.size() ) {
			case 1:
				data[ pos ] = specValues.preAltForm[ 0 ];
				++pos;
				break;
			case 2:
				data[ pos ]   = specValues.preAltForm[ 0 ];
				data[ ++pos ] = specValues.preAltForm[ 1 ];
				++pos;
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
			case 'c':
				buffer[ 0 ] = static_cast<char>(value);
				valueSize   = 1;
				return;
			case 'o': base = 8; break;
			case 'x': base = 16; break;
			case 'X':
				base    = 16;
				isUpper = true;
				break;
			default: base = 10; break;
		}
	auto end { std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, base).ptr };
	valueSize = end - data;
	if( !isUpper ) return;
	for( auto& ch: buffer ) {
			if( ch == *end ) return;
			if( ch >= 'a' && ch <= 'z' ) {
					ch -= 32;
			}
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::FormatString(std::back_insert_iterator<T>&& Iter, std::string_view val, int precision) {
	int size { static_cast<int>(val.size()) };
	if constexpr( std::is_same_v<T, std::string> ) {
			IteratorContainer(Iter).Container()->append(val.data(), (precision != 0 ? precision > size ? size : precision : size));
	} else {
			std::copy(val.data(), val.data() + (precision != 0 ? precision > size ? size : precision : size), Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Format(std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
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
	switch( argType ) {
			case StringType:
				{
					auto val { argStorage.string_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
				}
			case CharPointerType:
				{
					auto val { argStorage.c_string_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
				}
			case StringViewType:
				{
					auto val { argStorage.string_view_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
				}
			default: break;
		}
	!specValues.localize ? FormatArgument(precision, argType) : LocalizeArgument(default_locale, precision, argType);
	FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth);
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::Format(const std::locale& loc, std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
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
	switch( argType ) {
			case StringType:
				{
					auto val { argStorage.string_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
					break;
				}
			case CharPointerType:
				{
					auto val { argStorage.c_string_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
					break;
				}
			case StringViewType:
				{
					auto val { argStorage.string_view_state(specValues.argPosition) };
					totalWidth == 0 ? FormatString(std::forward<std::back_insert_iterator<T>>(Iter), val, precision)
									: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), val, totalWidth, precision);
					return;
					break;
				}
			default: break;
		}

	!specValues.localize ? FormatArgument(precision, argType) : LocalizeArgument(loc, precision, argType);
	FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth);
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

// Now that the runtime errors are organized in a neater fashion, would really love to figure out 
// how libfmt does compile-time checking.  A lot of what is being used to verify things are all 
// runtime-access stuff so I'm assuming achieving this won't be easy at all =/
constexpr void serenity::arg_formatter::ArgFormatter::ReportError(ErrorType err) {
	using enum serenity::ErrorType;
	switch (err) {
	case missing_bracket: throw format_error(format_error_messages[1]); break;
	case position_field_spec: throw format_error(format_error_messages[2]); break;
	case position_field_mode: throw format_error(format_error_messages[3]); break;
	case position_field_no_position: throw format_error(format_error_messages[4]); break;
	case position_field_runon: throw format_error(format_error_messages[5]); break;
	case max_args_exceeded: throw format_error(format_error_messages[6]); break;
	case invalid_fill_character: throw format_error(format_error_messages[7]); break;
	case invalid_alt_type: throw format_error(format_error_messages[8]); break;
	case invalid_precision_type: throw format_error(format_error_messages[9]); break;
	case invalid_locale_type: throw format_error(format_error_messages[10]); break;
	case invalid_int_spec: throw format_error(format_error_messages[11]); break;
	case invalid_float_spec: throw format_error(format_error_messages[12]); break;
	case invalid_string_spec: throw format_error(format_error_messages[13]); break;
	case invalid_bool_spec: throw format_error(format_error_messages[14]); break;
	case invalid_char_spec: throw format_error(format_error_messages[15]); break;
	case invalid_pointer_spec: throw format_error(format_error_messages[16]); break;
	default: throw format_error(format_error_messages[0]); break;
	}
}
