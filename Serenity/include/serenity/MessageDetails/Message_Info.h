#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Time.h>

#include <string>
#include <variant>
namespace serenity {
	static constexpr std::array<std::string_view, 2> delimiters = { "{}", "{ }" };
}

// Answer From Barry on https://stackoverflow.com/questions/45892170/how-do-i-check-if-an-stdvariant-can-hold-a-certain-type
// This definitely works, but I still can't seem to stop the compiler trying to store an unsupported type....
template<class T, class U> struct variant_contains;
template<class T, class... Ts> struct variant_contains<T, std::variant<Ts...>>: std::bool_constant<(std::is_same_v<T, Ts> || ...)>
{
};

struct ArgContainer
{
	using LazilySupportedTypes             = std::variant<std::string, const char*, std::string_view /*, int, double, float*/>;
	template<class T> using type_supported = variant_contains<T, LazilySupportedTypes>;

	template<typename... Args> constexpr void AllArgsSupportedImpl(bool& isSupported, Args&&... args) {
		(
		[ & ](auto& arg, auto& supported) {
			if( !type_supported<decltype(arg)>::value ) {
					isSupported = false;
			}
		}(args, isSupported),
		...);
	}

	template<typename... Args> constexpr bool AllArgsSupported(Args&&... args) {
		bool allSupported { true };
		AllArgsSupportedImpl(allSupported, std::forward<Args>(args)...);
		containsAnUnsupprtedArg = (!allSupported);
		return allSupported;
	}

	template<typename... Args> void EmplaceBackArgs(Args&&... args) {
		(
		// TODO: Figure a way to safely avoid trying to store an unsupported type and set the bool state
		[ & ](auto& arg) {
			// if supported type ->
			argContainer.emplace_back(arg);
			// else ->
			// containsAnUnsupprtedArg = true;
		}(args),
		...);
	}

	void Reset() {
		argContainer.clear();
		counterPos              = 0;
		endReached              = false;
		containsAnUnsupprtedArg = false;
	}

	template<typename... Args> void CaptureArgs(Args&&... args) {
		Reset();
		EmplaceBackArgs(std::forward<Args>(args)...);
		originalSize = argContainer.size();
	}

	void AdvanceToNextArg() {
		if( counterPos < originalSize ) {
				++counterPos;
		} else {
				endReached = true;
			}
	}

	bool ParseForArgSpecs(const std::string_view fmt) {
		auto size { fmt.size() };
		std::string_view argBracket;

		for( size_t i { 0 }; i < size; ++i ) {
				argBracket = "";
				if( fmt.at(i) == '{' ) {
						auto openBracketPos { fmt.find_first_of('{') };
						auto endBracketPos { fmt.find_first_of('}') };
						if( (openBracketPos != std::string_view::npos) && (endBracketPos != std::string_view::npos) ) {
								argBracket = std::move(fmt.substr(openBracketPos, endBracketPos + 1));
						}
						auto argBracketSize { argBracket.size() };
						switch( argBracketSize ) {
								case 0:
								case 1:
								case 2: break;
								case 3:
									if( argBracket.at(1) != ' ' ) {
											return true;
									}
									break;
								default:
									argBracket.remove_prefix(1);
									argBracket.remove_suffix(1);
									for( auto& ch: argBracket ) {
											if( ch != ' ' ) {
													return true;
											}
										}
									break;
							}
						if( argBracketSize == size ) break;
				}
			}
		return false;
	}

	bool EndReached() {
		return endReached;
	}
	void PopArgFromFront() {
		auto begin { argContainer.begin() };
		argContainer.erase(begin, begin + 1);
	}

	std::tuple<bool, std::string, size_t> GetArgInfo() {
		auto& arg { argContainer[ 0 ] };
		auto argIndex(counterPos);
		std::string tmp;
		switch( arg.index() ) {
				case 0:
					++counterPos;
					return std::make_tuple(true, std::get<0>(arg), argIndex);
					break;
				case 1:
					++counterPos;
					return std::make_tuple(true, std::get<1>(arg), argIndex);
					break;
				case 2:
					++counterPos;
					VFORMAT_TO(tmp, std::locale {}, "{}", std::get<2>(arg));
					return std::make_tuple(true, tmp, argIndex);
					break;
				// case 3:
				//	++counterPos;
				//	VFORMAT_TO(tmp, std::locale {}, "{}", std::get<3>(arg));
				//	return std::make_tuple(true, tmp, argIndex);
				//	break;
				// case 4:
				//	++counterPos;
				//	VFORMAT_TO(tmp, std::locale {}, "{}", std::get<4>(arg));
				//	return std::make_tuple(true, tmp, argIndex);
				//	break;
				// case 5:
				//	++counterPos;
				//	VFORMAT_TO(tmp, std::locale {}, "{}", std::get<5>(arg));
				//	return std::make_tuple(true, tmp, argIndex);
				//	break;
				default: return std::make_tuple(false, "", 50); break;
			}
	}

	std::vector<LazilySupportedTypes> argContainer;
	size_t originalSize { 0 };
	static size_t counterPos;
	bool endReached { false };
	bool containsAnUnsupprtedArg { false };
};

namespace serenity::msg_details {
	class Message_Info
	{
	      public:
		Message_Info(std::string_view name, LoggerLevel level, message_time_mode mode);
		Message_Info& operator=(const Message_Info& t) = delete;
		Message_Info(const Message_Info&)              = delete;
		Message_Info()                                 = delete;
		~Message_Info()                                = default;

		LoggerLevel& MsgLevel();
		std::string& Name();
		Message_Time& TimeDetails();
		void SetName(const std::string_view name);
		void SetMsgLevel(const LoggerLevel level);
		std::chrono::system_clock::time_point MessageTimePoint();
		void SetTimeMode(const message_time_mode mode);
		message_time_mode TimeMode();
		std::tm& TimeInfo();
		std::string& Message();
		const size_t MessageSize();
		void SetLocale(const std::locale& loc);
		std::locale GetLocale() const;

		void LazySubstitute(std::string& msg, std::string_view arg, size_t index) {
			std::string temp { msg.data(), msg.size() };
			m_parseString = msg;

			auto closeBraketPos { temp.find_first_of("}") };
			if( closeBraketPos != std::string::npos ) {
					temp.erase(0, closeBraketPos + 1);
			}

			auto tempSize { temp.size() };
			msg.clear();
			if( (tempSize == 0) || (testContainer.argContainer.size() <= 1) ) {
					msg.append(arg.data(), arg.size());
			} else {
					auto argOffset { m_parseString.end() - tempSize };
					msg.append(std::move(m_parseString.replace(m_parseString.begin(), argOffset, arg)));
				}
			testContainer.PopArgFromFront();
		}

		template<typename... Args> void SetMessage(std::string_view message, Args&&... args) {
			// m_message.clear();
			// VFORMAT_TO(m_message, m_locale, message, std::forward<Args>(args)...);
			// m_message.append(SERENITY_LUTS::line_ending.at(platformEOL));
			m_message.clear();
			lazy_message.clear();

			if( !testContainer.AllArgsSupported() || testContainer.ParseForArgSpecs(message) ) {
					VFORMAT_TO(m_message, m_locale, message, std::forward<Args>(args)...);
			} else {
					testContainer.CaptureArgs(std::forward<Args>(args)...);
					auto& argContainer { testContainer.argContainer };
					std::string temp { message };

					for( const auto& arg: argContainer ) {
							auto [ isStringType, argValue, argIndex ] = testContainer.GetArgInfo();
							if( isStringType ) {
									LazySubstitute(temp, argValue, argIndex);
									// assume that if '{}' is found for one argument, that it was intended,
									// otherwise, remove it before continuing here
									if( testContainer.originalSize > 1 ) {
											lazy_message.append(TidyUpLazyString(temp));
									} else {
											lazy_message.append(temp);
										}
							}
							testContainer.AdvanceToNextArg();
							if( testContainer.EndReached() ) {
									break;
							}
						}
					m_message.append(std::move(lazy_message));
				}
			m_message.append(SERENITY_LUTS::line_ending.at(platformEOL));
		}

		std::string TidyUpLazyString(const std::string& string) {
			std::string temp { string };
			auto pos = temp.find(delimiters[ 0 ]);
			if( pos != std::string::npos ) {
					temp.replace(pos, delimiters[ 0 ].size(), "");
			}
			pos = temp.find(delimiters[ 1 ]);
			if( pos != std::string::npos ) {
					temp.replace(pos, delimiters[ 1 ].size(), "");
			}
			return temp;
		}

	      private:
		LineEnd platformEOL;
		std::string m_name;
		LoggerLevel m_msgLevel;
		std::string lazy_message;
		std::string m_message;
		std::string m_parseString;
		Message_Time m_msgTime;
		std::locale m_locale;
		ArgContainer testContainer;
	};
}    // namespace serenity::msg_details
