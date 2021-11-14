#pragma once

#include "../Common.h"
#include <string>

template <typename... Args> void TargetBase::trace( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::trace, s, std::make_format_args( args... ) );
}
template <typename... Args> void TargetBase::info( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::info, s, std::make_format_args( args... ) );
}
template <typename... Args> void TargetBase::debug( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::debug, s, std::make_format_args( args... ) );
}
template <typename... Args> void TargetBase::warn( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::warning, s, std::make_format_args( args... ) );
}
template <typename... Args> void TargetBase::error( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::error, s, std::make_format_args( args... ) );
}
template <typename... Args> void TargetBase::fatal( std::string s, Args &&...args )
{
	PrintMessage( LoggerLevel::fatal, s, std::make_format_args( args... ) );
}
