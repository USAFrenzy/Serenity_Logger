#include <serenity/MessageDetails/ArgContainer.h>

namespace details = serenity::msg_details;

constexpr std::array<serenity::VType, details::MAX_ARG_COUNT>& details::ArgContainer::ArgStorage() {
	return argContainer;
}

constexpr std::array<details::SpecType, details::MAX_ARG_COUNT>& details::ArgContainer::SpecTypesCaptured() {
	return specContainer;
}

template<typename Iter, typename T> constexpr void details::ArgContainer::StoreCustomArg(Iter&& iter, T&& value) {
	using ref               = std::add_lvalue_reference_t<std::remove_cvref_t<T>>;
	using iterRef           = std::add_lvalue_reference_t<std::remove_cvref_t<Iter>>;
	argContainer[ counter ] = std::move(CustomValue(std::forward<ref>(ref(value)), std::forward<iterRef>(iterRef(iter))));
}

template<typename T> constexpr void details::ArgContainer::StoreNativeArg(T&& value) {
	using base_type = std::remove_cvref_t<decltype(value)>;
	using ref       = std::add_lvalue_reference_t<base_type>;
	if constexpr( is_native_ptr_type_v<T> ) {
			argContainer[ counter ] = std::forward<base_type>(base_type(value));
	} else {
			argContainer[ counter ] = std::forward<ref>(ref(value));
		}
}

template<typename Iter, typename... Args> constexpr void details::ArgContainer::StoreArgs(Iter&& iter, Args&&... args) {
	(
	[ = ](auto&& arg) {
		using base_type          = std::remove_cvref_t<decltype(arg)>;
		using ref                = std::add_lvalue_reference_t<base_type>;
		specContainer[ counter ] = GetArgType(std::forward<ref>(ref(arg)));
		if constexpr( is_supported_v<base_type> ) {
				StoreNativeArg(std::forward<ref>(ref(arg)));
		} else {
				StoreCustomArg(std::forward<Iter>(Iter(iter)), std::forward<ref>(ref(arg)));
			}
		if( ++counter > MAX_ARG_INDEX ) {
				std::printf("Warning: Max Argument Count Of  25 Reached - Ignoring Any Further Arguments\n");
				return;
		};
	}(args),
	...);
}
template<typename Iter, typename... Args> constexpr void details::ArgContainer::CaptureArgs(Iter&& iter, Args&&... args) {
	counter = 0;
	std::memset(specContainer.data(), 0, details::MAX_ARG_COUNT);
	StoreArgs(std::forward<Iter>(iter), std::forward<Args>(args)...);
}

constexpr std::string_view details::ArgContainer::string_state(size_t index) {
	return *std::get_if<1>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::c_string_state(size_t index) {
	return *std::get_if<2>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::string_view_state(size_t index) {
	return *std::get_if<3>(&argContainer[ index ]);
}

constexpr int& details::ArgContainer::int_state(size_t index) {
	return *std::get_if<4>(&argContainer[ index ]);
}

constexpr unsigned int& details::ArgContainer::uint_state(size_t index) {
	return *std::get_if<5>(&argContainer[ index ]);
}

constexpr long long& details::ArgContainer::long_long_state(size_t index) {
	return *std::get_if<6>(&argContainer[ index ]);
}

unsigned constexpr long long& details::ArgContainer::u_long_long_state(size_t index) {
	return *std::get_if<7>(&argContainer[ index ]);
}

constexpr bool& details::ArgContainer::bool_state(size_t index) {
	return *std::get_if<8>(&argContainer[ index ]);
}

constexpr char& details::ArgContainer::char_state(size_t index) {
	return *std::get_if<9>(&argContainer[ index ]);
}

constexpr float& details::ArgContainer::float_state(size_t index) {
	return *std::get_if<10>(&argContainer[ index ]);
}

constexpr double& details::ArgContainer::double_state(size_t index) {
	return *std::get_if<11>(&argContainer[ index ]);
}

constexpr long double& details::ArgContainer::long_double_state(size_t index) {
	return *std::get_if<12>(&argContainer[ index ]);
}
constexpr const void* details::ArgContainer::const_void_ptr_state(size_t index) {
	return *std::get_if<13>(&argContainer[ index ]);
}

constexpr void* details::ArgContainer::void_ptr_state(size_t index) {
	return *std::get_if<14>(&argContainer[ index ]);
}

constexpr serenity::CustomValue& serenity::msg_details::ArgContainer::custom_state(size_t index) {
	return *std::get_if<15>(&argContainer[ index ]);
}