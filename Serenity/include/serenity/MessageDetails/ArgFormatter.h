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
	using ArgContainer = msg_details::ArgContainer;
}

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

	struct SpecFormatting
	{
		void ResetSpecs();
		size_t argPosition { 0 };
		int alignmentPadding { 0 };
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
		bool hasClosingBrace { false };
	};

	struct BracketSearchResults
	{
		void Reset();
		bool isValid { false };
		size_t beginPos { 0 };
		size_t endPos { 0 };
	};

	template<typename T> struct IteratorContainer: std::back_insert_iterator<T>
	{
		using std::back_insert_iterator<T>::container_type;
		IteratorContainer(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		const auto& Container() {
			return this->container;
		}
	};

#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(65)
#endif    // !SERENITY_ARG_BUFFER_SIZE

	// Compatible class that provides some of the same functionality that mirrors <format> for pre C++23
	// (In the hopes of eliminating the usage of /std:latest compiler flag to use <forrmat> functionality)
	class ArgFormatter
	{
	  public:
		ArgFormatter() = delete;
		ArgFormatter(const std::locale& loc);
		ArgFormatter(const ArgFormatter&) = delete;
		ArgFormatter& operator=(const ArgFormatter&) = delete;
		~ArgFormatter()                              = default;

		template<typename... Args> std::string se_format(std::string_view sv, Args&&... args);
		template<typename T, typename... Args> constexpr void se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);
		void SetLocaleForUse(const std::locale& locale);

	  private:
		template<typename T> constexpr void Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv);

		void FindBrackets(std::string_view sv, size_t svSize);
		void FindNestedBrackets(std::string_view sv, size_t& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start);

		void VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize, msg_details::SpecType &argType);
		void VerifyFillAlignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec, msg_details::SpecType& argType);
		void VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type, msg_details::SpecType& argType);
		void HandlePotentialTypeField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, msg_details::SpecType& argType);
		void VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);

		bool IsValidStringSpec(const char& spec);
		bool IsValidIntSpec(const char& spec);
		bool IsValidBoolSpec(const char& spec);
		bool IsValidFloatingPointSpec(const char& spec);
		bool IsValidCharSpec(const char& spec);
		bool VerifySpec(msg_details::SpecType type, const char& spec);
		bool IsSimpleSubstitution(msg_details::SpecType& argType, int& precision, int& width);

		template<typename T> void FormatTokens(std::back_insert_iterator<T>&& Iter, msg_details::SpecType& argType);
		template<typename... Args> constexpr void CaptureArgs(Args&&... args);
		void AppendByPrecision(std::string_view val, int precision);
		template<typename T> void FormatFloatTypeArg(T&& value, int precision);
		template<typename T> void FormatIntTypeArg(T&& value);
		void FormatRawValueToStr(int& precision, msg_details::SpecType type);
		void LocalizeArgument(int precision, msg_details::SpecType type);
		void LocalizeIntegral(int precision, msg_details::SpecType type);
		void LocalizeFloatingPoint(int precision, msg_details::SpecType type);
		void LocalizeBool();
		void FormatIntegralGrouping(std::string& section, char separator);
		template<typename T> void WriteSimpleValue(std::back_insert_iterator<T>&& Iter, msg_details::SpecType);

	  private:
		int argCounter { 0 };
		IndexMode m_indexMode { IndexMode::automatic };

		std::string_view remainder { "" };
		BracketSearchResults bracketResults {};
		SpecFormatting specValues {};
		ArgContainer argStorage {};

		std::string rawValueTemp;
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};
		std::to_chars_result charsResult;

		std::unique_ptr<std::locale> loc;
		char separator;
		char decimal;
		std::string falseStr;
		std::string trueStr;
		std::string groupings;
		std::string localeTemp;
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>

}    // namespace serenity::arg_formatter
