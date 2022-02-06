#pragma once
#include <serenity/Common.h>
#pragma warning( push, 0 )

template <typename T, typename... Args> void Logger::se_trace( T message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::trace ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->trace( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_debug( T message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::debug ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->debug( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_info( T message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::info ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->info( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_warn( T &message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::warn ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->warn( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_error( T message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::err ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->error( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_fatal( T message, Args &&...args )
{
	if( ( ToMappedLevel( GetLogLevel( ) ) <= MappedLevel::critical ) && ( ShouldLog( ) ) ) {
		Logger::ClientSideLogger( )->critical( message, std::forward<Args>( args )... );
	}
}
#pragma warning( pop )