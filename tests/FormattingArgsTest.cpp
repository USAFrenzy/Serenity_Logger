#pragma once

#include "catch.hpp"

#include <format>

#include <serenity/MessageDetails/ArgFormatter.h>
using namespace serenity::arg_formatter;

// NOTE: Currently I have yet to add const void*, void*, or custom formatting

TEST_CASE("Base Auto Index Formatting") {
	ArgFormatter formatter;
	// not sure why msvc code analysis was failing with possibly implicating this formatString
	static constexpr std::string_view formatString { "{}" };

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

	REQUIRE(std::format("{} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l) ==
	        formatter.se_format("{} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l));
}

TEST_CASE("Base Manual Index Formatting") {
	ArgFormatter formatter;

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

	REQUIRE(std::format("{0}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{0}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{1}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{1}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{2}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{2}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{3}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{3}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{4}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{4}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{5}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{5}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{6}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{6}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{7}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{7}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{8}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{8}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{9}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{9}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{10}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{10}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{11}", a, b, c, d, e, f, g, h, i, j, k, l) == formatter.se_format("{11}", a, b, c, d, e, f, g, h, i, j, k, l));
	REQUIRE(std::format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11}", a, b, c, d, e, f, g, h, i, j, k, l) ==
	        formatter.se_format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11}", a, b, c, d, e, f, g, h, i, j, k, l));
}