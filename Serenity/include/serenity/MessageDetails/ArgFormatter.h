#pragma once

#include <serenity/MessageDetails/ArgContainer.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

// Given that this class is in charge of parsing and formatting the tokens found,
// it'd be more apt to call this whole thing "ArgFormatter" instead; especially
// since I plan to add elements of ArgContainer in here for type checking when
// parsing the message (ArgContainer will have some significant changes to reflect
// that idea).

namespace serenity::arg_formatter {

	enum class TokenType
	{
		Empty      = 0,
		FillAlign  = 1 << 0,
		Sign       = 1 << 1,
		Alternate  = 1 << 2,
		ZeroPad    = 1 << 3,
		Locale     = 1 << 4,
		Width      = 1 << 5,
		Precision  = 1 << 6,
		Type       = 1 << 7,
		Custom     = 1 << 8,
		Positional = 1 << 9,
	};
	constexpr TokenType operator|(TokenType lhs, TokenType rhs) {
		using uType = std::underlying_type_t<TokenType>;
		return static_cast<TokenType>(static_cast<uType>(lhs) | static_cast<uType>(rhs));
	}
	constexpr TokenType operator|=(TokenType& lhs, TokenType rhs) {
		return lhs = lhs | rhs;
	}

	constexpr TokenType operator&(TokenType& lhs, TokenType rhs) {
		using uType = std::underlying_type_t<TokenType>;
		return static_cast<TokenType>(static_cast<uType>(lhs) & static_cast<uType>(rhs));
	}

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
			nestedPrecArgPos = nestedWidthArgPos = zeroPadAmount = static_cast<size_t>(0);
			align                                                = Alignment::Empty;
			typeSpec                                             = '\0';
			preAltForm                                           = "\0";
			signType                                             = Sign::Empty;
			hasAltForm                                           = false;
		}

		size_t argPosition { 0 };
		size_t alignmentPadding { 0 };
		int precision { 0 };
		size_t nestedWidthArgPos { 0 };
		size_t nestedPrecArgPos { 0 };
		size_t zeroPadAmount { 0 };
		Alignment align { Alignment::Empty };
		char fillCharacter { '\0' };
		char typeSpec { '\0' };
		const char* preAltForm { "\0" };
		Sign signType { Sign::Empty };
		bool hasAltForm { false };
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
		// Given bench results, the back inserter works much better for smaller values (size <= ~10) but the string ref works better for much larger values.
		// Therefore, as annoying as it will be, I'll keep both versions here for open options. For much larger values, the string& is ~2x faster
		// than both the standard's format function and serenity's version using the back_insert_iterator. In all other cases, serenity's formatting
		// function is within 10-15% of the standard's version when compiled under utf-8 flag (depending on size, at a container of size >= ~35, it flips
		// in favor of serenity's version); when not compiled with this flag, serenity's version is ~1100% faster, which is absolutely fantastically
		// mind-blowing.Serenity's results are consistent and is statistically the same no matter if the utf-8 compile flag is present or not.
		// I really wish to know why there is such a drop in the standard's version w/o that compiler flag...
		template<typename... Args> void se_format_to(std::string& container, std::string_view sv, Args&&... args);

		template<typename T, typename... Args> void se_format_to(std::back_insert_iterator<T>&& Iter, std::string_view sv, Args&&... args);

	  private:
		void Parse(std::string& container, std::string_view sv);
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
		bool HandleIfEndOrWhiteSpace(std::string& container, std::string_view sv, size_t& currentPosition, const size_t& bracketSize);

		template<typename T>
		bool HandleIfEndOrWhiteSpace(std::back_insert_iterator<T>&& Iter, std::string_view sv, size_t& currentPosition, const size_t& bracketSize);

		bool IsFlagSet(TokenType checkValue);

		void FormatFillAlignToken(std::string& container);        // To Be Implemented
		void FormatSignToken(std::string& container);             // To Be Implemented
		void FormatAlternateToken(std::string& container);        // To Be Implemented
		void FormatZeroPadToken(std::string& container);          // To Be Implemented
		void FormatLocaleToken(std::string& container);           // To Be Implemented
		void FormatWidthToken(std::string& container);            // To Be Implemented
		void FormatPrecisionToken(std::string& container);        // To Be Implemented
		void FormatTypeToken(std::string& container);             // To Be Implemented
		void FormatCharAggregateToken(std::string& container);    // To Be Implemented
		void FormatCustomToken(std::string& container);           // To Be Implemented
		void FormatPositionalToken(std::string& container);       // To Be Implemented
		void FormatTokens(std::string& container);                // To Be Implemented
		void SimpleFormat(std::string& container);

		template<typename T> void FormatTokens(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatFillAlignToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatSignToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatAlternateToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatZeroPadToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatLocaleToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatWidthToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatPrecisionToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatTypeToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatCharAggregateToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatCustomToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void FormatPositionalToken(std::back_insert_iterator<T>&& Iter);
		template<typename T> void SimpleFormat(std::back_insert_iterator<T>&& Iter);

		template<typename... Args> constexpr void CaptureArgs(Args&&... args);

	  private:
		IndexMode m_indexMode { IndexMode::automatic };
		TokenType m_tokenType { TokenType::Empty };
		SpecFormatting specValues {};
		ParseResult result {};
		BracketSearchResults bracketResults {};
		int argCounter { 0 };
		serenity::experimental::msg_details::ArgContainer argStorage {};
		std::string temp;
		std::vector<char> buff {};
	};
#include <serenity/MessageDetails/ArgFormatterImpl.h>

}    // namespace serenity::arg_formatter
