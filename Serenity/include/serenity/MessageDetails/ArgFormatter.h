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
		char argPosition { 0 };
		Alignment align { Alignment::Empty };
		size_t alignmentPadding { 0 };
		char fillCharacter { '\0' };
		Sign signType { Sign::Empty };
		size_t nestedWidthArgPos { 0 };
		size_t nestedPrecArgPos { 0 };
		size_t zeroPadAmount { 0 };
		char typeSpec { '\0' };
		bool hasAltForm { false };
		const char* preAltForm { "\0" };
	};

	struct ParseResult
	{
		char defaultValue { '\0' };
		std::string_view preTokenStr { &defaultValue };
		std::string_view remainder { &defaultValue };
		std::string tokenResult;
		void Reset() {
			preTokenStr = remainder = &defaultValue;
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

	class ArgFormatter
	{
		using ArgContainer = serenity::experimental::msg_details::ArgContainer;

	      public:
		ArgFormatter()                               = default;
		ArgFormatter(const ArgFormatter&)            = delete;
		ArgFormatter& operator=(const ArgFormatter&) = delete;
		~ArgFormatter()                              = default;

		ParseResult& Parse(std::string_view sv);
		void FindBrackets(std::string_view& sv);
		void FindNestedBrackets(std::string_view sv, int& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex, size_t& start);
		size_t FindDigitEnd(std::string_view, size_t start);
		int NoCheckIntFromChars(std::string_view sv, const size_t& begin, const size_t& end);

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
		bool HandleIfEndOrWhiteSpace(std::string_view sv, size_t& currentPosition, const size_t& bracketSize);

		bool IsFlagSet(TokenType checkValue);

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
		void FormatTokens();                // To Be Implemented
		void SimpleFormat();

		template<typename... Args> constexpr void CaptureArgs(std::string_view fmtString, Args&&... args) {
			argStorage.CaptureArgs(fmtString, std::forward<Args>(args)...);
		}

	      private:
		IndexMode m_indexMode { IndexMode::automatic };
		TokenType m_tokenType { TokenType::Empty };
		SpecFormatting specValues {};
		ParseResult result {};
		BracketSearchResults bracketResults {};
		int argCounter { 0 };
		serenity::experimental::msg_details::ArgContainer argStorage {};
		std::string temp;
		std::array<char, 250> buffer {};
	};

}    // namespace serenity::arg_formatter
