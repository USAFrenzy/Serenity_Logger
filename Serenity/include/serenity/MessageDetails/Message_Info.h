#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Time.h>

#include <any>
#include <charconv>    // to_chars
#include <string>
#include <variant>

#define SERENITY_ARG_BUFFER_SIZE 24

struct LazyParseHelper
{
	void ClearPartitions() {
		partitionUpToArg.clear();
		remainder.clear();
	}

	void ClearBuffer() {
		resultBuffer.fill(0);
	}

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
	using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
	                                          unsigned long long, bool, char, float, double, long double, const void*>;

	const std::vector<LazilySupportedTypes>& ArgStorage() const {
		return argContainer;
	}

	LazyParseHelper& ParseHelper() {
		return parseHelper;
	}

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

	void Reset() {
		argContainer.clear();
		argIndex = maxIndex = 0;
		endReached          = false;
	}

	template<typename... Args> void CaptureArgs(Args&&... args) {
		Reset();
		EmplaceBackArgs(std::forward<Args>(args)...);
		size_t size { argContainer.size() };
		if( size != 0 ) {
				maxIndex = size - 1;
		}
	}

	void AdvanceToNextArg() {
		if( argIndex < maxIndex ) {
				++argIndex;
		} else {
				endReached = true;
			}
	}

	bool ContainsArgSpecs(const std::string_view fmt) {
		auto size { fmt.size() };
		std::string_view argBracket;

		for( size_t i { 0 }; i < size; ++i ) {
				argBracket = "";
				if( fmt.at(i) == '{' ) {
						parseHelper.openBracketPos  = fmt.find_first_of('{');
						parseHelper.closeBracketPos = fmt.find_first_of('}');
						if( (parseHelper.openBracketPos != std::string_view::npos) &&
						    (parseHelper.closeBracketPos != std::string_view::npos) ) {
								argBracket = std::move(
								fmt.substr(parseHelper.openBracketPos, parseHelper.closeBracketPos + 1));
						}
						auto argBracketSize { argBracket.size() };
						switch( argBracketSize ) {
								case 0: break;
								case 1: break;
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

	bool EndReached() const {
		return endReached;
	}

	bool ContainsUnsupportedType() const {
		return containsUnknownType;
	}
	/*************************************** Variant Order *******************************************
	 * [0] std::monostate, [1] std::string, [2] const char*, [3] std::string_view, [4] int,
	 * [5] unsigned int, [6] long long, [7] unsigned long long, [8] bool, [9] char, [10] float,
	 * [11] double, [12] long double, [13] const void*
	 ************************************************************************************************/
	/*************************************************************************************************/
	// TODO: As far as any more optimizations go, this function eats up ~3x more cpu cycles compared
	// TODO: to the very next cpu hungry function given a test of 3 empty specifier arguments (an int,
	// TODO: a float, and a string). Given the timings, this may not be any real issue, but it'd be
	// TODO: cool to see how I may be able to speed this up as well since any gains here are massive
	// TODO: gains everywhere else.
	/*************************************************************************************************/

	std::string&& GetArgValue() {
		auto& strRef { parseHelper.temp };
		strRef.clear();
		parseHelper.ClearBuffer();
		auto& arg { argContainer[ argIndex ] };
		auto& buffer { parseHelper.resultBuffer };
		auto& result { parseHelper.result };

		switch( arg.index() ) {
				case 0: return std::move(parseHelper.temp); break;
				case 1: return std::move(parseHelper.temp.append(std::move(std::get<1>(arg)))); break;
				case 2: return std::move(parseHelper.temp.append(std::move(std::get<2>(arg)))); break;
				case 3: return std::move(parseHelper.temp.append(std::move(std::get<3>(arg)))); break;
				case 4:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<4>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 5:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<5>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 6:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<6>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 7:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<7>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 8:
					strRef.append(std::get<8>(arg) == true ? "true" : "false");
					return std::move(strRef);
					break;
				case 9:
					strRef += std::move(std::get<9>(arg));
					return std::move(strRef);
					break;
				case 10:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<10>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 11:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<11>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 12:
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<12>(arg));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 13:
					// Couldn't get this to work with dynamic_cast, but reinterpret_cast at least isn't giving any issues.
					// Still need to test that this works as intended; changed base 10 to 16 for 0-F addressing.
					result = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					                       reinterpret_cast<size_t>(std::get<13>(arg)), 16);
					if( result.ec != std::errc::value_too_large ) {
							strRef.append("0x").append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				default: return std::move(strRef); break;
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

		void LazySubstitute(std::string& msg, std::string arg) {
			std::string_view temp { msg };
			auto& parseHelper { testContainer.ParseHelper() };
			parseHelper.ClearPartitions();
			bool bracketPositionsValid;

			parseHelper.openBracketPos  = temp.find_first_of(static_cast<char>('{'));
			parseHelper.closeBracketPos = temp.find_first_of(static_cast<char>(' }'));
			bracketPositionsValid       = ((parseHelper.openBracketPos != std::string::npos) &&
                                                 (parseHelper.closeBracketPos != std::string::npos));
			if( bracketPositionsValid ) {
					parseHelper.partitionUpToArg.append(std::move(temp.substr(0, parseHelper.openBracketPos)));
					temp.remove_prefix(parseHelper.closeBracketPos + 1);
					parseHelper.remainder.append(temp.data(), temp.size());
			}
			msg.clear();
			msg.append(std::move(parseHelper.partitionUpToArg)).append(arg.data(), arg.size()).append(std::move(parseHelper.remainder));
		}

		template<typename... Args> void SetMessage(std::string_view message, Args&&... args) {
			m_message.clear();
			lazy_message.clear();
			testContainer.CaptureArgs(std::forward<Args>(args)...);
			if( testContainer.ContainsUnsupportedType() || testContainer.ContainsArgSpecs(message) ) {
					VFORMAT_TO(m_message, m_locale, message, std::forward<Args>(args)...);
			} else {
					lazy_message.append(message.data(), message.size());
					for( ;; ) {
							LazySubstitute(lazy_message, std::move(testContainer.GetArgValue()));
							testContainer.AdvanceToNextArg();
							if( testContainer.EndReached() ) {
									break;
							}
						}
				}
			auto lineEnd { SERENITY_LUTS::line_ending.at(platformEOL) };
			m_message.append(lazy_message).append(lineEnd.data(), lineEnd.size());
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
