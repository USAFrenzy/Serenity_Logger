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
#include <chrono>
#include <cstring>
#include <locale>
#include <stdexcept>

using namespace serenity::msg_details;
namespace serenity {

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
		invalid_ctime_spec,
		missing_ctime_spec,
		invalid_codepoint,
	};

	struct error_handler
	{
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

		static constexpr std::array<const char*, 20> format_error_messages = {
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
			"Error In Format: Invalid Time Specifier For C-Time Type Argument.",
			"Error In Format: Missing C-Time Specifier After '%'.",
			"Error In Decoding Character Set: Illegal Code Point Present In Code Unit",
		};
		[[noreturn]] constexpr void ReportError(ErrorType err);
	};
}    // namespace serenity

namespace serenity::globals {
	static auto& TimeZoneInstance() {
		static const auto& timeZoneData { std::chrono::get_tzdb() };
		return timeZoneData;
	}
	static auto TimeZone() {
		static auto timeZone { TimeZoneInstance().current_zone() };
		return timeZone;
	}
	static auto& TZInfo() {
		static auto data { TimeZone()->get_info(std::chrono::system_clock::now()) };
		return data;
	}
	static auto& UtcOffset() {
		return TZInfo().offset;
	}
}    // namespace serenity::globals

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

	enum class LocaleFormat
	{
		standard = 0,
		localized,
	};

	struct TimeSpecs
	{
		constexpr void Reset();
		std::array<LocaleFormat, 25> timeSpecFormat {};
		std::array<unsigned char, 25> timeSpecContainer {};
		std::vector<unsigned char> localizationBuff {};
		int timeSpecCounter { 0 };
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

	/**********************************************************************************************************************************************************
	    Compatible class that provides some of the same functionality that mirrors <format> and libfmt for basic formatting needs for pre  C++20 and MSVC's
	    pre-backported fixes (which required C ++23) for some build versions of Visual Studio as well as for performance needs. Everything in this class either
	    matches (in the case of simple double substitution) or greatly exceeds the performance of MSVC's implementation -  with the caveat no utf-8 support and
	    no type-erasure as of right now. I believe libfmt is faster than this basic implementation  (and unarguably way more comprehensive as well) but I have
	yet to bench timings against it.
	***********************************************************************************************************************************************************/
	/*************************************************************************  NOTE *************************************************************************/
	/************************************************** Building this project on Ubuntu emitted the following **************************************************
	 * The class defaulted functions are not constexpr due to being default
	 *  Need to explicitly add headers for std::memset, std::strlen, std::memcpy,
	 *  Unsequenced modification and access to 'start' in ArgFormatterImpl.h: 383:84
	 *  enumeration values 'CTimeType' and 'CustomType' not handled in switch ArgFormatterImple.h: 966:10
	 *  There's probably more errors to be honest, build step automatically stopped due to the number of errors from the above
	 *  -- Kind of surprised MSVC didn't emit these warnings with '/Wall' and '/WX' ...
	 **********************************************************************************************************************************************************/
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
		constexpr bool FindBrackets(std::string_view sv);
		constexpr void Parse(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void VerifyTimeSpec(std::string_view sv, size_t& position);
		constexpr void ParseTimeField(std::string_view sv, size_t& currentPosition);
		constexpr bool VerifyPositionalField(std::string_view sv, size_t& start, unsigned char& positionValue);
		constexpr void VerifyFillAlignField(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void VerifyFillAlignTimeField(std::string_view sv, size_t& currentPosition);
		constexpr void VerifyAltField(std::string_view sv, const SpecType& argType);
		constexpr void VerifyWidthField(std::string_view sv, size_t& currentPosition);
		constexpr void VerifyPrecisionField(std::string_view sv, size_t& currentPosition, const SpecType& argType);
		constexpr void VerifyTimePrecisionField(std::string_view sv, size_t& currentPosition);
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
		constexpr void FormatArgument(const int& precision, const int& totalWidth, const SpecType& type);
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
		/********************************************************** Time Formatting Related Functions *********************************************************/
		template<typename T>
		requires std::is_integral_v<std::remove_cvref_t<T>>
		constexpr void TwoDigitToBuff(T val);
		template<typename T> constexpr void FormatTimeField(T&& container);
		template<typename T> constexpr void FormatTimeField(T&& container, const std::locale& loc);
		constexpr void FormatCTime(const std::tm& cTimeStruct, const int& precision, int startPos = 0, int endPos = 0);
		void LocalizeCTime(const std::locale& loc, std::tm& timeStruct, const int& precision);
		template<typename T> constexpr void WriteSimpleCTime(T&& container);
		template<typename T> constexpr void Write24HourTime(T&& container, const int& hour, const int& min, const int& sec);
		template<typename T> constexpr void WriteShortMonth(T&& container, const int& mon);
		template<typename T> constexpr void WriteShortWeekday(T&& container, const int& wkday);
		template<typename T> constexpr void WriteTimeDate(T&& container, const std::tm& time);
		template<typename T> constexpr void WriteShortYear(T&& container, const int& year);
		template<typename T> constexpr void WritePaddedDay(T&& container, const int& day);
		template<typename T> constexpr void WriteSpacePaddedDay(T&& container, const int& day);
		template<typename T> constexpr void WriteShortIsoWeekYear(T&& container, const int& year, const int& yrday, const int& wkday);
		template<typename T> constexpr void WriteDayOfYear(T&& container, const int& day);
		template<typename T> constexpr void WritePaddedMonth(T&& container, const int& month);
		template<typename T> constexpr void WriteLiteral(T&& container, unsigned char lit);
		template<typename T> constexpr void WriteAMPM(T&& container, const int& hr);
		template<typename T> constexpr void Write12HourTime(T&& container, const int& hour, const int& min, const int& sec);
		template<typename T> constexpr void WriteWeekdayDec(T&& container, const int& wkday);
		template<typename T> constexpr void WriteMMDDYY(T&& container, const int& month, const int& day, const int& year);
		template<typename T> constexpr void WriteIsoWeekDec(T&& container, const int& wkday);
		template<typename T> constexpr void WriteUtcOffset(T&& container);
		template<typename T> constexpr void WriteLongWeekday(T&& container, const int& wkday);
		template<typename T> constexpr void WriteLongMonth(T&& container, const int& mon);
		template<typename T> constexpr void WriteYYYYMMDD(T&& container, const int& year, const int& mon, const int& day);
		template<typename T> constexpr void WriteLongIsoWeekYear(T&& container, const int& year, const int& yrday, const int& wkday);
		template<typename T> constexpr void WriteLongYear(T&& container, int year);
		template<typename T> constexpr void WriteTruncatedYear(T&& container, const int& year);
		template<typename T> constexpr void Write24Hour(T&& container, const int& hour);
		template<typename T> constexpr void Write12Hour(T&& container, const int& hour);
		template<typename T> constexpr void WriteMinute(T&& container, const int& min);
		template<typename T> constexpr void Write24HM(T&& container, const int& hour, const int& min);
		template<typename T> constexpr void WriteSecond(T&& container, const int& sec);
		template<typename T> constexpr void WriteTime(T&& container, const int& hour, const int& min, const int& sec);
		template<typename T> constexpr void WriteTZName(T&& container);
		template<typename T> constexpr void WriteWeek(T&& container, const int& yrday, const int& wkday);
		template<typename T> constexpr void WriteIsoWeek(T&& container, const int& yrday, const int& wkday);
		template<typename T> constexpr void WriteIsoWeekNumber(T&& container, const int& year, const int& yrday, const int& wkday);

		// the distinct difference from these functions vs the 'Write' variants is that they should also handle localization & precision
		// Right now, they are just one-for-one with one-another, minus the actual container writing portion
		void FormatSubseconds(int precision);
		void FormatUtcOffset();
		void FormatTZName();
		constexpr void Format24HourTime(int hour, int min, int sec, int precision = 0);
		constexpr void FormatShortWeekday(int wkday);
		constexpr void FormatShortMonth(int mon);
		constexpr void FormatTimeDate(const std::tm& time);
		constexpr void FormatShortYear(int year);
		constexpr void FormatSpacePaddedDay(int day);
		constexpr void FormatShortIsoWeekYear(int year, int yrday, int wkday);
		constexpr void FormatDayOfYear(int day);
		constexpr void FormatLiteral(unsigned char lit);
		constexpr void FormatAMPM(int hr);
		constexpr void Format12HourTime(int hour, int min, int sec, int precision = 0);
		constexpr void FormatWeekdayDec(int wkday);
		constexpr void FormatMMDDYY(int month, int day, int year);
		constexpr void FormatIsoWeekDec(int wkday);
		constexpr void FormatLongWeekday(int wkday);
		constexpr void FormatLongMonth(int mon);
		constexpr void FormatYYYYMMDD(int year, int mon, int day);
		constexpr void FormatLongIsoWeekYear(int year, int yrday, int wkday);
		constexpr void FormatLongYear(int year);
		constexpr void FormatTruncatedYear(int year);
		constexpr void Format24HM(int hour, int min);
		constexpr void FormatIsoWeekNumber(int year, int yrday, int wkday);

		//  NOTE: Due to the usage of the numpunct functions, which are not constexpr, these functions can't really be specified as constexpr
		void LocalizeBool(const std::locale& loc);
		void FormatIntegralGrouping(const std::locale& loc, size_t end);
		void LocalizeArgument(const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		void LocalizeIntegral(const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		void LocalizeFloatingPoint(const std::locale& loc, const int& precision, const int& totalWidth, const SpecType& type);
		/******************************************************** Container Writing Related Functions *********************************************************/
		constexpr void BufferToUpper(char* begin, const char* end);
		constexpr void FillBuffWithChar(const int& totalWidth);
		constexpr void SetIntegralFormat(int& base, bool& isUpper);
		constexpr void SetFloatingFormat(std::chars_format& format, int& precision, bool& isUpper);
		constexpr void WriteChar(const char& value);
		constexpr void WriteBool(const bool& value);
		template<typename T> constexpr void WriteFormattedString(T&& container, const SpecType& type, const int& precisionFormatted);
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
		template<typename T> constexpr void WriteBufferToContainer(T&& container);

	  private:
		int argCounter;
		IndexMode m_indexMode;
		BracketSearchResults bracketResults;
		SpecFormatting specValues;
		ArgContainer argStorage;
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer;
		size_t valueSize;
		std::vector<char> fillBuffer;
		serenity::error_handler errHandle;
		TimeSpecs timeSpec {};
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

	// Now that the runtime errors are organized in a neater fashion, would really love to figure out how libfmt does compile-time checking.
	// A lot of what is being used to verify things are all runtime-access stuff so I'm assuming achieving this won't be easy at all =/
	constexpr void serenity::error_handler::ReportError(ErrorType err) {
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
				case invalid_ctime_spec: throw format_error(format_error_messages[ 17 ]); break;
				case missing_ctime_spec: throw format_error(format_error_messages[ 18 ]); break;
				case invalid_codepoint: throw format_error(format_error_messages[ 19 ]); break;
				default: throw format_error(format_error_messages[ 0 ]); break;
			}
	}

}    // namespace serenity
