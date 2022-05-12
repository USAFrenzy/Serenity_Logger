#pragma once

#include <serenity/Common.h>

#include <variant>

// Moving ArgContainer to it's own file as I work on integrating it with the new ArgFormatter

namespace serenity::experimental::msg_details {

	template<typename T, typename U> struct is_supported;
	template<typename T, typename... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same<T, Ts>::value || ...)>
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
			size_t pos { 0 };
			for( ;; ) {
					if( pos >= counter ) break;
					testSpecContainer[ pos ] = mapIndexToType[ testContainer[ pos ].index() ];
					++pos;
				}
		}

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				if( !std::is_constant_evaluated() ) {
						using base_type = std::decay_t<decltype(arg)>;
						using ref       = std::add_rvalue_reference_t<base_type>;
						if( counter >= testContainer.size() - 1 ) {
								std::printf("Warning: Max Argument Count Of  25 Reached - Ignoring Any Further Arguments\n");
								return;
						}
						testContainer[ counter ] = std::forward<base_type>(ref(arg));
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

		template<typename... Args> constexpr void CaptureArgs(Args&&... args) {
			Reset();
			EmplaceBackArgs(std::forward<Args>(args)...);
			StoreArgTypes();
		}

		ArgContainer()                               = default;
		ArgContainer(const ArgContainer&)            = delete;
		ArgContainer& operator=(const ArgContainer&) = delete;
		~ArgContainer()                              = default;

		const std::array<LazilySupportedTypes, 24>& ArgStorage() const;

		constexpr void Reset() {
			std::fill(testSpecContainer.data(), testSpecContainer.data() + counter, SpecType::MonoType);
			counter = 0;
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
			return *std::get_if<4>(&testContainer[ index ]);
		}

		unsigned int uint_state(size_t index) {
			return *std::get_if<5>(&testContainer[ index ]);
		}

		long long long_long_state(size_t index) {
			return *std::get_if<6>(&testContainer[ index ]);
		}

		unsigned long long u_long_long_state(size_t index) {
			return *std::get_if<7>(&testContainer[ index ]);
		}

		bool bool_state(size_t index) {
			return *std::get_if<8>(&testContainer[ index ]);
		}

		float float_state(size_t index) {
			return *std::get_if<10>(&testContainer[ index ]);
		}

		double double_state(size_t index) {
			return *std::get_if<11>(&testContainer[ index ]);
		}

		long double long_double_state(size_t index) {
			return *std::get_if<12>(&testContainer[ index ]);
		}

		// May not need any more, but keeping until I flesh things out more
		void CountNumberOfBrackets(std::string_view fmt);

		std::string_view FinalArgResult() {
			return finalArgValue;
		}
		// Used here and in ArgFormatter, so may move this to Common.h to remove copy-pasta between the two
		size_t FindDigitEnd(std::string_view sv);

		// used for ArgFormatter calls
		std::array<SpecType, 24>& SpecTypesCaptured() {
			return testSpecContainer;
		}

	  private:
		size_t maxIndex { 0 };
		size_t argIndex { 0 };
		size_t remainingArgs { 0 };
		bool endReached { false };
		bool containsUnknownType { false };
		bool isStdFallbackEnabled { false };
		std::string finalArgValue;
		std::array<char, SERENITY_ARG_BUFFER_SIZE> buffer = {};
		std::to_chars_result result {};
		std::array<LazilySupportedTypes, 24> testContainer {};
		std::array<SpecType, 24> testSpecContainer {};
		size_t counter {};
	};
}    // namespace serenity::experimental::msg_details

/**************************************** A RANDOM THOUGHT BEFORE HEADING TO BED ****************************************/
// Might be able to reduce the time spent storing arguments by using a std::array of a certain pre-defined max length
// By doing this, I could keep a counter as to the occupied size of the array and reduce the clearing section down to:
/*
 *  size_t pos {0};
 *	 for(;;){
 *	   if(pos >= counter) break;
 *       arr[pos].emplace(std::in_place_index_t<0>, std::monostate);
 *       ++pos;
 *   }
 */
// I could then store the argument types by indexing into the array at the incrementing counter position and just emplace
// that argument instead of having to construct the object in a vector
/*
 *  if(counter >= arr.size() -1) throw some error/warn/or return;
 *  arr[counter].emplace(std::forward<base_type(arg)>(ref(arg)));
 *  ++counter;
 */
// -> I think the clear times would be quicker but the storage times might be relatively the same. Then again, changing the
//    SpecType vector to index via an array saw ~ >2x improvements so this might be worth pursuing as a test?
