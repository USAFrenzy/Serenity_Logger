#pragma once
#include <filesystem>


#define LOGGER_DEFAULT_NAME                             "SERENITY"
#define INTERFACE_INTERNAL                              50
#define INTERFACE_CLIENT                                51
#define SERENITY_MAJOR                                  0
#define SERENITY_MINOR                                  1
#define SERENITY_REV                                    0
#define VERSION_STRING_FORMAT( major, minor, revision ) #major "." #minor "." #revision
#define VERSION_NUMBER( maj, min, rev )                 VERSION_STRING_FORMAT( maj, min, rev )
#define SE_EXPAND_MACRO( x )                            x
#define SE_MACRO_STRING( x )                            #x

#ifndef NDEBUG
	#if defined( WIN32 ) || defined( __WIN32 ) || defined( __WIN32__ )
		#define SE_DEBUG_BREAK __debugbreak( );
	#elif __APPLE__
		#define SE_DEBUG_BREAK __builtin_debugtrap( );
	#elif __linux__
		#define SE_DEBUG_BREAK __builtin_trap( );
	#else
		#define SE_DEBUG_BREAK ( void ) 0
	#endif  // Platform Debug Break Defines

	// Formatting of Message to be used by both
	#define SE_ASSERT_VAR_MSG( message, ... ) fmt::format( message, __VA_ARGS__ )

#endif  // NDEBUG

#define SE_NULL_PTR     nullptr
#define SE_OPT_NULL_PTR ( std::filesystem::path ) nullptr
#define SE_NULL_OPTION  std::nullopt


// This Needs Work
// namespace serenity {
//	namespace se_exception {
//#include <exception>
//#include <stdexcept>
//
//#define SE_EXCEPTION(code)  code
//#define SE_THROW(exception) throw(SE_EXCEPTION(exception))
//#define SE_NESTED_CATCH(ex) se_nested_catch(ex)
//
//		static std::exception_ptr eptr;
//
//		void se_nested_catch(std::exception_ptr ex_ptr)
//		{
//			try {
//				if(ex_ptr) {
//					std::rethrow_exception(ex_ptr);
//				}
//			} catch(const std::exception& e) {
//				fmt::format("EXCEPTION CAUGHT: {}", e.what( ));
//			}
//		}
//	} // namespace se_exception
//} // namespace serenity
