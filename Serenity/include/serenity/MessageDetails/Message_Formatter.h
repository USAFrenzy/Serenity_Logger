#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>
#include <format>
#include <string>
#include <variant>

namespace serenity::msg_details {

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
		const std::locale& Locale();
		std::string_view LineEnding() const;
		const std::string& Pattern();

	  private:
		Message_Info* msgInfo;
		std::locale& localeRef;
		std::string fmtPattern;
		LineEnd platformEOL;
		std::string temp;
		source_flag sourceFlag;
	};
}    // namespace serenity::msg_details
