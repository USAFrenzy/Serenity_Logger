#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Time.h>

#include <string>
#include <variant>

struct LazyParseHelper
{
	std::string partitionUpToArg;
	std::string remainder;
	std::string temp;
	size_t openBracketPos { 0 };
	size_t closeBracketPos { 0 };
	void ClearAll() {
		partitionUpToArg.clear();
		remainder.clear();
		temp.clear();
	}
};

struct ArgContainer
{
      public:
	using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
	                                          unsigned long long, bool, char, float, double, long double, const void*>;

	const std::vector<LazilySupportedTypes>& ArgStorage() const {
		return argContainer;
	}

	template<typename... Args> void EmplaceBackArgs(Args&&... args) {
		(
		[ & ](auto&& arg) {
			argContainer.emplace_back(std::move(arg));
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
		maxIndex = (argContainer.size() - 1);
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

	/*************************************** Variant Order *******************************************
	 * std::monostate,std::string, const char*, std::string_view, int, unsigned int, long long,
	 * unsigned long long, bool, char, float, double, long double, const void*
	 ************************************************************************************************/
	/*************************************************************************************************/
	// TODO: As far as any more optimizations go, this function eats up ~3x more cpu cycles compared
	// TODO: to the very next cpu hungry function given a test of 3 empty specifier arguments (an int,
	// TODO: a float, and a string). Given the timings, this may not be any real issue, but it'd be
	// TODO: cool to see how I may be able to speed this up as well since any gains here are massive
	// TODO: gains everywhere else.
	/*************************************************************************************************/

	std::string&& GetArgValue() {
		auto& arg { argContainer[ argIndex ] };
		parseHelper.temp.clear();
		switch( arg.index() ) {
				case 0: return std::move(parseHelper.temp); break;
				case 1: return std::move(parseHelper.temp.append(std::move(std::get<1>(arg)))); break;
				case 2: return std::move(parseHelper.temp.append(std::move(std::get<2>(arg)))); break;
				case 3: return std::move(parseHelper.temp.append(std::move(std::get<3>(arg)))); break;
				case 4:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<4>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 5:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<5>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 6:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<6>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 7:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<7>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 8:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<8>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 9:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<9>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 10:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<10>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 11:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<11>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 12:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<12>(arg)));
					return std::move(parseHelper.temp);
					break;
				case 13:
					VFORMAT_TO(parseHelper.temp, std::locale {}, "{}", std::move(std::get<13>(arg)));
					return std::move(parseHelper.temp);
					break;
				default: return std::move(parseHelper.temp); break;
			}
	}

      private:
	std::vector<LazilySupportedTypes> argContainer;
	size_t maxIndex { 0 };
	size_t argIndex { 0 };
	bool endReached { false };
	LazyParseHelper parseHelper;
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

		void LazySubstitute(std::string& msg, std::string_view arg) {
			std::string_view temp { msg };
			parseHelper.ClearAll();
			bool bracketPositionsValid;
			parseHelper.openBracketPos  = temp.find_first_of("{");
			parseHelper.closeBracketPos = temp.find_first_of("}");
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
			if( testContainer.ContainsArgSpecs(message) ) {
					VFORMAT_TO(m_message, m_locale, message, std::forward<Args>(args)...);
			} else {
					testContainer.CaptureArgs(std::forward<Args>(args)...);
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
		LazyParseHelper parseHelper;
	};
}    // namespace serenity::msg_details
