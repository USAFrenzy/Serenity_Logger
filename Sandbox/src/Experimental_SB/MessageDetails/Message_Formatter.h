#pragma once

#include <string>
#include "../Common.h"
#include "Message_Info.h"
#include <format>

#include <serenity/Utilities/Utilities.h>  // for duration cast typedefs

namespace serenity
{
	namespace expiremental
	{
		namespace msg_details
		{
			class Message_Formatter
			{
			  public:
				explicit Message_Formatter( std::string formatPattern, Message_Info *msgDetails );
				Message_Formatter &operator=( const Message_Formatter &t );
				// returns user format string
				std::string GetMsgFmt( );
				// sets the format pattern and stores the pattern in an internally efficient manner
				void SetPattern( std::string_view pattern );
				// Returns formatted message string based on args and if there's a log format string, appends to the
				// log format string before returning.
				std::string &FormatMsg( const std::string_view message, std::format_args &&args );
				// parses and stores format into partitions to be used more effieciently
				void StoreFormat( );
				// parses internal format flag partition string for time-date variables and updates them
				std::string &UpdateFormatForTime( Cached_Date_Time &cache );

				const InternalFormat &FormatSplices( );

				// for micro-benches
				struct TimeStats
				{
					float totalUpdateTime { 0 };
					float totalFormatTime { 0 };
				};
				TimeStats GetStats( )
				{
					return benches;
				}

			  private:
				// returns evaluated flag's value to the caller
				void FlagFormatter( Cached_Date_Time &cache, int flag );
				// ----------------------------------------------------------------------------
				// TODO: Work on implementing template specializations for all arguments below
				// ----------------------------------------------------------------------------
				// Twelve Hour Format (13 -> 1, 14 -> 2, 11 -> 11, 10 -> 10, etc)
				void Format_Arg_a( Cached_Date_Time &cache );
				// Short Month Format (Jan, Feb, Mar, etc)
				void Format_Arg_b( Cached_Date_Time &cache );
				// Padded Day Date (01-31)
				void Format_Arg_d( Cached_Date_Time &cache );
				// Short Message Level (T, I, D, W, E, F)
				void Format_Arg_l( );
				// Padded DDMMMYY Format (01Nov21, 09Oct22, 12Dec22 etc)
				void Format_Arg_n( Cached_Date_Time &cache );
				// HH:MM:SS Format With A 12 Hour Clock Time
				void Format_Arg_t( Cached_Date_Time &cache );
				// Decimal Weekday Format (0-6)
				void Format_Arg_w( Cached_Date_Time &cache );
				// Short Weekday Format (Mon, Tues, Wed, Thurs, Fri, Sat, Sun)
				void Format_Arg_x( Cached_Date_Time &cache );
				// Short Year Format (For 2021 -> 21, For 2022 -> 22)
				void Format_Arg_y( Cached_Date_Time &cache );
				// AM Or PM Specifier
				void Format_Arg_A( Cached_Date_Time &cache );
				// Long Month Format (January, February, March, etc)
				void Format_Arg_B( Cached_Date_Time &cache );
				// MM/DD/YY Format
				void Format_Arg_D( Cached_Date_Time &cache );
				// YYYY-MM-DD Format
				void Format_Arg_F( Cached_Date_Time &cache );
				// Padded 24 Hour Format (00-23)
				void Format_Arg_H( Cached_Date_Time &cache );
				// Long Message Level (Trace, Info, Debug, Warn, Error, Fatal)
				void Format_Arg_L( );
				// Padded Minute ( 07, 08, 13, 15, etc)
				void Format_Arg_M( Cached_Date_Time &cache );
				// Name Passed In To Target Or Default Name If None Specified
				void Format_Arg_N( );
				// Padded Second ( 07, 08, 13, 15, etc)
				void Format_Arg_S( Cached_Date_Time &cache );
				// HH:MM:SS Format With A 24 Hour Clock Time
				void Format_Arg_T( Cached_Date_Time &cache );
				// Short Weekday Format (Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
				void Format_Arg_X( Cached_Date_Time &cache );
				// Long Year Format (2021, 2022)
				void Format_Arg_Y( Cached_Date_Time &cache );

			  private:
				InternalFormat   internalFmt;
				std::string      fmtPattern;
				Message_Info *   msgInfo;
				std::string      buffer;
				std::vector<int> flags;

				// for some micro benches
				TimeStats benches = { };
			};
		}  // namespace msg_details
	}      // namespace expiremental
}  // namespace serenity