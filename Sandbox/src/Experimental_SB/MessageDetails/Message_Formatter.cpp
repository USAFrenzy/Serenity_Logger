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


			std::string Message_Formatter::Format_Arg_a( Cached_Date_Time &cache )
			{
				if( cache.hour > 12 ) {
					return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour - 12 ) );
				}
				else {
					return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
				}
			}

			std::string Message_Formatter::Format_Arg_A( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).DayHalf( cache.hour ) );
			}

			std::string Message_Formatter::Format_Arg_b( Cached_Date_Time &cache )
			{
				return std::move( cache.short_month );
			}

			std::string Message_Formatter::Format_Arg_B( Cached_Date_Time &cache )
			{
				return std::move( cache.long_month );
			}

			std::string Message_Formatter::Format_Arg_D( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ).append( "/" ) ) );
				date.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ).append( "/" ) ) );
				return std::move( date.append( std::to_string( cache.short_year ) ) );
			}

			std::string Message_Formatter::Format_Arg_F( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( std::to_string( cache.long_year ).append( "-" ) );
				date.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.dec_month ).append( "-" ) ) );
				return std::move( date.append( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) ) );
			}

			std::string Message_Formatter::Format_Arg_M( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ) );
			}

			std::string Message_Formatter::Format_Arg_S( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) );
			}

			std::string Message_Formatter::Format_Arg_T( Cached_Date_Time &cache )
			{
				std::string time;
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ).append( ":" ) ) );
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.min ).append( ":" ) ) );
				time.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.sec ) ) );
				return std::move( time );
			}

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

			std::string Message_Formatter::Format_Arg_w( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.dec_wkday ) );
			}

			std::string Message_Formatter::Format_Arg_y( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.short_year ) );
			}

			std::string Message_Formatter::Format_Arg_Y( Cached_Date_Time &cache )
			{
				return std::move( std::to_string( cache.long_year ) );
			}

			std::string Message_Formatter::Format_Arg_N( Cached_Date_Time & )
			{
				return msgInfo->Name( );
			}

			std::string Message_Formatter::Format_Arg_l( Cached_Date_Time & )
			{
				return std::move( svToString( MsgLevelToShortString( msgInfo->MsgLevel( ) ) ) );
			}

			std::string Message_Formatter::Format_Arg_L( Cached_Date_Time & )
			{
				return std::move( svToString( MsgLevelToString( msgInfo->MsgLevel( ) ) ) );
			}

			std::string Message_Formatter::Format_Arg_d( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) );
			}

			std::string Message_Formatter::Format_Arg_H( Cached_Date_Time &cache )
			{
				return std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.hour ) );
			}

			std::string Message_Formatter::Format_Arg_x( Cached_Date_Time &cache )
			{
				return std::move( cache.short_weekday );
			}

			std::string Message_Formatter::Format_Arg_X( Cached_Date_Time &cache )
			{
				return std::move( cache.long_weekday );
			}

			std::string Message_Formatter::Format_Arg_n( Cached_Date_Time &cache )
			{
				std::string date;
				date.append( std::move( msgInfo->TimeDetails( ).ZeroPadDecimal( cache.day ) ) );
				date.append( std::move( cache.short_month ) );
				date.append( std::move( std::to_string( cache.short_year ) ) );
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

			std::string Message_Formatter::FlagFormatter( Cached_Date_Time &cache, int flag )
			{
				switch( flag ) {
					case 0: return Format_Arg_a( cache ); break;
					case 1: return Format_Arg_b( cache ); break;
					case 2: return Format_Arg_d( cache ); break;
					case 3: return Format_Arg_l( cache ); break;
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
					case 14: return Format_Arg_L( cache ); break;
					case 15: return Format_Arg_M( cache ); break;
					case 16: return Format_Arg_N( cache ); break;
					case 17: return Format_Arg_S( cache ); break;
					case 18: return Format_Arg_T( cache ); break;
					case 19: return Format_Arg_X( cache ); break;
					case 20: return Format_Arg_Y( cache ); break;
					case SERENITY_SPACE_FLAG: return " "; break;
					// if arg after "%" isn't a flag handled here, do nothing
					default: return ""; break;
				}
			}

			void Message_Formatter::StoreFormat( )
			{
				auto cache = msgInfo->TimeDetails( ).UpdateCache( msgInfo->TimeDetails( ).UpdateTimeDate( ) );
				buffer.clear( );
				std::string fmt { fmtPattern };
				std::string flag;
				std::string indexStr;

				if( buffer.find( '%' ) ) {
					while( !fmt.empty( ) ) {
						if( fmt.front( ) == '%' ) {
							flag.clear( );
							flag.append( fmt.substr( 0, 2 ) );
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
								fmt.erase( 0, 2 );
							}
							else {
								if( std::any_of( otherFlags.begin( ), otherFlags.end( ),
										 [ this, &flag ]( const std::string_view sv ) {
											 return ( sv == flag ) ? true : false;
										 } ) )
								{
									indexStr.erase( 0, 1 );
									indexStr.erase( indexStr.size( ) - 1, indexStr.size( ) );
									buffer.append( FlagFormatter( cache, std::stoi( indexStr ) ) );
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
					internalFmt.partitionUpToSpecifier =
					  std::move( std::move( buffer.substr( 0, buffer.find_first_of( "%" ) ) ) );
					buffer.erase( 0, internalFmt.partitionUpToSpecifier.size( ) );
					internalFmt.timeDatePartition = std::move( buffer.substr( 0, buffer.find_last_of( "^" ) + 1 ) );
					buffer.erase( 0, internalFmt.timeDatePartition.size( ) );
					// Remove end boundary now that we have dealt with anything other than time-date flags. General
					// idea here for the end boundary was to separate any index flags from user numbers in format
					// string
					internalFmt.timeDatePartition.erase( std::remove( internalFmt.timeDatePartition.begin( ),
											  internalFmt.timeDatePartition.end( ), '^' ),
									     internalFmt.timeDatePartition.end( ) );
					// store flags as indexes for Update Function
					for( ;; ) {
						if( internalFmt.timeDatePartition.front( ) == '%' ) {
							internalFmt.timeDatePartition.erase( 0, 1 );
							flag.clear( );
							flag = internalFmt.timeDatePartition.substr(
							  0, internalFmt.timeDatePartition.find_first_of( "%" ) - 1 );
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
				}  // if fmtPattern contains at least one "%"
				else {
					internalFmt.wholeFormatString = fmtPattern;
				}
			}
			std::string &Message_Formatter::UpdateFormatForTime( std::chrono::seconds timePoint )
			{
				auto                    start = std::chrono::system_clock::now( );
				static std::vector<int> flagVec;
				flagVec = flags;
				buffer.clear( );

				using namespace std::chrono;
				auto cache = msgInfo->TimeDetails( ).UpdateCache( msgInfo->TimeDetails( ).UpdateTimeDate( ) );
				for( ;; ) {
					buffer.append( std::move( FlagFormatter( cache, flagVec.front( ) ) ) );
					flagVec.erase( flagVec.begin( ), flagVec.begin( ) + 1 );
					if( flagVec.empty( ) ) {
						break;
					}
				}
				auto end = std::chrono::system_clock::now( );
				benches.totalUpdateTime += std::chrono::duration_cast<pMicro<float>>( end - start ).count( );
				return internalFmt.wholeFormatString = std::move(
					 internalFmt.partitionUpToSpecifier + std::move( buffer ) + internalFmt.remainingPartition );
			}


			// From The Crude Micro-Benches, This Is Where The Most Time Is Spent (UpdateFormatForTime() takes < ~1us so
			// ignoring this function for now)
			std::string &Message_Formatter::FormatMsg( const std::string_view msg, std::format_args &&args )
			{
				auto               timePoint = msgInfo->MessageTimePoint( );
				std::string formatted;
				size_t             formattedSize { 0 };
				formatted.clear( );
				buffer.clear( );
				auto size_check = [ this ]( size_t formattedSize ) {
					if( buffer.capacity( ) < formattedSize ) {
						buffer.reserve( formattedSize );
					}
				};
				auto start = std::chrono::system_clock::now( );
				formatted  = std::move( std::vformat( msg, args ).append( "\n" ) );
				if( ( timePoint != msgInfo->TimeDetails( ).Cache( ).secsSinceLastLog ) ||
				    ( internalFmt.wholeFormatString.empty( ) ) ) {
					msgInfo->TimeDetails( ).Cache( ).secsSinceLastLog = timePoint;
					if( !flags.empty( ) ) {
						auto preFmt = std::move( UpdateFormatForTime( timePoint ) );
						size_check( preFmt.size( ) + formatted.size( ) );
						auto end = std::chrono::system_clock::now( );
						benches.totalFormatTime +=
						  std::chrono::duration_cast<pMicro<float>>( end - start ).count( );
						return buffer = std::move( preFmt ).append( std::move( formatted ) );
					}
					else {
						size_check( internalFmt.wholeFormatString.size( ) + formatted.size( ) );
						buffer   = internalFmt.wholeFormatString;
						auto end = std::chrono::system_clock::now( );
						benches.totalFormatTime +=
						  std::chrono::duration_cast<pMicro<float>>( end - start ).count( );
						return buffer.append( std::move( formatted ) );
					}
				}
				else {
					size_check( internalFmt.wholeFormatString.size( ) + formatted.size( ) );
					buffer   = internalFmt.wholeFormatString;
					auto end = std::chrono::system_clock::now( );
					benches.totalFormatTime += std::chrono::duration_cast<pMicro<float>>( end - start ).count( );
					return buffer.append( std::move( formatted ) );
				}
			}


		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity


// Example One Of std::formatter specialization  https://madridccppug.github.io/posts/stdformat/#integrating-user-defined-types
// Example Two Of std::formatter specialization  https://fmt.dev/latest/api.html#formatting-user-defined-types
//-----------------------------------------------------------------------------------------------------------------------------
// _________________________________________ Actual Custom Formatting Functions Here  _________________________________________
//-----------------------------------------------------------------------------------------------------------------------------
/*
  One way of thinking about this might be the following:
  1) When storing the user's format, just replace the internalFmt string with the corresponding format function type flag instead
     - If the char at the position being checked isn't a flag, replace it with a format function type flag that handles this
	  (User chars like "[", "]", etc or spaces for example)
  EXAMPLE:
  // Psuedo-Code
	StoreFormat(std::string fmt){
	std::string localBuffer;
	std::string fmt = fmtPattern;
		for(;;){
		if(fmt.front() == '%'){
		take substr at '%' position and '%' position +1 for flag
		 handle flag in another function that returns the format_arg representation
		 In this flag handling function:
		   - search validFlags array to check if it's an internal flag
		     - If it is, handle flag in another function that will return back the format_arg representation
		       - In this internal flag handler, if it's not a time-date flag, return the value for what the flag represented
  (i.e. %N -> return the name string)
			   - If this flag is a time-date flag, then return the format library's flag (i.e. if %t is the flag, this
  would equate to %I:%M:%S  or %r)
			   - If It's neither, then check if it's a user-defined flag
			     - If it is, then append this flag to a not-yet-created internalFmt.userDefinedPartion string and return %U
				 - If it isn't, then append this 'flag' directly into StoreFormat()'s local buffer
		   - append to StoreFormat local buffer
		   - erase flag token from local format copy
		   - if local format copy is empty, then break
		   } else {
		     - If it isn't then, check to see if it's a user-defined flag.
			   - If it is, then append this flag to a not-yet-created internalFmt.userFlagPartion string and return %U
			   - If it isn't, then append this 'flag' directly into StoreFormat()'s local buffer
			 - erase flag token from local format copy
		      - if local format copy is empty, then break
		   }
		   // Do some magic here to to partition out the internal format string like currently (but clean it up)
			}
		}
  // In Practice:
	StoreFormat("|%L| %t [%N]: "); (where it's local time used and not utc)
	-> internalized format would look like:
	   - partitionUpToSpecifier = "|Trace| "
	   - timeDatePartition = "%r"
	   - remainingPartition = " [Console_Logger]: "
	   - userDefinedPartition = ""
	   - wholeFormatString = "|Trace| %r [Console_Logger]: "

  2) On formatting, check to see if userDefinedPartition is empty or not


*/