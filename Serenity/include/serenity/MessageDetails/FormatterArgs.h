#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <string_view>
#include <string>
#include <variant>

namespace serenity::msg_details {
	/*********** Most All strftime Flags Have Been Added, Except For The Following ************/ /*
	  * %g (week based year last 2 digits)
	  * %h (short month abbrev on locale) //------------------------------ Using %b formatter
	  * %j (Day of year (001-366)
	  * %u (weekday as a number with Monday as 1 (1-7))
	  * %x (Date Representation locale dependant) //---------------------- Using %D formatter
	  * %U (week number with Sunday being the first day (00-53))
	  * %V (ISO 8601 week number (01-53)
	  * %W (week number as first Monday being the first daty (00-53))
	  * %X (Time Representation locale dependant) //---------------------- Using %T formatter
	  * %% (literal '%' sign)
	  * %G (week-based year)
	 //*******************************************************************************************/

	class Formatter
	{
	      public:
		Formatter()                                  = default;
		~Formatter()                                 = default;

		virtual std::string_view FormatUserPattern() = 0;
		virtual std::string& UpdateInternalView();
		std::string result;
	};

	// Formatting Structs For Flag Arguments

	class Format_Arg_a: public Formatter
	{
	      public:
		explicit Format_Arg_a(Message_Info& info);
		Format_Arg_a(const Format_Arg_a&)            = delete;
		Format_Arg_a& operator=(const Format_Arg_a&) = delete;
		~Format_Arg_a()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastWkday { 0 };
	};

	class Format_Arg_b: public Formatter
	{
	      public:
		explicit Format_Arg_b(Message_Info& info);
		Format_Arg_b(const Format_Arg_b&)            = delete;
		Format_Arg_b& operator=(const Format_Arg_b&) = delete;
		~Format_Arg_b()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastMonth;
	};

	class Format_Arg_c: public Formatter
	{
	      public:
		explicit Format_Arg_c(Message_Info& info);
		Format_Arg_c(const Format_Arg_c&)            = delete;
		Format_Arg_c& operator=(const Format_Arg_c&) = delete;
		~Format_Arg_c()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
	};

	class Format_Arg_d: public Formatter
	{
	      public:
		explicit Format_Arg_d(Message_Info& info);
		Format_Arg_d(const Format_Arg_d&)            = delete;
		Format_Arg_d& operator=(const Format_Arg_d&) = delete;
		~Format_Arg_d()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastDay;
	};

	class Format_Arg_e: public Formatter
	{
	      public:
		explicit Format_Arg_e(size_t precision);
		Format_Arg_e(const Format_Arg_e&)            = delete;
		Format_Arg_e& operator=(const Format_Arg_e&) = delete;
		~Format_Arg_e()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		std::array<char, defaultBufferSize> buffer;
		size_t m_precision;
	};

	class Format_Arg_l: public Formatter
	{
	      public:
		explicit Format_Arg_l(Message_Info& info);
		Format_Arg_l(const Format_Arg_l&)            = delete;
		Format_Arg_l& operator=(const Format_Arg_l&) = delete;
		~Format_Arg_l()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		LoggerLevel& levelRef;
		LoggerLevel lastLevel;
	};

	class Format_Arg_m: public Formatter
	{
	      public:
		explicit Format_Arg_m(Message_Info& info);
		Format_Arg_m(const Format_Arg_m&)            = delete;
		Format_Arg_m& operator=(const Format_Arg_m&) = delete;
		~Format_Arg_m()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastMonth;
	};

	class Format_Arg_n: public Formatter
	{
	      public:
		explicit Format_Arg_n(Message_Info& info);
		Format_Arg_n(const Format_Arg_n&)            = delete;
		Format_Arg_n& operator=(const Format_Arg_n&) = delete;
		~Format_Arg_n()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
		const std::tm& cacheRef;
		int lastDay;
	};

	class Format_Arg_p: public Formatter
	{
	      public:
		explicit Format_Arg_p(Message_Info& info);
		Format_Arg_p(const Format_Arg_p&)            = delete;
		Format_Arg_p& operator=(const Format_Arg_p&) = delete;
		~Format_Arg_p()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastHour;
	};

	class Format_Arg_r: public Formatter
	{
	      public:
		explicit Format_Arg_r(Message_Info& info);
		Format_Arg_r(const Format_Arg_r&)            = delete;
		Format_Arg_r& operator=(const Format_Arg_r&) = delete;
		~Format_Arg_r()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastMin;
		std::string_view min;
		std::string hour;
	};

	// TODO: Add bit field for combining specs
	class Format_Arg_s: public Formatter
	{
	      public:
		explicit Format_Arg_s(Message_Info& info, source_flag flag);
		Format_Arg_s(const Format_Arg_s&)            = delete;
		Format_Arg_s& operator=(const Format_Arg_s&) = delete;
		~Format_Arg_s()                              = default;

		size_t FindEndPos();
		void FormatAll();
		std::string FormatLine();
		std::string FormatColumn();
		std::string FormatFile();
		std::string FormatFunction();
		std::string_view FormatUserPattern() override;

	      private:
		const std::source_location& srcLocation;
		std::array<char, 6> buff;
		std::filesystem::path file;
		source_flag spec;
	};

	class Format_Arg_t: public Formatter
	{
	      public:
		Format_Arg_t(size_t precision);
		Format_Arg_t(const Format_Arg_t&)            = delete;
		Format_Arg_t& operator=(const Format_Arg_t&) = delete;
		~Format_Arg_t()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		size_t thread;
	};

	class Format_Arg_w: public Formatter
	{
	      public:
		explicit Format_Arg_w(Message_Info& info);
		Format_Arg_w(const Format_Arg_w&)            = delete;
		Format_Arg_w& operator=(const Format_Arg_w&) = delete;
		~Format_Arg_w()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastDay { 0 };
	};

	class Format_Arg_y: public Formatter
	{
	      public:
		explicit Format_Arg_y(Message_Info& info);
		Format_Arg_y(const Format_Arg_y&)            = delete;
		Format_Arg_y& operator=(const Format_Arg_y&) = delete;
		~Format_Arg_y()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		Message_Time& timeRef;
		int lastYear;
	};

	class Format_Arg_z: public Formatter
	{
	      public:
		explicit Format_Arg_z(Message_Info& info);
		Format_Arg_z(const Format_Arg_z&)            = delete;
		Format_Arg_z& operator=(const Format_Arg_z&) = delete;
		~Format_Arg_z()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		Message_Info& infoRef;
		std::tm local, gm;
		int lastMin;
	};

	class Format_Arg_A: public Formatter
	{
	      public:
		explicit Format_Arg_A(Message_Info& info);
		Format_Arg_A(const Format_Arg_A&)            = delete;
		Format_Arg_A& operator=(const Format_Arg_A&) = delete;
		~Format_Arg_A()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastWkday;
	};

	class Format_Arg_B: public Formatter
	{
	      public:
		explicit Format_Arg_B(Message_Info& info);
		Format_Arg_B(const Format_Arg_B&)            = delete;
		Format_Arg_B& operator=(const Format_Arg_B&) = delete;
		~Format_Arg_B()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastMonth;
	};

	class Format_Arg_C: public Formatter
	{
	      public:
		explicit Format_Arg_C(Message_Info& info);
		Format_Arg_C(const Format_Arg_B&)            = delete;
		Format_Arg_C& operator=(const Format_Arg_B&) = delete;
		~Format_Arg_C()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastYear;
	};

	class Format_Arg_D: public Formatter
	{
	      public:
		explicit Format_Arg_D(Message_Info& info);
		Format_Arg_D(const Format_Arg_D&)            = delete;
		Format_Arg_D& operator=(const Format_Arg_D&) = delete;
		~Format_Arg_D()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
		const std::tm& cacheRef;
		int lastDay;
	};

	class Format_Arg_F: public Formatter
	{
	      public:
		explicit Format_Arg_F(Message_Info& info);
		Format_Arg_F(const Format_Arg_F&)            = delete;
		Format_Arg_F& operator=(const Format_Arg_F&) = delete;
		~Format_Arg_F()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
		const std::tm& cacheRef;
		int lastDay;
	};

	class Format_Arg_H: public Formatter
	{
	      public:
		explicit Format_Arg_H(Message_Info& info);
		Format_Arg_H(const Format_Arg_H&)            = delete;
		Format_Arg_H& operator=(const Format_Arg_H&) = delete;
		~Format_Arg_H()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastHour;
	};

	class Format_Arg_I: public Formatter
	{
	      public:
		explicit Format_Arg_I(Message_Info& info);
		Format_Arg_I(const Format_Arg_I&)            = delete;
		Format_Arg_I& operator=(const Format_Arg_I&) = delete;
		~Format_Arg_I()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastHour;
	};

	class Format_Arg_L: public Formatter
	{
	      public:
		explicit Format_Arg_L(Message_Info& info);
		Format_Arg_L(const Format_Arg_L&)            = delete;
		Format_Arg_L& operator=(const Format_Arg_L&) = delete;
		~Format_Arg_L()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		LoggerLevel& levelRef;
		LoggerLevel lastLevel;
	};

	class Format_Arg_M: public Formatter
	{
	      public:
		explicit Format_Arg_M(Message_Info& info);
		Format_Arg_M(const Format_Arg_M&)            = delete;
		Format_Arg_M& operator=(const Format_Arg_M&) = delete;
		~Format_Arg_M()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastMin;
	};

	class Format_Arg_N: public Formatter
	{
	      public:
		explicit Format_Arg_N(Message_Info& info);
		Format_Arg_N(const Format_Arg_N&)            = delete;
		Format_Arg_N& operator=(const Format_Arg_N&) = delete;
		~Format_Arg_N()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		std::string& name;
	};

	class Format_Arg_R: public Formatter
	{
	      public:
		explicit Format_Arg_R(Message_Info& info);
		Format_Arg_R(const Format_Arg_R&)            = delete;
		Format_Arg_R& operator=(const Format_Arg_R&) = delete;
		~Format_Arg_R()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
	};

	class Format_Arg_S: public Formatter
	{
	      public:
		explicit Format_Arg_S(Message_Info& info);
		Format_Arg_S(const Format_Arg_S&)            = delete;
		Format_Arg_S& operator=(const Format_Arg_S&) = delete;
		~Format_Arg_S()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
		int lastSec;
	};

	class Format_Arg_T: public Formatter
	{
	      public:
		explicit Format_Arg_T(Message_Info& info);
		Format_Arg_T(const Format_Arg_T&)            = delete;
		Format_Arg_T& operator=(const Format_Arg_T&) = delete;
		~Format_Arg_T()                              = default;

		std::string_view FormatUserPattern() override;

	      private:
		const std::tm& cacheRef;
	};

	class Format_Arg_Y: public Formatter
	{
	      public:
		explicit Format_Arg_Y(Message_Info& info);
		Format_Arg_Y(const Format_Arg_Y&)            = delete;
		Format_Arg_Y& operator=(const Format_Arg_Y&) = delete;
		~Format_Arg_Y()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
		const std::tm& cacheRef;
		int lastYear;
	};

	class Format_Arg_Z: public Formatter
	{
	      public:
		explicit Format_Arg_Z(Message_Info& info);
		Format_Arg_Z(const Format_Arg_Z&)            = delete;
		Format_Arg_Z& operator=(const Format_Arg_Z&) = delete;
		~Format_Arg_Z()                              = default;

		std::string& UpdateInternalView() override;
		std::string_view FormatUserPattern() override;

	      private:
		Message_Time& timeRef;
		message_time_mode& timeModeRef;
		message_time_mode cachedMode;
	};

	class Format_Arg_Message: public Formatter
	{
	      public:
		explicit Format_Arg_Message(Message_Info& info);
		Format_Arg_Message(const Format_Arg_Message&)            = delete;
		Format_Arg_Message& operator=(const Format_Arg_Message&) = delete;
		~Format_Arg_Message()                                    = default;

		std::string_view FormatUserPattern() override;

	      private:
		std::string& message;
	};

	class Format_Arg_Char: public Formatter
	{
	      public:
		explicit Format_Arg_Char(std::string_view ch);
		Format_Arg_Char(const Format_Arg_Char&)            = delete;
		Format_Arg_Char& operator=(const Format_Arg_Char&) = delete;
		~Format_Arg_Char()                                 = default;

		std::string_view FormatUserPattern() override;
	};

}    // namespace serenity::msg_details