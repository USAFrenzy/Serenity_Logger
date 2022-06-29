#pragma once

#include <string_view>
#include <array>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

namespace serenity {
	template<typename T> struct IteratorContainer: std::back_insert_iterator<T>
	{
		// using type = std::remove_cvref_t<T>;
		using type = std::back_insert_iterator<T>::container_type;
		constexpr IteratorContainer(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		constexpr const auto& Container() {
			return this->container;
		}
		type value_type;
	};

	template<typename T> using IterContainerType = IteratorContainer<T>::type;

	// NOTE: Changed the signature of Format(), but at the moment, it's really only useful for when a string is the iterator container
	template<typename Value> struct CustomFormatter
	{
		constexpr void Parse(std::string_view) { }
		template<typename ValueType = std::remove_cvref_t<Value>, typename ContainerCtx> constexpr auto Format(ValueType&&, ContainerCtx&&) { }
	};

	static std::string dummy_str {};
	static auto dummy_iter = std::back_insert_iterator<std::string>(dummy_str);

	struct CallBackContainer
	{
		template<typename Iter> CallBackContainer(Iter&& iter) {
			AccessContainer(std::forward<std::remove_cvref_t<Iter>>(iter), true);
		}
		template<typename Iter> auto AccessContainer(Iter&& iter, bool firstPass = false) {
			using iter_type      = std::remove_cv_t<Iter>;
			using container_type = decltype(IteratorContainer(iter).value_type);
			using ref            = std::add_lvalue_reference_t<iter_type>;
			static const iter_type type { iter };

			static void* containerPtr;
			if( firstPass && (std::addressof(iter) != std::addressof(dummy_iter)) ) {
					containerPtr = std::addressof(iter);
					return IteratorContainer(*static_cast<std::remove_cvref_t<decltype(type)>*>(containerPtr)).Container();
			} else {
					return IteratorContainer(*static_cast<std::remove_cvref_t<decltype(type)>*>(containerPtr)).Container();
				}
		}
		// Having issues with this...
		decltype(auto) IterContainer() {
			return AccessContainer(dummy_iter);
		}
	};

	struct CustomValue
	{
		template<typename T, typename Iter>
		explicit CustomValue(T&& value, Iter&& iter)
			: container(std::forward<Iter>(iter)), data(std::addressof(value)),
			  CustomFormatCallBack([](std::string_view parseView, const void* ptr, CallBackContainer& container) -> void {
				  using UnqualifiedType = std::remove_cvref_t<T>;
				  using QualifiedType   = std::add_const_t<UnqualifiedType>;
				  auto formatter { CustomFormatter<UnqualifiedType> {} };
				  formatter.Parse(parseView);
				  formatter.Format(*static_cast<QualifiedType*>(ptr), *container.IterContainer());
			  }) { }

		// Given the callback uses a string out param, I can easily make the string case more efficient here, but would like to somehow store or pass the format
		// container without incurring a huge overhead; after tying a container forwarding approach into the call back and CustomValue constructor, there was a
		// pretty hefty overhead. I could store the container as a member variable, but that would also require templating both the ArgContainer and ArgFormatter
		// class on the container type (which may not be all that bad in all honesty -> If I understand it correctly, that's what the whole format_contex amd
		// templated 'Context' basically are there for in libfmt and <format>?)
		template<typename T> constexpr void FormatCallBack(std::string_view parseView, std::back_insert_iterator<T>&& iter) {
			CustomFormatCallBack(parseView, data, container);
		}

		~CustomValue()                             = default;
		CustomValue(const CustomValue&)            = delete;
		CustomValue& operator=(const CustomValue&) = delete;
		CustomValue(CustomValue&& o): container(std::move(o.container)), data(o.data), CustomFormatCallBack(std::move(o.CustomFormatCallBack)) { }
		CustomValue& operator=(CustomValue&& o) {
			container            = std::move(o.container);
			data                 = o.data;
			CustomFormatCallBack = std::move(o.CustomFormatCallBack);
			return *this;
		}

		CallBackContainer container;
		const void* data;
		void (*CustomFormatCallBack)(std::string_view parseView, const void* data, CallBackContainer& outResult);
	};

	// clang-format off
	using VType = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int,
		long long, unsigned long long, bool, char, float, double, long double, const void*, void*, CustomValue>;
	// clang-format on

	template<typename T, typename U> struct is_supported;
	template<typename T, typename... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same_v<std::remove_reference_t<T>, Ts> || ...)>
	{
	};
	template<typename T> inline constexpr bool is_supported_v = is_supported<T, VType>::value;

	template<typename T> struct is_native_ptr_type;
	template<typename T>
	struct is_native_ptr_type
		: std::bool_constant<std::is_same_v<std::remove_reference_t<T>, std::string_view> || std::is_same_v<std::remove_reference_t<T>, const char*> ||
	                         std::is_same_v<std::remove_reference_t<T>, void*> || std::is_same_v<std::remove_reference_t<T>, const void*>>
	{
	};
	template<typename T> inline constexpr bool is_native_ptr_type_v = is_native_ptr_type<std::remove_reference_t<T>>::value;

	template<typename T> struct has_formatter: std::bool_constant<std::is_default_constructible_v<CustomFormatter<T>>>
	{
	};
	template<typename T> inline constexpr bool has_formatter_v = has_formatter<T>::value;

	template<typename T> struct is_formattable;
	template<typename T>
	struct is_formattable: std::bool_constant<is_supported<std::remove_reference_t<T>, VType>::value || has_formatter<std::remove_cvref_t<T>>::value>
	{
	};
	template<typename T> inline constexpr bool is_formattable_v = is_formattable<T>::value;

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

		template<typename Iter, typename... Args> constexpr void CaptureArgs(Iter&&, Args&&... args);
		template<typename Iter, typename... Args> constexpr void StoreArgs(Iter&&, Args&&... args);
		template<typename T> constexpr void StoreNativeArg(T&& arg);
		template<typename Iter, typename T> constexpr void StoreCustomArg(Iter&&, T&& arg);

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
		constexpr CustomValue& custom_state(size_t index);

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
				static_assert(is_formattable_v<base_type>, "A Template Specialization Must Exist For A Custom Type Argument.\n\t"
				                                           "For Serenity, This Can Be Done By Specializing The CustomFormatter Template For Your Type And "
				                                           "Implementing The Parse() And Format() Functions.");
				return std::forward<SpecType>(CustomType);
			}
	}
#include <serenity/MessageDetails/ArgContainerImpl.h>
}    // namespace serenity::msg_details
