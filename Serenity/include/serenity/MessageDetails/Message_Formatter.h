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
	enum class SpecValue
	{
		none         = 0,
		s            = 1,
		a            = 2,
		A            = 3,
		b            = 4,
		B            = 5,
		c            = 6,
		d            = 7,
		e            = 8,
		E            = 9,
		f            = 10,
		F            = 11,
		g            = 12,
		G            = 13,
		o            = 14,
		x            = 15,
		X            = 16,
		p            = 17,
		std_fallback = 18,
	};

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

	static std::unordered_map<size_t, SpecType> mapIndexToType = {
		{ 0, SpecType::MonoType },        { 1, SpecType::StringType },        { 2, SpecType::CharPointerType },
		{ 3, SpecType::StringViewType },  { 4, SpecType::IntType },           { 5, SpecType::U_IntType },
		{ 6, SpecType::LongLongType },    { 7, SpecType::U_LongLongType },    { 8, SpecType::BoolType },
		{ 9, SpecType::CharType },        { 10, SpecType::FloatType },        { 11, SpecType::DoubleType },
		{ 12, SpecType::LongDoubleType }, { 13, SpecType::ConstVoidPtrType }, { 14, SpecType::VoidPtrType },
	};

	class ArgContainer
	{
	      public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
		                                          unsigned long long, bool, char, float, double, long double, const void*, void*>;

		std::vector<SpecType> argSpecTypes;
		std::vector<SpecValue> argSpecValue;

		void HandleStringSpec(char spec);
		void HandleIntSpec(char spec);
		void HandleBoolSpec(char spec);
		void HandleFloatingPointSpec(char spec);
		void HandleCharSpec(char spec);
		void VerifySpec(SpecType type, char spec);
		void VerifySpecWithPrecision(SpecType type, std::string_view spec);
		std::tuple<std::string, char> SplitPrecisionAndSpec(SpecType type, std::string_view spec);
		void HandleArgBracket(std::string_view argBracket, int counterToIndex);
		void EnableFallbackToStd(bool enable);

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto arg) {
				auto typeFound = is_supported<decltype(arg), LazilySupportedTypes> {};
				auto size { 0 };
				if constexpr( !typeFound.value ) {
						containsUnknownType = true;
				} else {
						if( containsUnknownType ) return;
						argContainer.emplace_back(std::move(arg));
						argSpecTypes.emplace_back(mapIndexToType[ argContainer[ size ].index() ]);
						++size;
					}
			}(args),
			...);
		}

		template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
			ParseForSpecifiers(formatString);
			size_t size { argContainer.size() };
			if( size != 0 ) {
					maxIndex = size - 1;
			}
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
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);

		// May not remain here
		void ParseForSpecifiers(std::string_view);

	      private:
		std::vector<LazilySupportedTypes> argContainer;
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		bool endReached { false };
		LazyParseHelper parseHelper;
		bool containsUnknownType { false };
		bool isStdFallbackEnabled { false };
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
		// template<typename... Args> void FormatMessage(MsgWithLoc& message, Args&&... args);
		template<typename... Args> void FormatMessageArgs(MsgWithLoc& message, Args&&... args) {
			lazy_message.clear();
			argStorage.CaptureArgs(message.msg, std::forward<Args>(args)...);
			// This check currently slows everything down if there are specs or types I don't support
			// due to unfortunately still not being able to take advantage of compile time parsing
			// and <format>'s compile time features yet...
			// TODO: Implement a relatively straight-forward approach to parsing arg types and specs
			// TODO: at compile time and use an if constexpr expression to toggle usage here
			// - I can easily parse the specs but the message needs to be known at compile time first
			// - For the arg types, std::variant is compile time already, so it really *shouldn't* be
			//   all that hard to implement (Then I can get rid of ArgContainer versions here)
			if( argStorage.ContainsUnsupportedType() || argStorage.ContainsUnsupportedSpecs(message.msg) ) {
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
		void SetLocaleReference(std::locale* loc);
		std::string_view LineEnding() const;
		void EnableFallbackToStd(bool enable);

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
