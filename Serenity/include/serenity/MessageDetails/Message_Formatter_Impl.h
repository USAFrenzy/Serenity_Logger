#pragma once

template<typename... Args> void serenity::msg_details::Message_Formatter::FormatMessageArgs(MsgWithLoc& message, Args&&... args) {
	msgInfo->SourceLocation() = message.source;
	auto& msg { msgInfo->Message() };
	msg.clear();
	VFORMAT_TO(msg, localeRef, message.msg, std::forward<Args>(args)...);
	msg.append(LineEnding());
}