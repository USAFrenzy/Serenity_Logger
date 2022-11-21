#pragma once

#ifdef DOXYGEN_DOCUMENTATION
	/// @brief If _WIN32 is defined, then this is also defined.
    /// @details If this macro is defined, includes the Windows.h and io.h headers
    /// as well as defines ISATTY to _isatty and FILENO to _fileno. If
    /// ENABLE_VIRTUAL_TERMINAL_PROCESSING is not defined, also defines this macro.
	#define WINDOWS_PLATFORM
	/// @brief If __APPLE__ or __MACH__ are defined, then this macro is also
    /// defined.
    /// @details If this macro is defined, includes the unistd.h header and defines
    /// ISATTY to isatty and FILENO to fileno
	#define MAC_PLATFORM
#endif

#ifdef _WIN32
	#define WINDOWS_PLATFORM
#elif defined(__APPLE__) || defined(__MACH__)
	#define MAC_PLATFORM
#else
	#define LINUX_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM
	#ifndef DOXYGEN_DOCUMENTATION
		#define WIN32_LEAN_AND_MEAN
		#define VC_EXTRALEAN
	#endif    // !DOXYGEN_DOCUMENTATION

	#include <Windows.h>
	#include <io.h>

	#define ISATTY _isatty
	#define FILENO _fileno
[[noreturn]] __forceinline constexpr void unreachable() {
	__assume(false);
}
	#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
		#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
	#endif
	#define LOCAL_TIME(tmStruct, timeT) localtime_s(&tmStruct, &timeT)
	#define GM_TIME(tmStruct, timeT)    gmtime_s(&tmStruct, &timeT)
#else
	#include <unistd.h>
	#define ISATTY                      isatty
	#define FILENO                      fileno
[[noreturn]] inline __attribute__((always_inline)) constexpr void unreachable() {
	__builtin_unreachable();
}
	#define LOCAL_TIME(tmStruct, timeT) localtime_r(&tmStruct, &timeT)
	#define GM_TIME(tmStruct, timeT)    gmtime_r(&tmStruct, &timeT)
#endif

#define KB                  (1024)
#define MB                  (1024 * KB)
#define GB                  (1024 * MB)
#define DEFAULT_BUFFER_SIZE (64 * KB)    // used for file buffers

#ifdef _DEBUG
	#ifndef SE_ASSERT
		#define SE_ASSERT(condition, message)                                                                                                                       \
			if( !(condition) ) {                                                                                                                                    \
					fprintf(stderr, "Assertion Failed: (%s) |File: %s | Line: %i\nMessage:%s\n", #condition, __FILE__, __LINE__, message);                          \
					abort();                                                                                                                                        \
			}
	#endif
#else
	#ifndef SE_ASSERT
		#define SE_ASSERT(condition, message) void(0)
	#endif
#endif