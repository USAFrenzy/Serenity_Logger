#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace serenity::lazy_parser {

	enum class TokenType
	{
		Empty            = 0,
		FillAlign        = 1 << 0,
		AlignmentPadding = 1 << 1,
		Sign             = 1 << 2,
		Alternate        = 1 << 3,
		ZeroPad          = 1 << 4,
		Locale           = 1 << 5,
		Width            = 1 << 6,
		Precision        = 1 << 7,
		Type             = 1 << 8,
		CharAggregate    = 1 << 9,
		Custom           = 1 << 10,
		Positional       = 1 << 11,
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
		Width,
		Precision
	};

	struct SpecFormatting
	{
		Alignment align { Alignment::Empty };
		size_t alignmentPadding { 0 };
		char fillCharacter { '\0' };
		Sign signType { Sign::Empty };
		size_t nestedWidthArgPos { 0 };
		size_t nestedPrecArgPos { 0 };
	};

	struct ParseResult
	{
		void Reset() {
			preTokenStr = tokenResult = remainder = "";
		}
		std::string_view preTokenStr;
		std::string_view tokenResult;
		std::string_view remainder;
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

	class LazyParser
	{
	      public:
		LazyParser()                             = default;
		LazyParser(const LazyParser&)            = delete;
		LazyParser& operator=(const LazyParser&) = delete;
		~LazyParser()                            = default;

		ParseResult& Parse(std::string_view sv);
		void FindBrackets(std::string_view& sv);
		void FindNestedBrackets(std::string_view sv, int& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex);
		size_t FindDigitEnd(std::string_view);
		int TwoDigitFromChars(std::string_view sv, const size_t& begin, const size_t& end);

		bool VerifyFillAlignField(std::string_view& sv, size_t& currentPosition, const size_t& bracketSIze);
		bool HasSignField(std::string_view& sv, const size_t& bracketSIze);
		bool HasValidNestedField(std::string_view& sv, NestedFieldType type, size_t index);

		bool IsFlagSet(TokenType& tokenFlags, TokenType checkValue);

		void FormatFillAlignToken();        // To Be Implemented
		void FormatSignToken();             // To Be Implemented
		void FormatAlternateToken();        // To Be Implemented
		void FormatZeroPadToken();          // To Be Implemented
		void FormatLocaleToken();           // To Be Implemented
		void FormatWidthToken();            // To Be Implemented
		void FormatPrecisionToken();        // To Be Implemented
		void FormatTypeToken();             // To Be Implemented
		void FormatCharAggregateToken();    // To Be Implemented
		void FormatCustomToken();           // To Be Implemented
		void FormatPositionalToken();       // To Be Implemented

		void FormatTokens();    // To Be Implemented

	      private:
		IndexMode m_indexMode { IndexMode::automatic };
		TokenType m_tokenType { TokenType::Empty };
		SpecFormatting specValues {};
		ParseResult result {};
		BracketSearchResults bracketResults {};
		int argCounter { 0 };
	};

}    // namespace serenity::lazy_parser
