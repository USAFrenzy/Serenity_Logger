#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <string_view>
#include <string>
#include <variant>

namespace serenity::msg_details {
	/************************************************************************************************/
	//           I can not for the life of me get anything to be faster than this setup.
	/************************************************************************************************/
	// * Re-visiting std::variant option where each format arg is a type stored in the variant and
	//   accessed by a switch on the index to a function call and optimizing it still saw ~6% losses.
	// * Re-visiting non-functor styled function pointers where each format arg is a function call
	//   and optimizing that saw ~8% losses
	// * I haven't re-visited std::function yet, but I can't see that being much better than
	//   function pointers
	// * So for now, the winner is still this basic inheritance model and storing derived pointers
	/************************************************************************************************/

	// TODO:#############################################################################
	// TODO:     Args in process of being renamed to mirror strftime tokens
	// TODO:#############################################################################

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

	static constexpr size_t maxPrecision              = 6;
	static constexpr size_t defaultSubSecondPrecision = 3;
	static constexpr size_t defaultBufferSize         = 19;
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

	// Missing %g (week based year last 2 digits)

	// Missing %h (it's the same as %b though)

	// Missing %j (Day of year (001-366)

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

	static constexpr size_t defaultThreadIdLength = 10;
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

	// Missing %u (weekday as a number with Monday as 1 (1-7))

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

	// Missing %x (Date Representation); this is the same as %D but locale dependant

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

	// Missing %z (ISO 8601 offset from UTC (1 min 1, 1 hr = 100) ex: +100)

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

	// Missing %G (week-based year)

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

	// Missing %R (24 hour time as HH:MM)

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

	// Missing %U (week number with first Sunday as the first day of week one (00-53))

	// Missing %V (ISO 8601 week number (01-53)

	// Missing %W (week number with first Monday as the first day of week one (00-53))

	// Missing %X (Time Representation) same as %T but locale dependant

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

	// Missing %Z (Timezone name or abbreviation)

	// Missing %% (literal '%' sign)

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