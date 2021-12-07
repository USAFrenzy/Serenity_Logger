#pragma once

#include "../Common.h"
#include <string>
#include <chrono>

template <typename... Args> void TargetBase::trace( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::trace ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::trace );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}


template <typename... Args> void TargetBase::info( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::info ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::info );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}


template <typename... Args> void TargetBase::debug( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::debug ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::debug );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}


template <typename... Args> void TargetBase::warn( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::warning ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::warning );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}


template <typename... Args> void TargetBase::error( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::error ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::error );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}


template <typename... Args> void TargetBase::fatal( std::string_view s, Args &&...args )
{
	if( logLevel <= LoggerLevel::fatal ) {
		std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
		static std::string   preFormat;
		static std::string   msg;
		preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
		msg.reserve( preFormat.capacity( ) + s.size( ) );
		msgDetails.SetMsgLevel( LoggerLevel::fatal );

		if( messageTimePoint != lastLogTime ) {
			lastLogTime = messageTimePoint;
			preFormat.clear( );
			preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
		}
		msg.clear( );
		msg = std::move( concatToStr( preFormat, s ).append( "\n" ) );
		internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
		std::format_to( std::back_inserter( internalBuffer ), std::move( msg ), std::forward<Args>( args )... );
		if( !isWriteToBuf( ) ) {
			PrintMessage( internalBuffer );
			internalBuffer.clear( );
		}
	}
}
