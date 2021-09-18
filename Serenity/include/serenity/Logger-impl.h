#pragma once


template <typename T, typename... Args> void Logger::se_trace( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->trace( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_debug( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->debug( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_info( T message, Args &&...args )
{
	if(  Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->info( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_warn( T &message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->warn( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_error( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->error( message, std::forward<Args>( args )... );
	}
}

template <typename T, typename... Args> void Logger::se_fatal( T message, Args &&...args )
{
	if( Logger::ClientSideLogger( ) != nullptr ) {
		Logger::ClientSideLogger( )->critical( message, std::forward<Args>( args )... );
	}
}
