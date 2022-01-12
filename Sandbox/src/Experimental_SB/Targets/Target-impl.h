#pragma once

#include "../Common.h"
#include <string>
#include <chrono>

template <typename... Args> void TargetBase::trace( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::trace ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::trace );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}

template <typename... Args> void TargetBase::info( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::info ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::info );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}

template <typename... Args> void TargetBase::debug( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::debug ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::debug );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}

template <typename... Args> void TargetBase::warn( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::warning ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::warning );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}

template <typename... Args> void TargetBase::error( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::error ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::error );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}

template <typename... Args> void TargetBase::fatal( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::fatal ) {
		using namespace std::chrono;
		auto    now              = msgDetails.MessageTimePoint( );
		seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
		if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
			msgDetails.TimeDetails( ).UpdateCache( now );
		}
		msgDetails.SetMsgLevel( LoggerLevel::fatal );
		msgDetails.SetMessage( s, std::forward<Args>( args )... );
		auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

		if( isWriteToBuf( ) ) {
			internalBuffer.append( formatted.data( ), formatted.size( ) );
			PolicyFlushOn( );
		}
		else {
			PrintMessage( formatted );
			PolicyFlushOn( );
		}
	}
}
