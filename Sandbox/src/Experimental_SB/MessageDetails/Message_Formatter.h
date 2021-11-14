#pragma once


#include <string>
#include "../Common.h"
#include "Message_Info.h"

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
				std::string        GetMsgFmt( );
				void               SetPattern( std::string_view pattern );
				std::string_view   FormatMessage( std::string message, std::format_args args);

			      private:
				// Passing In The Updated Cache Once From The FmtMessage() Call Beats Consistently Calling An Update In
				// The Format Argument Functions And In The Flag Formatter Top Level Function
				std::string FlagFormatter( Cached_Date_Time cache, char flag );

				std::string Format_Arg_a( Cached_Date_Time cache );
				std::string Format_Arg_A( Cached_Date_Time cache );
				std::string Format_Arg_b( Cached_Date_Time cache );
				std::string Format_Arg_B( Cached_Date_Time cache );
				std::string Format_Arg_D( Cached_Date_Time cache );
				std::string Format_Arg_F( Cached_Date_Time cache );
				std::string Format_Arg_M( Cached_Date_Time cache );
				std::string Format_Arg_S( Cached_Date_Time cache );
				std::string Format_Arg_t( Cached_Date_Time cache );
				std::string Format_Arg_T( Cached_Date_Time cache );
				std::string Format_Arg_w( Cached_Date_Time cache );
				std::string Format_Arg_y( Cached_Date_Time cache );
				std::string Format_Arg_Y( Cached_Date_Time cache );
				std::string Format_Arg_N( );
				std::string Format_Arg_l( );
				std::string Format_Arg_L( );
				std::string Format_Arg_d( Cached_Date_Time cache );
				std::string Format_Arg_H( Cached_Date_Time cache );
				std::string Format_Arg_x( Cached_Date_Time cache );
				std::string Format_Arg_X( Cached_Date_Time cache );
				std::string Format_Arg_n( Cached_Date_Time cache );

			      private:
				std::string   fmtPattern;
				Message_Info *msgInfo;
				std::string   buffer;
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity