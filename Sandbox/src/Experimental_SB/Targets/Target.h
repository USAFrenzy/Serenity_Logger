#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"
#include "../MessageDetails/FlushPolicy.h"

#include <chrono>

// Messing with buffer sizes
#define KB                  ( 1024 )
#define MB                  ( 1024 * KB )
#define GB                  ( 1024 * MB )

namespace serenity
{
	namespace expiremental
	{
		using namespace std::chrono_literals;
		namespace targets
		{
			class TargetBase
			{
			  public:
				TargetBase( );
				TargetBase( std::string_view name );
				TargetBase( std::string_view name, std::string_view msgPattern );
				~TargetBase( );

				void                             SetFlushPolicy( Flush_Policy pPolicy );
				Flush_Policy &                   Policy( );
				std::string                      LoggerName( );
				void                             SetPattern( std::string_view pattern );
				void                             ResetPatternToDefault( );
				void                             SetLogLevel( LoggerLevel level );
				void                             WriteToBaseBuffer( bool fmtToBuf = true );
				bool                             isWriteToBuf( );
				std::string *                    Buffer( );
				LoggerLevel                      Level( );
				template <typename... Args> void trace( std::string_view s, Args &&...args );
				template <typename... Args> void info( std::string_view s, Args &&...args );
				template <typename... Args> void debug( std::string_view s, Args &&...args );
				template <typename... Args> void warn( std::string_view s, Args &&...args );
				template <typename... Args> void error( std::string_view s, Args &&...args );
				template <typename... Args> void fatal( std::string_view s, Args &&...args );

				template <typename... Args> void test( std::string_view s, Args &&...args )
				{
					using namespace std::chrono;
					if( logLevel <= LoggerLevel::test ) {
						auto    now              = msgDetails.MessageTimePoint( );
						seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );
						if( messageTimePoint != msgDetails.TimeDetails( ).LastLogPoint( ) ) {
							msgDetails.TimeDetails( ).UpdateCache( now );
						}
						msgDetails.SetMsgLevel( LoggerLevel::test );
						msgDetails.SetMessage( s, std::forward<Args>( args )... );
						auto formatted { MsgFmt( )->GetFormatters( ).Format( ) };

						if( isWriteToBuf( ) ) {
							internalBuffer.append( formatted.data( ), formatted.size( ) );
							PolicyFlushOn( );
						}
						else {
							PrintMessage( formatted );
							PolicyFlushOn(  );
						}
					}
				}

				// for microbenches
				msg_details::Message_Formatter::TimeStats GetFormatStats( )
				{
					return MsgFmt( )->GetStats( );
				}

			  protected:
				virtual void                    PrintMessage( std::string_view formatted ) = 0;
				virtual void                    PolicyFlushOn(  ) { }
				msg_details::Message_Formatter *MsgFmt( );
				msg_details::Message_Info *     MsgInfo( );
				std::mutex                      base_mutex;

			  private:
				bool                           toBuffer;
				Flush_Policy                   policy;
				LoggerLevel                    logLevel;
				LoggerLevel                    msgLevel;
				std::string                    pattern;
				std::string                    loggerName;
				msg_details::Message_Info      msgDetails;
				msg_details::Message_Formatter msgPattern;
				std::string                    internalBuffer;
			};
#include "Target-impl.h"
		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity

// Need to test template specializations at least for flags currently being used as a prototype to see if this is the route I need to
// take to achieve faster times here (Currently with async formatting, its 2.6us, writes to buffer is 3.0us, and just plain writing to
// handle is 3.7us)
using Formatter = serenity::expiremental::msg_details::Message_Formatter;
template <> struct std::formatter<Formatter> : std::formatter<std::string_view>
{
	template <typename Context> auto format( const Formatter &fmt, const Context &context ) { }
};