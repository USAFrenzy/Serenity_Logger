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

			Message_Formatter::Message_Formatter( std::string_view pattern, Message_Info *details ) : msgInfo( *&details )
			{
				SetPattern( pattern );
				StoreFormat( );
			}

			void Message_Formatter::FlagFormatter( size_t flag )
			{
				switch( flag ) {
					case 0: formatter.Emplace_Back( std::make_unique<Format_Arg_a>( *msgInfo ) ); break;
					case 1: formatter.Emplace_Back( std::make_unique<Format_Arg_b>( *msgInfo ) ); break;
					case 2: formatter.Emplace_Back( std::make_unique<Format_Arg_d>( *msgInfo ) ); break;
					case 3: formatter.Emplace_Back( std::make_unique<Format_Arg_l>( *msgInfo ) ); break;
					case 4: formatter.Emplace_Back( std::make_unique<Format_Arg_n>( *msgInfo ) ); break;
					case 5: formatter.Emplace_Back( std::make_unique<Format_Arg_t>( *msgInfo ) ); break;
					case 6: formatter.Emplace_Back( std::make_unique<Format_Arg_w>( *msgInfo ) ); break;
					case 7: formatter.Emplace_Back( std::make_unique<Format_Arg_x>( *msgInfo ) ); break;
					case 8: formatter.Emplace_Back( std::make_unique<Format_Arg_y>( *msgInfo ) ); break;
					case 9: formatter.Emplace_Back( std::make_unique<Format_Arg_A>( *msgInfo ) ); break;
					case 10: formatter.Emplace_Back( std::make_unique<Format_Arg_B>( *msgInfo ) ); break;
					case 11: formatter.Emplace_Back( std::make_unique<Format_Arg_D>( *msgInfo ) ); break;
					case 12: formatter.Emplace_Back( std::make_unique<Format_Arg_F>( *msgInfo ) ); break;
					case 13: formatter.Emplace_Back( std::make_unique<Format_Arg_H>( *msgInfo ) ); break;
					case 14: formatter.Emplace_Back( std::make_unique<Format_Arg_L>( *msgInfo ) ); break;
					case 15: formatter.Emplace_Back( std::make_unique<Format_Arg_M>( *msgInfo ) ); break;
					case 16: formatter.Emplace_Back( std::make_unique<Format_Arg_N>( *msgInfo ) ); break;
					case 17: formatter.Emplace_Back( std::make_unique<Format_Arg_S>( *msgInfo ) ); break;
					case 18: formatter.Emplace_Back( std::make_unique<Format_Arg_T>( *msgInfo ) ); break;
					case 19: formatter.Emplace_Back( std::make_unique<Format_Arg_X>( *msgInfo ) ); break;
					case 20: formatter.Emplace_Back( std::make_unique<Format_Arg_Y>( *msgInfo ) ); break;
					case 21:
						formatter.Emplace_Back( std::make_unique<Format_Arg_Message>( *msgInfo ) );
						break;
						// if arg after "%" isn't a flag handled here, do nothing
					default: break;
				}
			}

			void Message_Formatter::SetPattern( std::string_view pattern )
			{
				fmtPattern = pattern;
			}
			Message_Formatter::Formatters &Message_Formatter::GetFormatters( )
			{
				return formatter;
			}

			void Message_Formatter::StoreFormat( )
			{
				std::string fmt { fmtPattern };
				std::string flag;
				size_t      index { 150 };

				while( !fmt.empty( ) ) {
					if( fmt.front( ) == '%' ) {
						flag.clear( );
						flag.append( fmt.substr( 0, 2 ) );
						auto position = std::find( SE_LUTS::allValidFlags.begin( ), SE_LUTS::allValidFlags.end( ), flag );
						if( position != SE_LUTS::allValidFlags.end( ) ) {
							index = std::distance( SE_LUTS::allValidFlags.begin( ), position );
						}
						FlagFormatter( index );
						fmt.erase( 0, flag.size( ) );
						if( fmt.empty( ) ) {
							break;
						}
					}
					else {
						formatter.Emplace_Back( std::make_unique<Format_Arg_Char>( fmt.substr( 0, 1 ) ) );
						fmt.erase( 0, 1 );
						if( fmt.empty( ) ) {
							break;
						}
					}
				}
			}

			Message_Info *Message_Formatter::MessageDetails( )
			{
				return msgInfo;
			}

			// Format_a Functions
			Message_Formatter::Format_Arg_a::Format_Arg_a( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
			{
				hour = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_a::UpdateInternalView( )
			{
				lastHour = cacheRef.tm_hour;
				auto hr { lastHour };
				// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
				std::string_view paddedHour { ( hr > 12 ) ? SE_LUTS::numberStr[ static_cast<int64_t>( hr ) - 12 ]
														  : SE_LUTS::numberStr[ hr ] };
				return std::string { paddedHour.data( ), paddedHour.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_a::Format( )
			{
				return ( cacheRef.tm_hour != lastHour ) ? hour = std::move( UpdateInternalView( ) ) : hour;
			}

			// Format_b Functions
			Message_Formatter::Format_Arg_b::Format_Arg_b( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastMonth( cacheRef.tm_mon )
			{
				month = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_b::UpdateInternalView( )
			{
				lastMonth = cacheRef.tm_mon;
				std::string_view paddedMonth { SE_LUTS::numberStr[ lastMonth ] };
				return std::string { paddedMonth.data( ), paddedMonth.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_b::Format( )
			{
				return ( cacheRef.tm_mon != lastMonth ) ? month = std::move( UpdateInternalView( ) ) : month;
			}

			// Format_d Functions
			Message_Formatter::Format_Arg_d::Format_Arg_d( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastDay( cacheRef.tm_mday )
			{
				day = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_d::UpdateInternalView( )
			{
				lastDay = cacheRef.tm_mday;
				std::string_view paddedDay { SE_LUTS::numberStr[ lastDay ] };
				return std::string { paddedDay.data( ), paddedDay.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_d::Format( )
			{
				return ( cacheRef.tm_mday != lastDay ) ? day = std::move( UpdateInternalView( ) ) : day;
			}

			// Format_l Functions
			Message_Formatter::Format_Arg_l::Format_Arg_l( Message_Info &info ) : levelRef( info.MsgLevel( ) )
			{
				levelStr = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_l::UpdateInternalView( )
			{
				lastLevel = levelRef;
				auto lvl { MsgLevelToShortString( lastLevel ) };
				return std::string { lvl.data( ), lvl.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_l::Format( )
			{
				return ( levelRef != lastLevel ) ? levelStr = std::move( UpdateInternalView( ) ) : levelStr;
			}

			// Format_n Functions
			Message_Formatter::Format_Arg_n::Format_Arg_n( Message_Info &info )
			  : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
			{
				ddmmyy = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_n::UpdateInternalView( )
			{
				lastDay = cacheRef.tm_mday;
				std::string result;
				auto        day { SE_LUTS::numberStr[ cacheRef.tm_mday ] };
				auto        month { SE_LUTS::short_months[ cacheRef.tm_mon ] };
				auto        year { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
				return result.append( day ).append( month ).append( year );
			}
			std::string_view Message_Formatter::Format_Arg_n::Format( )
			{
				return ( cacheRef.tm_mday != lastDay ) ? ddmmyy = std::move( UpdateInternalView( ) ) : ddmmyy;
			}

			// Format_t Functions
			Message_Formatter::Format_Arg_t::Format_Arg_t( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( info.TimeDetails( ).Cache( ).tm_min )
			{
				hmStr = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_t::UpdateInternalView( )
			{
				lastMin = cacheRef.tm_min;
				auto hr { cacheRef.tm_hour };  // just to avoid another lookup, make local copy
				// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
				auto        hour { ( hr > 12 ) ? SE_LUTS::numberStr[ static_cast<int64_t>( hr ) - 12 ] : SE_LUTS::numberStr[ hr ] };
				auto        min { SE_LUTS::numberStr[ lastMin ] };
				std::string result;
				return result.append( hour ).append( ":" ).append( min );
			}
			std::string_view Message_Formatter::Format_Arg_t::Format( )
			{
				auto sec = SE_LUTS::numberStr[ cacheRef.tm_sec ];
				if( cacheRef.tm_min != lastMin ) {
					auto result { hmStr = std::move( UpdateInternalView( ) ) };
					return result.append( ":" ).append( sec );
				}
				else {
					auto result { hmStr };
					return result.append( ":" ).append( sec );
				}
			}

			// Format_w Functions
			Message_Formatter::Format_Arg_w::Format_Arg_w( Message_Info &info ) : cacheRef( info.TimeDetails( ).Cache( ) )
			{
				lastDecDay = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_w::UpdateInternalView( )
			{
				lastDay = cacheRef.tm_wday;
				return std::move( std::vformat( "{}", std::make_format_args( lastDay ) ) );
			}
			std::string_view Message_Formatter::Format_Arg_w::Format( )
			{
				return ( cacheRef.tm_wday != lastDay ) ? lastDecDay = std::move( UpdateInternalView( ) ) : lastDecDay;
			}

			// Format_x Functions
			Message_Formatter::Format_Arg_x::Format_Arg_x( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastWkday( cacheRef.tm_wday )
			{
				wkday = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_x::UpdateInternalView( )
			{
				lastWkday = cacheRef.tm_wday;
				std::string_view sWkday { SE_LUTS::short_weekdays[ lastWkday ] };
				return std::string { sWkday.data( ), sWkday.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_x::Format( )
			{
				return ( lastWkday != cacheRef.tm_wday ) ? wkday = std::move( UpdateInternalView( ) ) : wkday;
			}

			// Format_y Functions
			Message_Formatter::Format_Arg_y::Format_Arg_y( Message_Info &info )
			  : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastYear( cacheRef.tm_year )
			{
				year = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_y::UpdateInternalView( )
			{
				lastYear = cacheRef.tm_year;
				auto yr { timeRef.GetCurrentYearSV( lastYear, true ) };
				return std::string { yr.data( ), yr.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_y::Format( )
			{
				return ( cacheRef.tm_year != lastYear ) ? year = std::move( UpdateInternalView( ) ) : year;
			}

			// Format_A Functions
			Message_Formatter::Format_Arg_A::Format_Arg_A( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
			{
				dayHalf = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_A::UpdateInternalView( )
			{
				lastHour = cacheRef.tm_hour;
				std::string_view dHalf { ( lastHour >= 12 ) ? "PM" : "AM" };
				return std::string { dHalf.data( ), dHalf.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_A::Format( )
			{
				return ( cacheRef.tm_hour != lastHour ) ? dayHalf = std::move( UpdateInternalView( ) ) : dayHalf;
			}

			// Format_B Functions
			Message_Formatter::Format_Arg_B::Format_Arg_B( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastMonth( cacheRef.tm_mon )
			{
				month = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_B::UpdateInternalView( )
			{
				lastMonth = cacheRef.tm_mon;
				std::string_view lMonth { SE_LUTS::long_months[ lastMonth ] };
				return std::string { lMonth.data( ), lMonth.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_B::Format( )
			{
				return ( cacheRef.tm_mon != lastMonth ) ? month = std::move( UpdateInternalView( ) ) : month;
			}

			// Format_D Functions
			Message_Formatter::Format_Arg_D::Format_Arg_D( Message_Info &info )
			  : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
			{
				mmddyy = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_D::UpdateInternalView( )
			{
				std::string result;
				lastDay = cacheRef.tm_mday;
				// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
				auto m { SE_LUTS::numberStr[ static_cast<int64_t>( cacheRef.tm_mon ) + 1 ] };
				auto d { SE_LUTS::numberStr[ lastDay ] };
				auto y { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
				return result.append( m ).append( "/" ).append( d ).append( "/" ).append( y );
			}
			std::string_view Message_Formatter::Format_Arg_D::Format( )
			{
				return ( cacheRef.tm_mday != lastDay ) ? mmddyy = std::move( UpdateInternalView( ) ) : mmddyy;
			}

			// Format_F Functions
			Message_Formatter::Format_Arg_F::Format_Arg_F( Message_Info &info )
			  : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastDay( cacheRef.tm_mday )
			{
				yymmdd = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_F::UpdateInternalView( )
			{
				std::string result;
				lastDay = cacheRef.tm_mday;
				auto y { timeRef.GetCurrentYearSV( cacheRef.tm_year, true ) };
				// static_cast to 8 byte value to remove C2451's warning (which would never occur here anyways...)
				auto m { SE_LUTS::numberStr[ static_cast<int64_t>( cacheRef.tm_mon ) + 1 ] };
				auto d { SE_LUTS::numberStr[ lastDay ] };
				return result.append( y ).append( "-" ).append( m ).append( "-" ).append( d );
			}
			std::string_view Message_Formatter::Format_Arg_F::Format( )
			{
				return ( cacheRef.tm_mday != lastDay ) ? yymmdd = std::move( UpdateInternalView( ) ) : yymmdd;
			}

			// Format_H Functions
			Message_Formatter::Format_Arg_H::Format_Arg_H( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastHour( cacheRef.tm_hour )
			{
				hour = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_H::UpdateInternalView( )
			{
				lastHour = cacheRef.tm_hour;
				std::string_view hr { SE_LUTS::numberStr[ lastHour ] };
				return std::string { hr.data( ), hr.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_H::Format( )
			{
				return ( cacheRef.tm_hour != lastHour ) ? hour = std::move( UpdateInternalView( ) ) : hour;
			}

			// Format_L Functions
			Message_Formatter::Format_Arg_L::Format_Arg_L( Message_Info &info ) : levelRef( info.MsgLevel( ) )
			{
				levelStr = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_L::UpdateInternalView( )
			{
				lastLevel = levelRef;
				auto lvl { MsgLevelToString( lastLevel ) };
				return std::string { lvl.data( ), lvl.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_L::Format( )
			{
				return ( levelRef != lastLevel ) ? levelStr = std::move( UpdateInternalView( ) ) : levelStr;
			}

			// Format_M Functions
			Message_Formatter::Format_Arg_M::Format_Arg_M( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( cacheRef.tm_min )
			{
				min = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_M::UpdateInternalView( )
			{
				lastMin = cacheRef.tm_min;
				std::string_view minute { SE_LUTS::numberStr[ lastMin ] };
				return std::string { minute.data( ), minute.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_M::Format( )
			{
				return ( cacheRef.tm_min != lastMin ) ? min = std::move( UpdateInternalView( ) ) : min;
			}

			// Format_N Functions
			Message_Formatter::Format_Arg_N::Format_Arg_N( Message_Info &info ) : name( info.Name( ) ) { }
			std::string Message_Formatter::Format_Arg_N::UpdateInternalView( )
			{
				// No Need to implement as it's not used
				return "";
			}
			std::string_view Message_Formatter::Format_Arg_N::Format( )
			{
				return name;
			}

			// Format_S Functions
			Message_Formatter::Format_Arg_S::Format_Arg_S( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastSec( cacheRef.tm_sec )
			{
				sec = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_S::UpdateInternalView( )
			{
				lastSec = cacheRef.tm_sec;
				std::string_view second { SE_LUTS::numberStr[ lastSec ] };
				return std::string { second.data( ), second.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_S::Format( )
			{
				return ( cacheRef.tm_sec != lastSec ) ? sec = std::move( UpdateInternalView( ) ) : sec;
			}

			// Format_T Functions
			Message_Formatter::Format_Arg_T::Format_Arg_T( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastMin( cacheRef.tm_min )
			{
				hmStr = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_T::UpdateInternalView( )
			{
				lastMin = cacheRef.tm_min;
				auto        hour { SE_LUTS::numberStr[ cacheRef.tm_hour ] };
				auto        min { SE_LUTS::numberStr[ lastMin ] };
				std::string result;
				return result.append( hour ).append( ":" ).append( min );
			}
			std::string_view Message_Formatter::Format_Arg_T::Format( )
			{
				auto sec = SE_LUTS::numberStr[ cacheRef.tm_sec ];
				if( cacheRef.tm_min != lastMin ) {
					auto result = hmStr = std::move( UpdateInternalView( ) );
					return result.append( ":" ).append( sec );
				}
				else {
					auto result = hmStr;
					return result.append( ":" ).append( sec );
				}
			}

			// Format_T Functions
			Message_Formatter::Format_Arg_X::Format_Arg_X( Message_Info &info )
			  : cacheRef( info.TimeDetails( ).Cache( ) ), lastWkday( cacheRef.tm_wday )
			{
				wkday = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_X::UpdateInternalView( )
			{
				lastWkday = cacheRef.tm_wday;
				std::string_view lWkday { SE_LUTS::long_weekdays[ lastWkday ] };
				return std::string { lWkday.data( ), lWkday.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_X::Format( )
			{
				return ( cacheRef.tm_wday != lastWkday ) ? wkday = std::move( UpdateInternalView( ) ) : wkday;
			}

			// Format_Y Functions
			Message_Formatter::Format_Arg_Y::Format_Arg_Y( Message_Info &info )
			  : timeRef( info.TimeDetails( ) ), cacheRef( timeRef.Cache( ) ), lastYear( cacheRef.tm_year )
			{
				year = std::move( UpdateInternalView( ) );
			}
			std::string Message_Formatter::Format_Arg_Y::UpdateInternalView( )
			{
				lastYear = cacheRef.tm_year;
				auto yr { timeRef.GetCurrentYearSV( lastYear ) };
				return std::string { yr.data( ), yr.size( ) };
			}
			std::string_view Message_Formatter::Format_Arg_Y::Format( )
			{
				return ( cacheRef.tm_year != lastYear ) ? year = std::move( UpdateInternalView( ) ) : year;
			}

			// Format_Message Functions
			Message_Formatter::Format_Arg_Message::Format_Arg_Message( Message_Info &info ) : message( info.Message( ) ) { }
			std::string Message_Formatter::Format_Arg_Message::UpdateInternalView( )
			{
				// No Need to implement as it's not used
				return "";
			}
			std::string_view Message_Formatter::Format_Arg_Message::Format( )
			{
				return message;
			}

			// Format_Char Functions
			Message_Formatter::Format_Arg_Char::Format_Arg_Char( std::string_view ch ) : m_char( ch.data( ), ch.size( ) ) { }
			std::string Message_Formatter::Format_Arg_Char::UpdateInternalView( )
			{
				// No Need to implement as it's not used
				return "";
			}
			std::string_view Message_Formatter::Format_Arg_Char::Format( )
			{
				return m_char;
			}

			// Formatters Functions
			Message_Formatter::Formatters::Formatters( std::vector<std::shared_ptr<Formatter>> &&container )
			  : m_Formatter( std::move( container ) )
			{
				// Reserve an estimated amount based off arg sizes
				localBuffer.reserve( m_Formatter.size( ) * 32 );
			}
			Message_Formatter::Formatters::Formatters( )
			{
				localBuffer.reserve( 512 );
			}
			void Message_Formatter::Formatters::Emplace_Back( std::unique_ptr<Formatter> &&formatter )
			{
				m_Formatter.emplace_back( std::move( formatter ) );
				// Reserve an estimated amount based off arg sizes
				localBuffer.reserve( m_Formatter.size( ) * 32 );
			}
			std::string_view Message_Formatter::Formatters::Format( )
			{
				localBuffer.clear( );
				for( auto &formatter : m_Formatter ) {
					auto formatted { formatter->Format( ) };
					localBuffer.append( std::move( formatted.data( ) ), formatted.size( ) );
				}
				return localBuffer;
			}

		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity