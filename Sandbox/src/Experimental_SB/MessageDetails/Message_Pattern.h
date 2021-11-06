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
			class Message_Pattern
			{
			      public:
				explicit Message_Pattern( std::string formatPattern, Message_Info *msgDetails );
				Message_Pattern &operator=( const Message_Pattern &t );
				std::string      GetMsgFmt( );

				// Iterate through the pre-message format pattern in order to find any flags. When a flag is reached,
				// append everything up until that flag to the buffer, erase the flag qualifier, handle the flag, then
				// erase the flag token at that position and continue iterating until the end of format pattern is
				// reached. If no flag is found, or no more flags are available to handle, just append the rest of the
				// format pattern to the buffer. Append the formatted message string (using the forwarded arguments to
				// format from) to the buffer and return the entire thing
				template <typename... Args> inline std::string_view FmtMessage( std::string message, Args &&...args )
				{
					buffer.clear( );
					size_t it { 0 }, pos { 0 };
					auto   fmt = fmtPattern;

					while( it != std::string::npos && ( !fmt.empty( ) ) ) {
						if( fmt.front( ) == '%' ) {
							buffer.append( fmt.substr( 0, pos ) );
							fmt.erase( 0, pos + 1 );
							buffer.append( FlagFormatter( fmt.front( ) ) );
							fmt.erase( 0, pos + 1 );  // Erase the Flag Token
						}
						else {
							buffer += fmt.at( 0 );
							fmt.erase( 0, 1 );
						}
					}
					return buffer.append( fmt + std::move( std::format( message, std::forward<Args>( args )... ) ) );
				}

			      private:
				std::string FlagFormatter( char flag );

				std::string Format_Arg_b( );
				std::string Format_Arg_B( );
				std::string Format_Arg_D( );
				std::string Format_Arg_F( );
				std::string Format_Arg_M( );
				std::string Format_Arg_S( );
				std::string Format_Arg_T( );
				std::string Format_Arg_w( );
				std::string Format_Arg_y( );
				std::string Format_Arg_Y( );
				std::string Format_Arg_N( );
				std::string Format_Arg_l( );
				std::string Format_Arg_L( );
				std::string Format_Arg_d( );
				std::string Format_Arg_H( );
				std::string Format_Arg_x( );
				std::string Format_Arg_X( );
				std::string Format_Arg_n( );

			      private:
				std::string   fmtPattern;
				Message_Info *msgInfo;
				std::string   buffer;
			};
		}  // namespace msg_details
	}          // namespace expiremental
}  // namespace serenity