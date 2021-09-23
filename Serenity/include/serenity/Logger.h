#pragma once

#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Sinks/Sinks.h>

#pragma warning( push, 0 )
#include <spdlog/fmt/ostr.h>
#pragma warning( pop )

namespace serenity
{
	static LoggerLevel  global_level { LoggerLevel::trace };
	std::string         GetSerenityVerStr( );
	void                SetGlobalLevel( LoggerLevel level );
	static LoggerLevel &GetGlobalLevel( )
	{
		return global_level;
	}

	class Logger : public ILogger
	{
	      public:
		explicit Logger( logger_info &infoStruct );
		Logger( )                     = delete;
		Logger( const Logger &copy )  = delete;
		Logger( const Logger &&move ) = delete;
		Logger &operator=( const Logger & ) = delete;
		~Logger( );

		static const std::shared_ptr<spdlog::logger> &ClientSideLogger( )
		{
			return m_clientLogger;
		}
		static const std::unique_ptr<LogFileHelper> &FileHelperHandle( )
		{
			return logFileHandle;
		}
		void                                         StartLogger( );
		void                                         StopLogger( );
		void                                         DropLogger( );
		void                                         Shutdown( );
		void                                         SetLogLevel( LoggerLevel logLevel, LoggerInterface logInterface );
		const LoggerLevel                            GetLogLevel( );
		std::string                                  LogLevelToStr( LoggerLevel level );
		std::string const                            LoggerName( );
		bool                                         RenameLog( std::string newName, bool replaceIfExists = true );
		void                                         UpdateFileInfo( ) override;
		void                                         OpenLog( file_helper::path filePath );
		void                                         CloseLog( file_helper::path filePath );
		template <typename T, typename... Args> void se_trace( T message, Args &&...args );
		template <typename T, typename... Args> void se_debug( T message, Args &&...args );
		template <typename T, typename... Args> void se_info( T message, Args &&...args );
		template <typename T, typename... Args> void se_warn( T &message, Args &&...args );
		template <typename T, typename... Args> void se_error( T message, Args &&...args );
		template <typename T, typename... Args> void se_fatal( T message, Args &&...args );
		/*
			------------------------------------ Work In Progress ------------------------------------
		*/
		// Plan Is To Work On These Type Functions, Abstract Away To LibLogger Class, And Have A
		// LibLogger Handle In The Logger Class. Doing So Will Also Allow Other Files To Use LibLogger
		// As I Plan On Making LibLogger A Singleton Class With The Handles Pointing To The Same Instance
		void                                          CustomizeInternalLogger( internal_logger_info &infoStruct ) { }
		void                                          CreateInternalLogger( logger_info &infoStruct );
		void                                          EnableInternalLogging( ) { }
		void                                          DisableInternalLogging( ) { }
		bool                                          InternalShouldLog( );
		bool                                          ClientShouldLog( );
		static const std::shared_ptr<spdlog::logger> &InternalLogger( )
		{
			return m_internalLogger;
		}
		// MappedLevel                                   ToMappedLevel( LoggerLevel level );
		// LoggerLevel                                   ToLogLevel( MappedLevel level );
		// -----------------------------------------------------------------------------------------
	      private:
		logger_info                            initInfo           = { };
		logger_info                            internalLoggerInfo = { };
		static std::shared_ptr<spdlog::logger> m_internalLogger;
		static std::shared_ptr<spdlog::logger> m_clientLogger;
		static std::unique_ptr<LogFileHelper>  logFileHandle;
		std::unique_ptr<Sink>                  m_sinks;


	      private:
		std::shared_ptr<spdlog::logger> CreateLogger( logger_info &infoStruct, bool internalLogger = false );
	};
#include <serenity/Logger-impl.h>
}  // namespace serenity


#if defined( SERENITY_TEST_RUN ) || !defined( NDEBUG )
// Client Side
	#define SE_ASSERT( condition, message, ... )                                                                                  \
		if( !( condition ) ) {                                                                                                \
			Logger::se_fatal( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),          \
					  std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                      \
					  ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                            \
			SE_DEBUG_BREAK( );                                                                                            \
		}
// Internal macros
	#define SE_INTERNAL_TRACE( ... )                                                                                              \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->trace( __VA_ARGS__ );                                                      \
			}                                                                                                             \
		}
	#define SE_INTERNAL_DEBUG( ... )                                                                                              \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->debug( __VA_ARGS__ );                                                      \
			}                                                                                                             \
		}
	#define SE_INTERNAL_INFO( ... )                                                                                               \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->info( __VA_ARGS__ );                                                       \
			}                                                                                                             \
		}
	#define SE_INTERNAL_WARN( ... )                                                                                               \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->warn( __VA_ARGS__ );                                                       \
			}                                                                                                             \
		}
	#define SE_INTERNAL_ERROR( ... )                                                                                              \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->error( __VA_ARGS__ );                                                      \
			}                                                                                                             \
		}
	#define SE_INTERNAL_FATAL( ... )                                                                                              \
		if( Logger::InternalLogger( ) != nullptr ) {                                                                          \
			if( Logger::InternalShouldLog( ) ) {                                                                          \
				Logger::InternalLogger( )->critical( __VA_ARGS__ );                                                   \
			}                                                                                                             \
		}
	#define SE_INTERNAL_ASSERT( condition, message, ... )                                                                         \
		if( !( condition ) ) {                                                                                                \
			SE_INTERNAL_FATAL( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),         \
					   std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                     \
					   ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                           \
			SE_DEBUG_BREAK( );                                                                                            \
		}
#else
	#define SE_ASSERT_VAR_MSG( message, ... )             ( void ) 0
	#define SE_INTERNAL_TRACE( ... )                      ( void ) 0
	#define SE_INTERNAL_DEBUG( ... )                      ( void ) 0
	#define SE_INTERNAL_INFO( ... )                       ( void ) 0
	#define SE_INTERNAL_WARN( ... )                       ( void ) 0
	#define SE_INTERNAL_ERROR( ... )                      ( void ) 0
	#define SE_INTERNAL_FATAL( ... )                      ( void ) 0
	#define SE_INTERNAL_ASSERT( condition, message, ... ) ( void ) 0
	#define SE_ASSERT( condition, message, ... )          ( void ) 0
#endif  // SERENITY_TEST_RUN || !NDEBUG