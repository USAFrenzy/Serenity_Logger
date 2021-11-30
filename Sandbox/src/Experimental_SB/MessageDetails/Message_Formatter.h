#pragma once

#include <string>
#include "../Common.h"
#include "Message_Info.h"
#include <functional>

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
					std::string flagPartition;
					std::string remainingPartition;
					std::string wholeFormatString;
				};

			      public:
				explicit Message_Formatter( std::string formatPattern, Message_Info *msgDetails );
				Message_Formatter &operator=( const Message_Formatter &t );
				std::string        GetMsgFmt( );
				void               SetPattern( std::string_view pattern );
				std::string        FormatMsg( const std::string_view message, std::format_args args );


				std::string Format_Arg_a( Cached_Date_Time &cache );
				std::string Format_Arg_A( Cached_Date_Time &cache );
				std::string Format_Arg_b( Cached_Date_Time &cache );
				std::string Format_Arg_B( Cached_Date_Time &cache );
				std::string Format_Arg_D( Cached_Date_Time &cache );
				std::string Format_Arg_F( Cached_Date_Time &cache );
				std::string Format_Arg_M( Cached_Date_Time &cache );
				std::string Format_Arg_S( Cached_Date_Time &cache );
				std::string Format_Arg_t( Cached_Date_Time &cache );
				std::string Format_Arg_T( Cached_Date_Time &cache );
				std::string Format_Arg_w( Cached_Date_Time &cache );
				std::string Format_Arg_y( Cached_Date_Time &cache );
				std::string Format_Arg_Y( Cached_Date_Time &cache );
				std::string Format_Arg_N( Cached_Date_Time &cache );
				std::string Format_Arg_l( Cached_Date_Time &cache );
				std::string Format_Arg_L( Cached_Date_Time &cache );
				std::string Format_Arg_d( Cached_Date_Time &cache );
				std::string Format_Arg_H( Cached_Date_Time &cache );
				std::string Format_Arg_x( Cached_Date_Time &cache );
				std::string Format_Arg_X( Cached_Date_Time &cache );
				std::string Format_Arg_n( Cached_Date_Time &cache );

			      private:
				// Passing In The Updated Cache Once From The FmtMessage() Call Beats Consistently Calling An Update In
				// The Format Argument Functions And In The Flag Formatter Top Level Function
				std::string  FlagFormatter( Cached_Date_Time &cache, std::string &flag );
				void         StoreFormat( );
				std::string &UpdateFormatForTime( std::chrono::system_clock::duration timePoint );
				void         InitFunctionHandler( );

			      private:
				std::vector<std::function<std::string( Cached_Date_Time )>> fmtFunctions;
				internalFormat                                              internalFmt;
				std::string                                                 fmtPattern;
				Message_Info *                                              msgInfo;
				std::string                                                 buffer;
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity