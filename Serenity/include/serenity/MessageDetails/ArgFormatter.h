#pragma once
/******************* Copyright from <format> header extending to libfmt *******************/
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

#include <serenity/MessageDetails/ArgContainer.h>

#include <charconv>
#include <locale>
#include <stdexcept>

using namespace serenity::msg_details;
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

	static constexpr bool IsDigit(const char& ch) {
		return ((ch >= '0') && (ch <= '9'));
	}

	static constexpr bool IsAlpha(const char& ch) {
		return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
	}

}    // namespace serenity

namespace serenity::arg_formatter {

	constexpr size_t SERENITY_ARG_BUFFER_SIZE { 66 };
	// defualt locale used for when no locale is provided, yet a locale flag is present when formatting
	static std::locale default_locale { std::locale("") };

	enum class Alignment : char
	{
		Empty = 0,
		AlignLeft,
		AlignRight,
		AlignCenter
	};

	enum class Sign : char
	{
		Empty = 0,
		Plus,
		Minus,
		Space
	};

	enum class IndexMode : char
	{
		automatic,
		manual
	};

	struct SpecFormatting
	{
		constexpr SpecFormatting()                                 = default;
		constexpr SpecFormatting(const SpecFormatting&)            = default;
		constexpr SpecFormatting& operator=(const SpecFormatting&) = default;
		constexpr SpecFormatting(SpecFormatting&&)                 = default;
		constexpr SpecFormatting& operator=(SpecFormatting&&)      = default;
		constexpr ~SpecFormatting()                                = default;

		constexpr void ResetSpecs();
		unsigned char argPosition { 0 };
		int alignmentPadding { 0 };
		int precision { 0 };
		unsigned char nestedWidthArgPos { 0 };
		unsigned char nestedPrecArgPos { 0 };
		Alignment align { Alignment::Empty };
		unsigned char fillCharacter { '\0' };
		unsigned char typeSpec { '\0' };
		std::string_view preAltForm { "" };
		Sign signType { Sign::Empty };
		bool localize { false };
		bool hasAlt { false };
		bool hasClosingBrace { false };
	};

	struct BracketSearchResults
	{
		constexpr BracketSearchResults()                                       = default;
		constexpr BracketSearchResults(const BracketSearchResults&)            = default;
		constexpr BracketSearchResults& operator=(const BracketSearchResults&) = default;
		constexpr BracketSearchResults(BracketSearchResults&&)                 = default;
		constexpr BracketSearchResults& operator=(BracketSearchResults&&)      = default;
		constexpr ~BracketSearchResults()                                      = default;

		constexpr void Reset();
		size_t beginPos { 0 };
		size_t endPos { 0 };
	};

	template<typename... Args> static constexpr void ReserveCapacityImpl(size_t& totalSize, Args&&... args) {
		size_t unreservedSize {};
		(
		[](size_t& totalSize, auto&& arg, size_t& unreserved) {
			using base_type = type<decltype(arg)>;
			if constexpr( std::is_same_v<base_type, std::string> || std::is_same_v<base_type, std::string_view> ) {
					totalSize += arg.size();
			} else if constexpr( std::is_same_v<base_type, const char*> ) {
					totalSize += std::strlen(arg) + 1;
			} else {
					// since this block is called for all other types, reserve double as there's no way to
					// know the formatted representation (ex: could be binary, scientific notation, etc...)
					auto argSize { sizeof(arg) * 2 };
					argSize + totalSize > sizeof(std::string) ? totalSize += argSize : unreserved += argSize;
				}
		}(totalSize, args, unreservedSize),
		...);
		// similar to the internal check, but now estimating whether or not the unreserved bytes can be stored via SBO
		totalSize + unreservedSize > sizeof(std::string) ? totalSize += unreservedSize : 0;
	}

	template<typename... Args> static constexpr size_t ReserveCapacity(Args&&... args) {
		size_t totalSize {};
		ReserveCapacityImpl(totalSize, std::forward<Args>(args)...);
		return std::forward<size_t>(totalSize);
	}

	/********************************************************************************************************************************************************
	    Compatible class that provides some of the same functionality that mirrors <format> and libfmt for basic formatting needs for pre  C++20 and MSVC's
	    pre-backported fixes (which required C ++23) for some build versions of Visual Studio as well as for performance needs. Everything in this class either
	    matches (in the case of simple double substitution) or greatly exceeds the performance of MSVC's implementation -  with the caveat no utf-8 support and
	    no type-erasure as of right now. I believe libfmt is faster than this basic implementation  (and unarguably way more comprehensive as well) but I have yet
	    to bench timings against it.
	********************************************************************************************************************************************************/
	class ArgFormatter
	{
	  public:
		constexpr ArgFormatter();
		constexpr ArgFormatter(const ArgFormatter&)            = delete;
		constexpr ArgFormatter& operator=(const ArgFormatter&) = delete;
		constexpr ArgFormatter(ArgFormatter&&)                 = default;
		constexpr ArgFormatter& operator=(ArgFormatter&&)      = default;
		constexpr ~ArgFormatter()                              = default;

		// clang-format off
		template<typename T, typename... Args> constexpr void se_format_to(std::back_insert_iterator<T>&& Iter, const std::locale& loc, std::string_view sv, Args&&... args);
		template<typename T, typename... Args> constexpr void se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);
		template<typename... Args> [[nodiscard]]  std::string se_format(const std::locale& locale, std::string_view sv, Args&&... args);
		template<typename... Args> [[nodiscard]]  std::string se_format(std::string_view sv, Args&&... args);
		// clang-format on

	  private:
		template<typename Iter, typename... Args> constexpr auto CaptureArgs(Iter&& iter, Args&&... args) -> decltype(iter);
		// At the moment ParseFormatString() and Format() are coupled together where ParseFormatString calls Format, hence the need
		// right now to have a version of ParseFormatString() that takes a locale object to forward to the locale overloaded Format()
		template<typename T> constexpr void ParseFormatString(std::back_insert_iterator<T>&& Iter, std::string_view sv);
		template<typename T> constexpr void ParseFormatString(std::back_insert_iterator<T>&& Iter, const std::locale& loc, std::string_view sv);
		template<typename T> constexpr void Format(T&& container, const SpecType& argType);
		template<typename T> constexpr void Format(T&& container, const std::locale& loc, const SpecType& argType);
		/******************************************************* Parsing/Verification Related Functions *******************************************************/
		[[noreturn]] constexpr void ReportError(ErrorType err);
		constexpr bool FindBrackets(std::string_view sv);
		constexpr void Parse(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr bool VerifyPositionalField(std::string_view sv, size_t& start, unsigned char& positionValue);
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
		template<typename T> constexpr void FormatStringType(T&& container, std::string_view val, const int& precision);
		template<typename T> constexpr void FormatArgument(T&& container, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T> constexpr void FormatAlignment(T&& container, const int& totalWidth);
		template<typename T> constexpr void FormatAlignment(T&& container, std::string_view val, const int& width, int prec);
		constexpr void FormatBoolType(bool& value);
		constexpr void FormatCharType(char& value);
		template<typename T>
		requires std::is_integral_v<std::remove_cvref_t<T>>
		constexpr void FormatIntegerType(T&& value);
		template<typename T>
		requires std::is_pointer_v<std::remove_cvref_t<T>>
		constexpr void FormatPointerType(T&& value, const SpecType& type);
		template<typename T>
		requires std::is_floating_point_v<std::remove_cvref_t<T>>
		constexpr void FormatFloatType(T&& value, int precision);
		//  NOTE: Due to the usage of the numpunct functions, which are not constexpr, these functions can't really be specified as constexpr
		template<typename T> void LocalizeBool(T&& container, const std::locale& loc);
		void FormatIntegralGrouping(const std::locale& loc, size_t end);
		template<typename T> void LocalizeArgument(T&& container, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T> void LocalizeIntegral(T&& container, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		template<typename T> void LocalizeFloatingPoint(T&& container, const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		/******************************************************** Container Writing Related Functions *********************************************************/
		constexpr void BufferToUpper(char* begin, const char* end);
		constexpr void FillBuffWithChar(const int& totalWidth);
		constexpr void SetIntegralFormat(int& base, bool& isUpper);
		constexpr void SetFloatingFormat(std::chars_format& format, int& precision, bool& isUpper);
		constexpr void WriteChar(const char& value);
		constexpr void WriteBool(const bool& value);
		template<typename T> constexpr void WriteString(T&& container, const SpecType& type, const int& precision, const int& totalWidth);
		template<typename T> constexpr void WriteSimpleValue(T&& container, const SpecType&);
		template<typename T> constexpr void WriteSimpleString(T&& container);
		template<typename T> constexpr void WriteSimpleCString(T&& container);
		template<typename T> constexpr void WriteSimpleStringView(T&& container);
		template<typename T> constexpr void WriteSimpleInt(T&& container);
		template<typename T> constexpr void WriteSimpleUInt(T&& container);
		template<typename T> constexpr void WriteSimpleLongLong(T&& container);
		template<typename T> constexpr void WriteSimpleULongLong(T&& container);
		template<typename T> constexpr void WriteSimpleBool(T&& container);
		template<typename T> constexpr void WriteSimpleFloat(T&& container);
		template<typename T> constexpr void WriteSimpleDouble(T&& container);
		template<typename T> constexpr void WriteSimpleLongDouble(T&& container);
		template<typename T> constexpr void WriteSimpleConstVoidPtr(T&& container);
		template<typename T> constexpr void WriteSimpleVoidPtr(T&& container);
		// clang-format off
		template<typename T> constexpr void WriteAlignedLeft(T &&container, const int& totalWidth);
		template<typename T>constexpr void WriteAlignedLeft(T &&container, std::string_view val, const int& precision, const int& totalWidth);
		template<typename T> constexpr void WriteAlignedRight(T &&container, const int& totalWidth, const size_t& fillAmount);
		template<typename T>constexpr void WriteAlignedRight(T &&container, std::string_view val, const int& precision, const int& totalWidth, const size_t& fillAmount);
		template<typename T> constexpr void WriteAlignedCenter(T &&container, const int& totalWidth, const size_t& fillAmount);
		template<typename T>constexpr void WriteAlignedCenter(T &&container, std::string_view val, const int& precision, const int& totalWidth, const size_t& fillAmount);
		template<typename T>constexpr void WriteSimplePadding(T &&container, const size_t& fillAmount);

		template<typename T> constexpr void WriteNonAligned(T &&container);
		template<typename T> constexpr void WriteNonAligned(T &&container, std::string_view val, const int& precision);
		template<typename T> requires std::is_arithmetic_v<std::remove_cvref_t<T>>
		constexpr void WriteSign(T&& value, int& pos);
		// clang-format on

	  private:
		int argCounter;
		IndexMode m_indexMode;
		BracketSearchResults bracketResults;
		SpecFormatting specValues;
		ArgContainer argStorage;
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer;
		size_t valueSize;
		std::vector<char> fillBuffer;
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>
}    // namespace serenity::arg_formatter

// These are made static so that when including this file, one can either use and modify the above class or just call the
// formatting functions directly, like the logger-side of this project where the VFORMAT_TO macros are defined
namespace serenity {
	namespace globals {
		static std::unique_ptr<arg_formatter::ArgFormatter> staticFormatter { std::make_unique<arg_formatter::ArgFormatter>() };
	}    // namespace globals
	template<typename T, typename... Args> static constexpr void format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args) {
		globals::staticFormatter->se_format_to(std::forward<FwdMoveIter<T>>(Iter), sv, std::forward<Args>(args)...);
	}
	template<typename T, typename... Args>
	static constexpr void format_to(std::back_insert_iterator<T>&& Iter, const std::locale& locale, std::string_view sv, Args&&... args) {
		globals::staticFormatter->se_format_to(std::forward<FwdMoveIter<T>>(Iter), locale, sv, std::forward<Args>(args)...);
	}
	template<typename... Args> [[nodiscard]] static std::string format(std::string_view sv, Args&&... args) {
		std::string tmp;
		tmp.reserve(serenity::arg_formatter::ReserveCapacity(std::forward<Args>(args)...));
		globals::staticFormatter->se_format_to(std::back_inserter(tmp), sv, std::forward<Args>(args)...);
		return tmp;
	}
	template<typename... Args> [[nodiscard]] static std::string format(const std::locale& locale, std::string_view sv, Args&&... args) {
		std::string tmp;
		tmp.reserve(serenity::arg_formatter::ReserveCapacity(std::forward<Args>(args)...));
		globals::staticFormatter->se_format_to(std::back_inserter(tmp), locale, sv, std::forward<Args>(args)...);
		return tmp;
	}
}    // namespace serenity

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

/********************************************************************** Note: ***********************************************************************/
//    An idea for more efficient compile time formatting, once things are fleshed out here a bit more that is, is to  save the bracket offsets in the
//    format string for a substitution bracket, then parse the bracket for validity checkes, rinse and repeat until the end of the format string is
//    reached, then on Format(), splice the format string adding the formatted value to the offset and concatenate the results before returning the
//    fully formatted string -> could speed things up a whole lot by basically just leaving the formatting step to runtime and quite literally everything
//    else at compile time. If  there's no need to parse the format string for brackets, verify the specs in the bracket, and verify the specs are valid for
//    the arg type given during runtime, then this could easily be a ~2x improvement as cpu cycles spend most of the time in the verification process
//    and formatting process (finding the brackets and verifying a manual position are pretty negligible at ~6-8ns and ~5-10ns respectively) rather
//    equally (with formatting leading the cycle usage by ~5-10% over the verify calls).
//    EDIT: Reading a lot about SIMD instructions and I feel like there might be some niche cases where that could apply here, such as if there are
//                multiple arguments of the same type - could then format them in tandem with one another?
/****************************************************************************************************************************************************/
