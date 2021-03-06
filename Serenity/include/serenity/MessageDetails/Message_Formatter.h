#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FormatterArgs.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>
#include <format>
#include <string>
#include <variant>

namespace serenity::msg_details {

	class LazyParseHelper
	{
	      public:
		enum class bracket_type
		{
			open = 0,
			close
		};
		enum class partition_type
		{
			primary = 0,
			remainder
		};

		LazyParseHelper()                                  = default;
		LazyParseHelper(const LazyParseHelper&)            = delete;
		LazyParseHelper& operator=(const LazyParseHelper&) = delete;
		~LazyParseHelper()                                 = default;

		void SetBracketPosition(bracket_type bracket, const size_t& pos);
		void SetConversionResult(const std::to_chars_result& convResult);
		void SetPartition(partition_type pType, std::string_view sv);
		std::array<char, SERENITY_ARG_BUFFER_SIZE>& ConversionResultBuffer();
		const std::to_chars_result ConversionResultInfo() const;
		size_t BracketPosition(bracket_type bracket) const;
		std::string& PartitionString(partition_type pType);
		std::string& StringBuffer();
		void ClearBuffer();
		void ClearPartitions();

	      private:
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

	class ArgContainer
	{
	      public:
		using LazilySupportedTypes        = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int,
                                                          long long, unsigned long long, bool, char, float, double, long double, const void*>;

		ArgContainer()                    = default;
		ArgContainer(const ArgContainer&) = delete;
		ArgContainer& operator=(const ArgContainer&) = delete;
		~ArgContainer()                              = default;

		const std::vector<LazilySupportedTypes>& ArgStorage() const;
		LazyParseHelper& ParseHelper();
		void Reset();
		void AdvanceToNextArg();
		bool ContainsArgSpecs(const std::string_view fmt);
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		std::string&& GetArgValue();
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
		template<typename... Args> void CaptureArgs(Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
			size_t size { argContainer.size() };
			if( size != 0 ) {
					maxIndex = size - 1;
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

	class Message_Formatter
	{
	      public:
		class Formatters
		{
		      public:
			Formatters(std::vector<std::unique_ptr<Formatter>>&& container);
			Formatters() = default;
			void Emplace_Back(std::unique_ptr<Formatter>&& formatter);
			std::string_view FormatUserPattern();
			void Clear();

		      private:
			std::string localBuffer;
			std::string temp;
			std::vector<std::unique_ptr<Formatter>> m_Formatter;
		};

		explicit Message_Formatter(std::string_view pattern, Message_Info* details);
		~Message_Formatter()                              = default;
		Message_Formatter()                               = delete;
		Message_Formatter(const Message_Formatter&)       = delete;
		Message_Formatter& operator=(const Message_Info&) = delete;

		size_t ParseForSpec(std::string& parseStr, size_t index);
		void ValidateCharSpec(size_t index, std::vector<char> specs);
		void ValidatePrecisionSpec(size_t index, size_t& value);
		void FlagFormatter(size_t index, size_t precision = 0);
		void SetPattern(std::string_view pattern);
		Formatters& GetFormatters();
		void StoreFormat();
		const Message_Info* MessageDetails();
		void LazySubstitute(std::string& msg, std::string&& arg);
		template<typename... Args> void FmtMessage(MsgWithLoc message, Args&&... args) {
			lazy_message.clear();
			argStorage.CaptureArgs(std::forward<Args>(args)...);
			if( argStorage.ContainsUnsupportedType() || argStorage.ContainsArgSpecs(message.msg) ) {
					VFORMAT_TO(lazy_message, *localeRef, message.msg, std::forward<Args>(args)...);
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
			auto lineEnd { SERENITY_LUTS::line_ending.at(platformEOL) };
			lazy_message.append(lineEnd.data(), lineEnd.size());
			msgInfo->SetMessage(lazy_message, message.source);
		}

	      private:
		Message_Info* msgInfo;
		std::locale* localeRef;
		int ePrecision;
		Formatters formatter;
		std::string fmtPattern;
		std::string lazy_message;
		LineEnd platformEOL;
		ArgContainer argStorage;
		std::string temp;
		source_flag sourceFlag;
	};

}    // namespace serenity::msg_details
