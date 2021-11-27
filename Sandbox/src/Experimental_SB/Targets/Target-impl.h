#pragma once

#include "../Common.h"
#include <string>

template <typename... Args> void TargetBase::trace( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::trace ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::trace );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
template <typename... Args> void TargetBase::info( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::info ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::info );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
template <typename... Args> void TargetBase::debug( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::debug ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::debug );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
template <typename... Args> void TargetBase::warn( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::warning ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::warning );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
template <typename... Args> void TargetBase::error( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::error ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::error );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
template <typename... Args> void TargetBase::fatal( std::string_view s, Args &&...args )
{
	if( Level( ) <= LoggerLevel::fatal ) {
		MsgInfo( )->SetMsgLevel( LoggerLevel::fatal );
		PrintMessage( msgDetails, s, std::make_format_args( args... ) );
	}
}
