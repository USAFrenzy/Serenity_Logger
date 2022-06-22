#pragma once

#include <string_view>
#include <any>    // testing
#include <array>
#include <iterator>
#include <memory>
#include <variant>

namespace serenity {

	template<typename T, typename U> struct is_supported;
	template<typename T, typename... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same<T, Ts>::value || ...)>
	{
	};

	template<typename T> struct CustomFormatter
	{
		virtual constexpr void Parse(std::string_view) = 0;
		virtual constexpr std::string_view Format()    = 0;
	};

	template<typename T> struct has_formatter: std::bool_constant<std::is_default_constructible_v<CustomFormatter<T>>>
	{
	};

	/****************************************************************** NOTE ******************************************************************/
	// CustomBase correctly stores the value address and it's alignment size, however, when storing a unique_ptr to CustomValue<T>, the type is
	// lost and cannot be used later to call the type's specialization of CustomFormatter<T>. Given that fact, I either need some clever way of
	// holding and transferring the type to the non-templated base, or somehow have a type-erased way of calling the CustomFormatter<T>.
	// I know very little of type erasure other than simple duck-typing from polymorphism so this is going to be particularly hard I think...
	struct CustomBase
	{
		explicit CustomBase(void* value, size_t size): rawValue(value), valueSize(size) { }
		~CustomBase()                            = default;
		CustomBase(const CustomBase&)            = delete;
		CustomBase& operator=(const CustomBase&) = delete;
		void* rawValue;
		size_t valueSize;
	};

	template<typename T> struct CustomValue: CustomBase
	{
		explicit CustomValue(T&& value): CustomBase(static_cast<void*>(std::addressof(value)), std::alignment_of_v<decltype(value)>) { }
		~CustomValue()                             = default;
		CustomValue(const CustomValue&)            = delete;
		CustomValue& operator=(const CustomValue&) = delete;
	};
	/****************************************************************** NOTE ******************************************************************/
	// With the above note on losing the value's type, this obviously encounters the same issue as it cannot instantiate the CustomFormatter<T>
	// struct specialization without that type value to pass in... The goal of this struct was to have a direct interface to parse the argument bracket
	// with the custom parse method and then format the value  directly into the provided iterator container using the custom Format function by
	// getting the value  form it's address using it's type alignment to ensure we grab the whole address alignment of the data storage
	template<typename T, typename U> struct CustomStateFormatter
	{
		using ValueType = std::remove_cvref_t<U>;
		CustomStateFormatter(CustomValue<U>& value, std::string_view parseView, std::back_insert_iterator<T>&& Iter)
			: pView(std::move(parseView)), iter(std::forward<std::back_insert_iterator<T>>(Iter)), formatter(CustomFormatter<ValueType> {}) { }

		std::string_view pView;
		std::back_insert_iterator<T> iter;
		CustomFormatter<ValueType> formatter;
	};

	// clang-format off
	using VType = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int,
		long long, unsigned long long, bool, char, float, double, long double, const void*, void*, std::unique_ptr<CustomBase>>;
	// clang-format on

	template<typename T> struct is_formattable;
	template<typename T> struct is_formattable: std::bool_constant<is_supported<T, VType>::value || has_formatter<T>::value>
	{
	};
}    // namespace serenity

namespace serenity::msg_details {

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
		CustomType       = 15,
	};

	constexpr size_t MAX_ARG_COUNT = 25;
	constexpr size_t MAX_ARG_INDEX = 24;

	class ArgContainer
	{
	  public:
		constexpr ArgContainer()                               = default;
		constexpr ArgContainer(const ArgContainer&)            = delete;
		constexpr ArgContainer& operator=(const ArgContainer&) = delete;
		constexpr ~ArgContainer()                              = default;

		template<typename... Args> constexpr void CaptureArgs(Args&&... args);
		template<typename... Args> constexpr void StoreArgs(Args&&... args);
		template<typename T> constexpr void StoreNativeArg(T&& arg);
		template<typename T> constexpr void StoreCustomArg(T&& arg);

		constexpr std::array<VType, MAX_ARG_COUNT>& ArgStorage();
		constexpr std::array<SpecType, MAX_ARG_COUNT>& SpecTypesCaptured();
		constexpr std::string_view string_state(size_t index);
		constexpr std::string_view c_string_state(size_t index);
		constexpr std::string_view string_view_state(size_t index);
		constexpr int& int_state(size_t index);
		constexpr unsigned int& uint_state(size_t index);
		constexpr long long& long_long_state(size_t index);
		constexpr unsigned long long& u_long_long_state(size_t index);
		constexpr bool& bool_state(size_t index);
		constexpr char& char_state(size_t index);
		constexpr float& float_state(size_t index);
		constexpr double& double_state(size_t index);
		constexpr long double& long_double_state(size_t index);
		constexpr const void* const_void_ptr_state(size_t index);
		constexpr void* void_ptr_state(size_t index);
		constexpr std::unique_ptr<CustomBase>& custom_state(size_t index);

	  private:
		std::array<VType, MAX_ARG_COUNT> argContainer {};
		std::array<SpecType, MAX_ARG_COUNT> specContainer {};
		size_t counter {};
	};
	// putting the definition here since clang was warning on extra qualifiers for some reason
	template<typename T> static constexpr SpecType GetArgType(T&& val) {
		using enum SpecType;
		using base_type = std::remove_cvref_t<decltype(val)>;
		if constexpr( std::is_same_v<base_type, std::monostate> ) {
				return std::forward<SpecType>(MonoType);
		} else if constexpr( std::is_same_v<base_type, std::string> ) {
				return std::forward<SpecType>(StringType);
		} else if constexpr( std::is_same_v<base_type, const char*> ) {
				return std::forward<SpecType>(CharPointerType);
		} else if constexpr( std::is_same_v<base_type, std::string_view> ) {
				return std::forward<SpecType>(StringViewType);
		} else if constexpr( std::is_same_v<base_type, int> ) {
				return std::forward<SpecType>(IntType);
		} else if constexpr( std::is_same_v<base_type, unsigned int> ) {
				return std::forward<SpecType>(U_IntType);
		} else if constexpr( std::is_same_v<base_type, long long> ) {
				return std::forward<SpecType>(LongLongType);
		} else if constexpr( std::is_same_v<base_type, unsigned long long> ) {
				return std::forward<SpecType>(U_LongLongType);
		} else if constexpr( std::is_same_v<base_type, bool> ) {
				return std::forward<SpecType>(BoolType);
		} else if constexpr( std::is_same_v<base_type, char> ) {
				return std::forward<SpecType>(CharType);
		} else if constexpr( std::is_same_v<base_type, float> ) {
				return std::forward<SpecType>(FloatType);
		} else if constexpr( std::is_same_v<base_type, double> ) {
				return std::forward<SpecType>(DoubleType);
		} else if constexpr( std::is_same_v<base_type, long double> ) {
				return std::forward<SpecType>(LongDoubleType);
		} else if constexpr( std::is_same_v<base_type, const void*> ) {
				return std::forward<SpecType>(ConstVoidPtrType);
		} else if constexpr( std::is_same_v<base_type, void*> ) {
				return std::forward<SpecType>(VoidPtrType);
		} else {
				static_assert(is_formattable<base_type>::value, "A Template Specialization Must Exist For A Custom Type Argument.\n\t"
				                                                "For Serenity, This Can Be Done By Specializing The CustomFormatter Template For Your Type And "
				                                                "Implementing The Parse() And Format() Functions.");
				return std::forward<SpecType>(CustomType);
			}
	}
#include <serenity/MessageDetails/ArgContainerImpl.h>
}    // namespace serenity::msg_details
