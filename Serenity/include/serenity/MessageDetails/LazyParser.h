#pragma once

#include <memory>
#include <string>
#include <vector>

namespace serenity::lazy_parser {

	enum class TokenType
	{
		FillAlign,
		Sign,
		Alternate,
		ZeroPad,
		Locale,
		Width,
		Precision,
		Type,
		Char,
		Custom
	};

	class BaseToken
	{
	      public:
		BaseToken()                               = default;
		BaseToken(const BaseToken&)               = delete;
		BaseToken& operator=(const BaseToken&)    = delete;
		virtual ~BaseToken()                      = default;

		virtual void ParseToken(std::string_view) = 0;
		virtual void FormatToken()                = 0;

	      private:
		std::string formattedToken;
	};

	class FillAlignToken final: public BaseToken
	{
	      public:
		FillAlignToken()                                 = default;
		FillAlignToken(const FillAlignToken&)            = delete;
		FillAlignToken& operator=(const FillAlignToken&) = delete;
		~FillAlignToken()                                = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class SignToken final: public BaseToken
	{
	      public:
		SignToken()                            = default;
		SignToken(const SignToken&)            = delete;
		SignToken& operator=(const SignToken&) = delete;
		~SignToken()                           = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class AlternateToken final: public BaseToken
	{
	      public:
		AlternateToken()                                 = default;
		AlternateToken(const AlternateToken&)            = delete;
		AlternateToken& operator=(const AlternateToken&) = delete;
		~AlternateToken()                                = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class ZeroPadToken final: public BaseToken
	{
	      public:
		ZeroPadToken()                               = default;
		ZeroPadToken(const ZeroPadToken&)            = delete;
		ZeroPadToken& operator=(const ZeroPadToken&) = delete;
		~ZeroPadToken()                              = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class LocaleToken final: public BaseToken
	{
	      public:
		LocaleToken()                              = default;
		LocaleToken(const LocaleToken&)            = delete;
		LocaleToken& operator=(const LocaleToken&) = delete;
		~LocaleToken()                             = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class WidthToken final: public BaseToken
	{
	      public:
		WidthToken()                             = default;
		WidthToken(const WidthToken&)            = delete;
		WidthToken& operator=(const WidthToken&) = delete;
		~WidthToken()                            = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class PrecisionToken final: public BaseToken
	{
	      public:
		PrecisionToken()                                 = default;
		PrecisionToken(const PrecisionToken&)            = delete;
		PrecisionToken& operator=(const PrecisionToken&) = delete;
		~PrecisionToken()                                = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class TypeToken final: public BaseToken
	{
	      public:
		TypeToken()                            = default;
		TypeToken(const TypeToken&)            = delete;
		TypeToken& operator=(const TypeToken&) = delete;
		~TypeToken()                           = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class CustomToken final: public BaseToken
	{
	      public:
		CustomToken()                              = default;
		CustomToken(const CustomToken&)            = delete;
		CustomToken& operator=(const CustomToken&) = delete;
		~CustomToken()                             = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class CharAggregateToken final: public BaseToken
	{
	      public:
		CharAggregateToken()                                     = default;
		CharAggregateToken(const CharAggregateToken&)            = delete;
		CharAggregateToken& operator=(const CharAggregateToken&) = delete;
		~CharAggregateToken()                                    = default;

		void ParseToken(std::string_view) override;
		void FormatToken() override;
	};

	class LazyParser
	{
	      public:
		LazyParser()                             = default;
		LazyParser(const LazyParser&)            = delete;
		LazyParser& operator=(const LazyParser&) = delete;
		~LazyParser()                            = default;

		void Parse(std::string_view sv);

	      private:
		std::vector<std::unique_ptr<BaseToken>> m_tokenStorage;
	};

}    // namespace serenity::lazy_parser
