#pragma once

template<typename... Args> void serenity::msg_details::Message_Formatter::FormatMessageArgs(std::string_view message, Args&&... args) {
	auto& msg { msgInfo->Message() };
	msg.clear();
	VFORMAT_TO(msg, localeRef, message, std::forward<Args>(args)...);
	msg.append(LineEnding());
}