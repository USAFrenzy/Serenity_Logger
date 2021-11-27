#pragma once

#include "../Common.h"

#include "../MessageDetails/Message_Info.h"
#include "../MessageDetails/Message_Time.h"
#include "../MessageDetails/Message_Formatter.h"

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
				~TargetBase( ) = default;

				void               SetFlushPolicy( Flush_Policy policy );
				const Flush_Policy FlushPolicy( );

				std::string                      LoggerName( );
				void                             SetPattern( std::string_view pattern );
				void                             ResetPatternToDefault( );
				void                             SetLogLevel( LoggerLevel level );
				LoggerLevel                      Level( );
				template <typename... Args> void trace( std::string_view s, Args &&...args );
				template <typename... Args> void info( std::string_view s, Args &&...args );
				template <typename... Args> void debug( std::string_view s, Args &&...args );
				template <typename... Args> void warn( std::string_view s, Args &&...args );
				template <typename... Args> void error( std::string_view s, Args &&...args );
				template <typename... Args> void fatal( std::string_view s, Args &&...args );


			      protected:
				virtual void PrintMessage( msg_details::Message_Info msgInfo, const std::string_view msg, std::format_args &&args ) = 0;
				virtual void PolicyFlushOn( Flush_Policy &policy, std::string_view msg) { }

				msg_details::Message_Formatter *MsgFmt( );
				msg_details::Message_Info *     MsgInfo( );

			      private:
				Flush_Policy                   policy;
				LoggerLevel                    logLevel;
				LoggerLevel                    msgLevel;
				std::string                    pattern;
				std::string                    loggerName;
				msg_details::Message_Info      msgDetails;
				msg_details::Message_Formatter msgPattern;
			};
#include "Target-impl.h"
		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
