#include "Message_Pattern.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Pattern::Message_Pattern( std::string formatPattern, Message_Info *msgDetails )
			  : fmtPattern( formatPattern ), msgInfo( msgDetails )
			{
			}
			Message_Pattern &Message_Pattern::operator=( const Message_Pattern &t )
			{
				buffer     = t.buffer;
				fmtPattern = t.fmtPattern;
				msgInfo    = t.msgInfo;
				return *this;
			}

			std::string Message_Pattern::GetMsgFmt( )
			{
				return fmtPattern;
			}

			std::string Message_Pattern::FlagFormatter( char flag )
			{
				switch( flag ) {
					case 'b': return Format_Arg_b( ); break;
					case 'B': return Format_Arg_B( ); break;
					case 'd': return Format_Arg_d( ); break;
					case 'D': return Format_Arg_D( ); break;
					case 'F': return Format_Arg_F( ); break;
					case 'H': return Format_Arg_H( ); break;
					case 'L': return Format_Arg_L( ); break;
					case 'l': return Format_Arg_l( ); break;
					case 'M': return Format_Arg_M( ); break;
					case 'n': return Format_Arg_n( ); break;
					case 'N': return Format_Arg_N( ); break;
					case 'S': return Format_Arg_S( ); break;
					case 'T': return Format_Arg_T( ); break;
					case 'w': return Format_Arg_w( ); break;
					case 'x': return Format_Arg_x( ); break;
					case 'X': return Format_Arg_X( ); break;
					case 'y': return Format_Arg_y( ); break;
					case 'Y': return Format_Arg_Y( ); break;
					// if arg after "%" isn't a flag handled here, do nothing
					default: return ""; break;
				}
			}

			// TODO: Finish Flag Argument Formatters

			std::string Message_Pattern::Format_Arg_b( )
			{
				return svToString( msgInfo->TimeDetails( ).Cache( ).short_month );
			}

			std::string Message_Pattern::Format_Arg_B( )
			{
				return svToString( msgInfo->TimeDetails( ).Cache( ).long_month );
			}

			std::string Message_Pattern::Format_Arg_D( )
			{
				// MM/DD/YY
				std::string date;
				date.append( Format_Arg_d( ) + "/" );
				date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).day ) + "/" );
				return date.append( Format_Arg_y( ) );
			}

			std::string Message_Pattern::Format_Arg_F( )
			{
				// YYYY-MM-DD
				std::string date;
				date.append( Format_Arg_Y( ) + "-" );
				date.append( Format_Arg_d( ) + "-" );
				return date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).day ) );
			}

			std::string Message_Pattern::Format_Arg_M( )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateTimeInfo( ).min );
			}

			std::string Message_Pattern::Format_Arg_S( )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateTimeInfo( ).sec );
			}

			// Seconds Conter Not Incrementing

			std::string Message_Pattern::Format_Arg_T( )
			{
				auto cache = msgInfo->TimeDetails( ).UpdateTimeInfo( );
				// HH:MM:SS
				std::string time;
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) + ":";
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) + ":";
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec );
				return time;
			}

			std::string Message_Pattern::Format_Arg_w( )
			{
				return std::to_string( msgInfo->TimeDetails( ).Cache( ).dec_wkday );
			}

			std::string Message_Pattern::Format_Arg_y( )
			{
				return std::to_string( msgInfo->TimeDetails( ).Cache( ).short_year );
			}

			std::string Message_Pattern::Format_Arg_Y( )
			{
				return std::to_string( msgInfo->TimeDetails( ).Cache( ).long_year );
			}

			std::string Message_Pattern::Format_Arg_N( )
			{
				return msgInfo->Name( );
			}

			std::string Message_Pattern::Format_Arg_l( )
			{
				return svToString( MsgLevelToShortString( msgInfo->MsgLevel( ) ) );
			}

			std::string Message_Pattern::Format_Arg_L( )
			{
				return svToString( MsgLevelToString( msgInfo->MsgLevel( ) ) );
			}

			std::string Message_Pattern::Format_Arg_d( )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).Cache( ).day );
			}

			std::string Message_Pattern::Format_Arg_H( )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).UpdateTimeInfo( ).hour );
			}

			std::string Message_Pattern::Format_Arg_x( )
			{
				return svToString( msgInfo->TimeDetails( ).Cache( ).short_weekday );
			}

			std::string Message_Pattern::Format_Arg_X( )
			{
				return svToString( msgInfo->TimeDetails( ).Cache( ).long_weekday );
			}

			std::string Message_Pattern::Format_Arg_n( )
			{
				std::string date;
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( msgInfo->TimeDetails( ).Cache( ).day ) );
				date.append( svToString( msgInfo->TimeDetails( ).Cache( ).short_month ) );
				date.append( std::to_string( msgInfo->TimeDetails( ).Cache( ).short_year ) );
				return date;
			}
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity