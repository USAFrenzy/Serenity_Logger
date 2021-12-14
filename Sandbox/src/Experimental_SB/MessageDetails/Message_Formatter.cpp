#include "Message_Formatter.h"

namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
#define SERENITY_SPACE_FLAG 99

			// for micro-benches
			using namespace se_utils;

			Message_Formatter::Message_Formatter( std::string formatPattern, Message_Info *msgDetails )
			  : fmtPattern( formatPattern ), msgInfo( msgDetails )
			{
				buffer.reserve( 512 );
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
				return internalFmt.wholeFormatString;
			}

			void Message_Formatter::Format_Arg_a( Cached_Date_Time &cache )
			{
				std::string hour;
				( cache.hour > 12 )?
					hour = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ) ):
					hour = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
				std::format_to( std::back_inserter( buffer ), "{}", hour );
			}

			void Message_Formatter::Format_Arg_A( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->TimeDetails( ).DayHalf( cache.hour ) );
			}

			void Message_Formatter::Format_Arg_b( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.short_month );
			}

			void Message_Formatter::Format_Arg_B( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.long_month );
			}

			void Message_Formatter::Format_Arg_D( Cached_Date_Time &cache )
			{
				auto month = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ));
				auto day =  std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
				std::format_to( std::back_inserter( buffer ), "{}/{}/{}", month, day, cache.short_year );
			}

			void Message_Formatter::Format_Arg_F( Cached_Date_Time &cache )
			{
				auto month = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ) );
				auto day   = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
				std::format_to( std::back_inserter( buffer ), "{}-{}-{}", cache.long_year, month, day );
			}

			void Message_Formatter::Format_Arg_M( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) );
			}

			void Message_Formatter::Format_Arg_S( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
			}

			void Message_Formatter::Format_Arg_T( Cached_Date_Time &cache )
			{
				auto hour = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
				auto min  = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) );
				auto sec  = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
				std::format_to( std::back_inserter( buffer ), "{}:{}:{}", hour, min, sec );
			}

			void Message_Formatter::Format_Arg_t( Cached_Date_Time &cache )
			{
				std::string hour;
				( cache.hour >= 12 ) ? hour.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ) ) )
									 : hour.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) ) );

				auto min = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) );
				auto sec = std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
				std::format_to( std::back_inserter( buffer ), "{}:{}:{}", hour, min, sec );
			}

			void Message_Formatter::Format_Arg_w( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.dec_wkday );
			}

			void Message_Formatter::Format_Arg_y( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.short_year );
			}

			void Message_Formatter::Format_Arg_Y( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.long_year );
			}

			void Message_Formatter::Format_Arg_N( )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->Name( ) );
			}

			void Message_Formatter::Format_Arg_l( )
			{
				std::format_to( std::back_inserter( buffer ), "{}", MsgLevelToShortString( msgInfo->MsgLevel( ) ) );
			}

			void Message_Formatter::Format_Arg_L( )
			{
				std::format_to( std::back_inserter( buffer ), "{}", MsgLevelToString( msgInfo->MsgLevel( ) ) );
			}

			void Message_Formatter::Format_Arg_d( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
			}

			void Message_Formatter::Format_Arg_H( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
			}

			void Message_Formatter::Format_Arg_x( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.short_weekday );
			}

			void Message_Formatter::Format_Arg_X( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ), "{}", cache.long_weekday );
			}

			void Message_Formatter::Format_Arg_n( Cached_Date_Time &cache )
			{
				std::format_to( std::back_inserter( buffer ),
								"{}{}{}",
								msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ),
								cache.short_month,
								cache.short_year );
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

			void Message_Formatter::FlagFormatter( Cached_Date_Time &cache, int flag )
			{
				switch( flag ) {
					case 0: return Format_Arg_a( cache ); break;
					case 1: return Format_Arg_b( cache ); break;
					case 2: return Format_Arg_d( cache ); break;
					case 3: return Format_Arg_l( ); break;
					case 4: return Format_Arg_n( cache ); break;
					case 5: return Format_Arg_t( cache ); break;
					case 6: return Format_Arg_w( cache ); break;
					case 7: return Format_Arg_x( cache ); break;
					case 8: return Format_Arg_y( cache ); break;
					case 9: return Format_Arg_A( cache ); break;
					case 10: return Format_Arg_B( cache ); break;
					case 11: return Format_Arg_D( cache ); break;
					case 12: return Format_Arg_F( cache ); break;
					case 13: return Format_Arg_H( cache ); break;
					case 14: return Format_Arg_L( ); break;
					case 15: return Format_Arg_M( cache ); break;
					case 16: return Format_Arg_N( ); break;
					case 17: return Format_Arg_S( cache ); break;
					case 18: return Format_Arg_T( cache ); break;
					case 19: return Format_Arg_X( cache ); break;
					case 20: return Format_Arg_Y( cache ); break;
					case SERENITY_SPACE_FLAG: std::format_to( std::back_inserter( buffer ), "{}", " " ); break;
					// if arg after "%" isn't a flag handled here, do nothing
					default: break;
				}
			}

			void Message_Formatter::StoreFormat( )
			{
				auto cache = msgInfo->TimeDetails().UpdateCache( std::chrono::system_clock::now());
				buffer.clear( );
				std::string fmt { fmtPattern };
				std::string flag;
				std::string indexStr;

				while( !fmt.empty( ) ) {
					if( fmt.front( ) == '%' ) {
						flag.clear( );
						flag.append( fmt.substr( 0, 2 ) );
						auto position = std::find( allValidFlags.begin( ), allValidFlags.end( ), flag );
						if( position != allValidFlags.end( ) ) {
							auto index = std::distance( allValidFlags.begin( ), position );
							indexStr   = "%" + std::to_string( index ) + "^";
						}
						if( std::any_of( timeDateFlags.begin( ),
										 timeDateFlags.end( ),
										 [ this, &flag ]( const std::string_view sv ) { return ( sv == flag ) ? true : false; } ) )
						{
							buffer.append( indexStr );
							fmt.erase( 0, 2 );
						}
						else {
							if( std::any_of( otherFlags.begin( ),
											 otherFlags.end( ),
											 [ this, &flag ]( const std::string_view sv ) { return ( sv == flag ) ? true : false; } ) )
							{
								indexStr.erase( 0, 1 );
								indexStr.erase( indexStr.size( ) - 1, indexStr.size( ) );
								FlagFormatter( cache, std::stoi( indexStr ) );
								fmt.erase( 0, flag.size( ) );
							}
						}
						if( fmt.empty( ) ) {
							break;
						}
					}
					else {
						buffer += fmt.front( );
						fmt.erase( 0, 1 );
						if( fmt.empty( ) ) {
							break;
						}
					}
				}
				internalFmt.partitionUpToSpecifier = std::move( std::move( buffer.substr( 0, buffer.find_first_of( "%" ) ) ) );
				buffer.erase( 0, internalFmt.partitionUpToSpecifier.size( ) );
				internalFmt.timeDatePartition = std::move( buffer.substr( 0, buffer.find_last_of( "^" ) + 1 ) );
				buffer.erase( 0, internalFmt.timeDatePartition.size( ) );
				// Remove end boundary now that we have dealt with anything other than time-date flags. General
				// idea here for the end boundary was to separate any index flags from user numbers in format
				// string
				internalFmt.timeDatePartition.erase(
				std::remove( internalFmt.timeDatePartition.begin( ), internalFmt.timeDatePartition.end( ), '^' ),
				internalFmt.timeDatePartition.end( ) );
				// store flags as indexes for Update Function
				for( ;; ) {
					if( internalFmt.timeDatePartition.front( ) == '%' ) {
						internalFmt.timeDatePartition.erase( 0, 1 );
						flag.clear( );
						flag = internalFmt.timeDatePartition.substr( 0, internalFmt.timeDatePartition.find_first_of( "%" ) - 1 );
						flags.emplace_back( std::stoi( flag ) );
						internalFmt.timeDatePartition.erase( 0, flag.size( ) );  // Erase the Flag Token
						if( internalFmt.timeDatePartition.empty( ) ) {
							break;
						}
					}
					else {
						if( internalFmt.timeDatePartition.empty( ) ) {
							break;
						}
						flags.emplace_back( SERENITY_SPACE_FLAG );
						internalFmt.timeDatePartition.erase( 0, 1 );
					}
				}
				internalFmt.remainingPartition = std::move( buffer );
			}

			std::string &Message_Formatter::UpdateFormatForTime( Cached_Date_Time &cache )
			{
				static std::vector<int> flagVec;
				flagVec = flags;
				buffer.clear( );
				for( ;; ) {
					FlagFormatter( cache, flagVec.front( ) );
					flagVec.erase( flagVec.begin( ), flagVec.begin( ) + 1 );
					if( flagVec.empty( ) ) {
						break;
					}
				}
				return internalFmt.wholeFormatString =
					   std::move( internalFmt.partitionUpToSpecifier + std::move( buffer ) + internalFmt.remainingPartition );
			}

			std::string &Message_Formatter::FormatMsg( const std::string_view msg, std::format_args &&args )
			{
				buffer.clear( );
				return buffer = std::move( std::vformat( msg, std::move( args ) ) );
			}

			const InternalFormat &Message_Formatter::FormatSplices( )
			{
				return internalFmt;
			}

		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity