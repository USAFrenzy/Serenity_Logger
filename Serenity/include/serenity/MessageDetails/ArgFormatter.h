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

#include <serenity/MessageDetails/ArgContainer.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace serenity {
	// drop-in replacement for format_error for the ArgFormatter class
	class format_error: public std::runtime_error
	{
	  public:
		explicit format_error(const char* message): std::runtime_error(message) { }
		explicit format_error(const std::string& message): std::runtime_error(message) { }
		format_error(const format_error&)            = default;
		format_error& operator=(const format_error&) = default;
		format_error(format_error&&)                 = default;
		format_error& operator=(format_error&&)      = default;
		~format_error() noexcept override            = default;
	};

	enum class ErrorType
	{
		none = 0,
		missing_bracket,
		position_field_spec,
		position_field_mode,
		position_field_no_position,
		position_field_runon,
		max_args_exceeded,
		invalid_fill_character,
		invalid_alt_type,
		invalid_precision_type,
		invalid_locale_type,
		invalid_int_spec,
		invalid_float_spec,
		invalid_string_spec,
		invalid_bool_spec,
		invalid_char_spec,
		invalid_pointer_spec,
	};

	static constexpr std::array<const char*, 17> format_error_messages = {
		"Unkown Formatting Error Occured.",
		"Missing Closing '}' In Argument Spec Field.",
		"Error In Position Field: No ':' Or '}' Found While In Automatic Indexing Mode.",
		"Error In Postion Field: Cannot Mix Manual And Automatic Indexing For Arguments."
		"Error In Position Field: Missing Positional Argument Before ':' In Manual Indexing Mode.",
		"Formatting Error Detected: Missing ':' Before Next Specifier.",
		"Error In Position Argument Field: Max Position (24) Exceeded.",
		"Error In Fill/Align Field: Invalid Fill Character Provided.",
		"Error In Alternate Field: Argument Type Has No Alternate Form.",
		"Error In Precision Field: An Integral Type Is Not Allowed To Have A Precsision Field.",
		"Error In Locale Field: Argument Type Cannot Be Localized.",
		"Error In Format: Invalid Type Specifier For Int Type Argument.",
		"Error In Format: Invalid Type Specifier For Float Type Argument.",
		"Error In Format: Invalid Type Specifier For String Type Argument.",
		"Error In Format: Invalid Type Specifier For Bool Type Argument.",
		"Error In Format: Invalid Type Specifier For Char Type Argument.",
		"Error In Format: Invalid Type Specifier For Pointer Type Argument.",
	};
}    // namespace serenity

namespace serenity::arg_formatter {

	using namespace msg_details;
	constexpr size_t SERENITY_ARG_BUFFER_SIZE { 65 };
	// defualt locale used for when no locale is provided, yet a locale flag is present when formatting
	static std::locale default_locale { std::locale("") };

	enum class Alignment : char
	{
		Empty = 0,
		AlignLeft,
		AlignRight,
		AlignCenter
	};

	enum class Sign
	{
		Empty = 0,
		Plus,
		Minus,
		Space
	};

	enum class IndexMode
	{
		automatic,
		manual
	};

	struct SpecFormatting
	{
		constexpr void ResetSpecs();
		size_t argPosition { 0 };
		int alignmentPadding { 0 };
		int precision { 0 };
		size_t nestedWidthArgPos { 0 };
		size_t nestedPrecArgPos { 0 };
		Alignment align { Alignment::Empty };
		char fillCharacter { '\0' };
		char typeSpec { '\0' };
		std::string_view preAltForm { "" };
		Sign signType { Sign::Empty };
		bool localize { false };
		bool hasAlt { false };
		bool hasClosingBrace { false };
	};

	struct BracketSearchResults
	{
		constexpr void Reset();
		size_t beginPos { 0 };
		size_t endPos { 0 };
	};

	template<typename T> struct IteratorContainer: std::back_insert_iterator<T>
	{
		using std::back_insert_iterator<T>::container_type;
		constexpr IteratorContainer(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		constexpr const auto& Container() {
			return this->container;
		}
	};

	template<typename... Args> static constexpr void ReserveCapacityImpl(size_t& totalSize, Args&&... args) {
		(
		[ = ](size_t& totalSize, auto&& arg) {
			using base_type = std::decay_t<decltype(arg)>;
			if constexpr( std::is_same_v<base_type, std::string> || std::is_same_v<base_type, std::string_view> ) {
					totalSize += arg.size();
			} else if constexpr( std::is_same_v<base_type, const char*> ) {
					totalSize += std::strlen(arg);
			} else {
					totalSize += 16;
				}
		}(totalSize, args),
		...);
	}

	template<typename... Args> static constexpr size_t ReserveCapacity(Args&&... args) {
		size_t totalSize {};
		ReserveCapacityImpl(totalSize, std::forward<Args>(args)...);
		return std::forward<size_t>(totalSize);
	}

	// WIP concepts -> The idea is to add constraints similar to what fmtlib has instead of the limitation on explicitly using a
	// back_insert_iterator; not sure why, but adding the OutputIter concept to the relevant functions drastically speeds up
	// compile times but heavily reduces runtime performance. Even when limiting the OutputIter concept to only require
	// the "is_iterator" concept and "has_output_functionality" concept, performance was heavily impacted. The complex
	//  pattern formatting for instance went from ~0.44us down to ~1.13us... So obviously I'm doing something  wrong here =/
	template<typename T>
	concept is_iterator = requires {
		T::iterator_category&& T::value_type&& T::difference_type&& T::pointer&& T::reference;
	};
	template<typename T>
	concept has_iterator_traits = requires {
		std::iterator_traits<T> {};
	};
	template<typename T>
	concept valid_iter = requires {
		is_iterator<T>&& has_iterator_traits<T>;
	};
	template<typename T>
	concept has_output_functionality = requires {
		valid_iter<T> && (std::forward_iterator<T> || std::bidirectional_iterator<T> || std::random_access_iterator<T>);
	};
	template<typename T>
	concept OutputIter = requires {
		has_output_functionality<T>;
	};

	/********************************************************************************************************************************************************
	    Compatible class that provides some of the same functionality that mirrors <format> and libfmt for basic formatting needs for pre  C++20 and MSVC's
	    pre-backported fixes (which required C ++23) for some build versions of Visual Studio as well as for performance needs. Everything in this class either
	    matches (in the case of simple double substitution) or greatly exceeds the performance of MSVC's implementation -  with the caveat of no custom
	    formatting support, no utf-8 support, and no type-erasure as of right now. I believe libfmt is faster than this basic implementation  (and unarguably
	    way more comprehensive as well) but I have yet to bench timings against it.
	********************************************************************************************************************************************************/
	class ArgFormatter
	{
	  public:
		constexpr ArgFormatter();
		constexpr ArgFormatter(const ArgFormatter&)            = delete;
		constexpr ArgFormatter& operator=(const ArgFormatter&) = delete;
		constexpr ~ArgFormatter()                              = default;
		template<typename... Args> [[nodiscard]] std::string se_format(std::string_view sv, Args&&... args);
		template<typename... Args> [[nodiscard]] std::string se_format(const std::locale& locale, std::string_view sv, Args&&... args);
		template<typename T, typename... Args> constexpr void se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);
		template<typename T, typename... Args>
		constexpr void se_format_to(const std::locale& loc, std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);

	  private:
		template<typename... Args> constexpr void CaptureArgs(Args&&... args);
		// At the moment ParseFormatString() and Format() are coupled together where ParseFormatString calls Format, hence the need
		// right now to have a version of ParseFormatString() that takes a locale object to forward to the locale overloaded Format()
		template<typename T> constexpr void ParseFormatString(std::back_insert_iterator<T>&& Iter, std::string_view sv);
		template<typename T> constexpr void ParseFormatString(const std::locale& loc, std::back_insert_iterator<T>&& Iter, std::string_view sv);
		template<typename T> constexpr void Format(std::back_insert_iterator<T>&& Iter, const SpecType& argType);
		template<typename T> constexpr void Format(const std::locale& loc, std::back_insert_iterator<T>&& Iter, const SpecType& argType);
		/******************************************************* Parsing/Verification Related Functions *******************************************************/
		[[noreturn]] constexpr void ReportError(ErrorType err);
		constexpr bool FindBrackets(std::string_view sv);
		constexpr void Parse(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr bool VerifyPositionalField(std::string_view sv, size_t& start, size_t& positionValue);
		constexpr void VerifyFillAlignField(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void VerifyAltField(std::string_view sv, const SpecType& argType);
		constexpr void VerifyWidthField(std::string_view sv, size_t& currentPosition);
		constexpr void VerifyPrecisionField(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void VerifyLocaleField(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void HandlePotentialTypeField(const char& ch, const SpecType& argType);
		constexpr bool IsSimpleSubstitution(const SpecType& argType, const int& precision);
		constexpr void OnAlignLeft(const char& ch, size_t& pos);
		constexpr void OnAlignRight(const char& ch, size_t& pos);
		constexpr void OnAlignCenter(const char& ch, size_t& pos);
		constexpr void OnAlignDefault(const SpecType& type, size_t& pos);
		constexpr void OnValidTypeSpec(const SpecType& type, const char& ch);
		constexpr void OnInvalidTypeSpec(const SpecType& type);
		/************************************************************ Formatting Related Functions ************************************************************/
		constexpr void FormatBoolType(bool& value);
		constexpr void FormatCharType(char& value);
		// clang-format off
		template<typename T>
		constexpr void FormatArgument(std::back_insert_iterator<T>&& Iter, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T> 
		constexpr void FormatStringType(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision);
		template<typename T> requires std::is_integral_v<std::remove_cvref_t<T>>
		constexpr void FormatIntegerType(T&& value);
		template<typename T> requires std::is_pointer_v<std::remove_cvref_t<T>>
		constexpr void FormatPointerType(T&& value, const SpecType& type);
		template<typename T> requires std::is_floating_point_v<std::remove_cvref_t<T>>
		constexpr void FormatFloatType(T&& value, int precision);
		// clang-format on
		/******************************************************** Container Writing Related Functions *********************************************************/
		constexpr void BufferToUpper(const char& end);
		constexpr void SetIntegralFormat(int& base, bool& isUpper);
		constexpr void SetFloatingFormat(std::chars_format& format, int& precision, bool& isUpper);
		constexpr void WritePreFormatChars(int& pos);
		constexpr void WriteChar(const char& value);
		constexpr void WriteBool(const bool& value);
		template<typename T> constexpr void WriteString(std::back_insert_iterator<T>&& Iter, const SpecType& type, const int& precision, const int& totalWidth);
		template<typename T> constexpr void WriteSimpleValue(std::back_insert_iterator<T>&& Iter, const SpecType&);
		template<typename T> constexpr void FormatAlignment(std::back_insert_iterator<T>&& Iter, const int& totalWidth);
		template<typename T> constexpr void FormatAlignment(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& width, int prec);
		template<typename T> constexpr void WriteSimpleString(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleCString(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleStringView(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleInt(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleUInt(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleLongLong(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleULongLong(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleBool(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleFloat(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleDouble(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleLongDouble(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleConstVoidPtr(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteSimpleVoidPtr(std::back_insert_iterator<T>&& Iter);
		// clang-format off
		template<typename T> constexpr void WriteAlignedLeft(std::back_insert_iterator<T>&& Iter, const int& totalWidth);
		template<typename T>constexpr void WriteAlignedLeft(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision, const int& totalWidth);
		template<typename T> constexpr void WriteAlignedRight(std::back_insert_iterator<T>&& Iter, const int& totalWidth, const size_t& fillAmount);
		template<typename T>constexpr void WriteAlignedRight(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision, const int& totalWidth, const size_t& fillAmount);
		template<typename T> constexpr void WriteAlignedCenter(std::back_insert_iterator<T>&& Iter, const int& totalWidth, const size_t& fillAmount);
		template<typename T>constexpr void WriteAlignedCenter(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision, const int& totalWidth, const size_t& fillAmount);
		template<typename T> constexpr void WriteNonAligned(std::back_insert_iterator<T>&& Iter);
		template<typename T> constexpr void WriteNonAligned(std::back_insert_iterator<T>&& Iter, std::string_view val, const int& precision);
		template<typename T> requires std::is_arithmetic_v<std::remove_cvref_t<T>>
		constexpr void WriteSign(T&& value, int& pos);
		// clang-format on

		// Due to the usage of the numpunct functions, which are not constexpr, these functions can't really be specified as constexpr
		template<typename T>
		void LocalizeArgument(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T>
		void LocalizeIntegral(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T>
		void LocalizeFloatingPoint(std::back_insert_iterator<T>&& Iter, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T> void LocalizeBool(std::back_insert_iterator<T>&& Iter, const std::locale& loc);
		void FormatIntegralGrouping(const std::locale& loc, size_t end);

	  private:
		int argCounter;
		IndexMode m_indexMode;
		BracketSearchResults bracketResults;
		SpecFormatting specValues;
		ArgContainer argStorage;
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer;
		size_t valueSize;
		std::vector<char> fillBuffer;
		size_t fillBufferDefaultSize;
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>

}    // namespace serenity::arg_formatter

/********************************************************************** Note: ***********************************************************************/
//    An idea for more efficient compile time formatting, once things are fleshed out here a bit more that is, is to  save the bracket offsets in the
//    format string for a substitution bracket, then parse the bracket for validity checkes, rinse and repeat until the end of the format string is
//    reached, then on Format(), splice the format string adding the formatted value to the offset and concatenate the results before returning the
//    fully formatted string -> could speed things up a whole lot by basically just leaving the formatting step to runtime and quite literally everything
//    else at compile time. If  there's no need to parse the format string for brackets, verify the specs in the bracket, and verify the specs are valid for
//    the arg type given during runtime, then this could easily be a ~2x improvement as cpu cycles spend most of the time in the verification process
//    and formatting process (finding the brackets and verifying a manual position are pretty negligible at ~6-8ns and ~5-10ns respectively) rather
//    equally (with formatting leading the cycle usage by ~5-10% over the verify calls).
/****************************************************************************************************************************************************/
/********************************************************************** Note: ***********************************************************************/
// As a second note, I may end up reworking some of this to mirror how libfmt has its formatters set up as I have literally no idea how to accomplish
// custom formatting at this point without something akin to "template<> struct formatter<Type>{};" . I might be able to get away with using an
// inheritance based approach but I think it would make more sense to follow libfmt/<format>'s lead here for the formatter approach. Storing a
// custom type's value should be relatively easy as I can just cast the value to a 'const void*' , store that value in the VType variant, and enumerate on
// 'CustomType' but without some sort of  function callback for both parsing the value for user specs and formatting the value based on those specs,
//  I would have no idea how to allow a user type to be formatted - nor do I know right now if it's possible to somehow store function pointers as
// callables when they aren't a member function (I assume it is as that would make sense for just being fptr, but I just have no idea how I would need
//  to approach that right now). I remember  back when I was working on the time-stamp formatting part of this project, that function pointers were
//  a bit slower, especially when you had to  index into a collection for the right offset to dispatch that function, but that was for already known functions;
//  I don't know if that approach would work in this case.
/***************************************************************************************************************************************************/
