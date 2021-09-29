#pragma once

#include <serenity/Common.h>
#include <serenity/Interfaces/IObserver.h>

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
		explicit InternalLibLogger( se_internal::internal_logger_info infoStruct );
		InternalLibLogger( ) = delete;
		InternalLibLogger( const InternalLibLogger &copy );
		InternalLibLogger( const InternalLibLogger &&move ) = delete;
		InternalLibLogger &operator=( const InternalLibLogger &ref ) = delete;
		~InternalLibLogger( )                                        = default;


		/// <summary>
		///  For Setting Internal Format String -> internalFormatStr Will Overwrite formatStr
		/// </summary>
		void                                    CustomizeInternalLogger( se_internal::internal_logger_info infoStruct );
		const std::shared_ptr<Sink>             sink_info( );
		const std::string                       name( );
		const se_internal::internal_logger_info internal_info( );
		std::string                             LogLevelToStr( LoggerLevel level ) override;
		void                                    SetLogLevel( LoggerLevel logLevel ) override;
		void                                    SetFlushLevel( LoggerLevel flushLevel ) override;

		static void EnableInternalLogging( )
		{
			loggingEnabled = true;
		}
		static void DisableInternalLogging( )
		{
			loggingEnabled = false;
		}

		static const std::shared_ptr<spdlog::logger> &InternalLogger( )
		{
			return m_internalLogger;
		}
		template <typename T, typename... Args> void trace( T message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->trace( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void debug( T message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->debug( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void info( T message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->info( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void warn( T &message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->warn( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void error( T message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->error( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void fatal( T message, Args &&...args )
		{
			if( ShouldLog( ) ) {
				InternalLogger( )->critical( message, std::forward<Args>( args )... );
			}
		}

	      private:
		// Funtionality Offered Via Call To CustomizeInternalLogger()
		void CreateInternalLogger( );
		// Specifically Using This To Just Update The Internal Logger If Customized
		void UpdateInfo( ) override;
		bool ShouldLog( ) override;


	      private:
		static bool                            loggingEnabled;
		bool                                   internalCustomized { false };
		std::shared_ptr<Sink>                  m_sinks;
		se_internal::internal_logger_info      internalLoggerInfo;
		static std::shared_ptr<spdlog::logger> m_internalLogger;
	};
}  // namespace serenity

#ifndef NDEBUG
	#define SE_INTERNAL_ASSERT( condition, message, ... )                                                                         \
		if( !( condition ) ) {                                                                                                \
			SE_INTERNAL_FATAL( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),         \
					   std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                     \
					   ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                           \
			SE_DEBUG_BREAK( );                                                                                            \
		}
#else
	#define SE_INTERNAL_ASSERT( condition, message, ... ) ( void ) 0
#endif  // !NDEBUG