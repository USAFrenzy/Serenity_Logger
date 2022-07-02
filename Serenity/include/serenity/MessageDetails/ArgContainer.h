#pragma once

#include <string_view>
#include <array>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

namespace serenity {

	// convenience typedefs
	template<typename T> using type        = std::remove_cvref_t<T>;
	template<typename T> using FwdRef      = std::add_lvalue_reference_t<type<T>>;
	template<typename T> using FwdMove     = std::add_rvalue_reference_t<type<T>>;
	template<typename T> using Iterator    = std::back_insert_iterator<type<T>>;
	template<typename T> using FwdMoveIter = std::add_rvalue_reference_t<Iterator<T>>;

	template<typename T> struct IteratorAccessHelper: std::back_insert_iterator<T>
	{
		using std::back_insert_iterator<T>::container_type;
		constexpr IteratorAccessHelper(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		constexpr const auto& Container() {
			return this->container;
		}
	};

	template<typename Value> struct CustomFormatter
	{
		constexpr void Parse(std::string_view) { }
		template<typename ValueType, typename ContainerCtx> constexpr auto Format(ValueType&&, ContainerCtx&&) { }
	};

	struct IteratorContainerHelper
	{
		template<typename T> constexpr auto StoreStaticTyping(T&&) {
			const static type<T> type;
			return std::forward<decltype(type)>(type);
		}
	};

	namespace dummy_obj {
		static std::string dummy_str;
		static std::back_insert_iterator<std::string> dummy_iter = std::back_insert_iterator<std::string> { dummy_str };
	}    // namespace dummy_obj

	struct IteratorContainer
	{
		mutable const void* containerPtr;
		mutable IteratorContainerHelper helper;

		template<typename Iter> constexpr decltype(auto) AccessContainer(Iter&& iter, bool firstPass = false) const {
			if( firstPass ) {
					helper.StoreStaticTyping(type<decltype(IteratorAccessHelper(iter))::container_type> {});
					containerPtr = std::addressof(*IteratorAccessHelper(iter).Container());
			}
			using ConstType    = std::add_const_t<decltype(helper.StoreStaticTyping(dummy_obj::dummy_str))>;
			using ContainerRef = std::add_lvalue_reference_t<std::remove_const_t<ConstType>>;
			return std::forward<ContainerRef>(ContainerRef(*static_cast<ConstType*>(containerPtr)));
		}

		constexpr decltype(auto) UnderlyingContainer() const {
			return AccessContainer(dummy_obj::dummy_iter);
		}

		template<typename Iter> constexpr IteratorContainer(Iter&& iter) {
			// If it's a reference, then pass it by reference. iI it was a copy by value param, the copy can be moved
			// so treat it as an rvalue and just move both the copy and rvalue cases
			if constexpr( std::is_same_v<FwdRef<Iter>, Iter> ) {
					AccessContainer(std::forward<FwdRef<Iter>>(iter), true);
			} else {
					AccessContainer(std::forward<FwdMove<Iter>>(iter), true);
				}
		}
		constexpr IteratorContainer()                                    = default;
		constexpr IteratorContainer(const IteratorContainer&)            = delete;
		constexpr IteratorContainer& operator=(const IteratorContainer&) = delete;
		constexpr IteratorContainer(IteratorContainer&& o): containerPtr(std::move(o.containerPtr)), helper(std::move(o.helper)) {
			helper.StoreStaticTyping(o.helper.StoreStaticTyping(dummy_obj::dummy_str));
		}
		constexpr IteratorContainer& operator=(IteratorContainer&& o) {
			containerPtr = std::move(o.containerPtr);
			helper       = std::move(o.helper);
			helper.StoreStaticTyping(o.helper.StoreStaticTyping(dummy_obj::dummy_str));
			return *this;
		}
		constexpr ~IteratorContainer() = default;
	};

	struct CustomValue
	{
		template<typename T>
		explicit CustomValue(T&& value)
			: data(std::addressof(value)),
			  CustomFormatCallBack([](std::string_view parseView, const void* ptr, IteratorContainer&& container) -> decltype(container.UnderlyingContainer()) {
				  using QualifiedType = std::add_const_t<type<T>>;
				  auto formatter { CustomFormatter<type<T>> {} };
				  formatter.Parse(parseView);
				  formatter.Format(*static_cast<QualifiedType*>(ptr), container.UnderlyingContainer());
				  return container.UnderlyingContainer();
			  }) { }

		template<typename Container>
		constexpr auto FormatCallBack(std::back_insert_iterator<Container>&& Iter, std::string_view parseView) -> std::back_insert_iterator<type<Container>> {
			return std::move(std::back_inserter(CustomFormatCallBack(parseView, data, std::move(IteratorContainer(std::move(Iter))))));
		}

		~CustomValue()                             = default;
		CustomValue(const CustomValue&)            = delete;
		CustomValue& operator=(const CustomValue&) = delete;
		CustomValue(CustomValue&& o): data(o.data), CustomFormatCallBack(std::move(o.CustomFormatCallBack)) { }
		CustomValue& operator=(CustomValue&& o) {
			data                 = o.data;
			CustomFormatCallBack = std::move(o.CustomFormatCallBack);
			return *this;
		}

		const void* data;
		auto(*CustomFormatCallBack)(std::string_view parseView, const void* data, IteratorContainer&& container) -> decltype(container.UnderlyingContainer());
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
		constexpr CustomValue& custom_state(size_t index);
		constexpr IteratorContainer& IterContainer();

	  private:
		std::array<VType, MAX_ARG_COUNT> argContainer {};
		std::array<SpecType, MAX_ARG_COUNT> specContainer {};
		size_t counter {};
		IteratorContainer iterContainer;
	};
	// putting the definition here since clang was warning on extra qualifiers for some reason
	template<typename T> static constexpr SpecType GetArgType(T&& val) {
		using enum SpecType;
		if constexpr( std::is_same_v<type<T>, std::monostate> ) {
				return std::forward<SpecType>(MonoType);
		} else if constexpr( std::is_same_v<type<T>, std::string> ) {
				return std::forward<SpecType>(StringType);
		} else if constexpr( std::is_same_v<type<T>, const char*> ) {
				return std::forward<SpecType>(CharPointerType);
		} else if constexpr( std::is_same_v<type<T>, std::string_view> ) {
				return std::forward<SpecType>(StringViewType);
		} else if constexpr( std::is_same_v<type<T>, int> ) {
				return std::forward<SpecType>(IntType);
		} else if constexpr( std::is_same_v<type<T>, unsigned int> ) {
				return std::forward<SpecType>(U_IntType);
		} else if constexpr( std::is_same_v<type<T>, long long> ) {
				return std::forward<SpecType>(LongLongType);
		} else if constexpr( std::is_same_v<type<T>, unsigned long long> ) {
				return std::forward<SpecType>(U_LongLongType);
		} else if constexpr( std::is_same_v<type<T>, bool> ) {
				return std::forward<SpecType>(BoolType);
		} else if constexpr( std::is_same_v<type<T>, char> ) {
				return std::forward<SpecType>(CharType);
		} else if constexpr( std::is_same_v<type<T>, float> ) {
				return std::forward<SpecType>(FloatType);
		} else if constexpr( std::is_same_v<type<T>, double> ) {
				return std::forward<SpecType>(DoubleType);
		} else if constexpr( std::is_same_v<type<T>, long double> ) {
				return std::forward<SpecType>(LongDoubleType);
		} else if constexpr( std::is_same_v<type<T>, const void*> ) {
				return std::forward<SpecType>(ConstVoidPtrType);
		} else if constexpr( std::is_same_v<type<T>, void*> ) {
				return std::forward<SpecType>(VoidPtrType);
		} else {
				static_assert(is_formattable_v<type<T>>, "A Template Specialization Must Exist For A Custom Type Argument.\n\t"
				                                         "For Serenity, This Can Be Done By Specializing The CustomFormatter Template For Your Type And "
				                                         "Implementing The Parse() And Format() Functions.");
				return std::forward<SpecType>(CustomType);
			}
	}
#include <serenity/MessageDetails/ArgContainerImpl.h>
}    // namespace serenity::msg_details
