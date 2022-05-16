#pragma once

#include "catch.hpp"

#include <format>

#include <serenity/MessageDetails/ArgFormatter.h>
using namespace serenity::arg_formatter;

// NOTE: Currently I have yet to add const void*, void*, or custom formatting

TEST_CASE("Base Formatting", "[Automatic Indexing]") {
	ArgFormatter formatter;

	std::string formatString { "{}" };

	int a                = 424'242'424;
	unsigned int b       = 4'242'424'242;
	long long c          = 424'242'424'242'424;
	unsigned long long d = 424'242'424'242'424'424;
	float e              = 424'242'424.424f;
	double f             = 424'242'424.42424242;
	long double g        = 424'424'242'424.42424242;
	std::string h { "This is a string arg" };
	const char* i { "This is a const char* arg" };
	std::string_view j { "This is a string_view arg" };
	bool k = true;
	char l = 'm';

	REQUIRE(std::format(formatString, a) == formatter.se_format(formatString, a));
	REQUIRE(std::format(formatString, b) == formatter.se_format(formatString, b));
	REQUIRE(std::format(formatString, c) == formatter.se_format(formatString, c));
	REQUIRE(std::format(formatString, d) == formatter.se_format(formatString, d));
	REQUIRE(std::format(formatString, e) == formatter.se_format(formatString, e));
	REQUIRE(std::format(formatString, f) == formatter.se_format(formatString, f));
	REQUIRE(std::format(formatString, g) == formatter.se_format(formatString, g));
	REQUIRE(std::format(formatString, h) == formatter.se_format(formatString, h));
	REQUIRE(std::format(formatString, i) == formatter.se_format(formatString, i));
	REQUIRE(std::format(formatString, j) == formatter.se_format(formatString, j));
	REQUIRE(std::format(formatString, k) == formatter.se_format(formatString, k));
	REQUIRE(std::format(formatString, l) == formatter.se_format(formatString, l));
}