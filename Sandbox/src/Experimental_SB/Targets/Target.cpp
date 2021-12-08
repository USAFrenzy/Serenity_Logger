#include "Target.h"

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			TargetBase::TargetBase( )
			  : toBuffer( false ),
			    policy( ),
			    logLevel( LoggerLevel::trace ),
			    msgLevel( LoggerLevel::trace ),
			    pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( "Base Logger", msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails )
			{
				internalBuffer.reserve( BUFFER_SIZE );
				base_futures.reserve( 512 );
				base_futures.clear( );
				internalBuffer.clear( );
			}

			TargetBase::TargetBase( std::string_view name )
			  : toBuffer( false ),
			    policy( ),
			    logLevel( LoggerLevel::trace ),
			    msgLevel( LoggerLevel::trace ),
			    pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( std::move( svToString( name ) ), msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails )
			{
				internalBuffer.reserve( BUFFER_SIZE );
				base_futures.reserve( 512 );
				base_futures.clear( );
				internalBuffer.clear( );
			}

			TargetBase::TargetBase( std::string_view name, std::string_view fmtPattern )
			  : toBuffer( false ),
			    policy( ),
			    logLevel( LoggerLevel::trace ),
			    msgLevel( LoggerLevel::trace ),
			    pattern( std::move( svToString( fmtPattern ) ) ),
			    msgDetails( std::move( svToString( name ) ), msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails )
			{
				internalBuffer.reserve( BUFFER_SIZE );
				base_futures.reserve( 512 );
				base_futures.clear( );
				internalBuffer.clear( );
			}

			TargetBase::~TargetBase( ) { }

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
				msgPattern.SetPattern( pattern );
			}

			void TargetBase::SetFlushPolicy( Flush_Policy policy )
			{
				this->policy = policy;
			}

			const Flush_Policy TargetBase::FlushPolicy( )
			{
				return policy;
			}

			std::string TargetBase::LoggerName( )
			{
				return loggerName;
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
				msgPattern.SetPattern( "|%l| %x %n %T [%N]: " );
			}
			void TargetBase::SetLogLevel( LoggerLevel level )
			{
				logLevel = level;
			}
			LoggerLevel TargetBase::Level( )
			{
				return logLevel;
			}

		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity