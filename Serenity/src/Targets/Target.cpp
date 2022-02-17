#include <serenity/Targets/Target.h>

namespace serenity::targets {
	constexpr const char *DEFAULT_PATTERN = "|%l| %x %n %T [%N]: %+";

	TargetBase::TargetBase( )
		: toBuffer( false ), policy( serenity::experimental::FlushSetting::never ), logLevel( LoggerLevel::trace ),
		  msgLevel( LoggerLevel::trace ), pattern( DEFAULT_PATTERN ), msgDetails( "Base Logger", msgLevel, message_time_mode::local ),
		  msgPattern( pattern, &msgDetails ), multiThreadSupport( false )
	{
	}

	TargetBase::TargetBase( std::string_view name )
		: toBuffer( false ), policy( serenity::experimental::FlushSetting::never ), logLevel( LoggerLevel::trace ),
		  msgLevel( LoggerLevel::trace ), pattern( DEFAULT_PATTERN ), msgDetails( name, msgLevel, message_time_mode::local ),
		  msgPattern( pattern, &msgDetails ), multiThreadSupport( false )
	{
	}

	TargetBase::TargetBase( std::string_view name, std::string_view fmtPattern )
		: toBuffer( false ), policy( serenity::experimental::FlushSetting::never ), logLevel( LoggerLevel::trace ),
		  msgLevel( LoggerLevel::trace ), pattern( fmtPattern ), msgDetails( name, msgLevel, message_time_mode::local ),
		  msgPattern( pattern, &msgDetails ), multiThreadSupport( false)
	{
	}

	void TargetBase::WriteToBaseBuffer( bool fmtToBuf )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		toBuffer = fmtToBuf;
	}

	const bool TargetBase::isWriteToBuf( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return toBuffer;
	}

	std::string *const TargetBase::Buffer( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return &internalBuffer;
	}

	void TargetBase::SetPattern( std::string_view pattern )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		msgPattern.SetPattern( std::string { pattern.data( ), pattern.size( ) } );
	}

	void TargetBase::SetFlushPolicy( const serenity::experimental::Flush_Policy &pPolicy )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		policy = pPolicy;
	}

	const serenity::experimental::Flush_Policy &TargetBase::Policy( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return policy;
	}

	const std::string TargetBase::LoggerName( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return msgDetails.Name( );
	}

	msg_details::Message_Formatter *TargetBase::MsgFmt( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return &msgPattern;
	}

	msg_details::Message_Info *TargetBase::MsgInfo( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return &msgDetails;
	}

	void TargetBase::ResetPatternToDefault( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		msgPattern.SetPattern( DEFAULT_PATTERN );
	}
	void TargetBase::SetLogLevel( LoggerLevel level )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		logLevel = level;
	}
	const LoggerLevel TargetBase::Level( )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		return logLevel;
	}

	void TargetBase::SetLoggerName( std::string_view name )
	{
		if( multiThreadSupport ) std::scoped_lock lock( mtMutex );
		msgDetails.SetName( name );
	}

	void TargetBase::EnableMultiThreadingSupport(bool enableMultiThreading) {
		std::scoped_lock lock( mtMutex );
		multiThreadSupport = enableMultiThreading;
	}

	bool TargetBase::isMTSupportEnabled() {
		std::scoped_lock lock( mtMutex );
		return multiThreadSupport;
	}
}  // namespace serenity::targets
