#include "Message_Formatter.h"

#include <serenity/Utilities/Utilities.h>

namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			Message_Formatter::Message_Formatter( std::string formatPattern, Message_Info *msgDetails )
			  : fmtPattern( formatPattern ), msgInfo( msgDetails )
			{
				buffer.reserve( 4096 );
				InitFunctionHandler( );
				internalFmt = { };
				StoreFormat( );
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

			// Twelve Hour Format (13 -> 1, 14 -> 2, 11 -> 11, 10 -> 10, etc)
			std::string Message_Formatter::Format_Arg_a( Cached_Date_Time &cache )
			{
				if( cache.hour > 12 ) {
					return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ) );
				}
				else {
					return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
				}
			}

			// AM Or PM Specifier
			std::string Message_Formatter::Format_Arg_A( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).DayHalf( cache.hour ) );
			}

			// Short Month Format (Jan, Feb, Mar, etc)
			std::string Message_Formatter::Format_Arg_b( Cached_Date_Time &cache )
			{
				return std::move( cache.short_month );
			}

			// Long Month Format (January, February, March, etc)
			std::string Message_Formatter::Format_Arg_B( Cached_Date_Time &cache )
			{
				return std::move( cache.long_month );
			}

			// MM/DD/YY Format
			std::string Message_Formatter::Format_Arg_D( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ).append( "/" ) );
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ).append( "/" ) );
				return std::move( date.append( std::to_string( cache.short_year ) ) );
			}

			// YYYY-MM-DD Format
			std::string Message_Formatter::Format_Arg_F( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( std::to_string( cache.long_year ).append( "-" ) );
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ).append( "-" ) );
				return std::move( date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) ) );
			}

			// Padded Minute ( 07, 08, 13, 15, etc)
			std::string Message_Formatter::Format_Arg_M( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) );
			}

			// Padded Second ( 07, 08, 13, 15, etc)
			std::string Message_Formatter::Format_Arg_S( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
			}

			// HH:MM:SS Format With A 24 Hour Clock Time
			std::string Message_Formatter::Format_Arg_T( Cached_Date_Time &cache )
			{
				std::string time;
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ).append( ":" ) ) );
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ).append( ":" ) ) );
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) ) );
				return std::move( time );
			}

			// HH:MM:SS Format With A 12 Hour Clock Time
			std::string Message_Formatter::Format_Arg_t( Cached_Date_Time &cache )
			{
				std::string time;
				if( cache.hour > 12 ) {
					time.append(
					  std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ).append( ":" ) ) );
				}
				else {
					time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ).append( ":" ) ) );
				}
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ).append( ":" ) ) );
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) ) );
				return std::move( time );
			}

			// Decimal Weekday Format (0-6)
			std::string Message_Formatter::Format_Arg_w( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.dec_wkday ) );
			}

			// Short Year Format (For 2021 -> 21, For 2022 -> 22)
			std::string Message_Formatter::Format_Arg_y( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.short_year ) );
			}

			// Long Year Format (2021, 2022)
			std::string Message_Formatter::Format_Arg_Y( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.long_year ) );
			}

			// Name Passed In To Target Or Default Name If None Specified
			std::string Message_Formatter::Format_Arg_N( Cached_Date_Time & )
			{
				return msgInfo->Name( );
			}

			// Short Message Level (T, I, D, W, E, F)
			std::string Message_Formatter::Format_Arg_l( Cached_Date_Time & )
			{
				return std::move( svToString( MsgLevelToShortString( msgInfo->MsgLevel( ) ) ) );
			}

			// Long Message Level (Trace, Info, Debug, Warn, Error, Fatal)
			std::string Message_Formatter::Format_Arg_L( Cached_Date_Time & )
			{
				return std::move( svToString( MsgLevelToString( msgInfo->MsgLevel( ) ) ) );
			}

			// Padded Day Date (01-31)
			std::string Message_Formatter::Format_Arg_d( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
			}

			// Padded 24 Hour Format (00-23)
			std::string Message_Formatter::Format_Arg_H( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
			}

			// Short Weekday Format (Mon, Tues, Wed, Thurs, Fri, Sat, Sun)
			std::string Message_Formatter::Format_Arg_x( Cached_Date_Time &cache )
			{
				return cache.short_weekday;
			}

			// Short Weekday Format (Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
			std::string Message_Formatter::Format_Arg_X( Cached_Date_Time &cache )
			{
				return std::move( cache.long_weekday );
			}

			// Padded DDMMMYY Format (01Nov21, 09Oct22, 12Dec22 etc)
			std::string Message_Formatter::Format_Arg_n( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
				date.append( cache.short_month );
				date.append( std::to_string( cache.short_year ) );
				return std::move( date );
			}

			void Message_Formatter::SetPattern( std::string_view pattern )
			{
				fmtPattern = std::move( pattern );
				StoreFormat( );
			}

			static constexpr std::array<std::string_view, 21> allValidFlags = { "%a", "%b", "%d", "%l", "%n", "%t", "%w",
											    "%x", "%y", "%A", "%B", "%D", "%F", "%H",
											    "%L", "%M", "%N", "%S", "%T", "%X", "%Y" };
			// separation for storage caching due to just storing the direct substitution of anything not time-date related
			static constexpr std::array<std::string_view, 18> timeDateFlags = {
			  "%a", "%b", "%d", "%n", "%t", "%w", "%x", "%y", "%A", "%B", "%D", "%F", "%H", "%M", "%S", "%T", "%X", "%Y" };
			static constexpr std::array<std::string_view, 3> otherFlags = { "%l", "%L", "%N" };

			void Message_Formatter::InitFunctionHandler( )
			{
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_a( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_b( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_d( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_l( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_n( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_t( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_w( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_x( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_y( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_A( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_B( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_D( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_F( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_H( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_L( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_M( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_N( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_S( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_T( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_X( cache ); } );
				fmtFunctions.emplace_back(
				  [ this ]( Cached_Date_Time cache ) -> std::string { return this->Format_Arg_Y( cache ); } );
			}

			// I was initially hoping that indexing straight into a function call would be faster - currently it's about
			// the same speed as it was beforehand. However, I feel like this approach is better, especially if I make the
			// allValidFlags a member vector instead of a constexpr array - that way, I can have things such as adding user
			// defined flags and formatting functions whereas with a constexpr array and a switch-case, I couldn't unless I
			// integrated more code than was needed for that functionality (Would have to be sure to check paramaters for
			// use-case though)
			std::string Message_Formatter::FlagFormatter( Cached_Date_Time &cache, std::string &indexStr )
			{
				return std::move( fmtFunctions.at( stoi( indexStr ) )( cache ) );
			}

			void Message_Formatter::StoreFormat( )
			{
				auto cache = msgInfo->TimeDetails( ).UpdateCache( msgInfo->TimeDetails( ).UpdateTimeDate( ) );
				std::chrono::seconds now = std::chrono::duration_cast<std::chrono::seconds>(
				  std::chrono::system_clock::now( ).time_since_epoch( ) );
				msgInfo->TimeDetails( ).Cache( ).secondsSinceEpoch = now;
				buffer.clear( );
				size_t      it { 0 };
				auto        fmt = fmtPattern;
				std::string flag;
				std::string indexStr;

				if( buffer.find( '%' ) ) {
					while( it != std::string::npos && ( !fmt.empty( ) ) ) {
						if( fmt.front( ) == '%' ) {
							flag.clear( );
							flag.append( fmt.substr( it, it + 2 ) );
							auto position = std::find( allValidFlags.begin( ), allValidFlags.end( ), flag );
							if( position != allValidFlags.end( ) ) {
								auto index = std::distance( allValidFlags.begin( ), position );
								indexStr   = "%" + std::to_string( index ) + "^";
							}

							if( std::any_of( timeDateFlags.begin( ), timeDateFlags.end( ),
									 [ this, &flag ]( const std::string_view sv ) {
										 return ( sv == flag ) ? true : false;
									 } ) )
							{
								buffer.append( indexStr );
								fmt.erase( it, it + 2 );
							}
							else {
								if( std::any_of( otherFlags.begin( ), otherFlags.end( ),
										 [ this, &flag ]( const std::string_view sv ) {
											 return ( sv == flag ) ? true : false;
										 } ) )
								{
									indexStr.erase( 0, 1 );
									indexStr.erase( indexStr.size( ) - 1, indexStr.size( ) );

									buffer.append( FlagFormatter( cache, indexStr ) );
									fmt.erase( it, flag.size( ) );
								}
							}
						}
						else {
							buffer += fmt.front( );
							fmt.erase( it, it + 1 );
						}
					}
					internalFmt.partitionUpToSpecifier =
					  std::move( std::move( buffer.substr( 0, buffer.find_first_of( "%" ) ) ) );
					buffer.erase( 0, internalFmt.partitionUpToSpecifier.size( ) );
					internalFmt.flagPartition = std::move( buffer.substr( 0, buffer.find_last_of( "^" ) + 1 ) );
					buffer.erase( 0, internalFmt.flagPartition.size( ) );
					// Remove end boundary mow that we have dealt with anything other than time-date flags.
					// Probably a more efficient way to do this, but the goal here was to not lose out on any
					// numbers the user may have added to the format string while still being able to use a direct
					// index into the formatting functions later on.
					std::string sub;
					while( true ) {
						if( internalFmt.flagPartition.find( "^" ) ) {
							sub.append( internalFmt.flagPartition.substr(
							  0, internalFmt.flagPartition.find_first_of( "^" ) ) );
							internalFmt.flagPartition.erase(
							  0, internalFmt.flagPartition.find_first_of( "^" ) + 1 );
							if( internalFmt.flagPartition.empty( ) ) {
								internalFmt.flagPartition = std::move( sub );
								break;
							}
						}
						else {
							internalFmt.flagPartition = std::move( sub );
							break;
						}
					}

					internalFmt.remainingPartition = std::move( buffer );
					internalFmt.wholeFormatString  = internalFmt.partitionUpToSpecifier +
									internalFmt.flagPartition + internalFmt.remainingPartition;
				}  // if fmtPattern contains at least one "%"
				else {
					internalFmt.wholeFormatString = fmtPattern;
				}
			}

			std::string &Message_Formatter::UpdateFormatForTime( std::chrono::system_clock::duration timePoint )
			{
				using namespace std::chrono;
				buffer.clear( );
				size_t      it { 0 };
				std::string flagPartition { internalFmt.flagPartition };
				std::string flag;

				if( !flagPartition.empty( ) ) {
					if( timePoint != msgInfo->TimeDetails( ).Cache( ).secondsSinceEpoch ) {
						auto cache =
						  msgInfo->TimeDetails( ).UpdateCache( msgInfo->TimeDetails( ).UpdateTimeDate( ) );
						msgInfo->TimeDetails( ).Cache( ).secondsSinceEpoch = duration_cast<seconds>( timePoint );

						while( it != std::string::npos && ( !flagPartition.empty( ) ) ) {
							if( flagPartition.front( ) == '%' ) {
								flag.clear( );
								flagPartition.erase( it, it + 1 );
								flag = flagPartition.substr( 0, flagPartition.find_first_of( "%" ) );
								// flag = flagPartition.substr( it, it + 2 );
								buffer.append( std::move( FlagFormatter( cache, flag ) ) );
								buffer.append( " " );                     // spacing
								flagPartition.erase( it, flag.size( ) );  // Erase the Flag Token
							}
							else {
								buffer.append( flagPartition.substr( it, it + 1 ) );
								flagPartition.erase( it, it + 1 );
							}
						}
						internalFmt.wholeFormatString = internalFmt.partitionUpToSpecifier +
										std::move( buffer ) + internalFmt.remainingPartition;
					}
				}
				return internalFmt.wholeFormatString;
			}

			std::string Message_Formatter::FormatMsg( const std::string_view msg, std::format_args args )
			{
				buffer.clear( );
				if( !internalFmt.flagPartition.empty( ) ) {
					auto preFmt = std::move( UpdateFormatForTime( msgInfo->MessageTimePoint( ) ) );
					return std::move(
					  buffer.append( std::move( preFmt ) ).append( std::move( std::vformat( msg, args ) ) ).append( "\n" ) );
				}
				else {
					return std::move( std::vformat( msg, args ).append( "\n" ) );
				}
			}


		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity