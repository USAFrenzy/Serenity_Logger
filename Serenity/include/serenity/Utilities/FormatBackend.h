#pragma once

#include <serenity/Defines.h>

#if defined USE_STD_FMT && defined WINDOWS_PLATFORM
	#if __cpp_lib_format
		#include <serenity/CustomFormat/stdfmt_backend.h>
		#define VFORMAT_TO(cont, msg, ...)        std::vformat_to(std::back_inserter(cont), msg, std::make_format_args(__VA_ARGS__))
		#define VFORMAT_TO_L(cont, loc, msg, ...) std::vformat_to(std::back_inserter(cont), loc, msg, std::make_format_args(__VA_ARGS__))
	#else
		#error "Unkown Error: Compiler And Language Standard Being Used Should Include <format> Header, But No <format> Header Was Detected"
	#endif
#elif defined USE_FMT_LIB
	#include <serenity/CustomFormat/fmtlib_backend.h>
	#define VFORMAT_TO(cont, msg, ...)        fmt::vformat_to(std::back_inserter(cont), msg, fmt::make_format_args(__VA_ARGS__))
	#define VFORMAT_TO_L(cont, loc, msg, ...) fmt::vformat_to(std::back_inserter(cont), loc, msg, fmt::make_format_args(__VA_ARGS__))
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
