#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/Message_Time.h>

#include <string>
#include <variant>

struct ArgContainer
{
	using LazilySupportedTypes = std::variant<std::string, const char*, std::string_view>;
	template<typename... Args> void CaptureArgs(Args&&... args) {
		argContainer.clear();
		counterPos = 0;
		argContainer.emplace_back(std::forward<Args>(args)...);
		numberOfArgs = argContainer.size();
		originalSize = numberOfArgs;
	}
	void AdvanceToNextArg() {
		if( counterPos < numberOfArgs ) {
				++counterPos;
		} else {
				endReached = true;
			}
	}

	bool EndReached() {
		return endReached;
	}
	void RemoveArgFromContainer(size_t index) {
		auto argOffset { (argContainer.end() - 1) - index };
		argContainer.erase(argOffset);
		numberOfArgs = argContainer.size();
		endReached   = false;
		counterPos   = 0;
		indicesRemoved.push_back(index);
	}

	std::string FormatRemainingArgs(std::string_view msg, const std::locale loc, std::format_args&& args) {
		std::string result { msg.data(), msg.size() };
		std::string temp;
		for( size_t i { 0 }; i < originalSize; ++i ) {
				temp.clear();
				auto pos { std::find(indicesRemoved.begin(), indicesRemoved.end(), i) };
				if( pos == indicesRemoved.end() ) {
						try {
								temp.append(std::vformat(loc, result, args));
							}
						catch( const std::format_error& ferr ) {
								printf("%s\n", ferr.what());
							}
						result = temp;
				}
			}
		return result;
	}

	std::tuple<bool, std::string_view, size_t> GetArgInfo() {
		auto& arg { argContainer.at(counterPos) };
		auto argIndex(counterPos);
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
					return std::make_tuple(true, std::get<2>(arg), argIndex);
					break;
				default: return std::make_tuple(false, "", 50); break;
			}
	}

	std::vector<LazilySupportedTypes> argContainer;
	size_t numberOfArgs;
	size_t originalSize;
	static size_t counterPos;
	bool endReached { false };
	std::vector<size_t> indicesRemoved;
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

		std::string LazySubstitute(const std::string_view msg, std::string_view arg, size_t index) {
			size_t offset { 1 };
			std::string temp { msg.data(), msg.size() };
			std::string parseString { msg.data(), msg.size() };
			for( size_t i { 0 }; i < (index + offset); ++i ) {
					temp.erase(0, temp.find_first_of("}") + offset);
				}
			auto tempOffset { temp.end() - temp.begin() };
			auto argOffset { parseString.end() - tempOffset };
			if( tempOffset != 0 ) {
					parseString.erase(argOffset, parseString.end());
					parseString.append(arg).append(temp.data(), temp.size());
			} else {
					parseString = arg;
				}
			testContainer.RemoveArgFromContainer(index);
			return parseString;
		}

		template<typename... Args> void SetMessage(std::string_view message, Args&&... args) {
			// ~75% of CPU cycles is spent parsing the string... the string is literally just "{}" -> should be way more efficient
			// m_message.clear();
			// VFORMAT_TO(m_message, m_locale, message, std::forward<Args>(args)...);
			// m_message.append(SERENITY_LUTS::line_ending.at(platformEOL));
			/*********************************************** Testing some stuff here ***********************************************/
			/***********************************************************************************************************************
			 * - So far, just capturing the arguments and taking note of the number of args passed in only takes ~2% CPU cycles
			 *   vs the ~75% above.
			 * - Given that the manual message stamp formatting only takes ~4% of CPU cycles and this takes ~75%, this is definitely
			 *   where any optimizations for faster code would be.
			 * - I would say this is a very good start for some lazy parsing and substitution. formatlib will still do the heavy
			 *   lifting here, but it'll be nice if I can speed up large strings - algorithmic types, custom types, pointers, etc
			 *   will default to formatlib as I don't see the need to re-implement their work for smaller args. (This is really all
			 *   based off the notion that the format functions copy bytes to the output instead of a way to move them).
			 * - Total Cpu cycles for GetArgTypes() is ~8%, which brings the total up to ~10%-11%
			 ***********************************************************************************************************************/
			m_message.clear();
			lazy_message.clear();
			testContainer.CaptureArgs(std::forward<Args>(args)...);
			auto& argContainer { testContainer.argContainer };
			for( const auto& arg: argContainer ) {
					auto [ isStringType, argValue, argIndex ] = testContainer.GetArgInfo();
					if( isStringType ) {
							lazy_message = LazySubstitute(message, argValue, argIndex);
					}
					testContainer.AdvanceToNextArg();
					if( testContainer.EndReached() ) {
							break;
					}
				}
			if( testContainer.numberOfArgs >= 1 ) {
					for( auto& arg: argContainer ) {
							lazy_message = testContainer.FormatRemainingArgs(
							lazy_message, m_locale, std::make_format_args(std::forward<Args>(args)...));
						}
			}
			m_message.append(lazy_message);
			m_message.append(SERENITY_LUTS::line_ending.at(platformEOL));
		}

	      private:
		LineEnd platformEOL;
		std::string m_name;
		LoggerLevel m_msgLevel;
		std::string lazy_message;
		std::string m_message;
		Message_Time m_msgTime;
		std::locale m_locale;
		ArgContainer testContainer;
	};
}    // namespace serenity::msg_details
