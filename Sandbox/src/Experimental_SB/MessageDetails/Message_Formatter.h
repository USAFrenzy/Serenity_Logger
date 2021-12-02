#pragma once

#include <string>
#include "../Common.h"
#include "Message_Info.h"
#include <functional>
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
				struct internalFormat
				{
					std::string partitionUpToSpecifier;
					std::string timeDatePartition;
					std::string remainingPartition;
					std::string wholeFormatString;
				};


			      public:
				explicit Message_Formatter( std::string formatPattern, Message_Info *msgDetails );
				Message_Formatter &operator=( const Message_Formatter &t );
				// returns user format string
				std::string GetMsgFmt( );
				// sets the format pattern and stores the pattern in an internally efficient manner
				void SetPattern( std::string_view pattern );

				// TODO: See if it's possible to have a work-around to keep formatting functions private
				/*
					These were initially private but had to be made public to initialize formatting vector handler.
					If possible, I would Like for these to remain private...
				*/
				// Returns formatted message string based on args and if there's a log format string, appends to the
				// log format string before returning.
				std::string &FormatMsg( const std::string_view message, std::format_args &&args );

				// TODO: Work on implementing template specializations for all arguments below
				/*
				   After a couple of talks with other people on ideas, it seems in order to get FormatMsg() to be
				   faster, I should look into template specialization of std::formatter. Each of the argument functions
				   below would have to either be a struct or a class instead that inherits from the template
				   specialization and implements the parse and format functions uniquely. I was trying for a method
				   that was similar but different from spdlog's approach, but it seems this method will be heavily
				   inspired by the formatting libraries specializations and how spdlog implemented something similar.
				   This method would allow for the removal of the Cached_Date_Time struct though in favor of just
				   grabbing the current time directly and using std::formatters based on std::chrono which could prove
				   very efficient
				*/

				// Twelve Hour Format (13 -> 1, 14 -> 2, 11 -> 11, 10 -> 10, etc)
				std::string Format_Arg_a( Cached_Date_Time &cache );
				// AM Or PM Specifier
				std::string Format_Arg_A( Cached_Date_Time &cache );
				// Short Month Format (Jan, Feb, Mar, etc)
				std::string Format_Arg_b( Cached_Date_Time &cache );
				// Long Month Format (January, February, March, etc)
				std::string Format_Arg_B( Cached_Date_Time &cache );
				// MM/DD/YY Format
				std::string Format_Arg_D( Cached_Date_Time &cache );
				// YYYY-MM-DD Format
				std::string Format_Arg_F( Cached_Date_Time &cache );
				// Padded Minute ( 07, 08, 13, 15, etc)
				std::string Format_Arg_M( Cached_Date_Time &cache );
				// Padded Second ( 07, 08, 13, 15, etc)
				std::string Format_Arg_S( Cached_Date_Time &cache );
				// HH:MM:SS Format With A 12 Hour Clock Time
				std::string Format_Arg_t( Cached_Date_Time &cache );
				// HH:MM:SS Format With A 24 Hour Clock Time
				std::string Format_Arg_T( Cached_Date_Time &cache );
				// Decimal Weekday Format (0-6)
				std::string Format_Arg_w( Cached_Date_Time &cache );
				// Short Year Format (For 2021 -> 21, For 2022 -> 22)
				std::string Format_Arg_y( Cached_Date_Time &cache );
				// Long Year Format (2021, 2022)
				std::string Format_Arg_Y( Cached_Date_Time &cache );
				// Name Passed In To Target Or Default Name If None Specified
				std::string Format_Arg_N( Cached_Date_Time &cache );
				// Short Message Level (T, I, D, W, E, F)
				std::string Format_Arg_l( Cached_Date_Time &cache );
				// Long Message Level (Trace, Info, Debug, Warn, Error, Fatal)
				std::string Format_Arg_L( Cached_Date_Time &cache );
				// Padded Day Date (01-31)
				std::string Format_Arg_d( Cached_Date_Time &cache );
				// Padded 24 Hour Format (00-23)
				std::string Format_Arg_H( Cached_Date_Time &cache );
				// Short Weekday Format (Mon, Tues, Wed, Thurs, Fri, Sat, Sun)
				std::string Format_Arg_x( Cached_Date_Time &cache );
				// Short Weekday Format (Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
				std::string Format_Arg_X( Cached_Date_Time &cache );
				// Padded DDMMMYY Format (01Nov21, 09Oct22, 12Dec22 etc)
				std::string Format_Arg_n( Cached_Date_Time &cache );

				// for micro-benches
				struct TimeStats
				{
					float totalUpdateTime;
					float totalFormatTime;
				};
				TimeStats GetStats( )
				{
					return benches;
				}

			      private:
				// Indexes into format function handler to retrieve and return flag value
				std::string FlagFormatter( Cached_Date_Time &cache, int flag );
				// parses and stores format into partitions to be used more effieciently
				void StoreFormat( );
				// parses internal format flag partition string for time-date variables and updates them
				std::string &UpdateFormatForTime( std::chrono::seconds timePoint );
				// adds all formatter functions to a format function handler to be indexed into
				void InitFunctionHandler( );

			      private:
				std::vector<std::function<std::string( Cached_Date_Time )>> fmtFunctions;
				internalFormat                                              internalFmt;
				std::string                                                 fmtPattern;
				Message_Info *                                              msgInfo;
				std::string                                                 buffer;
				std::vector<int>                                            flags;

				// for some micro benches
				TimeStats benches;
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity