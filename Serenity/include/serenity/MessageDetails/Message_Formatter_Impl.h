#pragma once

template<typename... Args> constexpr void ArgContainer::EmplaceBackArgs(Args&&... args) {
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

template<typename... Args> void ArgContainer::CaptureArgs(Args&&... args) {
	Reset();
	EmplaceBackArgs(std::forward<Args>(args)...);
	size_t size { argContainer.size() };
	if( size != 0 ) {
			maxIndex = size - 1;
	}
}

template<typename... Args> void Message_Formatter::FormatMessage(MsgWithLoc& message, Args&&... args) {
	lazy_message.clear();
	argStorage.CaptureArgs(std::forward<Args>(args)...);
	if( argStorage.ContainsUnsupportedType() || argStorage.ContainsArgSpecs(message.msg) ) {
			localeRef == nullptr ? VFORMAT_TO(lazy_message, message.msg, std::forward<Args>(args)...)
					     : L_VFORMAT_TO(lazy_message, *localeRef, message.msg, std::forward<Args>(args)...);
	} else {
			lazy_message.append(message.msg);
			for( ;; ) {
					LazySubstitute(lazy_message, std::move(argStorage.GetArgValue()));
					argStorage.AdvanceToNextArg();
					if( argStorage.EndReached() ) {
							break;
					}
				}
		}
	auto lineEnd { LineEnding() };
	lazy_message.append(lineEnd.data(), lineEnd.size());
	msgInfo->SetMessage(lazy_message, message.source);
}