#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"

#include <future>
#include <chrono>

// Messing with buffer sizes
#define KB                  ( 1024 )
#define MB                  ( 1024 * KB )
#define GB                  ( 1024 * MB )
#define DEFAULT_BUFFER_SIZE static_cast<size_t>( 256 * KB )

namespace serenity
{
	namespace expiremental
	{
		using namespace std::chrono_literals;

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
				memUsage,
				timeBased,
				logLevelBased,
				undef,
			};

			struct Flush_Settings
			{
				struct Sub_Settings
				{
					Sub_Settings( )
					  : memUsage( DEFAULT_BUFFER_SIZE ), timeBased( 5s ), logLevelBased( LoggerLevel::trace ), settingsChange( false )
					{
					}
					Sub_Settings( const Sub_Settings &ss )
					  : memUsage( ss.memUsage.load( ) ), timeBased( ss.timeBased.load( ) ), logLevelBased( ss.logLevelBased.load( ) )
					{
					}

					Sub_Settings operator=( const Sub_Settings &ss )
					{
						memUsage.store( ss.memUsage.load( ) );
						timeBased.store( ss.timeBased.load( ) );
						logLevelBased.store( ss.logLevelBased.load( ) );
						return *this;
					}
					std::atomic<int>                       memUsage;
					std::atomic<std::chrono::milliseconds> timeBased;
					std::atomic<LoggerLevel>               logLevelBased;
					std::atomic<bool>                      settingsChange;
				} subSettings;

				Flush            primaryOption;
				Periodic_Options subOption;
				bool             sFlush;
			};

		  public:
			/*Flush_Policy( ) : options( Flush_Settings { } ) { }*/

			Flush_Policy( Flush_Settings settings ) : options( std::move( settings ) ) { }
			Flush_Policy( Flush mode, Periodic_Options subOption )
			{
				options.primaryOption = mode;
				options.subOption     = subOption;
				options.sFlush        = ( mode != Flush::never ) ? true : false;
			}
			Flush_Policy( Flush_Policy &fp )
			{
				options.primaryOption = fp.options.primaryOption;
				options.subOption     = fp.options.subOption;
				options.sFlush        = ( options.primaryOption != Flush::never ) ? true : false;
			}

			Flush_Policy( Flush_Policy &&fp ) noexcept
			{
				options.primaryOption = fp.options.primaryOption;
				options.subOption     = fp.options.subOption;
				options.subSettings   = std::move( fp.options.subSettings );
				options.sFlush        = fp.options.sFlush;
			}
			Flush_Policy &operator=( const Flush_Policy &fp )
			{
				options = fp.options;
				return *this;
			}
			Flush_Policy &&operator=( const Flush_Policy &&fp ) noexcept
			{
				options = fp.options;
				return options;
			}

			~Flush_Policy( ) = default;

			const bool ShouldFlush( )
			{
				return options.sFlush;
			}

			void SetFlushOptions( Flush_Settings flushOptions )
			{
				options.primaryOption = flushOptions.primaryOption;
				options.subOption     = flushOptions.subOption;
				options.sFlush        = flushOptions.sFlush;
				options.subSettings.logLevelBased.exchange( flushOptions.subSettings.logLevelBased.load( ) );
				options.subSettings.memUsage.exchange( flushOptions.subSettings.memUsage.load( ) );
				options.subSettings.timeBased.exchange( flushOptions.subSettings.timeBased.load( ) );
			}

			Flush_Settings::Sub_Settings GetAtomics( )
			{
				return options.subSettings;
			}

			const Flush_Settings GetSettings( )
			{
				return options;
			}

			const Periodic_Options GetPeriodicSetting( )
			{
				return options.subOption;
			}

			const Flush GetFlushSetting( )
			{
				return options.primaryOption;
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

				void         SetFlushPolicy( Flush_Policy::Flush_Settings primaryOption );
				Flush_Policy Policy( );

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

				// A couple of fallacies here at the moment...
				/*
					- Shouldn't have to reserve internalBufferSize Unless isWriteToBuf = true
					- Should abstract away the message level setting from here - this function
					  should only be calling the related PrintMesage() & PolicyFlushOn() functions
					- Formatting Shouldn't be called here. That  should be called in PrintMessage()
					  - Although, originally it WAS done in the PrintMessage(), was having issues with
						being able to forward arguments to format into the buffer if isWriteToBuf = true
					- Asynchronous and, in the future, Multi-threading checks shouldnt be done here
					  - They should each be their own data structures that pass a reference to settings
						in the overall log calls (Should definitely do this soon before adding
						asynchronous/synchronous writing from different sources to file i/o and
						DEFINITELY before tackling any real multi-threading tasks
					- Should probably just have a simple buffer structure instead of keeping statics
					  all over the place
					- the if logLevel <= messageLevel check should be abstracted away as well
					  - An idea for abstraction and performance might be to just set bits similar to the
						original idea for formatting flags and just check if the bit value is a certain
						level or not instead of diving into byte comparison values here
						- This could even be applied to almost every check, like buffer writing and async/sync,
						  multi-threaded/single-threaded

						  ___________________________Idea for Ideal Calls__________________________________


						 std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
						 if (bitFieldCheck explicitly for multi-threaded flag set){
							std::lock_guard<std::mutex> lock( base_mutex );
						 }
						 if( messageTimePoint != lastLogTime ) {
							lastLogTime = messageTimePoint;
							somePreAllocatedBufferStructure = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
						 }
						 // Keep the idea for only changing the first pre-allocated buffer if time-stamps don't agree
						someOtherPreAllocattedBufferStructureForMessages.append/.push_back/...
						firstPreAllocatedBuffer .append/push_back/... svToString( s ).append( "\n" );

						HandleBitFieldFlagsFunction(AllocatedBufferWithFullMessage);
						-> where HandleBitFieldFlagsFunction() contains a switch on bitfield values like so:

						switch(bitFieldFlag){
						// Function Calls that deal with bitFieldValues
						// Example: Async To Buffer Logging if shouldLog bit, async bit, writeToBuffer bit, timeElapsedShoudFlush bit,
						//          and thisIsAFileCall so FLush() bit are all set (Variations Galore with this route)

						case bitFieldxFF:
							internalBuffer.reserve(internalBuffer.size() + Passed In PreAllocatedMessageBuffer.size() );
							auto async_format_call = [ this, = ]( )
							{
								// aysnc call should capture a copy since the messages can't be garuanteed to stay in message buffer
								// Any Formatting Calls GO IN HERE!!! (Unless I can make a faster one - default to std::format/vformat
								return std::move( std::vformat( msg, std::make_format_args( ( args )... ) ) );
								// conditional atomic or variable to ensure we can call flush() if need be
								Flush();
							};
							base_futures.push_back( std::async( std::launch::async, async_format_call) );
						}
						}
					}
					// Not a HUGE change overall, but may prove more efficient and easier to maintain going forward?
					// At the very least, there's less conditional checks as each condition down the line would set/reset bit values
					// to be passed into the HandleBitFieldFlagsFunction(), then future updates could in essence just make the bitField
					// larger and ADD code to the HandleBitFieldFlagsFunction() instead of having to make changes everywhere
				*/

				template <typename... Args> void test( std::string_view s, Args &&...args )
				{
					std::lock_guard<std::mutex> lock( base_mutex );
					if( logLevel <= LoggerLevel::test ) {
						msgDetails.SetMsgLevel( LoggerLevel::test );
						std::chrono::seconds messageTimePoint = msgDetails.MessageTimePoint( );
						static std::string   preFormat;
						std::string          msg;
						preFormat.reserve( msgPattern.FormatSplices( )->wholeFormatString.size( ) );
						msg.reserve( preFormat.capacity( ) + s.size( ) );

						if( messageTimePoint != lastLogTime ) {
							lastLogTime = messageTimePoint;
							preFormat.clear( );
							preFormat = std::move( msgPattern.UpdateFormatForTime( msgDetails.TimeInfo( ) ) );
						}
						msg = preFormat + svToString( s ).append( "\n" );
						internalBuffer.reserve( internalBuffer.size( ) + msg.size( ) );

						if( isWriteToBuf( ) ) {
							auto async_format = [ = ]( )
							{
								return std::move( std::vformat( msg, std::make_format_args( ( args )... ) ) );
							};
							base_futures.push_back( std::async( std::launch::async, async_format ) );
							PolicyFlushOn( policy );
						}
						else {
							std::format_to( std::back_inserter( internalBuffer ), msg, std::forward<Args &&>( args )... );
							PrintMessage( internalBuffer );
							PolicyFlushOn( policy );
						}
					}
				}

				std::vector<std::future<std::string>> *AsyncFutures( )
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
				virtual void                          PolicyFlushOn( Flush_Policy ) { }
				msg_details::Message_Formatter *      MsgFmt( );
				msg_details::Message_Info *           MsgInfo( );
				void                                  NotifyAllAtomicSubs( );
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
	}      // namespace expiremental
}  // namespace serenity
