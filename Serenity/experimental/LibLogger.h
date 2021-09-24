#pragma once

#include <serenity/Common.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Sinks/Sinks.h>

#pragma warning( push, 0 )
#include <spdlog/spdlog.h>
#pragma warning( pop )


/*
	Not Much Of A Simplification Of The Logger Class, But This Is An Attempt To Separate The Logic Of
	The Internal Logger From What The Client Should See As The Usage Of The Actual Logger Class
*/

namespace serenity
{
	class InternalLibLogger : public ILogger
	{
	      public:
		explicit InternalLibLogger( internal_logger_info infoStruct = { } );
		InternalLibLogger( )                                = delete;
		InternalLibLogger( const InternalLibLogger &copy )  = delete;
		InternalLibLogger( const InternalLibLogger &&move ) = delete;
		InternalLibLogger &operator=( const InternalLibLogger & ) = delete;
		~InternalLibLogger( )                                     = default;

		static const std::shared_ptr<spdlog::logger> &InternalLogger( )
		{
			return m_internalLogger;
		}
		std::string LogLevelToStr( LoggerLevel level ) override;
		void        CreateInternalLogger( );
		void        EnableInternalLogging( );
		void        DisableInternalLogging( );
		bool        ShouldLog( ) override;

	      private:
		void CustomizeInternalLogger( internal_logger_info infoStruct );

	      private:
		std::atomic<bool>                      loggingEnabled { false };
		std::unique_ptr<Sink>                  m_sinks;
		internal_logger_info                   internalLoggerInfo = { };
		static std::shared_ptr<spdlog::logger> m_internalLogger;
	};

}  // namespace serenity

#if defined( SERENITY_TEST_RUN ) || !defined( NDEBUG )
	#define SE_INTERNAL_TRACE( ... )                                                                                              \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->trace( __VA_ARGS__ );                                           \
			}                                                                                                             \
		}
	#define SE_INTERNAL_DEBUG( ... )                                                                                              \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->debug( __VA_ARGS__ );                                           \
			}                                                                                                             \
		}
	#define SE_INTERNAL_INFO( ... )                                                                                               \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->info( __VA_ARGS__ );                                            \
			}                                                                                                             \
		}
	#define SE_INTERNAL_WARN( ... )                                                                                               \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->warn( __VA_ARGS__ );                                            \
			}                                                                                                             \
		}
	#define SE_INTERNAL_ERROR( ... )                                                                                              \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->error( __VA_ARGS__ );                                           \
			}                                                                                                             \
		}
	#define SE_INTERNAL_FATAL( ... )                                                                                              \
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {                                                               \
			if( InternalLibLogger::ShouldLog( ) ) {                                                                       \
				InternalLibLogger::InternalLogger( )->critical( __VA_ARGS__ );                                        \
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
	#define SE_INTERNAL_TRACE( ... )                      ( void ) 0
	#define SE_INTERNAL_DEBUG( ... )                      ( void ) 0
	#define SE_INTERNAL_INFO( ... )                       ( void ) 0
	#define SE_INTERNAL_WARN( ... )                       ( void ) 0
	#define SE_INTERNAL_ERROR( ... )                      ( void ) 0
	#define SE_INTERNAL_FATAL( ... )                      ( void ) 0
	#define SE_INTERNAL_ASSERT( condition, message, ... ) ( void ) 0
#endif  // SERENITY_TEST_RUN || !NDEBUG