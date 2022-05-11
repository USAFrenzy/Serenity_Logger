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

	static std::monostate Type(std::monostate s) {
		return s;
	}
	static std::string Type(std::string s) {
		return s;
	}
	static const char* Type(const char* s) {
		return s;
	}
	static std::string_view Type(std::string_view s) {
		return s;
	}
	static int Type(int s) {
		return s;
	}
	static unsigned int Type(unsigned int s) {
		return s;
	}
	static long long Type(long long s) {
		return s;
	}
	static unsigned long long Type(unsigned long long s) {
		return s;
	}
	static bool Type(bool s) {
		return s;
	}
	static char Type(char s) {
		return s;
	}
	static float Type(float s) {
		return s;
	}
	static double Type(double s) {
		return s;
	}
	static long double Type(long double s) {
		return s;
	}
	static const void* Type(const void* s) {
		return s;
	}
	static void* Type(void* s) {
		return s;
	}

	static constexpr std::array<SpecType, 15> mapIndexToType = {
		SpecType::MonoType,   SpecType::StringType,    SpecType::CharPointerType, SpecType ::StringViewType,   SpecType ::IntType,
		SpecType ::U_IntType, SpecType ::LongLongType, SpecType::U_LongLongType,  SpecType ::BoolType,         SpecType::CharType,
		SpecType ::FloatType, SpecType ::DoubleType,   SpecType::LongDoubleType,  SpecType ::ConstVoidPtrType, SpecType ::VoidPtrType,
	};

	static std::unordered_map<SpecType, size_t> mapTypeToIndex = {
		{ SpecType::MonoType, 0 },        { SpecType::StringType, 1 },        { SpecType::CharPointerType, 2 }, { SpecType::StringViewType, 3 },
		{ SpecType::IntType, 4 },         { SpecType::U_IntType, 5 },         { SpecType::LongLongType, 6 },    { SpecType::U_LongLongType, 7 },
		{ SpecType::BoolType, 8 },        { SpecType::CharType, 9 },          { SpecType::FloatType, 10 },      { SpecType::DoubleType, 11 },
		{ SpecType::LongDoubleType, 12 }, { SpecType::ConstVoidPtrType, 13 }, { SpecType::VoidPtrType, 14 },
	};

#ifndef SERENITY_ARG_BUFFER_SIZE
	#define SERENITY_ARG_BUFFER_SIZE static_cast<size_t>(24)
#endif    // !SERENITY_ARG_BUFFER_SIZE

	class ArgContainer
	{
	  public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long, unsigned long long,
		                                          bool, char, float, double, long double, const void*, void*>;

		// Keeping
		bool IsValidStringSpec(const char& spec);
		bool IsValidIntSpec(const char& spec);
		bool IsValidBoolSpec(const char& spec);
		bool IsValidFloatingPointSpec(const char& spec);
		bool IsValidCharSpec(const char& spec);
		bool VerifySpec(SpecType type, const char& spec);
		// ~Keeping

		// Keeping to reference and improve upon when I work on the Format'x'Token functions
		template<typename T> void FormatFloatTypeArg(std::string& container, char&& spec, T&& value, int precision) {
			std::chars_format format { std::chars_format::general };
			if( spec != '\0' ) {
					switch( spec ) {
							case 'a': [[fallthrough]];
							case 'A':
								container.append("0x");
								precision = precision > 0 ? precision : 0;
								format    = std::chars_format::hex;
								break;
							case 'e': [[fallthrough]];
							case 'E':
								format    = std::chars_format::scientific;
								precision = precision > 0 ? precision : 6;
								break;
							case 'f': [[fallthrough]];
							case 'F':
								format    = std::chars_format::fixed;
								precision = precision > 0 ? precision : 6;
								break;
							case 'g': [[fallthrough]];
							case 'G':
								format    = std::chars_format::general;
								precision = precision > 0 ? precision : 6;
								break;
							default: break;
						}
			}
			auto data { buffer.data() };
			if( precision != 0 ) {
					result = std::to_chars(data, data + buffer.size(), std::forward<T>(value), format, precision);
			} else {
					result = std::to_chars(data, data + buffer.size(), std::forward<T>(value), format);
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
						for( auto& ch: buffer ) {
								if( ch == '\0' ) break;
								if( IsAlpha(ch) ) {
										ch -= 32;
								}
							}
						break;
					default: break;
				}    // capitilization
			container.append(data, result.ptr);
		}

		void EnableFallbackToStd(bool enable);
		// Keeping to reference and improve upon when I work on the Format'x'Token functions
		void AlignLeft(size_t index, std::string& container);
		void AlignRight(size_t index, std::string& container);
		void AlignCenter(size_t index, std::string& container);
		//***********************************************************************************

		constexpr void StoreArgTypes() {
			for( auto& arg: argContainer ) {
					argSpecTypes.emplace_back(mapIndexToType[ arg.index() ]);
				}
		}

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				if( !std::is_constant_evaluated() ) {
						using base_type = std::decay_t<decltype(arg)>;
						using ref       = std::add_rvalue_reference_t<base_type>;
						argContainer.emplace_back(std::forward<base_type>(ref(arg)));
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

		template<typename... Args> constexpr void CaptureArgs(Args&&... args) {
			Reset();
			argContainer.reserve(sizeof...(args));
			EmplaceBackArgs(std::forward<Args>(args)...);
			StoreArgTypes();
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
		void GetArgValueAsStr(std::string& container, size_t positionIndex, char&& additionalSpec = '\0', int precision = 0);
		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);

		int RawValue(int);
		unsigned int RawValue(unsigned int);
		long long RawValue(long long);
		unsigned long long RawValue(unsigned long long);
		bool RawValue(bool);
		float RawValue(float);
		double RawValue(double);
		long double RawValue(long double);

		int int_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<4>(argContainer[ index ]);
		}
		unsigned int uint_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<5>(argContainer[ index ]);
		}

		long long long_long_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<6>(argContainer[ index ]);
		}

		unsigned long long u_long_long_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<7>(argContainer[ index ]);
		}

		bool bool_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<8>(argContainer[ index ]);
		}

		float float_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<10>(argContainer[ index ]);
		}

		double double_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<11>(argContainer[ index ]);
		}

		long double long_double_state(size_t index) {
			if( index >= argContainer.size() ) {
					throw std::runtime_error("Error In GetRawArgValue(): Index Provided Exceeds Elements In Argument "
					                         "Container\n");
			}
			return std::get<12>(argContainer[ index ]);
		}

		// May not need any more, but keeping until I flesh things out more
		void CountNumberOfBrackets(std::string_view fmt);
		std::string_view FinalArgResult() {
			return finalArgValue;
		}
		// Used here and in ArgFormatter, so may move this to Common.h to remove copy-pasta between the two
		size_t FindDigitEnd(std::string_view sv);

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
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};
		std::to_chars_result result {};
	};
}    // namespace serenity::experimental::msg_details