#include <serenity/Targets/Target.h>

namespace serenity::expiremental::targets
{
	TargetBase::TargetBase( )
	  : toBuffer( false ),
		policy( Flush::never ),
		logLevel( LoggerLevel::trace ),
		msgLevel( LoggerLevel::trace ),
		pattern( "|%l| %x %n %T [%N]: %+" ),
		msgDetails( "Base Logger", msgLevel, message_time_mode::local ),
		msgPattern( pattern, &msgDetails )
	{
	}

	TargetBase::TargetBase( std::string_view name )
	  : toBuffer( false ),
		policy( Flush::never ),
		logLevel( LoggerLevel::trace ),
		msgLevel( LoggerLevel::trace ),
		pattern( "|%l| %x %n %T [%N]: %+" ),
		msgDetails( name, msgLevel, message_time_mode::local ),
		msgPattern( pattern, &msgDetails )
	{
	}

	TargetBase::TargetBase( std::string_view name, std::string_view fmtPattern )
	  : toBuffer( false ),
		policy( Flush::never ),
		logLevel( LoggerLevel::trace ),
		msgLevel( LoggerLevel::trace ),
		pattern( fmtPattern ),
		msgDetails( name, msgLevel, message_time_mode::local ),
		msgPattern( pattern, &msgDetails )
	{
	}

	void TargetBase::WriteToBaseBuffer( bool fmtToBuf )
	{
		toBuffer = fmtToBuf;
	}

	bool TargetBase::isWriteToBuf( )
	{
		return toBuffer;
	}

	std::string *TargetBase::Buffer( )
	{
		return &internalBuffer;
	}

	void TargetBase::SetPattern( std::string_view pattern )
	{
		msgPattern.SetPattern( std::string { pattern.data( ), pattern.size( ) } );
	}

	void TargetBase::SetFlushPolicy( Flush_Policy pPolicy )
	{
		policy = pPolicy;
	}

	Flush_Policy &TargetBase::Policy( )
	{
		return policy;
	}

	const std::string TargetBase::LoggerName( )
	{
		return msgDetails.Name( );
	}

	msg_details::Message_Formatter *TargetBase::MsgFmt( )
	{
		return &msgPattern;
	}

	msg_details::Message_Info *TargetBase::MsgInfo( )
	{
		return &msgDetails;
	}

	void TargetBase::ResetPatternToDefault( )
	{
		msgPattern.SetPattern( "|%l| %x %n %T [%N]: %+" );
	}
	void TargetBase::SetLogLevel( LoggerLevel level )
	{
		logLevel = level;
	}
	const LoggerLevel TargetBase::Level( )
	{
		return logLevel;
	}

	void TargetBase::SetLoggerName( std::string_view name )
	{
		msgDetails.SetName( name );
	}

}  // namespace serenity::expiremental::targets
