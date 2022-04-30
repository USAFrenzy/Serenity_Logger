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
		none = 0,
		s    = 1,
		a    = 2,
		A    = 3,
		b    = 4,
		B    = 5,
		c    = 6,
		d    = 7,
		e    = 8,
		E    = 9,
		f    = 10,
		F    = 11,
		g    = 12,
		G    = 13,
		o    = 14,
		x    = 15,
		X    = 16,
		p    = 17,
	};

	enum class SpecValueModifiers
	{
		fill_align   = 0,
		width        = 1,
		sign         = 2,
		hash         = 3,
		zero_pad     = 4,
		std_fallback = 5,
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

	static std::unordered_map<SpecType, size_t> mapTypeToIndex = {
		{ SpecType::MonoType, 0 },        { SpecType::StringType, 1 },        { SpecType::CharPointerType, 2 },
		{ SpecType::StringViewType, 3 },  { SpecType::IntType, 4 },           { SpecType::U_IntType, 5 },
		{ SpecType::LongLongType, 6 },    { SpecType::U_LongLongType, 7 },    { SpecType::BoolType, 8 },
		{ SpecType::CharType, 9 },        { SpecType::FloatType, 10 },        { SpecType::DoubleType, 11 },
		{ SpecType::LongDoubleType, 12 }, { SpecType::ConstVoidPtrType, 13 }, { SpecType::VoidPtrType, 14 },
	};

	class ArgContainer
	{
		struct FillAlignValues
		{
			std::string temp;
			size_t digitSpec { 0 };
			char fillSpec { '\0' };
			char fillAlignSpec { '<' };
			char additionalSpec { '\0' };
			void Reset() {
				digitSpec = 0;
				fillSpec = additionalSpec = '\0';
				fillAlignSpec             = '<';
			}
		};

	      private:
		struct PrecSpec
		{
			std::string precision;
			char spec { '\0' };
		};

	      public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
		                                          unsigned long long, bool, char, float, double, long double, const void*, void*>;

		std::vector<SpecType> argSpecTypes;
		std::vector<SpecValue> argSpecValue;

		bool IsValidStringSpec(char spec);
		bool IsValidIntSpec(char spec);
		bool IsValidBoolSpec(char spec);
		bool IsValidFloatingPointSpec(char spec);
		bool IsValidCharSpec(char spec);
		bool VerifySpec(SpecType type, char spec);
		bool VerifySpecWithPrecision(size_t index, SpecType type, std::string_view spec);
		void HandleWidthSpec(char spec);
		void HandleSignSpec(char spec);
		void HandleHashSpec(char spec);
		void HandlePrecisionSpec(size_t index, SpecType type);
		bool VerifyIfFillAndAlignSpec(SpecType type, std::string_view specView);
		void SplitPrecisionAndSpec(size_t index, SpecType type, std::string_view spec);

		template<typename T> void FormatFloatTypeArg(char spec, T value) {
			std::chars_format format { std::chars_format::general };
			int precisionValue { 0 };
			auto& buffer { parseHelper.ConversionResultBuffer() };
			auto& strRef { parseHelper.StringBuffer() };

			if( precisionSpecHelper.precision.size() != 0 ) {
					std::from_chars(precisionSpecHelper.precision.data(),
					                precisionSpecHelper.precision.data() + precisionSpecHelper.precision.size(),
					                precisionValue);
			}
			if( spec != '\0' ) {
					switch( spec ) {
							case 'a': [[fallthrough]];
							case 'A':
								strRef.append("0x");
								format = std::chars_format::hex;
								break;
							case 'e': format = std::chars_format::scientific; break;
							case 'E': format = std::chars_format::scientific; break;
							case 'f': [[fallthrough]];
							case 'F': format = std::chars_format::fixed; break;
							case 'g': format = std::chars_format::general; break;
							case 'G': format = std::chars_format::general; break;
							default: break;
						}
			}
			if( precisionValue != 0 ) {
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, format, precisionValue));
			} else {
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, format));
				}
			switch( spec ) {
					case 'A': [[fallthrough]];
					case 'E': [[fallthrough]];
					case 'G':
						for( auto& ch: strRef ) {
								if( IsAlpha(ch) ) {
										ch -= 32;
								}
							}
						for( auto& ch: buffer ) {
								if( ch == '\0' ) break;
								if( IsAlpha(ch) ) {
										ch -= 32;
								}
							}
						break;
					default: break;
				}    // capitilization
		}

		void EnableFallbackToStd(bool enable);
		std::string&& AlignLeft(size_t index);
		std::string&& AlignRight(size_t index);
		std::string&& AlignCenter(size_t index);

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				auto typeFound = is_supported<std::remove_reference_t<decltype(arg)>, LazilySupportedTypes> {};
				auto size { 0 };
				if constexpr( !typeFound.value ) {
						containsUnknownType = true;
				} else {
						if( containsUnknownType ) return;
						argContainer.emplace_back(std::remove_reference_t<decltype(arg)>(arg));
						argSpecTypes.emplace_back(mapIndexToType[ argContainer[ size ].index() ]);
						++size;
					}
			}(args),
			...);
		}

		template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
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
		size_t AdvanceToNextArg();
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		std::string&& GetArgValue(size_t positionIndex, char additionalSpec = '\0');
		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);

		// May not remain here
		std::string&& ParseForSpecifiers(std::string_view&);

	      private:
		std::vector<LazilySupportedTypes> argContainer;
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		bool endReached { false };
		LazyParseHelper parseHelper;
		bool containsUnknownType { false };
		bool isStdFallbackEnabled { false };
		FillAlignValues fillAlignValues;
		std::string finalArgValue;
		std::string tempStorage;

	      protected:
		PrecSpec precisionSpecHelper;
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

		size_t ParseUserPatternForSpec(std::string& parseStr, size_t index);
		void ValidateUserCharSpec(size_t index, std::vector<char> specs);
		void ValidateUserPrecisionSpec(size_t index, size_t& value);
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
			if( argStorage.ContainsUnsupportedType() ) {
					VFORMAT_TO(lazy_message, message.msg, std::forward<Args>(args)...);
			} else {
					lazy_message.append(message.msg);
					for( ;; ) {
							LazySubstitute(lazy_message, std::move(argStorage.ParseForSpecifiers(message.msg)));
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
