#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <serenity/Sinks/Sinks.h>
#include <serenity/Common.h>

namespace serenity
{
	class LibLogger
	{
	      public:
		explicit LibLogger( std::string name );
		std::shared_ptr<spdlog::logger>        InitLibLogger( logger_info infoStruct );
		void                                   CreateLibLogger( );
		const std::shared_ptr<spdlog::logger> &InternalLogger( );
		const std::string                      name( );
		void                                   Shutdown( );

		// I Feel Like There's Got To Be A Way To Separate Templated Functions And Their Implementations
		template <typename T, typename... Args> void se_trace( T &message, Args... args )
		{
			if( InternalLogger( ) != nullptr ) {
				InternalLogger( )->trace( fmt::format( message, std::forward<Args>( args )... ) );
			}
		}
		template <typename T, typename... Args> void se_debug( T &message, Args... args )
		{
			if( LibLogger::InternalLogger( ) != nullptr ) {
				LibLogger::InternalLogger( )->debug( fmt::format( message, std::forward<Args>( args )... ) );
			}
		}
		template <typename T, typename... Args> void se_info( T &message, Args... args )
		{
			if( LibLogger::InternalLogger( ) != nullptr ) {
				LibLogger::InternalLogger( )->info( message, std::forward<Args>( args )... );
			}
		}
		template <typename T, typename... Args> void se_warn( T &message, Args... args )
		{
			if( LibLogger::InternalLogger( ) != nullptr ) {
				LibLogger::InternalLogger( )->warn( fmt::format( message, std::forward<Args>( args )... ) );
			}
		}
		template <typename T, typename... Args> void se_error( T &message, Args... args )
		{
			if( LibLogger::InternalLogger( ) != nullptr ) {
				LibLogger::InternalLogger( )->error( fmt::format( message, std::forward<Args>( args )... ) );
			}
		}
		template <typename T, typename... Args> void se_fatal( T &message, Args... args )
		{
			if( LibLogger::InternalLogger( ) != nullptr ) {
				LibLogger::InternalLogger( )->critical( fmt::format( message, std::forward<Args>( args )... ) );
			}
		}
		void se_assert( void *condition, std::string message, ... );

	      private:
		std::shared_ptr<spdlog::logger> m_internalLogger;
		logger_info                     internalLoggerInfo = { };
		Sink                            m_sinks;
		std::string                     m_name;
	};
}  // namespace serenity
