#pragma once

#include "catch.hpp"
#include <format>

#include <iostream>

#include <serenity/MessageDetails/ArgFormatter.h>
using namespace serenity::arg_formatter;

// Right now this is just being used to see expected results vs what I get when I start implementing the token
// formatting functions, but this file will be used to test against those results to make sure that what I'm
// implementing matches the standard and can be dropped in instead of C++ latest flag being used

// Not going to lie, wish I had set out and did this way before hand as I'm learning some new things already
// just with the first test on the first type being test...

/*****************************************************************************************************/
/************************************** Knowledge Gained So Far **************************************/
/*****************************************************************************************************/

/********************************************* For Strings ********************************************
 * Not Supported:
 * Incorrect Type Specifiers(Duh), Sign Specifier, Alternate Form Specifier, other specifiers in width
 * and precision brackets
 ******************************************************************************************************
 * Supported:
 * Alignment, Fill, Precision, No Fill W/ ALignment, Width (As The Pad Field Of Fill/ALign)
 * ****************************************************************************************************/

// NOTE: Basically, '{0:*^{1}.{2}s}' is as complex as specifiers goes for strings
// TODO:#################################################################################################
// What I had misunderstood: I thought the numerical value after the alignment spec was one and the same
// with the fill/align field -> apparently that's not the case and that's the width field...
// - While it works in my code, I should change some things to reflect that new knowledge >.>
// TODO:#################################################################################################

TEST_CASE("Fill-Align Test") {
	ArgFormatter formatter;

	//std::string t0 { "Testing a message string" };
	// std::cout << std::format("String Center Aligned {}: {:^50}", "{:^50}", t0) << "\n";
	// std::cout << std::format("String Left Aligned {}: {:*<50}", "{:*<50}", t0) << "\n";
	// std::cout << std::format("String Right Aligned {}: {:*>50}", "{:*>50}", t0) << "\n\n";
	// std::cout << std::format("String Center Aligned {}: {:^{}}", "{:{}^50}", t0, 65) << "\n";

	// Precision is supported - looks like it just takes a range from the string to format
	// std::cout << std::format("String Center Aligned {}: {:*^50.{}}", "{:*>50.{}}", t0, 10) << "\n";
	// std::cout << std::format("String Left Aligned {}: {:*>50.{}}", "{:*>50.{}}", t0, 15) << "\n";
	// std::cout << std::format("String Right Aligned {}: {:*<50.{}}", "{:*>50.{}}", t0, 5) << "\n\n";

	//***************** First statement passes, second fails - type spec is accounted for *****************
	// std::cout << std::format("String Center Aligned {}: {:*^50.{}s}", "{:*^50.{}s}", t0, 10) << "\n";
	// std::cout << std::format("String Center Aligned {}: {:*^50.{}f}", "{:*^50.{}f}", t0, 10) << "\n\n";

	//****************************** This fails, so strings can't be localized *****************************
	// std::cout << std::format("String Center Aligned {}: {:*^50.{}Ls}", "{:*^50.{}Ls}", t0, 10) << "\n";

	// Not sure why, but these throw a "Missing '}' in format string." message instead of catching the '+'.
	// Possibly because it expects another argument to be present with '+' for it to be valid?
	//
	// std::cout << std::format("String Center Aligned {}: {:*^50+.{}s}", "{:*^50+.{}s}", t0, 10) << "\n";
	// std::cout << std::format("String Center Aligned {}: {:+*^50.{}s}", "{:+*^50.{}s}", t0, 10) << "\n";

	// First one fails with same message as previous failure, but second one fails with a
	// "Format specifier requires numeric argument." message, but '#' is accounted for both
	//
	// std::cout << std::format("String Center Aligned {}: {:*^50#.{}s}", "{:*^50#.{}s}", t0, 10) << "\n";
	// std::cout << std::format("String Center Aligned {}: {:#*^50.{}s}", "{:#*^50.{}s}", t0, 10) << "\n";



	//int t1 {42};
	//std::cout << std::format("{:*^#20x}", t1) << "\n";


}