#pragma once

#include <serenity/Common.h>

#include <variant>

// Moving ArgContainer to it's own file as I work on integrating it with the new ArgFormatter

namespace serenity::experimental::msg_details {

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

	static constexpr std::string_view SpecTypeString(SpecType type) {
		using enum SpecType;
		switch( type ) {
				case MonoType: return "MonoType"; break;
				case StringType: return "String  Type"; break;
				case CharPointerType: return "Const Char *"; break;
				case StringViewType: "String View"; break;
				case IntType: return "Int"; break;
				case U_IntType: return "Unsigned Int"; break;
				case LongLongType: return "Long Long"; break;
				case U_LongLongType: return "Unsigned Long Long"; break;
				case BoolType: return "Bool"; break;
				case CharType: return "Char"; break;
				case FloatType: return "Float"; break;
				case DoubleType: return "Double"; break;
				case LongDoubleType: return "Long Double"; break;
				case ConstVoidPtrType: return "Const Void *"; break;
				case VoidPtrType: return "Void *"; break;
				default: break;
			}
		return "";
	}

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

#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(24)
#endif    // !SERENITY_ARG_BUFFER_SIZE

	class ArgContainer
	{
	      public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long,
		                                          unsigned long long, bool, char, float, double, long double, const void*, void*>;

		// Keeping
		bool IsValidStringSpec(const char& spec);
		bool IsValidIntSpec(const char& spec);
		bool IsValidBoolSpec(const char& spec);
		bool IsValidFloatingPointSpec(const char& spec);
		bool IsValidCharSpec(const char& spec);
		bool VerifySpec(SpecType type, const char& spec);
		// ~Keeping

		// Keeping to reference and improve upon when I work on the Format'x'Token functions
		template<typename T>
		void FormatFloatTypeArg(std::string& container, char&& spec, T&& value, std::array<char, SERENITY_ARG_BUFFER_SIZE>& resultBuffer) {
			// std::chars_format format { std::chars_format::general };
			// int precisionValue { 6 };
			// auto& pStr { precisionSpecHelper.precision };
			//// precisionSpecHelper.precision is guaranteed to at most be size 2
			// if( pStr.size() != 0 ) {
			//		precisionValue = static_cast<int>(TwoDigitFromChars(pStr));
			// }
			// if( spec != '\0' ) {
			//		switch( spec ) {
			//				case 'a': [[fallthrough]];
			//				case 'A':
			//					container.append("0x");
			//					format = std::chars_format::hex;
			//					break;
			//				case 'e': format = std::chars_format::scientific; break;
			//				case 'E': format = std::chars_format::scientific; break;
			//				case 'f': [[fallthrough]];
			//				case 'F': format = std::chars_format::fixed; break;
			//				case 'g': format = std::chars_format::general; break;
			//				case 'G': format = std::chars_format::general; break;
			//				default: break;
			//			}
			// }
			// if( precisionValue != 0 ) {
			//		std::to_chars(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), std::forward<T>(value),
			//		              format, precisionValue);
			// } else {
			//		std::to_chars(resultBuffer.data(), resultBuffer.data() + resultBuffer.size(), std::forward<T>(value),
			//		              format);
			//	}
			// switch( spec ) {
			//		case 'A': [[fallthrough]];
			//		case 'E': [[fallthrough]];
			//		case 'G':
			//			for( auto& ch: container ) {
			//					if( ch == '\0' ) break;
			//					if( IsAlpha(ch) ) {
			//							ch -= 32;
			//					}
			//				}
			//			for( auto& ch: resultBuffer ) {
			//					if( ch == '\0' ) break;
			//					if( IsAlpha(ch) ) {
			//							ch -= 32;
			//					}
			//				}
			//			break;
			//		default: break;
			//	}    // capitilization
			// container.append(resultBuffer.data(), resultBuffer.data() + parseHelper.FindEndPos());
		}

		void EnableFallbackToStd(bool enable);
		// Keeping to reference and improve upon when I work on the Format'x'Token functions
		void AlignLeft(size_t index, std::string& container);
		void AlignRight(size_t index, std::string& container);
		void AlignCenter(size_t index, std::string& container);
		//***********************************************************************************

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				auto typeFound = is_supported<std::remove_reference_t<decltype(arg)>, LazilySupportedTypes> {};
				if constexpr( !typeFound.value ) {
						containsUnknownType = true;
				} else {
						if( containsUnknownType ) return;
						argContainer.emplace_back(arg);
						argSpecTypes.emplace_back(mapIndexToType[ argContainer.back().index() ]);
					}
			}(args),
			...);
		}

		template<typename... Args> constexpr void CaptureArgs(std::string_view formatString, Args&&... args) {
			Reset();
			// CountNumberOfBrackets(formatString);
			EmplaceBackArgs(std::forward<Args>(args)...);
			const size_t& size { argContainer.size() };
			if( size != 0 ) {
					maxIndex = size - 1;
			}
		}

		ArgContainer()                               = default;
		ArgContainer(const ArgContainer&)            = delete;
		ArgContainer& operator=(const ArgContainer&) = delete;
		~ArgContainer()                              = default;

		const std::vector<LazilySupportedTypes>& ArgStorage() const;

		constexpr void Reset() {
			argContainer.clear();
			argSpecTypes.clear();
			argIndex = maxIndex = remainingArgs = 0;
			endReached                          = false;
		}

		void AdvanceToNextArg();
		bool EndReached() const;
		bool ContainsUnsupportedType() const;
		void GetArgValue(std::string& container, size_t positionIndex, char&& additionalSpec = '\0');
		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);

		// May not need any more, but keeping until I flesh things out more
		void CountNumberOfBrackets(std::string_view fmt);
		std::string_view FinalArgResult() {
			return finalArgValue;
		}
		// Used here and in ArgFormatter, so may move this to Common.h to remove copy-pasta between the two
		size_t FindDigitEnd(std::string_view sv, size_t start);

		// used for ArgFormatter calls
		std::vector<SpecType>& SpecTypesCaptured() {
			return argSpecTypes;
		}

	      private:
		std::vector<LazilySupportedTypes> argContainer;
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		size_t remainingArgs { 0 };
		bool endReached { false };
		bool containsUnknownType { false };
		bool isStdFallbackEnabled { false };
		std::string finalArgValue;
		std::vector<SpecType> argSpecTypes;
	};
}    // namespace serenity::experimental::msg_details