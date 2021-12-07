#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"

#include <future>

// Messing with buffer sizes
#define KB          ( 1024 )
#define MB          ( 1024 * KB )
#define GB          ( 1024 * MB )
#define BUFFER_SIZE static_cast<size_t>( 512 * KB )

namespace serenity
{
	namespace expiremental
	{
		class Flush_Policy
		{
		      public:
			enum class Flush
			{
				always,
				periodically,
				never,
			};
			enum class Periodic_Options
			{
				mem_usage,
				time_based,
				undef,
			};

		      private:
			struct Flush_Settings
			{
				Flush            policy;
				Periodic_Options sub_options;
				float            interval;
				bool             sFlush;
			};

		      public:
			Flush_Policy( Flush mode = Flush::never, Periodic_Options sub_options = Periodic_Options::undef,
				      float interval = 0.0 )
			{
				options.policy      = mode;
				options.sub_options = sub_options;
				options.interval    = interval;
				options.sFlush      = ( mode != Flush::never ) ? true : false;
			}

			~Flush_Policy( ) = default;

			const bool ShouldFlush( )
			{
				return options.sFlush;
			}

			void SetFlushOptions( Flush_Settings flushOptions )
			{
				options = flushOptions;
			}

			const Flush_Settings GetSettings( )
			{
				return options;
			}

			const Periodic_Options GetPeriodicSetting( )
			{
				return options.sub_options;
			}

			const Flush GetFlushSetting( )
			{
				return options.policy;
			}

		      private:
			Flush_Settings options;
		};

		namespace targets
		{
			class TargetBase
			{
			      public:
				TargetBase( );
				TargetBase( std::string_view name );
				TargetBase( std::string_view name, std::string_view msgPattern );
				~TargetBase( );

				void               SetFlushPolicy( Flush_Policy policy );
				const Flush_Policy FlushPolicy( );

				std::string                      LoggerName( );
				void                             SetPattern( std::string_view pattern );
				void                             ResetPatternToDefault( );
				void                             SetLogLevel( LoggerLevel level );
				void                             WriteToInternalBuffer( bool fmtToBuf = true );
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
					std::lock_guard<std::mutex> lock( base_mutex );
					if( logLevel <= LoggerLevel::test ) {
						msgDetails.SetMsgLevel( LoggerLevel::test );
						std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
						static std::string   preFormat;
						std::string   msg;
						preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
						msg.reserve( preFormat.capacity( ) + s.size( ) );

						if( messageTimePoint != lastLogTime ) {
							lastLogTime = messageTimePoint;
							preFormat.clear( );
							preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
						}
						msg.clear( );
						msg = preFormat + svToString(s).append("\n");
						internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );
						if( isWriteToBuf( ) ) {
							auto async_format = [ this, msg ]( ) {
								return std::move(std::format( msg, std::forward<Args>( args )... ));
							};

							base_futures.push_back( std::async( std::launch::async, async_format ) );
						}
						else {
							auto formatted = std::move( std::format( msg, std::forward<Args>( args )... ) );
							PrintMessage(  formatted);
						}
					}
				}

				std::vector<std::future<std::string>>* AsyncFutures( )
				{
					return &base_futures;
				}

				// for microbenches
				msg_details::Message_Formatter::TimeStats GetFormatStats( )
				{
					return MsgFmt( )->GetStats( );
				}

			      protected:
				virtual void                          PrintMessage( std::string &buffer ) = 0;
				virtual void                          PolicyFlushOn( Flush_Policy &policy ) { }
				msg_details::Message_Formatter *      MsgFmt( );
				msg_details::Message_Info *           MsgInfo( );
				std::vector<std::future<std::string>> base_futures;


			      private:
				bool                           toBuffer;
				Flush_Policy                   policy;
				LoggerLevel                    logLevel;
				LoggerLevel                    msgLevel;
				std::string                    pattern;
				std::string                    loggerName;
				msg_details::Message_Info      msgDetails;
				msg_details::Message_Formatter msgPattern;
				std::chrono::seconds           lastLogTime { 0 };
				std::string                    internalBuffer;
				std::mutex                     base_mutex;
			};
#include "Target-impl.h"
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
