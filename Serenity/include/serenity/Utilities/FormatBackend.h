#pragma once

#include <serenity/Defines.h>

#if defined USE_STD_FMT && defined WINDOWS_PLATFORM
	#if __cpp_lib_format
		#include <format>
	#elif(_MSC_VER < 1929)
		#error "MSVC's Implementation Of <format> Not Supported On This Compiler Version. Please Use A Newer MSVC Compiler Version (VS 2019 v16.10/ VS 2022 v17.0 Or Later)'"
	#elif(_MSVC_LANG < 202002L)
		#error "MSVC's Implementation Of <format> Not Fully Implemented Prior To C++20. Please Use The  C++ Latest Compiler Flag'"
	#endif
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
	#else
		#error "Unkown Error: Compiler And Language Standard Being Used Should Include <format> Header, But No <format> Header Was Detected"
	#endif
#elif defined USE_FMT_LIB
	#include <fmt/format.h>
	#define CONTEXT                         std::back_insert_iterator<std::basic_string<char>>
	#define VFORMAT_TO(cont, loc, msg, ...) fmt::vformat_to<CONTEXT>(std::back_inserter(cont), loc, msg, fmt::make_format_args(__VA_ARGS__))
#elif defined USE_BUILT_IN_FMT
	#include <serenity/CustomFormat/argfmt_backend.h>
	#define VFORMAT_TO_L(cont, loc, msg, ...) formatter::format_to(std::back_inserter(cont), loc, msg, __VA_ARGS__)
	#define VFORMAT_TO(cont, msg, ...)        formatter::format_to(std::back_inserter(cont), msg, __VA_ARGS__)
#else
	#define VFORMAT_TO(cont, loc, msg, ...) void(0)
	#ifdef WINDOWS_PLATFORM
		#pragma message(                                                                                                                                            \
		"No Formatting Backend Was Defined - You Must Enable The Option Of Using Either <format> Via 'USE_STD_FORMAT', fmtlib Via 'USE_FMT_LIB', Or The Built-in Formatter Via 'USE_BUILT_IN_FMT'")
	#else
		#warning("No Formatting Backend Was Defined - You Must Enable The Option Of Using Either <format> Via 'USE_STD_FORMAT', fmtlib Via 'USE_FMT_LIB', Or The Built-in Formatter Via 'USE_BUILT_IN_FMT'")
	#endif    // WINDOWS_PLATFORM
#endif
