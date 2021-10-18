#pragma once

// Very Simple And Basic Platform Detection
#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __NT__ )
	#define PLATFORM_WINDOWS
#elif __linux__
	#define PLATFORM_LINUX
#endif


#define INTERNAL_DEFAULT_NAME "SERENITY"
#define INTERNAL_DEFAULT_LOG  "Internal_Log.txt"
#define DEFAULT_LOGGER_NAME   "Logger"
#define DEFAULT_LOG           "Log.txt"
#define SE_EXPAND_MACRO( x )  x
#define SE_MACRO_STRING( x )  #x

#ifndef NDEBUG
	#if defined( WIN32 ) || defined( __WIN32 ) || defined( __WIN32__ )
		#define SE_DEBUG_BREAK __debugbreak
	#elif __APPLE__
		#define SE_DEBUG_BREAK __builtin_debugtrap
	#elif __linux__
		#define SE_DEBUG_BREAK __builtin_trap
	#else
		#define SE_DEBUG_BREAK ( void ) 0
	#endif  // Platform Debug Break Defines

// Formatting of Message to be used by both
	#define SE_ASSERT_VAR_MSG( message, ... ) fmt::format( message, __VA_ARGS__ )
#else
	#define SE_ASSERT_VAR_MSG( message, ... ) ( void ) 0
#endif  // NDEBUG

#define SE_NULL_PTR nullptr
