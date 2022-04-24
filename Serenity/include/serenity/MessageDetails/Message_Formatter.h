#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FormatterArgs.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>
#include <format>
#include <string>
#include <variant>

#ifdef WINDOWS_PLATFORM
	#ifdef FormatMessage
		#undef FormatMessage
	#endif    // FORMATMESSAGE
#endif

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
		size_t FindEndPos();

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
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
		                                          unsigned long long, bool, char, float, double, long double, const void*, void*>;

		enum class SpecType
		{
			MonoType         = 0,
			StringType       = 1,
			CharPointerType  = 2,
			StringViewType   = 3,
			IntType          = 4,
			U_IntType        = 5,
			LongLongType     = 6,
			U_LongLongType   = 7,
			BoolType         = 8,
			CharType         = 9,
			FloatType        = 10,
			DoubleType       = 11,
			LongDoubleType   = 12,
			ConstVoidPtrType = 13,
			VoidPtrType      = 14,
		};

		std::unordered_map<size_t, SpecType> typeMap = {
			{ 0, SpecType::MonoType },        { 1, SpecType::StringType },        { 2, SpecType::CharPointerType },
			{ 3, SpecType::StringViewType },  { 4, SpecType::IntType },           { 5, SpecType::U_IntType },
			{ 6, SpecType::LongLongType },    { 7, SpecType::U_LongLongType },    { 8, SpecType::BoolType },
			{ 9, SpecType::CharType },        { 10, SpecType::FloatType },        { 11, SpecType::DoubleType },
			{ 12, SpecType::LongDoubleType }, { 13, SpecType::ConstVoidPtrType }, { 14, SpecType::VoidPtrType },
		};

		std::vector<SpecType> argSpecTypes;

		// Moved this here while I work on some things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto arg) {
				auto typeFound = is_supported<decltype(arg), LazilySupportedTypes> {};
				if constexpr( !typeFound.value ) {
						containsUnknownType = true;
				} else {
						if( containsUnknownType ) return;
						argContainer.emplace_back(std::move(arg));
						argSpecTypes.emplace_back(typeMap[ argContainer.back().index() ]);
					}
			}(args),
			...);
		}

		ArgContainer()                               = default;
		ArgContainer(const ArgContainer&)            = delete;
		ArgContainer& operator=(const ArgContainer&) = delete;
		~ArgContainer()                              = default;

		const std::vector<LazilySupportedTypes>& ArgStorage() const;
		LazyParseHelper& ParseHelper();
		void Reset();
		void AdvanceToNextArg();
		bool ContainsUnsupportedSpecs(const std::string_view fmt);
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		std::string&& GetArgValue();
		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		template<typename... Args> void CaptureArgs(Args&&... args);

		// May not remain here
		void ParseForSpecifiers(const std::string_view);

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
		template<typename... Args> void FormatMessage(MsgWithLoc& message, Args&&... args);
		void SetLocaleReference(std::locale* loc);
		std::string_view LineEnding() const;

	      private:
		Message_Info* msgInfo;
		std::locale* localeRef;
		Formatters formatter;
		std::string fmtPattern;
		std::string lazy_message;
		LineEnd platformEOL;
		ArgContainer argStorage;
		std::string temp;
		source_flag sourceFlag;
	};
#include <serenity/MessageDetails/Message_Formatter_Impl.h>
}    // namespace serenity::msg_details
