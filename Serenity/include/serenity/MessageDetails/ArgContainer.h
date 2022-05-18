#pragma once
/************************************** What The ArgFormatter And ArgContainer Classes Offer **************************************/
// This work is a very simple reimplementation with limititations on my end of Victor Zverovich's fmt library.
// Currently the standard's implementation of his work is still underway although for the most part, it's feature
// complete with Victor's library - there are some huge performance drops when it's not compiled under the /utf-8
// flag on MSVC though.
//
// The ArgFormatter and ArgContainer classes work in tandem to deliver a very bare-bones version of what the fmt,
// and MSVC's implementation of fmt, libraries provides and is only intended for usage until MSVC's code is as performant
// as it is when compiled with /utf-8 for compilation without the need for this flag.
//
// With that being said, these classes provide the functionality of formatting to a container with a back insert iterator
// object which mirrors the std::format_to()/std::vformat_to() via the se_format_to() function, as well as a way to recieve
// a string with the formatted result via the se_format() function, mirroring std::format()/std::vformat().
// Unlike MSVC's implementations, however, the default locale used when the locale specifier is present will refer to the
// default locale created on construction of this class. The downside is that this will not reflect any changes when the
// locale is changed globally; but the benefit of this approach is reducing the construction of an empty locale on every
// format, as well as the ability to change the locale with "SetLocale(const std::locale &)" function without affecting
// the locale of the rest of the program. All formatting specifiers and manual/automatic indexing from the fmt library
// are available and supported.
//
// EDIT: It now seems that MSVC build  192930145 fixes the performance issues among other things with the <format> lib;
// kinda sad that the update came out JUST as I was almost done with the formatting section, however, the performance
// times of serenity is STILL faster than the MSVC's implementation - the consistency of their performance is now a
// non-issue though (same performance with or without the UTF-8 flag)
/**********************************************************************************************************************************/

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

	class ArgContainer
	{
	  public:
		using LazilySupportedTypes = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int, long long, unsigned long long,
		                                          bool, char, float, double, long double, const void*, void*>;

		void EnableFallbackToStd(bool enable);

		// Moved Formatting Templates Here While I Work On Some Things
		template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args) {
			(
			[ = ](auto&& arg) {
				using base_type = std::decay_t<decltype(arg)>;
				// ran into issues with std::string type args with adding an rvalue reference
				// (understandably), so swapped over to lvalue reference instead
				using ref       = std::add_lvalue_reference_t<base_type>;
				if( !std::is_constant_evaluated() ) {
						if( counter >= testContainer.size() - 1 ) {
								std::printf("Warning: Max Argument Count Of  25 Reached - Ignoring Any Further Arguments\n");
								return;
						}
						testContainer[ counter ] = std::forward<ref>(ref(arg));
						++counter;
				} else {
						auto typeFound = is_supported<base_type, LazilySupportedTypes> {};
						if constexpr( !typeFound.value ) {
								// this is leftover from original impl, but leaving here
								// for the time-being for potential compile time warning
						} else {
							}
					}
			}(args),
			...);
		}

		constexpr void Reset() {
			std::fill(testSpecContainer.data(), testSpecContainer.data() + counter, SpecType::MonoType);
			counter = 0;
		}

		constexpr void StoreArgTypes() {
			size_t pos { 0 };
			for( ;; ) {
					if( pos >= counter ) break;
					testSpecContainer[ pos ] = mapIndexToType[ testContainer[ pos ].index() ];
					++pos;
				}
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
		std::array<SpecType, 24>& SpecTypesCaptured();

		//	template<typename... Args> constexpr void EmplaceBackArgs(Args&&... args);
		// template<typename... Args> void CaptureArgs(std::string_view formatString, Args&&... args);

		std::string string_state(size_t index);
		const char* c_string_state(size_t index);
		std::string_view string_view_state(size_t index);
		int int_state(size_t index);
		unsigned int uint_state(size_t index);
		long long long_long_state(size_t index);
		unsigned long long u_long_long_state(size_t index);
		bool bool_state(size_t index);
		char char_state(size_t index);
		float float_state(size_t index);
		double double_state(size_t index);
		long double long_double_state(size_t index);
		const void* const_void_ptr_state(size_t index);
		void* void_ptr_state(size_t index);

	  private:
		bool endReached { false };
		bool isStdFallbackEnabled { false };
		std::array<LazilySupportedTypes, 24> testContainer {};
		std::array<SpecType, 24> testSpecContainer {};
		size_t counter {};
	};
}    // namespace serenity::experimental::msg_details
