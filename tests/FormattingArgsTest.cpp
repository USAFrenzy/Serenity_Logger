#pragma once

#include "catch.hpp"

#include <serenity/MessageDetails/ArgFormatter.h>
#include <format>
#include <iostream>
using namespace serenity::arg_formatter;

// common testing variables
static int a                = 424'242'424;
static unsigned int b       = 4'242'424'242;
static long long c          = 424'242'424'242'424;
static unsigned long long d = 424'242'424'242'424'424;
static float e              = 424'242'424.424f;
static double f             = 424'242'424.42424242;
static long double g        = 424'424'242'424.42424242;
static std::string h { "This is a string arg" };
static const char* i { "This is a const char* arg" };
static std::string_view j { "This is a string_view arg" };
static bool k = true;
static char l = 'm';
const void* m = static_cast<const void*>(&a);
void* n       = static_cast<void*>(&b);

// NOTE: Currently I have yet to add custom formatting

TEST_CASE("Base Auto Index Formatting") {
	ArgFormatter formatter;

	static constexpr std::string_view formatString { "{}" };
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
	REQUIRE(std::format(formatString, m) == formatter.se_format(formatString, m));
	REQUIRE(std::format(formatString, n) == formatter.se_format(formatString, n));

	REQUIRE(std::format("{} {} {} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.se_format("{} {} {} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
}

TEST_CASE("Base Manual Index Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{0}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{0}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{2}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{2}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{3}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{3}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{4}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{4}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{5}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{5}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{6}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{6}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{7}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{7}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{8}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{8}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{9}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{9}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{10}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{10}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{11}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{11}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{12}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{12}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.se_format("{13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));

	REQUIRE(std::format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11} {12} {13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.se_format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11} {12} {13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));

	REQUIRE(std::format("{9} {3} {2} {10} {4} {12} {6} {5} {13} {11} {8} {7} {0} {1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.se_format("{9} {3} {2} {10} {4} {12} {6} {5} {13} {11} {8} {7} {0} {1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
}

TEST_CASE("Type Specifier Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", a) == formatter.se_format("{:b}", a));
	REQUIRE(std::format("{:B}", a) == formatter.se_format("{:B}", a));
	REQUIRE(std::format("{:d}", a) == formatter.se_format("{:d}", a));
	REQUIRE(std::format("{:o}", a) == formatter.se_format("{:o}", a));
	REQUIRE(std::format("{:x}", a) == formatter.se_format("{:x}", a));
	REQUIRE(std::format("{:X}", a) == formatter.se_format("{:X}", a));

	REQUIRE(std::format("{:b}", b) == formatter.se_format("{:b}", b));
	REQUIRE(std::format("{:B}", b) == formatter.se_format("{:B}", b));
	REQUIRE(std::format("{:d}", b) == formatter.se_format("{:d}", b));
	REQUIRE(std::format("{:o}", b) == formatter.se_format("{:o}", b));
	REQUIRE(std::format("{:x}", b) == formatter.se_format("{:x}", b));
	REQUIRE(std::format("{:X}", b) == formatter.se_format("{:X}", b));

	REQUIRE(std::format("{:b}", c) == formatter.se_format("{:b}", c));
	REQUIRE(std::format("{:B}", c) == formatter.se_format("{:B}", c));
	REQUIRE(std::format("{:d}", c) == formatter.se_format("{:d}", c));
	REQUIRE(std::format("{:o}", c) == formatter.se_format("{:o}", c));
	REQUIRE(std::format("{:x}", c) == formatter.se_format("{:x}", c));
	REQUIRE(std::format("{:X}", c) == formatter.se_format("{:X}", c));

	REQUIRE(std::format("{:b}", d) == formatter.se_format("{:b}", d));
	REQUIRE(std::format("{:B}", d) == formatter.se_format("{:B}", d));
	REQUIRE(std::format("{:d}", d) == formatter.se_format("{:d}", d));
	REQUIRE(std::format("{:o}", d) == formatter.se_format("{:o}", d));
	REQUIRE(std::format("{:x}", d) == formatter.se_format("{:x}", d));
	REQUIRE(std::format("{:X}", d) == formatter.se_format("{:X}", d));

	REQUIRE(std::format("{:a}", e) == formatter.se_format("{:a}", e));
	REQUIRE(std::format("{:A}", e) == formatter.se_format("{:A}", e));

	REQUIRE(std::format("{:a}", f) == formatter.se_format("{:a}", f));
	REQUIRE(std::format("{:A}", f) == formatter.se_format("{:A}", f));

	REQUIRE(std::format("{:a}", g) == formatter.se_format("{:a}", g));
	REQUIRE(std::format("{:A}", g) == formatter.se_format("{:A}", g));

	REQUIRE(std::format("{:s}", h) == formatter.se_format("{:s}", h));
	REQUIRE(std::format("{:s}", i) == formatter.se_format("{:s}", i));
	REQUIRE(std::format("{:s}", j) == formatter.se_format("{:s}", j));

	REQUIRE(std::format("{:B}", k) == formatter.se_format("{:B}", k));
	REQUIRE(std::format("{:o}", k) == formatter.se_format("{:o}", k));
	REQUIRE(std::format("{:x}", k) == formatter.se_format("{:x}", k));
	REQUIRE(std::format("{:X}", k) == formatter.se_format("{:X}", k));

	REQUIRE(std::format("{:B}", l) == formatter.se_format("{:B}", l));
	REQUIRE(std::format("{:c}", l) == formatter.se_format("{:c}", l));
	REQUIRE(std::format("{:d}", l) == formatter.se_format("{:d}", l));
	REQUIRE(std::format("{:o}", l) == formatter.se_format("{:o}", l));
	REQUIRE(std::format("{:x}", l) == formatter.se_format("{:x}", l));
	REQUIRE(std::format("{:X}", l) == formatter.se_format("{:X}", l));

	REQUIRE(std::format("{:p}", m) == formatter.se_format("{:p}", m));
	REQUIRE(std::format("{:p}", n) == formatter.se_format("{:p}", n));
}

TEST_CASE("Alternate Int Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", a) == formatter.se_format("{:#}", a));
	REQUIRE(std::format("{:#b}", a) == formatter.se_format("{:#b}", a));
	REQUIRE(std::format("{:#B}", a) == formatter.se_format("{:#B}", a));
	REQUIRE(std::format("{:#d}", a) == formatter.se_format("{:#d}", a));
	REQUIRE(std::format("{:#o}", a) == formatter.se_format("{:#o}", a));
	REQUIRE(std::format("{:#x}", a) == formatter.se_format("{:#x}", a));
	REQUIRE(std::format("{:#X}", a) == formatter.se_format("{:#X}", a));
}

TEST_CASE("Alternate U_Int Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", b) == formatter.se_format("{:#}", b));
	REQUIRE(std::format("{:#b}", b) == formatter.se_format("{:#b}", b));
	REQUIRE(std::format("{:#B}", b) == formatter.se_format("{:#B}", b));
	REQUIRE(std::format("{:#d}", b) == formatter.se_format("{:#d}", b));
	REQUIRE(std::format("{:#o}", b) == formatter.se_format("{:#o}", b));
	REQUIRE(std::format("{:#x}", b) == formatter.se_format("{:#x}", b));
	REQUIRE(std::format("{:#X}", b) == formatter.se_format("{:#X}", b));
}

TEST_CASE("Alternate Long Long Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", c) == formatter.se_format("{:#}", c));
	REQUIRE(std::format("{:#b}", c) == formatter.se_format("{:#b}", c));
	REQUIRE(std::format("{:#B}", c) == formatter.se_format("{:#B}", c));
	REQUIRE(std::format("{:#d}", c) == formatter.se_format("{:#d}", c));
	REQUIRE(std::format("{:#o}", c) == formatter.se_format("{:#o}", c));
	REQUIRE(std::format("{:#x}", c) == formatter.se_format("{:#x}", c));
	REQUIRE(std::format("{:#X}", c) == formatter.se_format("{:#X}", c));
}

TEST_CASE("Alternate Unsigned Long Long Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", d) == formatter.se_format("{:#}", d));
	REQUIRE(std::format("{:#b}", d) == formatter.se_format("{:#b}", d));
	REQUIRE(std::format("{:#B}", d) == formatter.se_format("{:#B}", d));
	REQUIRE(std::format("{:#d}", d) == formatter.se_format("{:#d}", d));
	REQUIRE(std::format("{:#o}", d) == formatter.se_format("{:#o}", d));
	REQUIRE(std::format("{:#x}", d) == formatter.se_format("{:#x}", d));
	REQUIRE(std::format("{:#X}", d) == formatter.se_format("{:#X}", d));
}

TEST_CASE("Alternate Float Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", e) == formatter.se_format("{:#}", e));
	REQUIRE(std::format("{:#a}", e) == formatter.se_format("{:#a}", e));
	REQUIRE(std::format("{:#A}", e) == formatter.se_format("{:#A}", e));
}

TEST_CASE("Alternate Double Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", f) == formatter.se_format("{:#}", f));
	REQUIRE(std::format("{:#a}", f) == formatter.se_format("{:#a}", f));
	REQUIRE(std::format("{:#A}", f) == formatter.se_format("{:#A}", f));
}

TEST_CASE("Alternate Long Double Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", g) == formatter.se_format("{:#}", g));
	REQUIRE(std::format("{:#a}", g) == formatter.se_format("{:#a}", g));
	REQUIRE(std::format("{:#A}", g) == formatter.se_format("{:#A}", g));
}

TEST_CASE("String Type Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:s}", h) == formatter.se_format("{:s}", h));
	REQUIRE(std::format("{:s}", i) == formatter.se_format("{:s}", i));
	REQUIRE(std::format("{:s}", j) == formatter.se_format("{:s}", j));
}

TEST_CASE("Bool Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", k) == formatter.se_format("{:b}", k));
	REQUIRE(std::format("{:B}", k) == formatter.se_format("{:B}", k));
	REQUIRE(std::format("{:o}", k) == formatter.se_format("{:o}", k));
	REQUIRE(std::format("{:x}", k) == formatter.se_format("{:x}", k));
	REQUIRE(std::format("{:X}", k) == formatter.se_format("{:X}", k));
}

TEST_CASE("Char Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", l) == formatter.se_format("{:b}", l));
	REQUIRE(std::format("{:B}", l) == formatter.se_format("{:B}", l));
	REQUIRE(std::format("{:c}", l) == formatter.se_format("{:c}", l));
	REQUIRE(std::format("{:d}", l) == formatter.se_format("{:d}", l));
	REQUIRE(std::format("{:o}", l) == formatter.se_format("{:o}", l));
	REQUIRE(std::format("{:x}", l) == formatter.se_format("{:x}", l));
	REQUIRE(std::format("{:X}", l) == formatter.se_format("{:X}", l));
}

TEST_CASE("Width Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:10}", a) == formatter.se_format("{:10}", a));
	REQUIRE(std::format("{:10}", b) == formatter.se_format("{:10}", b));
	REQUIRE(std::format("{:10}", c) == formatter.se_format("{:10}", c));
	REQUIRE(std::format("{:10}", d) == formatter.se_format("{:10}", d));
	REQUIRE(std::format("{:10}", e) == formatter.se_format("{:10}", e));
	REQUIRE(std::format("{:10}", f) == formatter.se_format("{:10}", f));
	REQUIRE(std::format("{:10}", g) == formatter.se_format("{:10}", g));
	REQUIRE(std::format("{:10}", h) == formatter.se_format("{:10}", h));
	REQUIRE(std::format("{:10}", i) == formatter.se_format("{:10}", i));
	REQUIRE(std::format("{:10}", j) == formatter.se_format("{:10}", j));
	REQUIRE(std::format("{:10}", k) == formatter.se_format("{:10}", k));
	REQUIRE(std::format("{:10}", l) == formatter.se_format("{:10}", l));
	REQUIRE(std::format("{:10}", m) == formatter.se_format("{:10}", m));
	REQUIRE(std::format("{:10}", n) == formatter.se_format("{:10}", n));
}

TEST_CASE("Precision Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:.5}", e) == formatter.se_format("{:.5}", e));
	REQUIRE(std::format("{:.5}", f) == formatter.se_format("{:.5}", f));
	REQUIRE(std::format("{:.5}", g) == formatter.se_format("{:.5}", g));
	REQUIRE(std::format("{:.5}", h) == formatter.se_format("{:.5}", h));
	REQUIRE(std::format("{:.5}", i) == formatter.se_format("{:.5}", i));
	REQUIRE(std::format("{:.5}", j) == formatter.se_format("{:.5}", j));
}

TEST_CASE("Sign Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:+}", a) == formatter.se_format("{:+}", a));
	REQUIRE(std::format("{:+}", b) == formatter.se_format("{:+}", b));
	REQUIRE(std::format("{:+}", c) == formatter.se_format("{:+}", c));
	REQUIRE(std::format("{:+}", d) == formatter.se_format("{:+}", d));
	REQUIRE(std::format("{:+}", e) == formatter.se_format("{:+}", e));
	REQUIRE(std::format("{:+}", f) == formatter.se_format("{:+}", f));
	REQUIRE(std::format("{:+}", g) == formatter.se_format("{:+}", g));

	REQUIRE(std::format("{:-}", a) == formatter.se_format("{:-}", a));
	REQUIRE(std::format("{:-}", b) == formatter.se_format("{:-}", b));
	REQUIRE(std::format("{:-}", c) == formatter.se_format("{:-}", c));
	REQUIRE(std::format("{:-}", d) == formatter.se_format("{:-}", d));
	REQUIRE(std::format("{:-}", e) == formatter.se_format("{:-}", e));
	REQUIRE(std::format("{:-}", f) == formatter.se_format("{:-}", f));
	REQUIRE(std::format("{:-}", g) == formatter.se_format("{:-}", g));

	REQUIRE(std::format("{: }", a) == formatter.se_format("{: }", a));
	REQUIRE(std::format("{: }", b) == formatter.se_format("{: }", b));
	REQUIRE(std::format("{: }", c) == formatter.se_format("{: }", c));
	REQUIRE(std::format("{: }", d) == formatter.se_format("{: }", d));
	REQUIRE(std::format("{: }", e) == formatter.se_format("{: }", e));
	REQUIRE(std::format("{: }", f) == formatter.se_format("{: }", f));
	REQUIRE(std::format("{: }", g) == formatter.se_format("{: }", g));
}

TEST_CASE("Localization Formatting") {
	std::locale locale("en_US.UTF-8");
	ArgFormatter formatter;

	REQUIRE(std::format(locale, "{:L}", a) == formatter.se_format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.se_format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.se_format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.se_format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.se_format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.se_format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.se_format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.se_format(locale, "{:L}", k));

	locale = std::locale("de_DE");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.se_format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.se_format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.se_format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.se_format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.se_format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.se_format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.se_format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.se_format(locale, "{:L}", k));

	locale = std::locale("zh-HK");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.se_format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.se_format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.se_format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.se_format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.se_format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.se_format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.se_format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.se_format(locale, "{:L}", k));

	locale = std::locale("hi_IN");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.se_format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.se_format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.se_format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.se_format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.se_format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.se_format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.se_format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.se_format(locale, "{:L}", k));
}

TEST_CASE("Fill-Align Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:*^20}", a) == formatter.se_format("{:*^20}", a));
	REQUIRE(std::format("{:*<20}", a) == formatter.se_format("{:*<20}", a));
	REQUIRE(std::format("{:*>20}", a) == formatter.se_format("{:*>20}", a));

	REQUIRE(std::format("{:*^20}", b) == formatter.se_format("{:*^20}", b));
	REQUIRE(std::format("{:*<20}", b) == formatter.se_format("{:*<20}", b));
	REQUIRE(std::format("{:*>20}", b) == formatter.se_format("{:*>20}", b));

	REQUIRE(std::format("{:*^20}", c) == formatter.se_format("{:*^20}", c));
	REQUIRE(std::format("{:*<20}", c) == formatter.se_format("{:*<20}", c));
	REQUIRE(std::format("{:*>20}", c) == formatter.se_format("{:*>20}", c));

	REQUIRE(std::format("{:*^20}", d) == formatter.se_format("{:*^20}", d));
	REQUIRE(std::format("{:*<20}", d) == formatter.se_format("{:*<20}", d));
	REQUIRE(std::format("{:*>20}", d) == formatter.se_format("{:*>20}", d));

	REQUIRE(std::format("{:*^20}", e) == formatter.se_format("{:*^20}", e));
	REQUIRE(std::format("{:*<20}", e) == formatter.se_format("{:*<20}", e));
	REQUIRE(std::format("{:*>20}", e) == formatter.se_format("{:*>20}", e));

	REQUIRE(std::format("{:*^20}", f) == formatter.se_format("{:*^20}", f));
	REQUIRE(std::format("{:*<20}", f) == formatter.se_format("{:*<20}", f));
	REQUIRE(std::format("{:*>20}", f) == formatter.se_format("{:*>20}", f));

	REQUIRE(std::format("{:*^20}", g) == formatter.se_format("{:*^20}", g));
	REQUIRE(std::format("{:*<20}", g) == formatter.se_format("{:*<20}", g));
	REQUIRE(std::format("{:*>20}", g) == formatter.se_format("{:*>20}", g));

	REQUIRE(std::format("{:*^20}", h) == formatter.se_format("{:*^20}", h));
	REQUIRE(std::format("{:*<20}", h) == formatter.se_format("{:*<20}", h));
	REQUIRE(std::format("{:*>20}", h) == formatter.se_format("{:*>20}", h));

	REQUIRE(std::format("{:*^20}", i) == formatter.se_format("{:*^20}", i));
	REQUIRE(std::format("{:*<20}", i) == formatter.se_format("{:*<20}", i));
	REQUIRE(std::format("{:*>20}", i) == formatter.se_format("{:*>20}", i));

	REQUIRE(std::format("{:*^20}", j) == formatter.se_format("{:*^20}", j));
	REQUIRE(std::format("{:*<20}", j) == formatter.se_format("{:*<20}", j));
	REQUIRE(std::format("{:*>20}", j) == formatter.se_format("{:*>20}", j));

	REQUIRE(std::format("{:*^20}", k) == formatter.se_format("{:*^20}", k));
	REQUIRE(std::format("{:*<20}", k) == formatter.se_format("{:*<20}", k));
	REQUIRE(std::format("{:*>20}", k) == formatter.se_format("{:*>20}", k));

	REQUIRE(std::format("{:*^20}", l) == formatter.se_format("{:*^20}", l));
	REQUIRE(std::format("{:*<20}", l) == formatter.se_format("{:*<20}", l));
	REQUIRE(std::format("{:*>20}", l) == formatter.se_format("{:*>20}", l));

	REQUIRE(std::format("{:*^20}", m) == formatter.se_format("{:*^20}", m));
	REQUIRE(std::format("{:*<20}", m) == formatter.se_format("{:*<20}", m));
	REQUIRE(std::format("{:*>20}", m) == formatter.se_format("{:*>20}", m));

	REQUIRE(std::format("{:*^20}", n) == formatter.se_format("{:*^20}", n));
	REQUIRE(std::format("{:*<20}", n) == formatter.se_format("{:*<20}", n));
	REQUIRE(std::format("{:*>20}", n) == formatter.se_format("{:*>20}", n));
}

#include <serenity/Common.h>

TEST_CASE("Format Function Test") {
	ArgFormatter formatter;
	int width { 20 };
	std::string stdStr, argFmtStr;
	constexpr std::string_view fmt { "{0:*^#{1}x}" };

	formatter.se_format_to(std::back_inserter(argFmtStr), fmt, a, width);
	// once appveyor supports the new update in VS 16.14/16.15 then this macro can dissapear.
	// Build is failing when using the normal std::vformat_to() due to how it used to work before
	// the back-ported fixes to <format>
	VFORMAT_TO(stdStr, fmt, a, width);

	REQUIRE(std::format(fmt, a, width) == formatter.se_format(fmt, a, width));
	REQUIRE(stdStr == argFmtStr);
}