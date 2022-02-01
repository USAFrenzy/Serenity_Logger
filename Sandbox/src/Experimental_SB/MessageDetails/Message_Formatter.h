#pragma once

#include "../Common.h"
#include "Message_Info.h"

#include <format>
#include <string>

// TODO: Finish the light documentation
namespace serenity::expiremental::msg_details
{
	/// Controls how the prepended string of text before the actual log message will be displayed and how each flag/argument
	/// is formatted.
	class Message_Formatter
	{
	  public:
		/// @brief Constructor that takes in a format pattern paramater and a Message_Info class pointer.
		/// @details The format pattern is set and internally stored by its individual flag arguments to iterate over when a
		/// message is logged. The Message_Info class is used to populate the arguments or references when storing and using
		/// the individual flag components
		explicit Message_Formatter( std::string_view pattern, Message_Info *details );
		~Message_Formatter( )                          = default;
		Message_Formatter( )                           = delete;
		Message_Formatter( const Message_Formatter & ) = delete;
		Message_Formatter &operator=( const Message_Info & ) = delete;
		/// @brief Virtual Base Struct that all formatter structs inherit from and implement
		struct Formatter
		{
			/// @brief Each derived class or struct must implement this funtion. This function determines how an argument
			/// will be formatted
			virtual std::string_view Format( ) = 0;
			/// @brief A derived class or struct may implement this function. This function determines if an argument needs
			/// to be updated if a derived class or struct implements some form of caching with argument values
			virtual std::string UpdateInternalView( )
			{
				return "";
			};
		};
		/// @brief This class is the top-level formatting class that holds and controls each flag arguments specific
		/// formatting abilities
		class Formatters
		{
		  public:
			/// @brief Constructor that takes ownership of the vector of Formatter pointers passed in.
			Formatters( std::vector<std::unique_ptr<Formatter>> &&container );
			///  @brief Default Constructor
			Formatters( ) = default;
			/// @brief Takes ownership of the Formatter pointer passed in and stores this pointer in the internal Formatters
			/// container.
			void Emplace_Back( std::unique_ptr<Formatter> &&formatter );
			/// @brief Calls each Formatter pointer's specific Format() implementation that is stored in the internal
			/// Formatter container in the order they are stored and returns the whole formatted string as a view
			std::string_view Format( );
			///  @brief Clears the internal Formatter pointers container
			void Clear( );

		  private:
			std::string                             localBuffer;
			std::vector<std::unique_ptr<Formatter>> m_Formatter;
		};
		/// @brief Intializes and stores the Formatter struct at the index provided into the Formatters container
		void FlagFormatter( size_t flag );
		/// @brief Sets the format pattern variable and then parses the format string to store each flag as its own
		/// individual Formmater struct that will be in charge of how each flag is formatted
		void SetPattern( std::string_view pattern );
		/// @brief Returns the Message_Formatter's instance of the Formatters container which holds the individual Formatter
		/// pointers. Can be called to manually call the Format( ) function for all arguments stored
		Formatters &GetFormatters( );
		/// @brief Store the format pattern internally to be used more efficiently when formatting log messages
		/// @details SetPattern( ) calls this function internally. Parses the internal format pattern string stored from
		/// SetPattern( ) or from a constructor that took in a format pattern argument and for each flag found by the
		/// delimiter "%", will try to match the potential flag to the index of the valid flags array. If a match is found,
		/// initializes and stores the respective Formatter struct for that flag, otherwise, stores this value as well as any
		/// other char, by passing in the value to initialize and store a Format_Arg_Char struct instead
		void StoreFormat( );
		///  @brief Returns a pointer to the Message_Info instance used by Message_Formatter
		Message_Info *MessageDetails( );

		// Formatting Structs For Flag Arguments

		/// @brief Formatter struct that maps to flag "%a" and represents a padded 12-hour clock's hour value
		struct Format_Arg_a : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and initializes the
			/// current and last known hour value
			Format_Arg_a( Message_Info &info );
			/// @brief will update both the last known hour to the current hour as well as the padded string representation
			/// of the current hour value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details  When called, if the current hour value doesn't match the last known hour value, will call
			/// UpdateInternalView( ) and then return the internal string representation of the 12-hour clock hour value,
			/// otherwise, just returns the internal string representation of the padded 12-hour clock hour value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    hour;
		};

		/// @brief Formatter struct that maps to flag "%b" and represents a padded numerical month value
		struct Format_Arg_b : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and initializes the
			/// current and last known month values
			Format_Arg_b( Message_Info &info );
			/// @brief Will update both the last known month to the current month as well as the padded string representation
			/// of the current month value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current month value doesn't match the last known month value, will call
			/// UpdateInternalView( ) and then return the internal string representation of the numerical month value,
			/// otherwise, just returns the internal string representation of the numerical month value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMonth;
			std::string    month;
		};

		/// @brief Formatter struct that maps to flag "%d" and represents a padded numerical day value
		struct Format_Arg_d : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and initializes the
			/// current and last known numerical day values
			Format_Arg_d( Message_Info &info );
			/// @brief will update both the last known day to the current day as well as the padded string representation of
			/// the current day value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current day value doesn't match the last known day value, will call
			/// UpdateInternalView( ) and then return the internal string representation of the numerical day value,
			/// otherwise, just returns the internal string representation of the numerical day value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastDay;
			std::string    day;
		};

		/// @brief Formatter struct that maps to flag "%l" and represents the one character log level (i.e. "T" for trace,
		/// "F" for fatal)
		struct Format_Arg_l : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the message level
			Format_Arg_l( Message_Info &info );
			/// @brief Will update the last known level to the current level and return the one character representation of
			/// the log level
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current log level doesn't match the last level seen, will update the last known
			/// level variable as well as the internal representation of the level string and return the internal
			/// representation, otherwise, just returns the internal representation of the level string stored
			std::string_view Format( ) override;

		  private:
			LoggerLevel &levelRef;
			LoggerLevel  lastLevel;
			std::string  levelStr;
		};

		/// @brief Formatter struct that maps to flag "%n" and represents the numerical day, textual month, and numerical
		/// year in a DDMMMYY format
		struct Format_Arg_n : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known day values
			Format_Arg_n( Message_Info &info );
			/// @brief Will update the last known day to the current day and then return a string representation of the date
			/// in DDMMYY format
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current day doesn't match the last known day, will update the last known day to
			/// the current day and then call UpdateInternalView( ) to update the internal string representation of the date
			/// in DDMMMYY format, otherwise, just returns the internal string representation of the date in DDMMMYY format
			/// stored
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    ddmmyy;
		};

		/// @brief Formatter struct that maps to flag "%t" and represents the wall clock time in a 12 hour format (i.e.
		/// 12:45:05) format
		struct Format_Arg_t : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known minute values
			Format_Arg_t( Message_Info &info );
			/// @brief Will update the last known minute to the current minute and then return a string representation of the
			/// wall clock time in a 12 hour format
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current minute doesn't match the last known minute, will update the last known
			/// minute to the current minute and then call UpdateInternalView( ) to update the internal string representation
			/// of the time in a 12-hour format, otherwise, just returns the internal string representation of the time in a
			/// 12-hour format stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    hmStr;
		};

		/// @brief Formatter struct that maps to flag "%w" and represents the unpadded string representation of the numerical
		/// weekday value (as in tm_wkday) format
		struct Format_Arg_w : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known numerical weekday values
			Format_Arg_w( Message_Info &info );
			/// @brief Will update the last known numerical weekday to the current numerical weekday and then return an
			/// unpadded string representation of the numerical weekday
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current numerical weekday value doesn't match the last known numerical weekday
			/// value, will update the last known numerical weekday value to the current numerical weekday value and then
			/// call UpdateInternalView( ) to update the internal string representation of the new weekday value, otherwise,
			/// just returns the internal numerical weekday representation stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastDay { 0 };
			std::string    lastDecDay;
		};

		/// @brief Formatter struct that maps to flag "%x" and represents the string representation of the shortened weekday
		/// name (i.e. For Monday - Mon, for Tuesday - Tues, etc...)
		struct Format_Arg_x : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known weekday values
			Format_Arg_x( Message_Info &info );
			/// @brief Will update the last known weekday to the current weekday and then return a string representation of
			/// the shortened weekday
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current weekday value doesn't match the last known weekday value, will update
			/// the last known weekday value to the current weekday value and then call UpdateInternalView( ) to
			/// update the internal string representation of the new shortened weekday value, otherwise, just returns the
			/// internal shortened weekday representation stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			std::string    wkday;
			int            lastWkday { 0 };
		};

		/// @brief Formatter struct that maps to flag "%y" and represents the string representation of the shortened year
		/// (i.e. "21", "22", etc)
		struct Format_Arg_y : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known shortened year values
			Format_Arg_y( Message_Info &info );
			/// @brief Will update the last known shortened year to the current shortened year and then return a string
			/// representation of the shortened year
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			///@details  When called, if the current shortened year value doesn't match the last known shortened year value,
			/// will
			/// update the last known shortened year value to the current shortened year value and then call
			/// UpdateInternalView( ) to update the internal string representation of the new shortened year value,
			/// otherwise, just returns the internal year representation stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			Message_Time & timeRef;
			int            lastYear;
			std::string    year;
		};

		/// @brief Formatter struct that maps to flag "%A" and represents the string representation of "AM" or "PM" dependant
		/// on the hour
		struct Format_Arg_A : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known hour values
			Format_Arg_A( Message_Info &info );
			/// @brief Will update the last known hour to the current hour and then return a string representation of the day
			/// half marker
			/// ("AM" or "PM")
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current hour value doesn't match the last known hour value, will update
			/// the last known hour value to the current hour value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current day half ("AM" or "PM"), otherwise, just returns the
			/// internal day half representation stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    dayHalf;
		};

		/// @brief Formatter struct that maps to flag "%B" and represents the string representation the whole month name
		/// (i.e. "January", "February", etc..)
		struct Format_Arg_B : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known month values
			Format_Arg_B( Message_Info &info );
			/// @brief Will update the last known month to the current month and then return a string representation of the
			/// whole month name
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			///@details  When called, if the current month value doesn't match the last known month value, will update
			/// the last known month value to the current month value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current month, otherwise, just returns the internal month
			/// representation stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMonth;
			std::string    month;
		};

		/// @brief Formatter struct that maps to flag "%D" and represents the string representation the date in MM/DD/YY
		/// format (i.e. 01/29/22)
		struct Format_Arg_D : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known day values
			Format_Arg_D( Message_Info &info );
			/// @brief Will update the last known day to the current day and then return a string representation of the date
			/// in MMDDYY format
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current day value doesn't match the last known day value, will update
			/// the last known day value to the current day value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current date in MM/DD/YY format, otherwise, just returns the
			/// internal date representation stored
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    mmddyy;
		};

		/// @brief Formatter struct that maps to flag "%F" and represents the string representation the date in YY-MM-DD
		/// format
		struct Format_Arg_F : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known day values
			Format_Arg_F( Message_Info &info );
			/// @brief Will update the last known day to the current day and then return a string representation of the date
			/// in YY-MM-DD format
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current day value doesn't match the last known day value, will update
			/// the last known day value to the current day value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current date in YY-MM-DD format, otherwise, just returns the
			/// internal date representation stored
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastDay;
			std::string    yymmdd;
		};

		/// @brief Formatter struct that maps to flag "%H" and represents the string representation of a padded hour on a
		/// 24-hour clock
		struct Format_Arg_H : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known hour values
			Format_Arg_H( Message_Info &info );
			/// @brief Will update the last known hour to the current hour and then return a string representation of the
			/// padded hour value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current hour value doesn't match the last known hour value, will update
			/// the last known hour value to the current hour value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current padded hour, otherwise, just returns the internal
			/// padded hour value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastHour;
			std::string    hour;
		};

		/// @brief Formatter struct that maps to flag "%L" and represents the string representation of the full log level
		/// (i.e. "Trace" for trace, "Fatal" for fatal, etc...)
		struct Format_Arg_L : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the message level
			Format_Arg_L( Message_Info &info );
			/// @brief Will update the last known level to the current level and return the string representation of the log
			/// level
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current log level doesn't match the last level seen, will update the last known
			/// level variable as well as the internal representation of the level string and return the internal
			/// representation, otherwise, just returns the internal representation of the level string stored
			std::string_view Format( ) override;

		  private:
			LoggerLevel &levelRef;
			LoggerLevel  lastLevel;
			std::string  levelStr;
		};

		/// @brief Formatter struct that maps to flag "%M" and represents the string representation of a padded minute
		struct Format_Arg_M : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known minute values
			Format_Arg_M( Message_Info &info );
			/// @brief Will update the last known minute to the current minute and then return a string representation of the
			/// padded minute value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current minute value doesn't match the last known minute value, will update
			/// the last known minute value to the current minute value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current padded minute, otherwise, just returns the internal
			/// padded minute value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    min;
		};

		/// @brief Formatter struct that maps to flag "%N" and represents the Logger's name to use when formatting
		struct Format_Arg_N : Formatter
		{
			///  @brief Constructor that uses the Message_Info pointer to store a reference to the logger's name
			Format_Arg_N( Message_Info &info );
			///  @brief When called, returns the logger's name as-is
			std::string_view Format( ) override;

		  private:
			std::string &name;
		};

		/// @brief Formatter struct that maps to flag "%S" and represents the string representation of a padded second
		struct Format_Arg_S : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known second values
			Format_Arg_S( Message_Info &info );
			/// @brief Will update the last known second to the current second and then return a string representation of the
			/// padded second value
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current second value doesn't match the last known second value, will update
			/// the last known second value to the current second value and then call UpdateInternalView( ) to
			/// update the internal string representation of the current padded second, otherwise, just returns the internal
			/// padded second value stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastSec;
			std::string    sec;
		};

		/// @brief Formatter struct that maps to flag "%T" and represents the wall clock time in a 24-hour format (i.e.
		/// 23:45:05) format
		struct Format_Arg_T : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known minute values
			Format_Arg_T( Message_Info &info );
			/// @brief Will update the last known minute to the current minute and then return a string representation of the
			/// wall clock time in a 24-hour format
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current minute doesn't match the last known minute, will update the last known
			/// minute to the current minute and then call UpdateInternalView( ) to update the internal string representation
			/// of the time in a 24-hour format, otherwise, just returns the internal string representation of the time in a
			/// 24-hour format stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastMin;
			std::string    hmStr;
		};

		/// @brief Formatter struct that maps to flag "%X" and represents the entire Weekday name (i.e. "Monday", "Tuesday",
		/// etc...) format
		struct Format_Arg_X : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known weekday values
			Format_Arg_X( Message_Info &info );
			/// @brief Will update the last known weekday to the current weekday and then return a string representation of
			/// the entire weekday name
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current weekday doesn't match the last known weekday , will update the last
			/// known weekday to the current weekday  and then call UpdateInternalView( ) to update the internal string
			/// representation of the weekday, otherwise, just returns the internal string representation of the weekday
			/// stored
			std::string_view Format( ) override;

		  private:
			const std::tm &cacheRef;
			int            lastWkday;
			std::string    wkday;
		};

		/// @brief Formatter struct that maps to flag "%Y" and represents the year in YYYY format format
		struct Format_Arg_Y : Formatter
		{
			/// @brief Constructor that uses the Message_Info pointer to store a reference to the cache and intitialize the
			/// current and last known year values
			Format_Arg_Y( Message_Info &info );
			///  @brief Will update the last known year to the current year and then return a string representation of the
			///  year
			std::string UpdateInternalView( ) override;
			/// @brief  Returns The Formatted Argument
			/// @details When called, if the current year doesn't match the last known year, will update the last known year
			/// to the current year and then call UpdateInternalView( ) to update the internal string representation of the
			/// year, otherwise, just returns the internal string representation of the year stored
			std::string_view Format( ) override;

		  private:
			Message_Time & timeRef;
			const std::tm &cacheRef;
			int            lastYear;
			std::string    year;
		};

		/// @brief Formatter struct that maps to flag "%+" and represents the formatted message with platform-dependant end
		/// of line applied
		struct Format_Arg_Message : Formatter
		{
			///  @brief Constructor that uses the Message_Info pointer to store a reference to the logger's message
			Format_Arg_Message( Message_Info &info );
			///  @brief When called, returns the formatted message with the platform-dependant end of line already applied
			std::string_view Format( ) override;

		  private:
			std::string &message;
		};

		/// @brief Formatter struct that doesn't map to a specific flag and represents any char that that doesn't end up
		/// matching with any of the known internal flags
		struct Format_Arg_Char : Formatter
		{
			///  @brief Constructor that takes a string_view argument to store for later use in appending formatting
			///  arguments
			Format_Arg_Char( std::string_view ch );
			///  @brief Returns the stored representation of the value passed in during construction
			std::string_view Format( ) override;

		  private:
			std::string m_char;
		};

		Formatters    formatter;
		std::string   fmtPattern;
		Message_Info *msgInfo;
	};
}  // namespace serenity::expiremental::msg_details
