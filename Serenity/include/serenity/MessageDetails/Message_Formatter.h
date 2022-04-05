#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>    // to_chars
#include <format>
#include <string>
#include <variant>

// TODO: ###################################################################################################################
// Not the biggest deal breaker, but might want to look into how to make Format_Arg_Char more efficient on allocations.
// During the mem heap profiling, this function allocated the most bytes and more frequently than other functions,
// which is kind of surprising to me. The next function to look at was the Formatters::FormatUserPattern() function and
// its overloads. All-in-all though, it wasn't terrible. This lib runs faster than spdlog with, at times, less mem usage.
// File targets used roughly the same amount of memory (this libs targets had only ~5 less allocations but total mem
// allocated was about the same).
// TODO: ###################################################################################################################

namespace serenity::msg_details {

	struct LazyParseHelper
	{
		void ClearPartitions();
		void ClearBuffer();

		std::string partitionUpToArg;
		std::string remainder;
		std::string temp;
		size_t openBracketPos { 0 };
		size_t closeBracketPos { 0 };
		std::to_chars_result result                             = {};
		std::array<char, SERENITY_ARG_BUFFER_SIZE> resultBuffer = {};
	};

	template<class T, class U> struct is_supported;
	template<class T, class... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same<T, Ts>::value || ...)>
	{
	};

	struct ArgContainer
	{
	      public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int,
		                                          long long, unsigned long long, bool, char, float, double, long double, const void*>;

		const std::vector<LazilySupportedTypes>& ArgStorage() const;
		LazyParseHelper& ParseHelper();
		void Reset();
		void AdvanceToNextArg();
		bool ContainsArgSpecs(const std::string_view fmt);
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		std::string&& GetArgValue();
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto arg) {
				auto typeFound = is_supported<decltype(arg), LazilySupportedTypes> {};
				if constexpr( !typeFound.value ) {
						containsUnknownType = true;
				} else {
						if( containsUnknownType ) return;
						argContainer.emplace_back(std::move(arg));
					}
			}(args),
			...);
		}
		template<typename... Args> void CaptureArgs(Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
			size_t size { argContainer.size() };
			if( size != 0 ) {
					maxIndex = size - 1;
			}
		}

	      private:
		std::vector<LazilySupportedTypes> argContainer;
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		bool endReached { false };
		LazyParseHelper parseHelper;
		bool containsUnknownType { false };
	};

	class Message_Formatter
	{
	      public:
		explicit Message_Formatter(std::string_view pattern, Message_Info* details);
		~Message_Formatter()                              = default;
		Message_Formatter()                               = delete;
		Message_Formatter(const Message_Formatter&)       = delete;
		Message_Formatter& operator=(const Message_Info&) = delete;

		struct Formatter
		{
			virtual std::string_view FormatUserPattern() = 0;
			virtual std::string UpdateInternalView();
		};

		class Formatters
		{
		      public:
			Formatters(std::vector<std::unique_ptr<Formatter>>&& container);
			Formatters() = default;
			void Emplace_Back(std::unique_ptr<Formatter>&& formatter);
			std::string_view FormatUserPattern();
			void Clear();

		      private:
			std::string localBuffer;
			std::vector<std::unique_ptr<Formatter>> m_Formatter;
		};

		void FlagFormatter(size_t flag);
		void SetPattern(std::string_view pattern);
		Formatters& GetFormatters();
		void StoreFormat();
		const Message_Info* MessageDetails();
		void LazySubstitute(std::string& msg, std::string arg);
		template<typename... Args> void FmtMessage(std::string_view message, Args&&... args) {
			lazy_message.clear();
			argStorage.CaptureArgs(std::forward<Args>(args)...);
			if( argStorage.ContainsUnsupportedType() || argStorage.ContainsArgSpecs(message) ) {
					VFORMAT_TO(lazy_message, *localeRef, message, std::forward<Args>(args)...);
			} else {
					lazy_message.append(message.data(), message.size());
					for( ;; ) {
							LazySubstitute(lazy_message, std::move(argStorage.GetArgValue()));
							argStorage.AdvanceToNextArg();
							if( argStorage.EndReached() ) {
									break;
							}
						}
				}
			auto lineEnd { SERENITY_LUTS::line_ending.at(platformEOL) };
			msgInfo->SetMessage(lazy_message.append(lineEnd.data(), lineEnd.size()));
		}

	      private:
		// Formatting Structs For Flag Arguments
		struct Format_Arg_a: Formatter
		{
			Format_Arg_a(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastHour;
			std::string hour;
		};

		struct Format_Arg_b: Formatter
		{
			Format_Arg_b(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastMonth;
			std::string month;
		};

		struct Format_Arg_d: Formatter
		{
			Format_Arg_d(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastDay;
			std::string day;
		};

		struct Format_Arg_l: Formatter
		{
			Format_Arg_l(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			LoggerLevel& levelRef;
			LoggerLevel lastLevel;
			std::string levelStr;
		};

		struct Format_Arg_n: Formatter
		{
			Format_Arg_n(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			Message_Time& timeRef;
			const std::tm& cacheRef;
			int lastDay;
			std::string ddmmyy;
		};

		struct Format_Arg_t: Formatter
		{
			Format_Arg_t(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastMin;
			std::string hmStr;
		};

		struct Format_Arg_w: Formatter
		{
			Format_Arg_w(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastDay { 0 };
			std::string lastDecDay;
		};

		struct Format_Arg_x: Formatter
		{
			Format_Arg_x(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			std::string wkday;
			int lastWkday { 0 };
		};

		struct Format_Arg_y: Formatter
		{
			Format_Arg_y(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			Message_Time& timeRef;
			int lastYear;
			std::string year;
		};

		struct Format_Arg_A: Formatter
		{
			Format_Arg_A(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastHour;
			std::string dayHalf;
		};

		struct Format_Arg_B: Formatter
		{
			Format_Arg_B(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastMonth;
			std::string month;
		};

		struct Format_Arg_D: Formatter
		{
			Format_Arg_D(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			Message_Time& timeRef;
			const std::tm& cacheRef;
			int lastDay;
			std::string mmddyy;
		};

		struct Format_Arg_F: Formatter
		{
			Format_Arg_F(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			Message_Time& timeRef;
			const std::tm& cacheRef;
			int lastDay;
			std::string yymmdd;
		};

		struct Format_Arg_H: Formatter
		{
			Format_Arg_H(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastHour;
			std::string hour;
		};

		struct Format_Arg_L: Formatter
		{
			Format_Arg_L(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			LoggerLevel& levelRef;
			LoggerLevel lastLevel;
			std::string levelStr;
		};

		struct Format_Arg_M: Formatter
		{
			Format_Arg_M(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastMin;
			std::string min;
		};

		struct Format_Arg_N: Formatter
		{
			Format_Arg_N(Message_Info& info);
			std::string_view FormatUserPattern() override;

		      private:
			std::string& name;
		};

		struct Format_Arg_S: Formatter
		{
			Format_Arg_S(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastSec;
			std::string sec;
		};

		struct Format_Arg_T: Formatter
		{
			Format_Arg_T(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastMin;
			std::string hmStr;
		};

		struct Format_Arg_X: Formatter
		{
			Format_Arg_X(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			const std::tm& cacheRef;
			int lastWkday;
			std::string wkday;
		};

		struct Format_Arg_Y: Formatter
		{
			Format_Arg_Y(Message_Info& info);
			std::string UpdateInternalView() override;
			std::string_view FormatUserPattern() override;

		      private:
			Message_Time& timeRef;
			const std::tm& cacheRef;
			int lastYear;
			std::string year;
		};

		struct Format_Arg_Message: Formatter
		{
			Format_Arg_Message(Message_Info& info);
			std::string_view FormatUserPattern() override;

		      private:
			std::string& message;
		};

		struct Format_Arg_Char: Formatter
		{
			Format_Arg_Char(std::string_view ch);
			std::string_view FormatUserPattern() override;

		      private:
			std::string m_char;
		};

	      private:
		Formatters formatter;
		std::string fmtPattern;
		Message_Info* msgInfo;
		std::string lazy_message;
		LineEnd platformEOL;
		ArgContainer argStorage;
		std::locale* localeRef;
	};

}    // namespace serenity::msg_details
