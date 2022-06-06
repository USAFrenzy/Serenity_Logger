#pragma once

template<typename... Args> void serenity::msg_details::Message_Formatter::FormatMessageArgs(MsgWithLoc& message, Args&&... args) {
	lazy_message.clear();
	argFormatter.se_format_to(localeRef, std::back_inserter(lazy_message), message.msg, std::forward<Args>(args)...);
	auto lineEnd { LineEnding() };
	lazy_message.append(lineEnd.data(), lineEnd.size());
	msgInfo->SetMessage(lazy_message, message.source);
}