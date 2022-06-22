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

/************************************** What The ArgFormatter And ArgContainer Classes Offer **************************************/
// This work is a very simple reimplementation with limitations on my end of Victor Zverovich's fmt library.
// Currently the standard's implementation of his work is still underway although for the most part, it's feature
// complete with Victor's library - there are some huge performance drops when it's not compiled under the /utf-8
// flag on MSVC though.
//
// The ArgFormatter and ArgContainer classes work in tandem to deliver a very bare-bones version of what the fmt,
// and MSVC's implementation of fmt, libraries provides and is only intended for usage until MSVC's code is as performant
// as it is when compiled with /utf-8 for compilation without the need for this flag.
//
// With that being said, these classes provide the functionality of formatting to a container with a back insert iterator
// object which mirrors the std::format_to()/std::vformat_to() via the se_format_to() function, as well as a way to receive
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

#include <serenity/MessageDetails/ArgContainer.h>
#include <serenity/MessageDetails/ArgFormatter.h>
#include <string_view>

constexpr auto fillBuffDefaultCapacity { 512 };

constexpr serenity::arg_formatter::ArgFormatter::ArgFormatter()
	: argCounter(0), m_indexMode(IndexMode::automatic), bracketResults(BracketSearchResults {}), specValues(SpecFormatting {}), argStorage(ArgContainer {}),
	  buffer(std::array<char, SERENITY_ARG_BUFFER_SIZE> {}), valueSize(), fillBuffer() {
	fillBuffer.reserve(fillBuffDefaultCapacity);
}

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
			align                    = Alignment::Empty;
			signType                 = Sign::Empty;
			preAltForm               = "";
		}
}

template<typename... Args> constexpr void serenity::arg_formatter::ArgFormatter::CaptureArgs(Args&&... args) {
	argStorage.CaptureArgs(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	ParseFormatString(std::forward<std::back_insert_iterator<T>>(Iter), sv);
}

template<typename T, typename... Args>
constexpr void serenity::arg_formatter::ArgFormatter::se_format_to(std::back_insert_iterator<T>&& Iter, const std::locale& loc, std::string_view sv, Args&&... args) {
	CaptureArgs(std::forward<Args>(args)...);
	ParseFormatString(loc, std::forward<std::back_insert_iterator<T>>(Iter), sv);
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

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Format(std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 ) {    // use this check to guard from any unnecessary additional checks
			if( IsSimpleSubstitution(argType, precision) ) {
					return WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argType);
			}
	}
	!specValues.localize ? FormatArgument(std::forward<std::back_insert_iterator<T>>(Iter), precision, totalWidth, argType)
						 : LocalizeArgument(std::forward<std::back_insert_iterator<T>>(Iter), default_locale, precision, totalWidth, argType);
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::Format(const std::locale& loc, std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	auto precision { specValues.nestedPrecArgPos != 0 ? argStorage.int_state(specValues.nestedPrecArgPos) : specValues.precision != 0 ? specValues.precision : 0 };
	auto totalWidth { specValues.nestedWidthArgPos != 0  ? argStorage.int_state(specValues.nestedWidthArgPos)
		              : specValues.alignmentPadding != 0 ? specValues.alignmentPadding
		                                                 : 0 };
	if( totalWidth == 0 ) {    // use this check to guard from any unnecessary additional checks
			if( IsSimpleSubstitution(argType, precision) ) {
					return WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argType);
			}
	}
	!specValues.localize ? FormatArgument(std::forward<std::back_insert_iterator<T>>(Iter), precision, totalWidth, argType)
						 : LocalizeArgument(std::forward<std::back_insert_iterator<T>>(Iter), loc, precision, totalWidth, argType);
}

constexpr void serenity::arg_formatter::ArgFormatter::Parse(std::string_view sv, size_t& start, const msg_details::SpecType& argType) {
	auto svSize { sv.size() };
	VerifyFillAlignField(sv, start, argType);
	if( start >= svSize ) return;
	switch( sv[ start ] ) {
			case '+':
				specValues.signType = Sign::Plus;
				++start;
				break;
			case '-':
				specValues.signType = Sign::Minus;
				++start;
				break;
			case ' ':
				specValues.signType = Sign::Space;
				++start;
				break;
			default: break;
		}
	if( start >= svSize ) return;
	if( sv[ start ] == '#' ) {
			VerifyAltField(sv, argType);
			++start;
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
			if( start >= svSize ) return;
	}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::ParseFormatString(std::back_insert_iterator<T>&& Iter, std::string_view sv) {
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
			/*Handle Escaped Brackets*/
			if( argBracket[ pos ] == '{' ) {
					Iter = '{';
					++pos;
			}
			auto bracketSize { argBracket.size() };
			if( bracketSize > 3 && argBracket[ bracketSize - 2 ] == '}' ) specValues.hasClosingBrace = true;
			/*Handle Positional Args*/
			if( !VerifyPositionalField(argBracket, pos, specValues.argPosition) ) {
					auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
					// Nothing Else to Parse- just a simple substitution after position field so write it and continute parsing format string
					if( argType == SpecType::CustomType ) {
							/************************************************************* TODO *************************************************************/
							// I can store the custom type, verify it has a specialization, throw a compiler error if it doesn't, store the value and value size. but
							// I cannot for the life of me get this to work... I'm running into object slicing at the moment, which makes a little bit of sense due
							//  to not templating the CustomBase, but I have no idea how to do this correctly... I tried just adding  the following ---------------->
							//  template<typename ArgType> VType. = ..CustomValue<ArgType>; but that made a mess of the ArgFormatter class where I
							// had to then template the class and was forcing even the static forms of the formatting functions to require a templated type
							// value... I just need to sit down and think of how to approach this. All I reealllyy need right now is just simply the value type; the
							// CustomBase struct holds the raw value and value size, but I can't reinterpret it to the original value type OR call the value type's
							// CustomFormatter specialization without being able to track it's typing...
							auto& customArg { argStorage.custom_state(specValues.argPosition) };
					} else {
							WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argType);
						}
					if( specValues.hasClosingBrace ) {
							Iter = '}';
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			/* Handle What's Left Of The Bracket */
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };

			if( argType == SpecType::CustomType ) {
					// TODO
			} else {
					if( pos < bracketSize ) {
							Parse(argBracket, pos, argType);
					}
					Format(std::forward<std::back_insert_iterator<T>>(Iter), argType);
				}
			if( specValues.hasClosingBrace ) {
					Iter = '}';
			}
			sv.remove_prefix(bracketSize + 1);
		}
	if( sv.size() != 0 ) {
			std::copy(sv.data(), sv.data() + sv.size(), Iter);
	}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::ParseFormatString(const std::locale& loc, std::back_insert_iterator<T>&& Iter, std::string_view sv) {
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
			auto bracketSize { argBracket.size() };
			if( bracketSize > 3 && argBracket[ bracketSize - 2 ] == '}' ) specValues.hasClosingBrace = true;
			/*Handle Positional Args*/
			if( !VerifyPositionalField(argBracket, pos, specValues.argPosition) ) {
					// Nothing Else to Parse- just a simple substitution after position field so write it and continute parsing format string
					WriteSimpleValue(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.SpecTypesCaptured()[ specValues.argPosition ]);
					if( specValues.hasClosingBrace ) {
							Iter = '}';
					}
					sv.remove_prefix(bracketSize + 1);
					continue;
			}
			/* Handle What's Left Of The Bracket */
			auto& argType { argStorage.SpecTypesCaptured()[ specValues.argPosition ] };
			if( pos < bracketSize ) {
					Parse(argBracket, pos, argType);
			}
			Format(loc, std::forward<std::back_insert_iterator<T>>(Iter), argType);
			if( specValues.hasClosingBrace ) {
					Iter = '}';
			}
			sv.remove_prefix(bracketSize + 1);
		}
	if( sv.size() != 0 ) {
			std::copy(sv.data(), sv.data() + sv.size(), Iter);
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

constexpr bool serenity::arg_formatter::ArgFormatter::VerifyPositionalField(std::string_view sv, size_t& start, size_t& positionValue) {
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
	specValues.fillCharacter = ' ';
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

// Splitting up the logic into their own function calls made this slightly faster, most likely due to being able to aid in inlining?
// I'm honestly not too sure why, but it saves ~0.45% CPU cycles being used and the timing for the complex format pattern
// was reduced from ~0.457us down to ~0.442us. In an effort to just make it look cleaner, it saved some time, so I'll take it
constexpr void serenity::arg_formatter::ArgFormatter::VerifyFillAlignField(std::string_view sv, size_t& currentPos, const msg_details::SpecType& argType) {
	const auto& ch { sv[ currentPos ] };
	switch( ++currentPos >= sv.size() ? sv.back() : sv[ currentPos ] ) {
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
			currentPosition = std::from_chars(sv.data() + currentPosition, sv.data() + sv.size(), specValues.alignmentPadding).ptr - sv.data();
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

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleString(std::back_insert_iterator<T>&& Iter) {
	std::string_view sv { std::move(argStorage.string_state(specValues.argPosition)) };
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					cont.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleCString(std::back_insert_iterator<T>&& Iter) {
	std::string_view sv { std::move(argStorage.c_string_state(specValues.argPosition)) };
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					cont.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleStringView(std::back_insert_iterator<T>&& Iter) {
	std::string_view sv { std::move(argStorage.string_view_state(specValues.argPosition)) };
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(sv.data(), sv.size());
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), sv.begin(), sv.end());
			if( sv.back() != '\0' ) {
					cont.push_back('\0');
			}
	} else {
			std::copy(sv.begin(), sv.end(), Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleInt(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.int_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleUInt(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.uint_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleLongLong(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.long_long_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleULongLong(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.u_long_long_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleBool(std::back_insert_iterator<T>&& Iter) {
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(argStorage.bool_state(specValues.argPosition) ? "true" : "false");
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			std::string_view sv { argStorage.bool_state(specValues.argPosition) ? "true" : "false" };
			cont.insert(cont.end(), sv.begin(), sv.end());
	} else {
			std::string_view sv { argStorage.bool_state(specValues.argPosition) ? "true" : "false" };
			std::copy(sv.begin(), sv.end(), Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleFloat(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	std::memset(data, 0, buffer.size());
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + buffer.size(), argStorage.float_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleDouble(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.double_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleLongDouble(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr);
	} else {
			std::move(data, std::to_chars(data, data + size, argStorage.long_double_state(specValues.argPosition)).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleConstVoidPtr(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append("0x").append(
			data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			static std::string_view sv { "0x" };
			cont.insert(cont.end(), sv.begin(), sv.end());
			cont.insert(cont.end(), data,
			            std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr);
	} else {
			Iter = '0';
			Iter = 'x';
			std::move(data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.const_void_ptr_state(specValues.argPosition)), 16).ptr, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleVoidPtr(std::back_insert_iterator<T>&& Iter) {
	auto data { buffer.data() };
	auto size { buffer.size() };
	std::memset(data, 0, size);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append("0x").append(
			data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr - data);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			static std::string_view sv { "0x" };
			cont.insert(cont.end(), sv.begin(), sv.end());
			cont.insert(cont.end(), data,
			            std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr);
	} else {
			Iter = '0';
			Iter = 'x';
			std::move(data, std::to_chars(data, data + buffer.size(), reinterpret_cast<size_t>(argStorage.void_ptr_state(specValues.argPosition)), 16).ptr, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteSimpleValue(std::back_insert_iterator<T>&& Iter, const msg_details::SpecType& argType) {
	using enum msg_details::SpecType;
	switch( argType ) {
			case StringType: WriteSimpleString(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case CharPointerType: WriteSimpleCString(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case StringViewType: WriteSimpleStringView(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case IntType: WriteSimpleInt(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case U_IntType: WriteSimpleUInt(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case LongLongType: WriteSimpleLongLong(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case U_LongLongType: WriteSimpleULongLong(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case BoolType: WriteSimpleBool(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case CharType: Iter = argStorage.char_state(specValues.argPosition); return;
			case FloatType: WriteSimpleFloat(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case DoubleType: WriteSimpleDouble(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case LongDoubleType: WriteSimpleLongDouble(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case ConstVoidPtrType: WriteSimpleConstVoidPtr(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			case VoidPtrType: WriteSimpleVoidPtr(std::forward<std::back_insert_iterator<T>>(Iter)); return;
			default: return;
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedLeft(std::back_insert_iterator<T>&& Iter, const int& totalWidth) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData, buffer.data(), valueSize);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedLeft(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision,
                                                                       const int& totalWidth) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData, val.data(), precision);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedRight(std::back_insert_iterator<T>&& Iter, const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, buffer.data(), valueSize);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedRight(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision,
                                                                        const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount, val.data(), precision);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedCenter(std::back_insert_iterator<T>&& Iter, const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount / 2, buffer.data(), valueSize);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteAlignedCenter(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision,
                                                                         const int& totalWidth, const size_t& fillAmount) {
	auto fillData { fillBuffer.data() };
	std::memcpy(fillData + fillAmount / 2, val.data(), precision);
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(fillData, totalWidth);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), fillData, fillData + totalWidth);
	} else {
			std::copy(fillData, fillData + totalWidth, Iter);
		}
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::WriteNonAligned(std::back_insert_iterator<T>&& Iter) {
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(buffer.data(), valueSize);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), buffer.data(), buffer.data() + valueSize);
	} else {
			std::copy(buffer.data(), buffer.data() + valueSize, Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteNonAligned(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision) {
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(val.data(), precision);
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), val.data(), val.data() + precision);
	} else {
			std::copy(val.data(), val.data() + precision, Iter);
		}
}

/************************************************************************* Possible TODO? ************************************************************************/
// A possible enhancement to this approach might be to just reserve the size in the value buffer (making it a vector instead of an array) then memset from the
// valueSize to the end of the new capacity with the fill character, and finally use std::rotate to rotate the appropriate amount for right/center aligned (left
// aligned would be done  before the rotate step) -> don't really know if there will be a performance benfit or hit but it's worth testing either way. If it matches
// the current performance or beats it out-right, then I'll roll with that change as it gets rid of an extra member variable that way and streamlines some usage; if
// it doesn't, then the two buffers  remain as they are.
/*****************************************************************************************************************************************************************/
/*************************************************************************** EDIT:********************************************************************************/
// Due to the intial approach (I NEED to research more into this) I presume that the use of  underlying unitialized storage is obviously a no-go; stepping through
// the debugger shows the logic works and under MSVC's  "_MyVal2" variable, the value is indeed what is expected after formatting the value and then adding the
// appropriate number of fill characters, but once we get to the rotation step, this function throws. I would like to see how I can get the unitialized values into
// the actual underlying array. Not sure if I'm missing something simple here or if the unitialized storage is somehow different from the reserved memory? Kind of
// thought that by calling the unitialized_copy/fill  functions here and in the formatting functions that the in-place memory would have initialized those values
// from the reserved memory =/ Honestly, I think what I have in mind for this requires a custom allocator of sorts so I may end up just abandoning this idea for now
// as that's not  the path I wanted to head down here...
/*****************************************************************************************************************************************************************/
//
//     template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::Test_Alignment(std::back_insert_iterator<T>&& Iter, const int& totalWidth) {
//    	auto SetFill = [&]() {
//    		if (!std::is_constant_evaluated()) {
//    			static_cast<void>(std::memset(testBuffer.data() + valueSize, specValues.fillCharacter, totalWidth - valueSize));
//    		}
//    		else {
//    			std::uninitialized_fill(testBuffer.begin() + valueSize, testBuffer.begin() + totalWidth, specValues.fillCharacter);
//    		}
//    	};
//    	auto ReserveAndSetFill = [&]() {
//    		testBuffer.reserve(totalWidth);
//    		SetFill();
//    	};
//    	totalWidth <= testBuffer.size() ? SetFill() : ReserveAndSetFill();
//    	switch (specValues.align) {
//    	case Alignment::AlignCenter:
//    	{
//             // TODO: make this simpler; was just trying to get it to work, so, this is messy hack for finding the center cut-off
//             auto begin { testBuffer.begin() };
//    		auto rotatePoint{ begin + (testBuffer.end() - (begin + valueSize)) / 2 };
//    		std::rotate(begin + valueSize, begin, rotatePoint);
//    		return;
//    	}
//    	case Alignment::AlignRight: std::rotate(testBuffer.begin() + valueSize, testBuffer.begin(), testBuffer.end()); return;
//    	case Alignment::AlignLeft: [[fallthrough]]; /* Due To The Fill Above, Left Aligned Is Done At This Step */
//    	default: return;
//    	}
//    }
//

constexpr void serenity::arg_formatter::ArgFormatter::FillBuffWithChar(const int& totalWidth) {
	!std::is_constant_evaluated() ? static_cast<void>(std::memset(fillBuffer.data(), specValues.fillCharacter, totalWidth))
								  : fillBuffer.resize(totalWidth, specValues.fillCharacter);
}

template<typename T> constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(std::back_insert_iterator<T>&& Iter, const int& totalWidth) {
	if( auto fill { (totalWidth > valueSize) ? totalWidth - valueSize : 0 }; fill != 0 ) {
			if( totalWidth > fillBuffDefaultCapacity ) fillBuffer.reserve(totalWidth);
			FillBuffWithChar(totalWidth);
			switch( specValues.align ) {
					case Alignment::AlignLeft: return WriteAlignedLeft(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth);
					case Alignment::AlignRight: return WriteAlignedRight(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth, fill);
					case Alignment::AlignCenter: return WriteAlignedCenter(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth, fill);
					default: return;
				}
	} else {
			WriteNonAligned(std::forward<std::back_insert_iterator<T>>(Iter));
		}
}

template<typename T>    // string type overload for alignment
constexpr void serenity::arg_formatter::ArgFormatter::FormatAlignment(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& totalWidth, int precision) {
	auto size { static_cast<int>(val.size()) };
	precision = precision != 0 ? precision > size ? size : precision : size;
	if( auto fill { totalWidth > size ? totalWidth - size : 0 }; fill != 0 ) {
			if( totalWidth > fillBuffDefaultCapacity ) fillBuffer.reserve(totalWidth);
			FillBuffWithChar(totalWidth);
			switch( specValues.align ) {
					case Alignment::AlignLeft: return WriteAlignedLeft(std::forward<std::back_insert_iterator<T>>(Iter), val, precision, totalWidth);
					case Alignment::AlignRight: return WriteAlignedRight(std::forward<std::back_insert_iterator<T>>(Iter), val, precision, totalWidth, fill);
					case Alignment::AlignCenter: return WriteAlignedCenter(std::forward<std::back_insert_iterator<T>>(Iter), val, precision, totalWidth, fill);
					default: return;
				}
	} else {
			WriteNonAligned(std::forward<std::back_insert_iterator<T>>(Iter), val, precision);
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
	switch( type ) {
			case ConstVoidPtrType:
				{
					buffer[ 0 ] = '0';
					buffer[ 1 ] = 'x';
					auto data { buffer.data() };
					valueSize = std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(std::forward<T>(value)), 16).ptr - data;
					return;
				}
			case VoidPtrType:
				{
					buffer[ 0 ] = '0';
					buffer[ 1 ] = 'x';
					auto data { buffer.data() };
					valueSize = std::to_chars(data + 2, data + buffer.size(), reinterpret_cast<size_t>(std::forward<T>(value)), 16).ptr - data;
					return;
				}
			default: return;
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::FormatArgument(std::back_insert_iterator<T>&& Iter, const int& precision, const int& totalWidth,
                                                                     const msg_details::SpecType& type) {
	using enum msg_details::SpecType;
	switch( type ) {
			case IntType: FormatIntegerType(argStorage.int_state(specValues.argPosition)); break;
			case U_IntType: FormatIntegerType(argStorage.uint_state(specValues.argPosition)); break;
			case LongLongType: FormatIntegerType(argStorage.long_long_state(specValues.argPosition)); break;
			case U_LongLongType: FormatIntegerType(argStorage.u_long_long_state(specValues.argPosition)); break;
			case BoolType: FormatBoolType(argStorage.bool_state(specValues.argPosition)); break;
			case CharType: FormatCharType(argStorage.char_state(specValues.argPosition)); break;
			case FloatType: FormatFloatType(argStorage.float_state(specValues.argPosition), precision); break;
			case DoubleType: FormatFloatType(argStorage.double_state(specValues.argPosition), precision); break;
			case LongDoubleType: FormatFloatType(argStorage.long_double_state(specValues.argPosition), precision); break;
			case StringType: [[fallthrough]];
			case CharPointerType: [[fallthrough]];
			case StringViewType: WriteString(std::forward<std::back_insert_iterator<T>>(Iter), type, precision, totalWidth); return;
			case ConstVoidPtrType: FormatPointerType(argStorage.const_void_ptr_state(specValues.argPosition), type); break;
			case VoidPtrType: FormatPointerType(argStorage.void_ptr_state(specValues.argPosition), type); break;
			default: return; break;
		}
	if( !specValues.localize ) FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth);
}

constexpr void serenity::arg_formatter::ArgFormatter::BufferToUpper(const char& end) {
	for( auto& ch: buffer ) {
			if( ch == end ) return;
			if( ch >= 'a' && ch <= 'z' ) {
					ch -= 32;
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

constexpr void serenity::arg_formatter::ArgFormatter::WritePreFormatChars(int& pos) {
	switch( specValues.preAltForm.size() ) {
			case 1:
				buffer[ pos ] = specValues.preAltForm[ 0 ];
				++pos;
				return;
			case 2:
				buffer[ pos ]   = specValues.preAltForm[ 0 ];
				buffer[ ++pos ] = specValues.preAltForm[ 1 ];
				++pos;
				return;
			default: return;
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
	auto data { buffer.data() };
	int pos { 0 };
	bool isUpper { false };
	std::chars_format format {};

	if( !std::is_constant_evaluated() ) {
			std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE);
	} else {
			std::fill(buffer.begin(), buffer.end(), '\0');
		}
	WriteSign(std::forward<T>(value), pos);
	SetFloatingFormat(format, precision, isUpper);
	auto end { (precision != 0 ? std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format, precision)
		                       : std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, format))
		       .ptr };
	valueSize = end - data;
	if( isUpper ) BufferToUpper(*end);
}

template<typename T>
requires std::is_integral_v<std::remove_cvref_t<T>>
constexpr void serenity::arg_formatter::ArgFormatter::FormatIntegerType(T&& value) {
	int pos {};
	int base {};
	bool isUpper { false };
	auto data { buffer.data() };

	if( !std::is_constant_evaluated() ) {
			std::memset(data, 0, SERENITY_ARG_BUFFER_SIZE);
	} else {
			std::fill(buffer.begin(), buffer.end(), '\0');
		}
	WriteSign(std::forward<T>(value), pos);
	WritePreFormatChars(pos);
	SetIntegralFormat(base, isUpper);
	auto end { std::to_chars(data + pos, data + SERENITY_ARG_BUFFER_SIZE, value, base).ptr };
	valueSize = end - data;
	if( isUpper ) BufferToUpper(*end);
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::FormatStringType(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision) {
	int size { static_cast<int>(val.size()) };
	auto& cont { *IteratorContainer(Iter).Container() };
	if constexpr( std::is_same_v<T, std::string> ) {
			cont.append(val.data(), (precision != 0 ? precision > size ? size : precision : size));
	} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
			cont.insert(cont.end(), val.data(), val.data() + (precision != 0 ? precision > size ? size : precision : size));
			if( val.back() != '\0' ) {
					cont.push_back('\0');
			}
	} else {
			std::copy(val.data(), val.data() + (precision != 0 ? precision > size ? size : precision : size), Iter);
		}
}

template<typename T>
constexpr void serenity::arg_formatter::ArgFormatter::WriteString(std::back_insert_iterator<T>&& Iter, const SpecType& type, const int& precision,
                                                                  const int& totalWidth) {
	using enum msg_details::SpecType;
	switch( type ) {
			case StringType:
				totalWidth == 0
				? FormatStringType(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.string_state(specValues.argPosition), precision)
				: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.string_state(specValues.argPosition), totalWidth, precision);
				return;
			case CharPointerType:
				totalWidth == 0
				? FormatStringType(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.c_string_state(specValues.argPosition), precision)
				: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.c_string_state(specValues.argPosition), totalWidth, precision);
				return;
			case StringViewType:
				totalWidth == 0
				? FormatStringType(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.string_view_state(specValues.argPosition), precision)
				: FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), argStorage.string_view_state(specValues.argPosition), totalWidth, precision);
				return;
			default: break;
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
void serenity::arg_formatter::ArgFormatter::LocalizeArgument(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision,
                                                             const int& totalWidth, const msg_details::SpecType& type) {
	using enum serenity::msg_details::SpecType;
	// NOTE: The following types should have been caught in the verification process:  monostate, string, c-string, string view, const void*, void *
	switch( type ) {
			case IntType: [[fallthrough]];
			case U_IntType: [[fallthrough]];
			case LongLongType: LocalizeIntegral(std::forward<std::back_insert_iterator<T>>(Iter), loc, precision, totalWidth, type); break;
			case FloatType: [[fallthrough]];
			case DoubleType: [[fallthrough]];
			case LongDoubleType: [[fallthrough]];
			case U_LongLongType: LocalizeFloatingPoint(std::forward<std::back_insert_iterator<T>>(Iter), loc, precision, totalWidth, type); break;
			case BoolType: LocalizeBool(std::forward<std::back_insert_iterator<T>>(Iter), loc); break;
		}
	FormatAlignment(std::forward<std::back_insert_iterator<T>>(Iter), totalWidth);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeIntegral(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision,
                                                             const int& totalWidth, const msg_details::SpecType& type) {
	FormatArgument(std::forward<std::back_insert_iterator<T>>(Iter), precision, totalWidth, type);
	FormatIntegralGrouping(loc, valueSize);
}

template<typename T>
void serenity::arg_formatter::ArgFormatter::LocalizeFloatingPoint(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision,
                                                                  const int& totalWidth, const msg_details::SpecType& type) {
	FormatArgument(std::forward<std::back_insert_iterator<T>>(Iter), precision, totalWidth, type);
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

template<typename T> void serenity::arg_formatter::ArgFormatter::LocalizeBool(std::back_insert_iterator<T>&& Iter, const std::locale& loc) {
	std::string_view sv { argStorage.bool_state(specValues.argPosition) ? std::use_facet<std::numpunct<char>>(loc).truename()
		                                                                : std::use_facet<std::numpunct<char>>(loc).falsename() };
	valueSize = sv.size();
	std::copy(sv.data(), sv.data() + valueSize, buffer.begin());
}
