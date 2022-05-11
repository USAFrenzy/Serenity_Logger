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
			typeSpec = alignmentPadding = '\0';
			preAltForm                  = "\0";
			signType                    = Sign::Empty;
			hasAltForm                  = false;
		}

		char argPosition { 0 };
		int alignmentPadding { 0 };
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

	class ArgFormatter
	{
		using ArgContainer = serenity::experimental::msg_details::ArgContainer;

	  public:
		ArgFormatter()                               = default;
		ArgFormatter(const ArgFormatter&)            = delete;
		ArgFormatter& operator=(const ArgFormatter&) = delete;
		~ArgFormatter()                              = default;

		template<typename... Args> void se_format_to(std::string& container, std::string_view sv, Args&&... args) {
			CaptureArgs(std::forward<Args>(args)...);
			Parse(container, sv);
		}

	  private:
		void Parse(std::string& container, std::string_view sv);
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

		template<typename... Args> constexpr void CaptureArgs(Args&&... args) {
			argStorage.CaptureArgs(std::forward<Args>(args)...);
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
		std::vector<char> buff {};
	};

}    // namespace serenity::arg_formatter
