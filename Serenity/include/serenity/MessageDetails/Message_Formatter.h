#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FormatterArgs.h>
#include <serenity/MessageDetails/Message_Info.h>

#include <charconv>
#include <format>
#include <string>
#include <variant>

namespace serenity::msg_details {
	// due to reworking this in ArgFormatter.h
#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(24)
#endif    // !SERENITY_ARG_BUFFER_SIZE

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

		void SetBracketPosition(bracket_type bracket, size_t pos);
		void SetPartition(partition_type pType, std::string_view sv);
		std::array<char, SERENITY_ARG_BUFFER_SIZE>& ConversionResultBuffer();
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
		std::array<char, SERENITY_ARG_BUFFER_SIZE> resultBuffer = {};
	};

	template<class T, class U> struct is_supported;
	template<class T, class... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same<T, Ts>::value || ...)>
	{
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

	static constexpr std::array<SpecType, 15> mapIndexToType = {
		SpecType::MonoType,  SpecType::StringType,   SpecType::CharPointerType, SpecType::StringViewType,   SpecType::IntType,
		SpecType::U_IntType, SpecType::LongLongType, SpecType::U_LongLongType,  SpecType::BoolType,         SpecType::CharType,
		SpecType::FloatType, SpecType::DoubleType,   SpecType::LongDoubleType,  SpecType::ConstVoidPtrType, SpecType::VoidPtrType,
	};

	static std::unordered_map<SpecType, size_t> mapTypeToIndex = {
		{ SpecType::MonoType, 0 },        { SpecType::StringType, 1 },        { SpecType::CharPointerType, 2 }, { SpecType::StringViewType, 3 },
		{ SpecType::IntType, 4 },         { SpecType::U_IntType, 5 },         { SpecType::LongLongType, 6 },    { SpecType::U_LongLongType, 7 },
		{ SpecType::BoolType, 8 },        { SpecType::CharType, 9 },          { SpecType::FloatType, 10 },      { SpecType::DoubleType, 11 },
		{ SpecType::LongDoubleType, 12 }, { SpecType::ConstVoidPtrType, 13 }, { SpecType::VoidPtrType, 14 },
	};

	class ArgContainer
	{
	  private:
		struct FillAlignValues
		{
			std::string temp;
			std::array<char, 250> buff {};
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

		struct PrecSpec
		{
			std::string temp;
			std::string precision;
			char spec { '\0' };
		};

	  public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long, unsigned long long,
		                                          bool, char, float, double, long double, const void*, void*>;

		bool IsValidStringSpec(char& spec);
		bool IsValidIntSpec(char& spec);
		bool IsValidBoolSpec(char& spec);
		bool IsValidFloatingPointSpec(char& spec);
		bool IsValidCharSpec(char& spec);
		bool VerifySpec(SpecType type, char& spec);
		bool VerifySpecWithPrecision(SpecType type, std::string_view& spec);
		void HandleWidthSpec(char& spec);
		void HandleSignSpec(size_t index, char& spec, std::string_view sv);
		void HandleHashSpec(char& spec);
		void HandlePrecisionSpec(size_t index, SpecType type);
		bool VerifyIfFillAndAlignSpec(size_t index, SpecType type, std::string_view& specView);
		void SplitPrecisionAndSpec(SpecType type, std::string_view spec);

		static constexpr std::array<char, 10> charDigits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		size_t TwoDigitFromChars(std::string_view sv) {
			if( sv[ 0 ] == '-' || sv[ 0 ] == '+' ) {
					// ignore any signed designation
					sv.remove_prefix(1);
			}

			size_t pos { 0 }, finalValue { 0 };
			switch( sv.size() ) {
					case 0: break;
					case 1:
						for( ;; ) {
								auto ch { charDigits[ pos ] };
								if( ch == sv[ 0 ] ) break;
								if( ch > sv[ 0 ] ) {
										--pos;
										break;
								}
								pos = ((pos + 2) % charDigits.size());
							}
						finalValue = pos;
						break;
					case 2:
						for( ;; ) {
								auto ch { charDigits[ pos ] };
								if( ch == sv[ 0 ] ) break;
								if( ch > sv[ 0 ] ) {
										--pos;
										break;
								}
								pos = ((pos + 2) % charDigits.size());
							}
						finalValue = pos * 10;
						pos        = 0;
						for( ;; ) {
								auto ch { charDigits[ pos ] };
								if( ch == sv[ 1 ] ) break;
								if( ch > sv[ 1 ] ) {
										--pos;
										break;
								}
								pos = ((pos + 2) % charDigits.size());
							}
						finalValue += pos;
						break;
					default:
						std::string throwMsg { "TwoDigitFromChars() Only Handles String Types Of Size 2 Or Less: "
							                   "\"" };
						throwMsg.append(sv.data(), sv.size()).append("\" Doesn't Adhere To This Limitation\n");
						throw std::runtime_error(std::move(throwMsg));
						break;
				}
			return finalValue;
		}

		template<typename T> void FormatFloatTypeArg(std::string& container, char&& spec, T&& value, std::array<char, SERENITY_ARG_BUFFER_SIZE>& resultBuffer) {
			std::chars_format format { std::chars_format::general };
			int precisionValue { 6 };
			auto& pStr { precisionSpecHelper.precision };
			// precisionSpecHelper.precision is guaranteed to at most be size 2
			if( pStr.size() != 0 ) {
					precisionValue = static_cast<int>(TwoDigitFromChars(pStr));
			}
			if( spec != '\0' ) {
					switch( spec ) {
							case 'a': [[fallthrough]];
							case 'A':
								container.append("0x");
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
					std::to_chars(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), std::forward<T>(value), format, precisionValue);
			} else {
					std::to_chars(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), std::forward<T>(value), format);
				}
			switch( spec ) {
					case 'A': [[fallthrough]];
					case 'E': [[fallthrough]];
					case 'G':
						for( auto& ch: container ) {
								if( ch == '\0' ) break;
								if( IsAlpha(ch) ) {
										ch -= 32;
								}
							}
						for( auto& ch: resultBuffer ) {
								if( ch == '\0' ) break;
								if( IsAlpha(ch) ) {
										ch -= 32;
								}
							}
						break;
					default: break;
				}    // capitilization
			container.append(resultBuffer.data(), resultBuffer.data() + parseHelper.FindEndPos());
		}

		void EnableFallbackToStd(bool enable);
		void AlignLeft(size_t index, std::string& container);
		void AlignRight(size_t index, std::string& container);
		void AlignCenter(size_t index, std::string& container);

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				if( !std::is_constant_evaluated() ) {
						using base_type = std::decay_t<decltype(arg)>;
						using ref       = std::add_rvalue_reference_t<base_type>;
						if( counter >= argContainer.size() - 1 ) {
								std::printf("Warning: Max Argument Count Of  25 Reached - Ignoring Any Further Arguments\n");
								return;
						}
						argContainer[ counter ] = std::forward<base_type>(ref(arg));
						++counter;

				} else {
						auto typeFound = is_supported<std::decay_t<decltype(arg)>, LazilySupportedTypes> {};
						if constexpr( !typeFound.value ) {
								containsUnknownType = true;
								// this is leftover from original impl, but leaving here
								// for the time-being for potential compile time warning
						} else {
								if( containsUnknownType ) return;
							}
					}
			}(args),
			...);
		}
		constexpr void StoreArgTypes() {
			size_t pos { 0 };
			for( ;; ) {
					if( pos >= counter ) break;
					argSpecTypes[ pos ] = mapIndexToType[ argContainer[ pos ].index() ];
					++pos;
				}
		}

		template<typename... Args> void CaptureArgs(Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
			StoreArgTypes();
			if( counter != 0 ) {
					maxIndex = counter - 1;
			}
		}

		ArgContainer()                               = default;
		ArgContainer(const ArgContainer&)            = delete;
		ArgContainer& operator=(const ArgContainer&) = delete;
		~ArgContainer()                              = default;

		const std::array<LazilySupportedTypes, 24>& ArgStorage() const;
		LazyParseHelper& ParseHelper();
		void Reset();
		void AdvanceToNextArg();
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		void GetArgValue(std::string& container, size_t positionIndex, char&& additionalSpec = '\0');
		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);
		void ParseForSpecifiers(std::string_view);
		void CountNumberOfBrackets(std::string_view fmt);
		std::string_view FinalArgResult() {
			return finalArgValue;
		}

	  private:
		  std::array<LazilySupportedTypes, 24> argContainer{LazilySupportedTypes {}};
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		size_t remainingArgs { 0 };
		bool endReached { false };
		LazyParseHelper parseHelper {};
		bool containsUnknownType { false };
		bool isStdFallbackEnabled { false };
		FillAlignValues fillAlignValues {};
		PrecSpec precisionSpecHelper {};
		std::string finalArgValue;
		std::array<SpecType, 24> argSpecTypes {SpecType::MonoType};
		size_t counter { 0 };
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
		void LazySubstitute(std::string& msg, std::string_view arg);

		// template<typename... Args> void FormatMessage(MsgWithLoc& message, Args&&... args);
		template<typename... Args> void FormatMessageArgs(MsgWithLoc& message, Args&&... args) {
			lazy_message.clear();
			argStorage.CountNumberOfBrackets(message.msg);
			argStorage.CaptureArgs(std::forward<Args>(args)...);
			if( argStorage.ContainsUnsupportedType() ) {
					VFORMAT_TO(lazy_message, message.msg, std::forward<Args>(args)...);
			} else {
					lazy_message.append(message.msg);
					for( ;; ) {
							argStorage.ParseForSpecifiers(message.msg);
							LazySubstitute(lazy_message, argStorage.FinalArgResult());
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
