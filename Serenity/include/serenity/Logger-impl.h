#pragma once
#include <serenity/Common.h>
#pragma warning( push, 0 )

template <typename T, typename... Args> void Logger::se_trace( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::trace ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->trace( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_debug( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::debug ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->debug( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_info( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::info ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->info( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_warn( T &message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::warn ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->warn( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_error( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::err ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->error( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_fatal( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::critical ) && ( GetLogLevel( ) <= GetGlobalLevel( ) ) ) {
			Logger::ClientSideLogger( )->critical( message, std::forward<Args>( args )... );
		}
	}
}
#pragma warning( pop )