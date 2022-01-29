#pragma once

#include "../Common.h"
#include "Message_Info.h"

#include <format>
#include <string>

// TODO: Finish the light documentation
namespace serenity::expiremental::msg_details
{
	class Message_Formatter
	{
	  public:
		/// <summary>
		/// Constructor that takes in a format pattern paramater and a Message_Info class pointer.
		/// The format pattern is set and internally stored by its individual flag arguments to iterate over when a message is logged.
		/// The Message_Info class is used to populate the arguments or references when storing and using the individual flag
		/// components
		/// </summary>
		explicit Message_Formatter( std::string_view pattern, Message_Info *details );
		/// <summary> Default Destructor clean up </summary>
		~Message_Formatter( ) = default;
		/// <summary> Deleted </summary>
		Message_Formatter( ) = delete;
		/// <summary> Deleted </summary>
		Message_Formatter( const Message_Formatter & ) = delete;
		/// <summary> Deleted </summary>
		Message_Formatter &operator=( const Message_Info & ) = delete;
		/// <summary>
		/// Virtual Base Struct that all formatter structs inherit from and implement
		/// </summary>
		struct Formatter
		{
			/// <summary> Each derived class or struct must implement this funtion. This function determines how an argument will be
			/// formatted </summary>
			virtual std::string_view Format( ) = 0;
			/// <summary> A derived class or struct may implement this function. This function determines if an argument needs to be
			/// updated if a derived class or struct implements some form of caching with argument values</summary>
			virtual std::string UpdateInternalView( )
			{
				return "";
			};
		};

		class Formatters
		{
		  public:
			/// <summary>
			/// Constructor that takes ownership of the vector of Formatter pointers passed in. The class internal buffer reserves 32
			/// bytes per pointer to help mitigate reallocations for how these arguments are formatted from the formatter pointers
			/// </summary>
			Formatters( std::vector<std::unique_ptr<Formatter>> &&container );
			/// <summary> Default Constructor. Internal formatters container is empty and internal buffer holds the compiler-specific
			/// capacity of a string </summary>
			Formatters( ) = default;
			/// <summary>
			/// Takes ownership of the Formatter pointer passed in and stores this pointer in the internal Formatters container.
			/// Reserves the current size of the Formatters container plus 32 bytes for the Formatter pointer passed in to help
			/// mitigate reallocations for how these arguments are formatted from the formatter pointers
			/// </summary>
			void Emplace_Back( std::unique_ptr<Formatter> &&formatter );
			/// <summary>
			/// Calls each Formatter pointer's specific Format() implementation that is stored in the internal Formatter container in
			/// the order they are stored and returns the whole formatted string as a view
			/// </summary>
			/// <returns>string view of cumulative formatted arguments</returns>
			std::string_view Format( );
			/// <summary> Clears the internal Formatter pointers container </summary>
			void Clear( );

		  private:
			std::string                             localBuffer;
			std::vector<std::unique_ptr<Formatter>> m_Formatter;
		};
		/// <summary>
		/// If called by StoreFormat(), Takes in an index found by matching the position of the potential flag to the position of that
		/// flag in the valid flags array, initializes and stores the Formatter struct in the Message_Formatter's instance of
		/// Formatters class, otherwise,  just intializes and stores the Formatter struct at this index
		/// </summary>
		void FlagFormatter( size_t flag );
		/// <summary>
		/// Sets the format pattern variable and then parses the format string to store each flag as its own individual Formmater
		/// struct that will be in charge of how each flag is formatted
		/// </summary>
		void SetPattern( std::string_view pattern );
		/// <summary>
		/// Returns the Message_Formatter's instance of the Formatters container which holds the individual Formatter pointers. Can be
		/// called to manually call the Format( ) function for all arguments stored
		/// </summary>
		Formatters &GetFormatters( );
		/// <summary>
		/// SetPattern( ) calls this function internally. Parses the internal format pattern string stored from SetPattern( )
		/// or from a constructor that took in a format pattern argument and for each flag found by the delimiter "%", will try to
		/// match the potential flag to the index of the valid flags array. If a match is found, initializes and stores the respective
		/// Formatter struct for that flag, otherwise, stores this value as well as any other char, by passing in the value to
		/// initialize and store a Format_Arg_Char struct
		/// </summary>
		void StoreFormat( );
		/// <summary> Returns a pointer to the Message_Info instance used by Message_Formatter </summary>
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
