#pragma once

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
			typeSpec                             = '\0';
			preAltForm                           = "\0";
			signType                             = Sign::Empty;
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
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(24)
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

	  private:
		template<typename T> void Parse(std::back_insert_iterator<T>&& Iter, std::string_view sv);

		void FindBrackets(std::string_view& sv);
		void FindNestedBrackets(std::string_view sv, int& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start);

		void VerifyArgumentBracket(std::string_view& sv, size_t& start, const size_t& bracketSize);
		void VerifyFillAlignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifySignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyAltField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyWidthField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, bool isDigit);
		void VerifyPrecisionField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyLocaleField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyTypeSpec(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize, const char& spec);
		void VerifyEscapedBracket(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);
		void VerifyNestedBracket(std::string_view sv, size_t& currentPosition, const size_t& bracketSize, NestedFieldType type);
		void HandlePotentialTypeField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSize);

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

		std::string_view FormatRawValueToStr(int& precision);
		std::string_view AppendByPrecision(std::string_view val, int precision);
		template<typename T> std::string_view FormatFloatTypeArg(T&& value, int precision);
		std::string_view FormatIntTypeArg(int&& value);

	  private:
		int argCounter { 0 };
		IndexMode m_indexMode { IndexMode::automatic };

		ParseResult result {};
		BracketSearchResults bracketResults {};
		SpecFormatting specValues {};
		serenity::experimental::msg_details::ArgContainer argStorage {};

		std::vector<char> buff {};
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};
		std::to_chars_result charsResult;
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>

}    // namespace serenity::arg_formatter
