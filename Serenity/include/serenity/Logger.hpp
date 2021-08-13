#pragma once

#include <memory>

#pragma warning(push)
#pragma warning(disable : 26812)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "Defines.hpp"
#include "Common.hpp"
// Just an Initial Test function
void PrintHello( );
// currently in global namespace
using MappedLevel = spdlog::level::level_enum;


class Logger
{
      public:
	Logger( );
	~Logger( );
	static void Init(std::string fileName, LoggerLevel level);
	void SetLoggerLevel(LoggerLevel logLevel, LoggerInterface logInterface);
	static std::shared_ptr<spdlog::logger>& GetInternalLogger( )
	{
		return m_internalLogger;
	}
	static std::shared_ptr<spdlog::logger>& GetClientSideLogger( )
	{
		return m_clientLogger;
	}

      private:
	inline static MappedLevel MapLogLevel(LoggerLevel level);
	static std::shared_ptr<spdlog::logger> m_internalLogger;
	static std::shared_ptr<spdlog::logger> m_clientLogger;
	MappedLevel m_level {MappedLevel::off};
};




namespace serenity {
	std::string GetSerenityVerStr( );
} // namespace serenity


// Internal or "non-user" side macros
	#define SE_INTERNAL_TRACE(...)                                                                            \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->trace(__VA_ARGS__);                                         \
		}
	#define SE_INTERNAL_DEBUG(...)                                                                            \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->debug(__VA_ARGS__);                                         \
		}
	#define SE_INTERNAL_INFO(...)                                                                             \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->info(__VA_ARGS__);                                          \
		}
	#define SE_INTERNAL_WARN(...)                                                                             \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->warn(__VA_ARGS__);                                          \
		}
	#define SE_INTERNAL_ERROR(...)                                                                            \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->error(__VA_ARGS__);                                         \
		}
	#define SE_INTERNAL_FATAL(...)                                                                            \
		if(Logger::GetInternalLogger( ) != nullptr) {                                                     \
			Logger::GetInternalLogger( )->critical(__VA_ARGS__);                                      \
		}
	#define SE_INTERNAL_ASSERT(condition, message, ...)                                                       \
		if(!(condition)) {                                                                                \
			SE_INTERNAL_FATAL("ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}",                    \
					  SE_MACRO_STRING(condition),                                             \
					  std::filesystem::path(__FILE__).filename( ).string( ),                  \
					  (__LINE__),                                                             \
					  (SE_ASSERT_VAR_MSG(message, __VA_ARGS__)));                             \
			SE_DEBUG_BREAK                                                                            \
		}

#ifndef NDEBUG
// Client side macros
	#define SE_TRACE(...)                                                                                     \
		if(Logger::GetClientSideLogger( ) != nullptr) {                                                   \
			Logger::GetClientSideLogger( )->trace(__VA_ARGS__);                                       \
		}
	#define SE_DEBUG(...)                                                                                     \
		if(Logger::GetClientSideLogger( ) != nullptr) {                                                   \
			Logger::GetClientSideLogger( )->debug(__VA_ARGS__);                                       \
		}
	#define SE_INFO(...)                                                                                      \
		if(Logger::GetInternaGetClientSideLogger( ) != nullptr) {                                         \
			Logger::GetClientSideLogger( )->info(__VA_ARGS__);                                        \
		}
	#define SE_WARN(...)                                                                                      \
		if(Logger::GetClientSideLogger( ) != nullptr) {                                                   \
			Logger::GetClientSideLogger( )->warn(__VA_ARGS__);                                        \
		E
	#define SE_ERROR(...)                                                                                     \
		if(Logger::GetClientSideLogger( ) != nullptr) {                                                   \
			Logger::GetClientSideLogger( )->error(__VA_ARGS__);                                       \
		}
	#define SE_FATAL(...)                                                                                     \
		if(Logger::GetClientSideLogger( ) != nullptr) {                                                   \
			Logger::GetClientSideLogger( )->critical(__VA_ARGS__);                                    \
		}
	#define SE_ASSERT(condition, message, ...)                                                                \
		if(!(condition)) {                                                                                \
			SE_FATAL("ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}",                             \
				 SE_MACRO_STRING(condition),                                                      \
				 std::filesystem::path(__FILE__).filename( ).string( ),                           \
				 (__LINE__),                                                                      \
				 (SE_ASSERT_VAR_MSG(message, __VA_ARGS__)));                                      \
			SE_DEBUG_BREAK                                                                            \
		}

#else
	#define SE_ASSERT_VAR_MSG(message, ...)             ( void ) 0
	#define SERENITY_TRACE(...)                         ( void ) 0
	#define SERENITY_DEBUG(...)                         ( void ) 0
	#define SERENITY_INFO(...)                          ( void ) 0
	#define SERENITY_WARN(...)                          ( void ) 0
	#define SERENITY_ERROR(...)                         ( void ) 0
	#define SERENITY_FATAL(...)                         ( void ) 0
	#define SE_INTERNAL_ASSERT(condition, message, ...) ( void ) 0
	#define SE_TRACE(...)                               ( void ) 0
	#define SE_DEBUG(...)                               ( void ) 0
	#define SE_INFO(...)                                ( void ) 0
	#define SE_WARN(...)                                ( void ) 0
	#define SE_ERROR(...)                               ( void ) 0
	#define SE_FATAL(...)                               ( void ) 0
	#define SE_ASSERT(condition, message, ...)          ( void ) 0
	#endif // NDEBUG