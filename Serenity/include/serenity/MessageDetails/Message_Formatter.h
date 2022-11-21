#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>
#include <format>
#include <string>
#include <variant>

namespace serenity::msg_details {

	/*****************************************************************  vvv WORK IN PROGRESS vvv *****************************************************************/
	// Taking a hint from what I did with source_flag to avoid the checks later on in the formatting call stage and just jump straight to the formatting call
	enum class SeFmtFuncFlags
	{
		Invalid                        = 0,
		Base                           = 1,
		Src                            = 2,
		Thread                         = 4,
		Time                           = 8,
		Src_Base                       = 3,
		Src_Thread_Base                = 7,
		Thread_Base                    = 5,
		Time_Base                      = 9,
		Time_Src_Base                  = 11,
		Time_Thread_Base               = 13,
		Time_Src_Thread_Base           = 15,
		Localize_Time                  = 16,
		Localized_Time_Base            = 25,
		Localized_Time_Src_Base        = 27,
		Localized_Time_Thread_Base     = 29,
		Localized_Time_Src_Thread_Base = 31,
	};

	constexpr SeFmtFuncFlags operator|(SeFmtFuncFlags lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(static_cast<std::underlying_type<SeFmtFuncFlags>::type>(lhs) | static_cast<std::underlying_type<SeFmtFuncFlags>::type>(rhs));
	}

	constexpr SeFmtFuncFlags operator-(SeFmtFuncFlags lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(static_cast<std::underlying_type<SeFmtFuncFlags>::type>(lhs) - static_cast<std::underlying_type<SeFmtFuncFlags>::type>(rhs));
	}
	constexpr SeFmtFuncFlags operator&(SeFmtFuncFlags lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(static_cast<std::underlying_type<SeFmtFuncFlags>::type>(lhs) & static_cast<std::underlying_type<SeFmtFuncFlags>::type>(rhs));
	}
	constexpr SeFmtFuncFlags operator>(SeFmtFuncFlags lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(static_cast<std::underlying_type<SeFmtFuncFlags>::type>(lhs) > static_cast<std::underlying_type<SeFmtFuncFlags>::type>(rhs));
	}
	constexpr SeFmtFuncFlags operator>=(SeFmtFuncFlags& lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(lhs = lhs > rhs);
	}
	constexpr SeFmtFuncFlags operator-=(SeFmtFuncFlags& lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(lhs = lhs - rhs);
	}
	constexpr SeFmtFuncFlags operator|=(SeFmtFuncFlags& lhs, SeFmtFuncFlags rhs) {
		return static_cast<SeFmtFuncFlags>(lhs = lhs | rhs);
	}
	/***************************************************************** ^^^ WORK IN PROGRESS ^^^ *****************************************************************/

	class Message_Formatter
	{
	  public:
		explicit Message_Formatter(std::string_view pattern, Message_Info* details);
		~Message_Formatter()                              = default;
		Message_Formatter()                               = delete;
		Message_Formatter(const Message_Formatter&)       = delete;
		Message_Formatter& operator=(const Message_Info&) = delete;

		void SetPattern(std::string_view pattern);
		const Message_Info* MessageDetails();
		void SetLocaleReference(const std::locale& loc);
		const std::locale& Locale() const;
		std::string_view LineEnding() const;
		const std::string& Pattern() const;
		void ModifyInternalFormatStringIfNeeded();
		SeFmtFuncFlags FmtFunctionFlag() const;
		source_flag SourceFmtFlag() const;
		int ThreadFmtLength() const;

	  private:
		Message_Info* msgInfo;
		std::locale& localeRef;
		std::string fmtPattern;
		LineEnd platformEOL;
		std::string temp;
		source_flag sourceFlag;
		SeFmtFuncFlags fmtFlags;
		int threadLength;
	};
}    // namespace serenity::msg_details
