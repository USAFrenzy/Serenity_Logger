#pragma once
#include <serenity/Common.h>
template <typename T, typename... Args> void Logger::se_trace( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::trace ) {
			Logger::ClientSideLogger( )->trace( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_debug( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::debug ) {
			Logger::ClientSideLogger( )->debug( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_info( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::info ) {
			Logger::ClientSideLogger( )->info( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_warn( T &message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::warn ) {
			Logger::ClientSideLogger( )->warn( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_error( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::err ) {
			Logger::ClientSideLogger( )->error( message, std::forward<Args>( args )... );
		}
	}
}

template <typename T, typename... Args> void Logger::se_fatal( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		if( ToMappedLevel( LoggerLevel( ) ) <= MappedLevel::critical ) {
			Logger::ClientSideLogger( )->critical( message, std::forward<Args>( args )... );
		}
	}
}
