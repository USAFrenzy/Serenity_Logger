#include "Message_Formatter.h"


namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Formatter::Message_Formatter( std::string formatPattern, Message_Info *msgDetails )
			  : fmtPattern( formatPattern ), msgInfo( msgDetails )
			{
			}
			Message_Formatter &Message_Formatter::operator=( const Message_Formatter &t )
			{
				buffer     = t.buffer;
				fmtPattern = t.fmtPattern;
				msgInfo    = t.msgInfo;
				return *this;
			}

			std::string Message_Formatter::GetMsgFmt( )
			{
				return fmtPattern;
			}

			std::string Message_Formatter::FlagFormatter( Cached_Date_Time cache, char flag )
			{
				switch( flag ) {
					case 'a': return Format_Arg_a( cache ); break;
					case 'A': return Format_Arg_A( cache ); break;
					case 'b': return Format_Arg_b( cache ); break;
					case 'B': return Format_Arg_B( cache ); break;
					case 'd': return Format_Arg_d( cache ); break;
					case 'D': return Format_Arg_D( cache ); break;
					case 'F': return Format_Arg_F( cache ); break;
					case 'H': return Format_Arg_H( cache ); break;
					case 'L': return Format_Arg_L( ); break;
					case 'l': return Format_Arg_l( ); break;
					case 'M': return Format_Arg_M( cache ); break;
					case 'n': return Format_Arg_n( cache ); break;
					case 'N': return Format_Arg_N( ); break;
					case 'S': return Format_Arg_S( cache ); break;
					case 't': return Format_Arg_t( cache ); break;
					case 'T': return Format_Arg_T( cache ); break;
					case 'w': return Format_Arg_w( cache ); break;
					case 'x': return Format_Arg_x( cache ); break;
					case 'X': return Format_Arg_X( cache ); break;
					case 'y': return Format_Arg_y( cache ); break;
					case 'Y': return Format_Arg_Y( cache ); break;
					// if arg after "%" isn't a flag handled here, do nothing
					default: return ""; break;
				}
			}

			// TODO: Finish Flag Argument Formatters

			// Twelve Hour Format (13 -> 1, 14 -> 2, 11 -> 11, 10 -> 10, etc)
			std::string Message_Formatter::Format_Arg_a( Cached_Date_Time cache )
			{
				if( cache.hour > 12 ) {
					return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 );
				}
				else {
					return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour );
				}
			}

			// AM Or PM Specifier
			std::string Message_Formatter::Format_Arg_A( Cached_Date_Time cache )
			{
				return svToString( msgInfo->TimeDetails( ).DayHalf( cache.hour ) );
			}

			// Short Month Format (Jan, Feb, Mar, etc)
			std::string Message_Formatter::Format_Arg_b( Cached_Date_Time cache )
			{
				return svToString( cache.short_month );
			}

			// Long Month Format (January, February, March, etc)
			std::string Message_Formatter::Format_Arg_B( Cached_Date_Time cache )
			{
				return svToString( cache.long_month );
			}

			// MM/DD/YY Format
			std::string Message_Formatter::Format_Arg_D( Cached_Date_Time cache )
			{
				std::string date;
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ) + "/" );
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) + "/" );
				return date.append( std::to_string( cache.short_year ) );
			}

			// YYYY-MM-DD Format
			std::string Message_Formatter::Format_Arg_F( Cached_Date_Time cache )
			{
				std::string date;
				date.append( std::to_string( cache.long_year ) + "-" );
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ) + "-" );
				return date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
			}

			// Padded Minute ( 07, 08, 13, 15, etc)
			std::string Message_Formatter::Format_Arg_M( Cached_Date_Time cache )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min );
			}

			// Padded Second ( 07, 08, 13, 15, etc)
			std::string Message_Formatter::Format_Arg_S( Cached_Date_Time cache )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec );
			}

			// HH:MM:SS Format With A 24 Hour Clock Time
			std::string Message_Formatter::Format_Arg_T( Cached_Date_Time cache )
			{
				std::string time;
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) + ":";
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) + ":";
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec );
				return time;
			}

			// HH:MM:SS Format With A 12 Hour Clock Time
			std::string Message_Formatter::Format_Arg_t( Cached_Date_Time cache )
			{
				std::string time;
				if( cache.hour > 12 ) {
					time += (msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ) + ":");
				}
				else {
					time += (std::to_string(cache.hour ) + ":");
				}
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) + ":";
				time += msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec );
				return time;
			}

			// Decimal Weekday Format (0-6)
			std::string Message_Formatter::Format_Arg_w( Cached_Date_Time cache )
			{
				return std::to_string( cache.dec_wkday );
			}

			// Short Year Format (For 2021 -> 21, For 2022 -> 22)
			std::string Message_Formatter::Format_Arg_y( Cached_Date_Time cache )
			{
				return std::to_string( cache.short_year );
			}

			// Long Year Format (2021, 2022)
			std::string Message_Formatter::Format_Arg_Y( Cached_Date_Time cache )
			{
				return std::to_string( cache.long_year );
			}

			// Name Passed In To Target Or Default Name If None Specified
			std::string Message_Formatter::Format_Arg_N( )
			{
				return msgInfo->Name( );
			}

			// Short Message Level (T, I, D, W, E, F)
			std::string Message_Formatter::Format_Arg_l( )
			{
				return svToString( MsgLevelToShortString( msgInfo->MsgLevel( ) ) );
			}

			// Long Message Level (Trace, Info, Debug, Warn, Error, Fatal)
			std::string Message_Formatter::Format_Arg_L( )
			{
				return svToString( MsgLevelToString( msgInfo->MsgLevel( ) ) );
			}

			// Padded Day Date (01-31)
			std::string Message_Formatter::Format_Arg_d( Cached_Date_Time cache )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day );
			}

			// Padded 24 Hour Format (00-23)
			std::string Message_Formatter::Format_Arg_H( Cached_Date_Time cache )
			{
				return msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour );
			}

			// Short Weekday Format (Mon, Tues, Wed, Thurs, Fri, Sat, Sun)
			std::string Message_Formatter::Format_Arg_x( Cached_Date_Time cache )
			{
				return svToString( cache.short_weekday );
			}

			// Short Weekday Format (Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
			std::string Message_Formatter::Format_Arg_X( Cached_Date_Time cache )
			{
				return svToString( cache.long_weekday );
			}

			// Padded DDMMMYY Format (01Nov21, 09Oct22, 12Dec22 etc)
			std::string Message_Formatter::Format_Arg_n( Cached_Date_Time cache )
			{
				std::string date;
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
				date.append( svToString( cache.short_month ) );
				date.append( std::to_string( cache.short_year ) );
				return date;
			}

			void Message_Formatter::SetPattern( std::string_view pattern )
			{
				fmtPattern = svToString( pattern );
			}

			// Iterate through the pre-message format pattern in order to find any flags. When a flag is reached,
			// append everything up until that flag to the buffer, erase the flag qualifier, handle the flag, then
			// erase the flag token at that position and continue iterating until the end of format pattern is
			// reached. If no flag is found, or no more flags are available to handle, just append the rest of the
			// format pattern to the buffer. Append the formatted message string (using the forwarded arguments to
			// format from) to the buffer and return the entire thing
			std::string_view Message_Formatter::FormatMessage( std::string message, std::format_args args )
			{
				auto cache = msgInfo->TimeDetails( ).UpdateCache( );
				buffer.clear( );
				size_t it { 0 }, pos { 0 };
				auto   fmt = fmtPattern;

				while( it != std::string::npos && ( !fmt.empty( ) ) ) {
					if( fmt.front( ) == '%' ) {
						buffer.append( fmt.substr( 0, pos ) );
						fmt.erase( 0, pos + 1 );
						buffer.append( FlagFormatter( cache, fmt.front( ) ) );
						fmt.erase( 0, pos + 1 );  // Erase the Flag Token
					}
					else {
						buffer += fmt.at( 0 );
						fmt.erase( 0, 1 );
					}
				}
				return buffer.append( fmt + std::move( std::vformat( message, args ) + "\n" ) );
			}

		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity