#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace serenity::lazy_parser::helper {
	// due to original Message_Formatter.h inclusion
#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(24)
#endif    // !SERENITY_ARG_BUFFER_SIZE

	class LazyParseHelper
	{
	      public:
		enum class bracket_type
		{
			open = 0,
			close
		};
		enum class partition_type
		{
			primary = 0,
			remainder
		};

		LazyParseHelper()                                  = default;
		LazyParseHelper(const LazyParseHelper&)            = delete;
		LazyParseHelper& operator=(const LazyParseHelper&) = delete;
		~LazyParseHelper()                                 = default;

		void SetBracketPosition(bracket_type bracket, size_t pos);
		void SetPartition(partition_type pType, std::string_view sv);
		std::array<char, SERENITY_ARG_BUFFER_SIZE>& ConversionResultBuffer();
		size_t BracketPosition(bracket_type bracket) const;
		std::string& PartitionString(partition_type pType);
		std::string& StringBuffer();
		void ClearBuffer();
		void ClearPartitions();
		size_t FindEndPos();
		void ResetBracketPositions();

	      private:
		std::string partitionUpToArg;
		std::string remainder;
		std::string temp;
		size_t openBracketPos { std::string_view::npos };
		size_t closeBracketPos { std::string_view::npos };
		std::array<char, SERENITY_ARG_BUFFER_SIZE> resultBuffer = {};
	};

}    // namespace serenity::lazy_parser::helper

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

	class BaseToken
	{
	      public:
		explicit BaseToken(int argIndex);
		explicit BaseToken(std::string_view sv, TokenType tType, int argIndex);
		BaseToken(const BaseToken&)            = delete;
		BaseToken& operator=(const BaseToken&) = delete;
		virtual ~BaseToken()                   = default;

		virtual void FormatToken()             = 0;

	      protected:
		TokenType m_tokenType;
		helper::LazyParseHelper parseHelper;
		std::string initialString;
		std::string formattedToken;
		int argToCapture;
	};

	class EmptyToken final: public BaseToken
	{
	      public:
		explicit EmptyToken(int argIndex);
		EmptyToken(const EmptyToken&)            = delete;
		EmptyToken& operator=(const EmptyToken&) = delete;
		~EmptyToken()                            = default;

		void FormatToken() override;
	};

	enum class Alignment : char
	{
		Empty = 0,
		AlignLeft,
		AlignRight,
		AlignCenter
	};

	class FillAlignToken final: public BaseToken
	{
	      public:
		FillAlignToken(std::string_view sv, TokenType tType, int argIndex);
		FillAlignToken(const FillAlignToken&)            = delete;
		FillAlignToken& operator=(const FillAlignToken&) = delete;
		~FillAlignToken()                                = default;

		void FormatToken() override;
	};

	enum class Sign
	{
		Empty = 0,
		Plus,
		Minus,
		Space
	};
	class SignToken final: public BaseToken
	{
	      public:
		SignToken(std::string_view sv, TokenType tType, int argIndex);
		SignToken(const SignToken&)            = delete;
		SignToken& operator=(const SignToken&) = delete;
		~SignToken()                           = default;

		void FormatToken() override;
	};

	class AlternateToken final: public BaseToken
	{
	      public:
		AlternateToken(std::string_view sv, TokenType tType, int argIndex);
		AlternateToken(const AlternateToken&)            = delete;
		AlternateToken& operator=(const AlternateToken&) = delete;
		~AlternateToken()                                = default;

		void FormatToken() override;
	};

	class ZeroPadToken final: public BaseToken
	{
	      public:
		ZeroPadToken(std::string_view sv, TokenType tType, int argIndex);
		ZeroPadToken(const ZeroPadToken&)            = delete;
		ZeroPadToken& operator=(const ZeroPadToken&) = delete;
		~ZeroPadToken()                              = default;

		void FormatToken() override;
	};

	class LocaleToken final: public BaseToken
	{
	      public:
		LocaleToken(std::string_view sv, TokenType tType, int argIndex);
		LocaleToken(const LocaleToken&)            = delete;
		LocaleToken& operator=(const LocaleToken&) = delete;
		~LocaleToken()                             = default;

		void FormatToken() override;
	};

	class WidthToken final: public BaseToken
	{
	      public:
		WidthToken(std::string_view sv, TokenType tType, int argIndex);
		WidthToken(const WidthToken&)            = delete;
		WidthToken& operator=(const WidthToken&) = delete;
		~WidthToken()                            = default;

		void FormatToken() override;
	};

	class PrecisionToken final: public BaseToken
	{
	      public:
		PrecisionToken(std::string_view sv, TokenType tType, int argIndex);
		PrecisionToken(const PrecisionToken&)            = delete;
		PrecisionToken& operator=(const PrecisionToken&) = delete;
		~PrecisionToken()                                = default;

		void FormatToken() override;
	};

	class TypeToken final: public BaseToken
	{
	      public:
		TypeToken(std::string_view sv, TokenType tType, int argIndex);
		TypeToken(const TypeToken&)            = delete;
		TypeToken& operator=(const TypeToken&) = delete;
		~TypeToken()                           = default;

		void FormatToken() override;
	};

	class CustomToken final: public BaseToken
	{
	      public:
		CustomToken(std::string_view sv, TokenType tType, int argIndex);
		CustomToken(const CustomToken&)            = delete;
		CustomToken& operator=(const CustomToken&) = delete;
		~CustomToken()                             = default;

		void FormatToken() override;
	};

	class CharAggregateToken final: public BaseToken
	{
	      public:
		explicit CharAggregateToken(std::string_view sv, TokenType tType);
		CharAggregateToken(const CharAggregateToken&)            = delete;
		CharAggregateToken& operator=(const CharAggregateToken&) = delete;
		~CharAggregateToken()                                    = default;

		void FormatToken() override;
	};

	static constexpr std::array<char, 10> charDigits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	enum class IndexMode
	{
		automatic,
		manual
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

	enum class NestedFieldType
	{
		Width,
		Precision
	};

	class LazyParser
	{
	      public:
		LazyParser()                             = default;
		LazyParser(const LazyParser&)            = delete;
		LazyParser& operator=(const LazyParser&) = delete;
		~LazyParser()                            = default;

		void Parse(std::string_view sv);
		bool FindBrackets(std::string_view sv);
		void FindNestedBrackets(std::string_view sv, size_t& currentPos);

		bool ParsePositionalField(std::string_view& sv, int& argIndex);
		size_t FindDigitEnd(std::string_view);
		int TwoDigitFromChars(std::string_view sv);

		bool HasFillAlignField(std::string_view& sv);
		bool HasSignField(std::string_view& sv);
		bool HasValidNestedField(std::string_view& sv, NestedFieldType type, size_t index);

		bool IsFlagSet(TokenType& tokenFlags, TokenType checkValue);

		void FormatTokens();    // To Be Implemented

	      private:
		helper::LazyParseHelper parseHelper {};
		std::vector<std::unique_ptr<BaseToken>> m_tokenStorage;
		IndexMode m_indexMode { IndexMode::automatic };
		TokenType m_tokenType { TokenType::Empty };
		SpecFormatting specValues {};
	};

}    // namespace serenity::lazy_parser
