#pragma once
#include <string>
#include <vector>

namespace serenity::lazy_parser {

	enum class TokenType
	{
		FIllAlign,
		Sign,
		Alternate,
		ZeroPad,
		Locale,
		Width,
		Precision,
		Type
	};

	class TokenTypeStorage
	{
	      public:
		TokenTypeStorage()                        = default;
		TokenTypeStorage(const TokenTypeStorage&) = delete;
		TokenTypeStorage& operator=(const TokenTypeStorage&) = delete;
		~TokenTypeStorage()                                  = default;

	      private:
		std::vector<TokenType> tokenTypes;
	};

	class TokenValueStorage
	{
	      public:
		TokenValueStorage()                         = default;
		TokenValueStorage(const TokenValueStorage&) = delete;
		TokenValueStorage& operator=(const TokenValueStorage&) = delete;
		~TokenValueStorage()                                   = default;

	      private:
		std::vector<std::string_view> tokenValues;
	};

	class LazyParser
	{
	      public:
		LazyParser()                  = default;
		LazyParser(const LazyParser&) = delete;
		LazyParser& operator=(const LazyParser&) = delete;
		~LazyParser()                            = default;

		void Parse(std::string_view sv) { }

	      private:
		TokenTypeStorage tokenTypeHelper;
		TokenValueStorage tokenStorageHelper;
	};

}    // namespace serenity::lazy_parser
