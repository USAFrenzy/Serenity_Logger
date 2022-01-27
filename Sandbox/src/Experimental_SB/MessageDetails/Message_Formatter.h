#pragma once

#include "../Common.h"
#include "Message_Info.h"

#include <format>
#include <string>

namespace serenity::expiremental::msg_details
{
	class Message_Formatter
	{
	  public:
		explicit Message_Formatter( std::string_view pattern, Message_Info *details );
		~Message_Formatter( )                          = default;
		Message_Formatter( )                           = delete;
		Message_Formatter( const Message_Formatter & ) = delete;
		Message_Formatter &operator=( const Message_Info & ) = delete;

		struct Formatter
		{
			virtual std::string_view Format( ) = 0;
			virtual std::string      UpdateInternalView( )
			{
				return "";
			};
		};

		class Formatters
		{
		  public:
			Formatters( std::vector<std::unique_ptr<Formatter>> &&container );
			Formatters( );
			void             Emplace_Back( std::unique_ptr<Formatter> &&formatter );
			std::string_view Format( );
			void             Clear( );

		  private:
			std::string                             localBuffer;
			std::vector<std::unique_ptr<Formatter>> m_Formatter;
		};

		void          FlagFormatter( size_t flag );
		void          SetPattern( std::string_view pattern );
		Formatters &  GetFormatters( );
		void          StoreFormat( );
		Message_Info *MessageDetails( );

		// Formatting Structs For Flag Arguments
	  private:
		struct Format_Arg_a : Formatter
		{
			Format_Arg_a( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    hour;
		};

		struct Format_Arg_b : Formatter
		{
			Format_Arg_b( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMonth;
			std::string    month;
		};

		struct Format_Arg_d : Formatter
		{
			Format_Arg_d( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastDay;
			std::string    day;
		};

		struct Format_Arg_l : Formatter
		{
			Format_Arg_l( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			LoggerLevel &levelRef;
			LoggerLevel  lastLevel;
			std::string  levelStr;
		};

		struct Format_Arg_n : Formatter
		{
			Format_Arg_n( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    ddmmyy;
		};

		struct Format_Arg_t : Formatter
		{
			Format_Arg_t( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    hmStr;
		};

		struct Format_Arg_w : Formatter
		{
			Format_Arg_w( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastDay { 0 };
			std::string    lastDecDay;
		};

		struct Format_Arg_x : Formatter
		{
			Format_Arg_x( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			std::string    wkday;
			int            lastWkday { 0 };
		};

		struct Format_Arg_y : Formatter
		{
			Format_Arg_y( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			Message_Time & timeRef;
			int            lastYear;
			std::string    year;
		};

		struct Format_Arg_A : Formatter
		{
			Format_Arg_A( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    dayHalf;
		};

		struct Format_Arg_B : Formatter
		{
			Format_Arg_B( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMonth;
			std::string    month;
		};

		struct Format_Arg_D : Formatter
		{
			Format_Arg_D( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    mmddyy;
		};

		struct Format_Arg_F : Formatter
		{
			Format_Arg_F( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    yymmdd;
		};

		struct Format_Arg_H : Formatter
		{
			Format_Arg_H( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    hour;
		};

		struct Format_Arg_L : Formatter
		{
			Format_Arg_L( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			LoggerLevel &levelRef;
			LoggerLevel  lastLevel;
			std::string  levelStr;
		};

		struct Format_Arg_M : Formatter
		{
			Format_Arg_M( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    min;
		};

		struct Format_Arg_N : Formatter
		{
			Format_Arg_N( Message_Info &info );
			std::string_view Format( ) override;

		  private:
			std::string &name;
		};

		struct Format_Arg_S : Formatter
		{
			Format_Arg_S( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastSec;
			std::string    sec;
		};

		struct Format_Arg_T : Formatter
		{
			Format_Arg_T( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    hmStr;
		};

		struct Format_Arg_X : Formatter
		{
			Format_Arg_X( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastWkday;
			std::string    wkday;
		};

		struct Format_Arg_Y : Formatter
		{
			Format_Arg_Y( Message_Info &info );
			std::string      UpdateInternalView( ) override;
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastYear;
			std::string    year;
		};

		struct Format_Arg_Message : Formatter
		{
			Format_Arg_Message( Message_Info &info );
			std::string_view Format( ) override;

		  private:
			std::string &message;
		};

		struct Format_Arg_Char : Formatter
		{
			Format_Arg_Char( std::string_view ch );
			std::string_view Format( ) override;

		  private:
			std::string m_char;
		};

		Formatters    formatter;
		std::string   fmtPattern;
		Message_Info *msgInfo;
	};
}  // namespace serenity::expiremental::msg_details
