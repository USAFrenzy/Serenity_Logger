#pragma once
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

#include <serenity/MessageDetails/ArgContainer.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace serenity::arg_formatter {

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

	enum class NestedFieldType
	{
		Prec,
		Width
	};

	// Will shrink the size once I have everything accounted for
	struct SpecFormatting
	{
		void ResetSpecs() {
			argPosition = alignmentPadding = precision = 0;
			nestedPrecArgPos = nestedWidthArgPos = static_cast<size_t>(0);
			align                                = Alignment::Empty;
			typeSpec = fillCharacter = '\0';
			preAltForm               = "\0";
			signType                 = Sign::Empty;
			localize = hasAlt = false;
		}

		size_t argPosition { 0 };
		size_t alignmentPadding { 0 };
		int precision { 0 };
		size_t nestedWidthArgPos { 0 };
		size_t nestedPrecArgPos { 0 };
		Alignment align { Alignment::Empty };
		char fillCharacter { '\0' };
		char typeSpec { '\0' };
		std::string_view preAltForm { "\0" };
		Sign signType { Sign::Empty };
		bool localize { false };
		bool hasAlt { false };
	};

	struct ParseResult
	{
		std::string_view preTokenStr { "" };
		std::string_view remainder { "" };
		std::string tokenResult;
		void Reset() {
			tokenResult.clear();
		}
	};

	struct BracketSearchResults
	{
		void Reset() {
			isValid  = false;
			beginPos = endPos = 0;
		}
		bool isValid { false };
		size_t beginPos { 0 };
		size_t endPos { 0 };
	};

#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(64)
#endif    // !SERENITY_ARG_BUFFER_SIZE

	// Compatible class that provides some of the same functionality that mirrors <format> for pre C++23
	// (In the hopes of eliminating the usage of /std:latest compiler flag to use <forrmat> functionality)
	class ArgFormatter
	{
		using ArgContainer = serenity::experimental::msg_details::ArgContainer;

	  public:
		ArgFormatter()                               = default;
		ArgFormatter(const ArgFormatter&)            = delete;
		ArgFormatter& operator=(const ArgFormatter&) = delete;
		~ArgFormatter()                              = default;

		template<typename... Args> std::string se_format(std::string_view sv, Args&&... args);
		template<typename T, typename... Args> void se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);
		void SetLocaleForUse(const std::locale& locale);

	  private:
		template<typename T> void Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv);

		void FindBrackets(std::string_view& sv);
		void FindNestedBrackets(std::string_view sv, int& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start);

		void VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize);
		void VerifyFillAlignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec);
		void VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type);
		void HandlePotentialTypeField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);

		bool IsValidStringSpec(const char& spec);
		bool IsValidIntSpec(const char& spec);
		bool IsValidBoolSpec(const char& spec);
		bool IsValidFloatingPointSpec(const char& spec);
		bool IsValidCharSpec(const char& spec);
		bool VerifySpec(experimental::msg_details::SpecType type, const char& spec);

		template<typename T>
		bool HandleIfEndOrWhiteSpace(std::back_insert_iterator<T>&& Iter, std::string_view sv, size_t& currentPosition, const size_t& bracketSize);

		template<typename T> void FormatTokens(std::back_insert_iterator<T>&& Iter);
		template<typename... Args> constexpr void CaptureArgs(Args&&... args);

		void FormatRawValueToStr(int& precision, experimental::msg_details::SpecType type);
		void AppendByPrecision(std::string_view val, int precision);
		template<typename T> void AppendDirectly(std::back_insert_iterator<T>&& Iter, experimental::msg_details::SpecType type);
		template<typename T> void FormatFloatTypeArg(T&& value, int precision);
		template<typename T> void FormatIntTypeArg(T&& value);
		void LocalizeArgument(int precision, experimental::msg_details::SpecType type);
		void LocalizeIntegral(int precision, experimental::msg_details::SpecType type);
		void LocalizeFloatingPoint(int precision, experimental::msg_details::SpecType type);
		void LocalizeBool();
		void FormatIntegralGrouping(std::string& section, char separator);

	  private:
		int argCounter { 0 };
		IndexMode m_indexMode { IndexMode::automatic };

		ParseResult result {};
		BracketSearchResults bracketResults {};
		SpecFormatting specValues {};
		serenity::experimental::msg_details::ArgContainer argStorage {};

		std::string rawValueTemp;
		std::vector<char> buff {};
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};
		std::to_chars_result charsResult;
		// The idea here is to have a constructor that takes in a locale and sets these by default unless SetLocale() is called, in which case, these get
		// updated.When I introduce this class into the loggers, their respective SetLocale() arguments will call this one as well. This way, any class (the
		// logging ones are at the forefront of this thought) can instantiate an ArgFormatter class with a locale and circumvent the <format> performance hit;
		// if none are supplied, the default constructor won't poll for a locale everytime like <format> does when no locale is supplied, it will use the
		// default constructed locale for the system if none is provided and this default locale will be the one that is referenced when 'L' is present).
		std::unique_ptr<std::locale> loc { std::make_unique<std::locale>(std::locale("")) };
		char separator { std::use_facet<std::numpunct<char>>(*loc.get()).thousands_sep() };
		char decimal { std::use_facet<std::numpunct<char>>(*loc.get()).decimal_point() };
		std::string falseStr { std::use_facet<std::numpunct<char>>(*loc.get()).falsename() };
		std::string trueStr { std::use_facet<std::numpunct<char>>(*loc.get()).truename() };
		std::string groupings { std::use_facet<std::numpunct<char>>(*loc.get()).grouping() };
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>

}    // namespace serenity::arg_formatter
