#pragma once

#ifdef USE_BUILT_IN_FMT
	#include <serenity/CustomFormat/argfmt_backend.h>
#else
	// #include <ArgFormatter/ArgFormatter.h>
#endif

#include "catch.hpp"
#include <format>
#include <iostream>

using namespace formatter::arg_formatter;

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

	static constexpr std::string_view parseString { "{}" };
	REQUIRE(std::format(parseString, a) == formatter.format(parseString, a));
	REQUIRE(std::format(parseString, b) == formatter.format(parseString, b));
	REQUIRE(std::format(parseString, c) == formatter.format(parseString, c));
	REQUIRE(std::format(parseString, d) == formatter.format(parseString, d));
	REQUIRE(std::format(parseString, e) == formatter.format(parseString, e));
	REQUIRE(std::format(parseString, f) == formatter.format(parseString, f));
	REQUIRE(std::format(parseString, g) == formatter.format(parseString, g));
	REQUIRE(std::format(parseString, h) == formatter.format(parseString, h));
	REQUIRE(std::format(parseString, i) == formatter.format(parseString, i));
	REQUIRE(std::format(parseString, j) == formatter.format(parseString, j));
	REQUIRE(std::format(parseString, k) == formatter.format(parseString, k));
	REQUIRE(std::format(parseString, l) == formatter.format(parseString, l));
	REQUIRE(std::format(parseString, m) == formatter.format(parseString, m));
	REQUIRE(std::format(parseString, n) == formatter.format(parseString, n));

	REQUIRE(std::format("{} {} {} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.format("{} {} {} {} {} {} {} {} {} {} {} {} {} {}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
}

TEST_CASE("Base Manual Index Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{0}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{0}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{2}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{2}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{3}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{3}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{4}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{4}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{5}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{5}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{6}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{6}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{7}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{7}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{8}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{8}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{9}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{9}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{10}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{10}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{11}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{11}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{12}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{12}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
	REQUIRE(std::format("{13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) == formatter.format("{13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));

	REQUIRE(std::format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11} {12} {13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.format("{0} {1} {2} {3} {4} {5} {6} {7} {8} {9} {10} {11} {12} {13}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));

	REQUIRE(std::format("{9} {3} {2} {10} {4} {12} {6} {5} {13} {11} {8} {7} {0} {1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n) ==
	        formatter.format("{9} {3} {2} {10} {4} {12} {6} {5} {13} {11} {8} {7} {0} {1}", a, b, c, d, e, f, g, h, i, j, k, l, m, n));
}

TEST_CASE("Type Specifier Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", a) == formatter.format("{:b}", a));
	REQUIRE(std::format("{:B}", a) == formatter.format("{:B}", a));
	REQUIRE(std::format("{:d}", a) == formatter.format("{:d}", a));
	REQUIRE(std::format("{:o}", a) == formatter.format("{:o}", a));
	REQUIRE(std::format("{:x}", a) == formatter.format("{:x}", a));
	REQUIRE(std::format("{:X}", a) == formatter.format("{:X}", a));

	REQUIRE(std::format("{:b}", b) == formatter.format("{:b}", b));
	REQUIRE(std::format("{:B}", b) == formatter.format("{:B}", b));
	REQUIRE(std::format("{:d}", b) == formatter.format("{:d}", b));
	REQUIRE(std::format("{:o}", b) == formatter.format("{:o}", b));
	REQUIRE(std::format("{:x}", b) == formatter.format("{:x}", b));
	REQUIRE(std::format("{:X}", b) == formatter.format("{:X}", b));

	REQUIRE(std::format("{:b}", c) == formatter.format("{:b}", c));
	REQUIRE(std::format("{:B}", c) == formatter.format("{:B}", c));
	REQUIRE(std::format("{:d}", c) == formatter.format("{:d}", c));
	REQUIRE(std::format("{:o}", c) == formatter.format("{:o}", c));
	REQUIRE(std::format("{:x}", c) == formatter.format("{:x}", c));
	REQUIRE(std::format("{:X}", c) == formatter.format("{:X}", c));

	REQUIRE(std::format("{:b}", d) == formatter.format("{:b}", d));
	REQUIRE(std::format("{:B}", d) == formatter.format("{:B}", d));
	REQUIRE(std::format("{:d}", d) == formatter.format("{:d}", d));
	REQUIRE(std::format("{:o}", d) == formatter.format("{:o}", d));
	REQUIRE(std::format("{:x}", d) == formatter.format("{:x}", d));
	REQUIRE(std::format("{:X}", d) == formatter.format("{:X}", d));

	REQUIRE(std::format("{:a}", e) == formatter.format("{:a}", e));
	REQUIRE(std::format("{:A}", e) == formatter.format("{:A}", e));

	REQUIRE(std::format("{:a}", f) == formatter.format("{:a}", f));
	REQUIRE(std::format("{:A}", f) == formatter.format("{:A}", f));

	REQUIRE(std::format("{:a}", g) == formatter.format("{:a}", g));
	REQUIRE(std::format("{:A}", g) == formatter.format("{:A}", g));

	REQUIRE(std::format("{:s}", h) == formatter.format("{:s}", h));
	REQUIRE(std::format("{:s}", i) == formatter.format("{:s}", i));
	REQUIRE(std::format("{:s}", j) == formatter.format("{:s}", j));

	REQUIRE(std::format("{:B}", k) == formatter.format("{:B}", k));
	REQUIRE(std::format("{:o}", k) == formatter.format("{:o}", k));
	REQUIRE(std::format("{:x}", k) == formatter.format("{:x}", k));
	REQUIRE(std::format("{:X}", k) == formatter.format("{:X}", k));

	REQUIRE(std::format("{:B}", l) == formatter.format("{:B}", l));
	REQUIRE(std::format("{:c}", l) == formatter.format("{:c}", l));
	REQUIRE(std::format("{:d}", l) == formatter.format("{:d}", l));
	REQUIRE(std::format("{:o}", l) == formatter.format("{:o}", l));
	REQUIRE(std::format("{:x}", l) == formatter.format("{:x}", l));
	REQUIRE(std::format("{:X}", l) == formatter.format("{:X}", l));

	REQUIRE(std::format("{:p}", m) == formatter.format("{:p}", m));
	REQUIRE(std::format("{:p}", n) == formatter.format("{:p}", n));
}

TEST_CASE("Alternate Int Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", a) == formatter.format("{:#}", a));
	REQUIRE(std::format("{:#b}", a) == formatter.format("{:#b}", a));
	REQUIRE(std::format("{:#B}", a) == formatter.format("{:#B}", a));
	REQUIRE(std::format("{:#d}", a) == formatter.format("{:#d}", a));
	REQUIRE(std::format("{:#o}", a) == formatter.format("{:#o}", a));
	REQUIRE(std::format("{:#x}", a) == formatter.format("{:#x}", a));
	REQUIRE(std::format("{:#X}", a) == formatter.format("{:#X}", a));
}

TEST_CASE("Alternate U_Int Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", b) == formatter.format("{:#}", b));
	REQUIRE(std::format("{:#b}", b) == formatter.format("{:#b}", b));
	REQUIRE(std::format("{:#B}", b) == formatter.format("{:#B}", b));
	REQUIRE(std::format("{:#d}", b) == formatter.format("{:#d}", b));
	REQUIRE(std::format("{:#o}", b) == formatter.format("{:#o}", b));
	REQUIRE(std::format("{:#x}", b) == formatter.format("{:#x}", b));
	REQUIRE(std::format("{:#X}", b) == formatter.format("{:#X}", b));
}

TEST_CASE("Alternate Long Long Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", c) == formatter.format("{:#}", c));
	REQUIRE(std::format("{:#b}", c) == formatter.format("{:#b}", c));
	REQUIRE(std::format("{:#B}", c) == formatter.format("{:#B}", c));
	REQUIRE(std::format("{:#d}", c) == formatter.format("{:#d}", c));
	REQUIRE(std::format("{:#o}", c) == formatter.format("{:#o}", c));
	REQUIRE(std::format("{:#x}", c) == formatter.format("{:#x}", c));
	REQUIRE(std::format("{:#X}", c) == formatter.format("{:#X}", c));
}

TEST_CASE("Alternate Unsigned Long Long Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", d) == formatter.format("{:#}", d));
	REQUIRE(std::format("{:#b}", d) == formatter.format("{:#b}", d));
	REQUIRE(std::format("{:#B}", d) == formatter.format("{:#B}", d));
	REQUIRE(std::format("{:#d}", d) == formatter.format("{:#d}", d));
	REQUIRE(std::format("{:#o}", d) == formatter.format("{:#o}", d));
	REQUIRE(std::format("{:#x}", d) == formatter.format("{:#x}", d));
	REQUIRE(std::format("{:#X}", d) == formatter.format("{:#X}", d));
}

TEST_CASE("Alternate Float Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", e) == formatter.format("{:#}", e));
	REQUIRE(std::format("{:#a}", e) == formatter.format("{:#a}", e));
	REQUIRE(std::format("{:#A}", e) == formatter.format("{:#A}", e));
}

TEST_CASE("Alternate Double Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", f) == formatter.format("{:#}", f));
	REQUIRE(std::format("{:#a}", f) == formatter.format("{:#a}", f));
	REQUIRE(std::format("{:#A}", f) == formatter.format("{:#A}", f));
}

TEST_CASE("Alternate Long Double Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:#}", g) == formatter.format("{:#}", g));
	REQUIRE(std::format("{:#a}", g) == formatter.format("{:#a}", g));
	REQUIRE(std::format("{:#A}", g) == formatter.format("{:#A}", g));
}

TEST_CASE("String Type Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:s}", h) == formatter.format("{:s}", h));
	REQUIRE(std::format("{:s}", i) == formatter.format("{:s}", i));
	REQUIRE(std::format("{:s}", j) == formatter.format("{:s}", j));
}

TEST_CASE("Bool Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", k) == formatter.format("{:b}", k));
	REQUIRE(std::format("{:B}", k) == formatter.format("{:B}", k));
	REQUIRE(std::format("{:o}", k) == formatter.format("{:o}", k));
	REQUIRE(std::format("{:x}", k) == formatter.format("{:x}", k));
	REQUIRE(std::format("{:X}", k) == formatter.format("{:X}", k));
}

TEST_CASE("Char Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:b}", l) == formatter.format("{:b}", l));
	REQUIRE(std::format("{:B}", l) == formatter.format("{:B}", l));
	REQUIRE(std::format("{:c}", l) == formatter.format("{:c}", l));
	REQUIRE(std::format("{:d}", l) == formatter.format("{:d}", l));
	REQUIRE(std::format("{:o}", l) == formatter.format("{:o}", l));
	REQUIRE(std::format("{:x}", l) == formatter.format("{:x}", l));
	REQUIRE(std::format("{:X}", l) == formatter.format("{:X}", l));
}

TEST_CASE("Width Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:10}", a) == formatter.format("{:10}", a));
	REQUIRE(std::format("{:10}", b) == formatter.format("{:10}", b));
	REQUIRE(std::format("{:10}", c) == formatter.format("{:10}", c));
	REQUIRE(std::format("{:10}", d) == formatter.format("{:10}", d));
	REQUIRE(std::format("{:10}", e) == formatter.format("{:10}", e));
	REQUIRE(std::format("{:10}", f) == formatter.format("{:10}", f));
	REQUIRE(std::format("{:10}", g) == formatter.format("{:10}", g));
	REQUIRE(std::format("{:10}", h) == formatter.format("{:10}", h));
	REQUIRE(std::format("{:10}", i) == formatter.format("{:10}", i));
	REQUIRE(std::format("{:10}", j) == formatter.format("{:10}", j));
	REQUIRE(std::format("{:10}", k) == formatter.format("{:10}", k));
	REQUIRE(std::format("{:10}", l) == formatter.format("{:10}", l));
	REQUIRE(std::format("{:10}", m) == formatter.format("{:10}", m));
	REQUIRE(std::format("{:10}", n) == formatter.format("{:10}", n));
}

TEST_CASE("Precision Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:.5}", e) == formatter.format("{:.5}", e));
	REQUIRE(std::format("{:.5}", f) == formatter.format("{:.5}", f));
	REQUIRE(std::format("{:.5}", g) == formatter.format("{:.5}", g));
	REQUIRE(std::format("{:.5}", h) == formatter.format("{:.5}", h));
	REQUIRE(std::format("{:.5}", i) == formatter.format("{:.5}", i));
	REQUIRE(std::format("{:.5}", j) == formatter.format("{:.5}", j));
}

TEST_CASE("Sign Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:+}", a) == formatter.format("{:+}", a));
	REQUIRE(std::format("{:+}", b) == formatter.format("{:+}", b));
	REQUIRE(std::format("{:+}", c) == formatter.format("{:+}", c));
	REQUIRE(std::format("{:+}", d) == formatter.format("{:+}", d));
	REQUIRE(std::format("{:+}", e) == formatter.format("{:+}", e));
	REQUIRE(std::format("{:+}", f) == formatter.format("{:+}", f));
	REQUIRE(std::format("{:+}", g) == formatter.format("{:+}", g));

	REQUIRE(std::format("{:-}", a) == formatter.format("{:-}", a));
	REQUIRE(std::format("{:-}", b) == formatter.format("{:-}", b));
	REQUIRE(std::format("{:-}", c) == formatter.format("{:-}", c));
	REQUIRE(std::format("{:-}", d) == formatter.format("{:-}", d));
	REQUIRE(std::format("{:-}", e) == formatter.format("{:-}", e));
	REQUIRE(std::format("{:-}", f) == formatter.format("{:-}", f));
	REQUIRE(std::format("{:-}", g) == formatter.format("{:-}", g));

	REQUIRE(std::format("{: }", a) == formatter.format("{: }", a));
	REQUIRE(std::format("{: }", b) == formatter.format("{: }", b));
	REQUIRE(std::format("{: }", c) == formatter.format("{: }", c));
	REQUIRE(std::format("{: }", d) == formatter.format("{: }", d));
	REQUIRE(std::format("{: }", e) == formatter.format("{: }", e));
	REQUIRE(std::format("{: }", f) == formatter.format("{: }", f));
	REQUIRE(std::format("{: }", g) == formatter.format("{: }", g));
}

TEST_CASE("Localization Formatting") {
	std::locale locale("en_US.UTF-8");
	ArgFormatter formatter;

	REQUIRE(std::format(locale, "{:L}", a) == formatter.format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.format(locale, "{:L}", k));

	locale = std::locale("de_DE");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.format(locale, "{:L}", k));

	locale = std::locale("zh-HK");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.format(locale, "{:L}", k));

	locale = std::locale("hi_IN");
	REQUIRE(std::format(locale, "{:L}", a) == formatter.format(locale, "{:L}", a));
	REQUIRE(std::format(locale, "{:L}", b) == formatter.format(locale, "{:L}", b));
	REQUIRE(std::format(locale, "{:L}", c) == formatter.format(locale, "{:L}", c));
	REQUIRE(std::format(locale, "{:L}", d) == formatter.format(locale, "{:L}", d));
	REQUIRE(std::format(locale, "{:L}", e) == formatter.format(locale, "{:L}", e));
	REQUIRE(std::format(locale, "{:L}", f) == formatter.format(locale, "{:L}", f));
	REQUIRE(std::format(locale, "{:L}", g) == formatter.format(locale, "{:L}", g));
	REQUIRE(std::format(locale, "{:L}", k) == formatter.format(locale, "{:L}", k));
}

TEST_CASE("Fill-Align Formatting") {
	ArgFormatter formatter;

	REQUIRE(std::format("{:*^20}", a) == formatter.format("{:*^20}", a));
	REQUIRE(std::format("{:*<20}", a) == formatter.format("{:*<20}", a));
	REQUIRE(std::format("{:*>20}", a) == formatter.format("{:*>20}", a));

	REQUIRE(std::format("{:*^20}", b) == formatter.format("{:*^20}", b));
	REQUIRE(std::format("{:*<20}", b) == formatter.format("{:*<20}", b));
	REQUIRE(std::format("{:*>20}", b) == formatter.format("{:*>20}", b));

	REQUIRE(std::format("{:*^20}", c) == formatter.format("{:*^20}", c));
	REQUIRE(std::format("{:*<20}", c) == formatter.format("{:*<20}", c));
	REQUIRE(std::format("{:*>20}", c) == formatter.format("{:*>20}", c));

	REQUIRE(std::format("{:*^20}", d) == formatter.format("{:*^20}", d));
	REQUIRE(std::format("{:*<20}", d) == formatter.format("{:*<20}", d));
	REQUIRE(std::format("{:*>20}", d) == formatter.format("{:*>20}", d));

	REQUIRE(std::format("{:*^20}", e) == formatter.format("{:*^20}", e));
	REQUIRE(std::format("{:*<20}", e) == formatter.format("{:*<20}", e));
	REQUIRE(std::format("{:*>20}", e) == formatter.format("{:*>20}", e));

	REQUIRE(std::format("{:*^20}", f) == formatter.format("{:*^20}", f));
	REQUIRE(std::format("{:*<20}", f) == formatter.format("{:*<20}", f));
	REQUIRE(std::format("{:*>20}", f) == formatter.format("{:*>20}", f));

	REQUIRE(std::format("{:*^20}", g) == formatter.format("{:*^20}", g));
	REQUIRE(std::format("{:*<20}", g) == formatter.format("{:*<20}", g));
	REQUIRE(std::format("{:*>20}", g) == formatter.format("{:*>20}", g));

	REQUIRE(std::format("{:*^20}", h) == formatter.format("{:*^20}", h));
	REQUIRE(std::format("{:*<20}", h) == formatter.format("{:*<20}", h));
	REQUIRE(std::format("{:*>20}", h) == formatter.format("{:*>20}", h));

	REQUIRE(std::format("{:*^20}", i) == formatter.format("{:*^20}", i));
	REQUIRE(std::format("{:*<20}", i) == formatter.format("{:*<20}", i));
	REQUIRE(std::format("{:*>20}", i) == formatter.format("{:*>20}", i));

	REQUIRE(std::format("{:*^20}", j) == formatter.format("{:*^20}", j));
	REQUIRE(std::format("{:*<20}", j) == formatter.format("{:*<20}", j));
	REQUIRE(std::format("{:*>20}", j) == formatter.format("{:*>20}", j));

	REQUIRE(std::format("{:*^20}", k) == formatter.format("{:*^20}", k));
	REQUIRE(std::format("{:*<20}", k) == formatter.format("{:*<20}", k));
	REQUIRE(std::format("{:*>20}", k) == formatter.format("{:*>20}", k));

	REQUIRE(std::format("{:*^20}", l) == formatter.format("{:*^20}", l));
	REQUIRE(std::format("{:*<20}", l) == formatter.format("{:*<20}", l));
	REQUIRE(std::format("{:*>20}", l) == formatter.format("{:*>20}", l));

	REQUIRE(std::format("{:*^20}", m) == formatter.format("{:*^20}", m));
	REQUIRE(std::format("{:*<20}", m) == formatter.format("{:*<20}", m));
	REQUIRE(std::format("{:*>20}", m) == formatter.format("{:*>20}", m));

	REQUIRE(std::format("{:*^20}", n) == formatter.format("{:*^20}", n));
	REQUIRE(std::format("{:*<20}", n) == formatter.format("{:*<20}", n));
	REQUIRE(std::format("{:*>20}", n) == formatter.format("{:*>20}", n));
}

#include <serenity/Utilities/FormatBackend.h>
// Since the inclusion of USE_STD_FORMAT and USE_FMTLIB , need to force this to be MSVC for testing purposes atm
#ifdef VFORMAT_TO
	#undef VFORMAT_TO
	#if _MSC_VER >= 1930 && (_MSVC_LANG >= 202002L)
		#define CONTEXT                         std::back_insert_iterator<std::basic_string<char>>
		#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to<CONTEXT>(std::back_inserter(cont), loc, msg, std::make_format_args(__VA_ARGS__))
	#elif(_MSC_VER >= 1929) && (_MSVC_LANG >= 202002L)
		#if _MSC_FULL_VER >= 192930145    // MSVC build that backported fixes for <format> under C++20 switch instead of C++ latest
			#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to(std::back_inserter(cont), loc, msg, std::make_format_args(__VA_ARGS__))
		#else
			#define CONTEXT                         std::basic_format_context<std::back_insert_iterator<std::basic_string<char>>, char>
			#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to(std::back_inserter(cont), loc, msg, std::make_format_args<CONTEXT>(__VA_ARGS__))
		#endif
	#endif
#endif    // VFORMAT_TO

TEST_CASE("Format Function Test") {
	ArgFormatter formatter;
	std::locale loc("");
	int width { 20 };
	std::string stdStr, argFmtStr;
	constexpr std::string_view fmt { "{0:*^#{1}x}" };

	// Once appveyor supports the new update in VS 16.14/16.15 then this macro can dissapear. Build is failing
	// when using the normal std::vformat_to() due to how it used to work before the back-ported fixes to <format>
	VFORMAT_TO(stdStr, loc, fmt, a, width);
	formatter.format_to(std::back_inserter(argFmtStr), loc, fmt, a, width);
	REQUIRE(std::format(fmt, a, width) == formatter.format(fmt, a, width));
	REQUIRE(stdStr == argFmtStr);
}

#ifdef USE_BUILT_IN_FMT
TEST_CASE("Custom Formatting Test For Message_Info") {

	serenity::MsgWithLoc message { "This is a test message for custom formatting of Message_Info" };
	const serenity::msg_details::Message_Info testInfo { "TestFormatting", serenity::LoggerLevel::trace, serenity::message_time_mode::local };
	testInfo.Message() = message.msg;
	testInfo.SetSrcLoc(message.source);

	serenity::source_flag srcFlag { serenity::source_flag::empty };
	Format_Source_Loc testSrc(testInfo, srcFlag);

	REQUIRE(formatter::format("{:%L}", testInfo) == "Trace");
	REQUIRE(formatter::format("{:%l}", testInfo) == "T");
	testInfo.SetMsgLevel(serenity::LoggerLevel::info);
	REQUIRE(formatter::format("{:%L}", testInfo) == "Info");
	REQUIRE(formatter::format("{:%l}", testInfo) == "I");
	testInfo.SetMsgLevel(serenity::LoggerLevel::debug);
	REQUIRE(formatter::format("{:%L}", testInfo) == "Debug");
	REQUIRE(formatter::format("{:%l}", testInfo) == "D");
	testInfo.SetMsgLevel(serenity::LoggerLevel::warning);
	REQUIRE(formatter::format("{:%L}", testInfo) == "Warn");
	REQUIRE(formatter::format("{:%l}", testInfo) == "W");
	testInfo.SetMsgLevel(serenity::LoggerLevel::error);
	REQUIRE(formatter::format("{:%L}", testInfo) == "Error");
	REQUIRE(formatter::format("{:%l}", testInfo) == "E");
	testInfo.SetMsgLevel(serenity::LoggerLevel::fatal);
	REQUIRE(formatter::format("{:%L}", testInfo) == "Fatal");
	REQUIRE(formatter::format("{:%l}", testInfo) == "F");
	testInfo.SetMsgLevel(serenity::LoggerLevel::off);
	REQUIRE(formatter::format("{:%L}", testInfo) == "");
	REQUIRE(formatter::format("{:%l}", testInfo) == "");

	REQUIRE(formatter::format("{:%s}", testInfo) == testSrc.FormatUserPattern());
	REQUIRE(formatter::format("{:%t}", testInfo) == "[PlaceHolder]");
	REQUIRE(formatter::format("{:%+}", testInfo) == testInfo.Message());
}

#endif    //  USE_BUILT_IN_FMT
