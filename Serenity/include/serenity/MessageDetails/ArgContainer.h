#pragma once

#include <string_view>
#include <array>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

namespace serenity {
	template<typename T> struct IteratorAccessHelper: std::back_insert_iterator<T>
	{
		// using type = std::remove_cvref_t<T>;
		using std::back_insert_iterator<T>::container_type;

		constexpr IteratorAccessHelper(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		constexpr const auto& Container() {
			return this->container;
		}
	};

	// NOTE: Changed the signature of Format(), but at the moment, it's really only useful for when a string is the iterator container
	template<typename Value> struct CustomFormatter
	{
		constexpr void Parse(std::string_view) { }
		template<typename ValueType = std::remove_cvref_t<Value>, typename ContainerCtx> constexpr auto Format(ValueType&&, ContainerCtx&&) { }
	};

	// Initially was using a static variable to keep track of the typing in IteratorContainer::AccessContainer(),
	// however, that prevented the function from being declared constexpr. This is a work around for that,
	// though, for the same reasons as the initial reason, this only works in runtime context and not constexpr
	// context
	struct IteratorContainerHelper
	{
		template<typename T> auto StoreStaticTyping(T&&) {
			using value_type = std::remove_cvref_t<T>;
			const static value_type type;
			// can add lvalue ref  here due to const static storage
			using Ref = std::add_lvalue_reference_t<decltype(type)>;
			return std::forward<Ref>(type);
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
					using container_type = std::remove_cvref_t<decltype(IteratorAccessHelper(iter))::container_type>;
					helper.StoreStaticTyping(container_type {});
					containerPtr = std::addressof(*IteratorAccessHelper(iter).Container());
			}
			using type         = std::add_const_t<decltype(helper.StoreStaticTyping(dummy_obj::dummy_str))>;
			using ContainerRef = std::add_lvalue_reference_t<std::remove_const_t<type>>;
			return std::forward<ContainerRef>(ContainerRef(*static_cast<type*>(containerPtr)));
		}

		constexpr decltype(auto) UnderlyingContainer() const {
			return AccessContainer(dummy_obj::dummy_iter);
		}

		template<typename Iter> IteratorContainer(Iter&& iter) {
			AccessContainer(std::forward<std::add_rvalue_reference_t<std::remove_cvref_t<Iter>>>(iter), true);
		}
		template<typename Iter> IteratorContainer(Iter& iter) {
			AccessContainer(std::forward<std::add_lvalue_reference_t<std::remove_cvref_t<Iter>>>(iter), true);
		}
		IteratorContainer()                                    = default;
		IteratorContainer(const IteratorContainer&)            = delete;
		IteratorContainer& operator=(const IteratorContainer&) = delete;
		IteratorContainer(IteratorContainer&& o)
			: containerPtr(std::move(o.containerPtr)), helper(IteratorContainerHelper {} /*no data members so no need to move, just instantiate it*/) {
			helper.StoreStaticTyping(o.helper.StoreStaticTyping(dummy_obj::dummy_str));
		}
		IteratorContainer& operator=(IteratorContainer&& o) {
			containerPtr = std::move(o.containerPtr);
			helper       = IteratorContainerHelper {};    // like move ctr, no need to move since it's empty, just instantiate it
			helper.StoreStaticTyping(o.helper.StoreStaticTyping(dummy_obj::dummy_str));
			return *this;
		}
		~IteratorContainer() = default;
	};

	struct CustomValue
	{
		template<typename T>
		explicit CustomValue(T&& value)
			: data(std::addressof(value)), CustomFormatCallBack([](std::string_view parseView, const void* ptr, IteratorContainer&& container) -> void {
				  using UnqualifiedType = std::remove_cvref_t<T>;
				  using QualifiedType   = std::add_const_t<UnqualifiedType>;
				  auto formatter { CustomFormatter<UnqualifiedType> {} };
				  formatter.Parse(parseView);
				  formatter.Format(*static_cast<QualifiedType*>(ptr), container.UnderlyingContainer());
			  }) { }

		template<typename Container> constexpr void FormatCallBack(Container&& container, std::string_view parseView) {
			using ContainerType = std::remove_cvref_t<Container>;
			using ContainerRef  = std::add_lvalue_reference_t<ContainerType>;
			/**********************************************************************  NOTE **********************************************************************/
			// Debating about backtracking just a little bit. Right now I'm forwarding the underlying container instead of the iterator like I was doing before, but
			// if I just go back to forwarding the iterator, then I could forward the iterator here and just construct the IteratorContainer object with the already
			// constructed iterator object instead of adding a call to the back_insert_iterator constructor. In which case, I could also remove the move assignment
			// of the IteratorContainer move constructor in the se_format_to() functions. I would imagine that  the ~10ns hit would likely disappear  if I do it this
			// way? Will have to test this out tomorrow if I can get around to it then. I would be stoked if I could actually straight up gain a net positive over
			// the current use case and the previous use case while still being able to directly format into the container object with the custom formatters.
			// For my own reference, the current use case is ~26% faster than the standard, the previous use case (which didn't offer direct formatting to the
			// container) was ~35% faster than the standard. So if I revert some of these changes and end up at ~35% or  faster than the standard while still being
			// able to directly format to the container from the custom formatters, then I would consider it a win.
			CustomFormatCallBack(parseView, data,
			                     std::move(IteratorContainer(std::move(std::back_insert_iterator<ContainerType>(std::forward<ContainerRef>(container))))));
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
		void (*CustomFormatCallBack)(std::string_view parseView, const void* data, IteratorContainer&& outResult);
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
