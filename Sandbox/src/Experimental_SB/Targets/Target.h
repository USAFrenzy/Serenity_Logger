#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"
#include "../MessageDetails/FlushPolicy.h"

#include <future>
#include <chrono>

// Messing with buffer sizes
#define KB                  ( 1024 )
#define MB                  ( 1024 * KB )
#define GB                  ( 1024 * MB )
#define DEFAULT_BUFFER_SIZE static_cast<size_t>( 1 * KB )

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

				void          SetFlushPolicy( Flush_Policy pPolicy );
				Flush_Policy &Policy( );

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

#define ESTIMATED_ARG_SIZE 32
				template <typename... Args> void test( std::string_view s, Args &&...args )
				{
					using namespace std::chrono;
					if( logLevel <= LoggerLevel::test ) {
						msgDetails.SetMsgLevel( LoggerLevel::test );
						auto    now              = msgDetails.MessageTimePoint( );
						seconds messageTimePoint = duration_cast<seconds>( now.time_since_epoch( ) );

						if( ( messageTimePoint != msgDetails.TimeDetails( ).Cache( ).secsSinceLastLog ) ||
							( MsgFmt( )->FormatSplices( ).wholeFormatString.empty( ) ) )
						{
							msgDetails.TimeDetails( ).Cache( ).secsSinceLastLog = messageTimePoint;

							auto cache = std::move( msgDetails.TimeDetails( ).UpdateCache( now ) );
							msgPattern.UpdateFormatForTime( cache );
						}
						auto msg =
						std::move( msgPattern.FormatSplices( ).wholeFormatString + std::move( svToString( s ).append( "\n" ) ) );
						internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) + ESTIMATED_ARG_SIZE );
						// Using this branching in case both async format & buffer enabled...
						if( isAsyncWrites( ) ) {
							internalBuffer.clear( );
							std::format_to( std::back_inserter( internalBuffer ), msg, std::forward<Args &&>( args )... );
							AsyncWriteMessage( std::move( internalBuffer ) );
						}
						else {
							if( isAsyncFormat( ) ) {
								AsyncFormat( std::move( msg ), std::forward<Args &&>( args )... );
								PolicyFlushOn( policy );
							}
							else {
								if( isWriteToBuf( ) ) {
									std::format_to( std::back_inserter( internalBuffer ),
													std::move( msg ),
													std::forward<Args &&>( args )... );
									PolicyFlushOn( policy );
								}
								else {
									// Using the format_to() since it's faster than just passing in via format()
									std::format_to( std::back_inserter( internalBuffer ),
													std::move( msg ),
													std::forward<Args &&>( args )... );
									PrintMessage( );
									PolicyFlushOn( policy );
								}
							}
						}
					}
				}

				std::vector<std::future<std::string>> *AsyncFormatFutures( )
				{
					return &base_futures;
				}

				// for microbenches
				msg_details::Message_Formatter::TimeStats GetFormatStats( )
				{
					return MsgFmt( )->GetStats( );
				}

			  protected:
				virtual void PrintMessage( ) = 0;
				virtual void PolicyFlushOn( Flush_Policy & ) { }
				virtual void AsyncWriteMessage( std::string formatted ) { };
			

				template <typename... Args> void AsyncFormat( std::string msg, Args... args )
				{
					// Not true async in the purest sense, concurrent formatting is the main goal here
					// Return the formatted text to be written to the file when flushed
					auto async_format = [ = ]( )
					{
						std::string formatted;
						formatted.reserve( std::formatted_size( msg, args... ) );
						std::format_to( std::back_inserter( formatted ), msg, args... );
						return std::move( formatted );
					};
					base_futures.emplace_back( std::move( std::async( std::launch::async, async_format ) ) );
				};

				void EnableAsyncFormat( bool enable )
				{
					isAsyncFormatted = enable;
				}
				void EnableAsyncWrites( bool enable )
				{
					isAsyncFormatted = enable;
				}
				bool isAsyncWrites() {
					return isAsyncWrite;
				}

				bool isAsyncFormat( )
				{
					return isAsyncFormatted;
				}
				std::mutex                            base_mutex;
				msg_details::Message_Formatter *      MsgFmt( );
				msg_details::Message_Info *           MsgInfo( );
				std::vector<std::future<std::string>> base_futures;

			  private:
				bool                           isAsyncWrite { false };
				bool                           toBuffer;
				bool                           isAsyncFormatted { false };
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
