#include "Target.h"

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			TargetBase::TargetBase( )
			  : pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( "Base Logger", msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    msgLevel( LoggerLevel::trace )
			{
			}

			TargetBase::TargetBase( std::string_view name )
			  : pattern( "|%l| %x %n %T [%N]: " ),
			    msgDetails( std::move( svToString( name ) ), msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    msgLevel( LoggerLevel::trace )
			{
			}

			TargetBase::TargetBase( std::string_view name, std::string_view msgPattern )
			  : pattern( std::move( svToString( msgPattern ) ) ),
			    msgDetails( std::move( svToString( name ) ), msgLevel, message_time_mode::local ),
			    msgPattern( pattern, &msgDetails ),
			    msgLevel( LoggerLevel::trace )
			{
			}
			void TargetBase::SetPattern( std::string_view pattern )
			{
				msgPattern.SetPattern( pattern );
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

		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity