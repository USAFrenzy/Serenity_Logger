#include <serenity/MessageDetails/ArgContainer.h>

namespace details = serenity::msg_details;

constexpr std::array<serenity::VType, details::MAX_ARG_COUNT>& details::ArgContainer::ArgStorage() {
	return argContainer;
}

constexpr std::array<details::SpecType, details::MAX_ARG_COUNT>& details::ArgContainer::SpecTypesCaptured() {
	return specContainer;
}

template<typename Iter, typename T> constexpr auto details::ArgContainer::StoreCustomArg(Iter&& iter, T&& value) -> decltype(iter) {
	argContainer[ counter ] =
	std::move(CustomValue(IteratorAccessHelper(std::remove_reference_t<decltype(iter)>(iter)).Container(), std::forward<FwdConstRef<T>>(FwdConstRef<T>(value))));
	return std::move(std::add_rvalue_reference_t<decltype(iter)>(iter));
}

template<typename T> constexpr void details::ArgContainer::StoreNativeArg(T&& value) {
	if constexpr( is_supported_ptr_type_v<T> || std::is_same_v<type<T>, std::string> ) {
			argContainer[ counter ] = std::forward<type<T>>(type<T>(value));
	} else {
			argContainer[ counter ] = std::forward<FwdConstRef<T>>(FwdConstRef<T>(value));
		}
}

template<typename Iter, typename... Args> constexpr auto details::ArgContainer::StoreArgs(Iter&& iter, Args&&... args) -> decltype(iter) {
	(
	[ this ](auto&& arg, auto&& iter) {
		specContainer[ counter ] = GetArgType(std::forward<FwdRef<decltype(arg)>>(FwdRef<decltype(arg)>(arg)));
		if constexpr( is_supported_v<type<decltype(arg)>> ) {
				StoreNativeArg(std::forward<FwdConstRef<decltype(arg)>>(FwdConstRef<decltype(arg)>(arg)));
		} else {
				iter = std::move(StoreCustomArg(std::move(iter), std::forward<FwdConstRef<decltype(arg)>>(FwdConstRef<decltype(arg)>(arg))));
			}
		++counter;
		SE_ASSERT(counter < MAX_ARG_COUNT, "Too Many Arguments Supplied To Formatting Function");
	}(std::forward<FwdConstRef<Args>>(args), std::move(iter)),
	...);
	return std::move(iter);
}
template<typename Iter, typename... Args> constexpr auto details::ArgContainer::CaptureArgs(Iter&& iter, Args&&... args) -> decltype(iter) {
	counter = 0;
	std::memset(specContainer.data(), 0, details::MAX_ARG_COUNT);
	return std::move(StoreArgs(std::move(iter), std::forward<Args>(args)...));
}

constexpr std::string_view details::ArgContainer::string_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<1>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::c_string_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<2>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::string_view_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<3>(&argContainer[ index ]);
}

constexpr int& details::ArgContainer::int_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<4>(&argContainer[ index ]);
}

constexpr unsigned int& details::ArgContainer::uint_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<5>(&argContainer[ index ]);
}

constexpr long long& details::ArgContainer::long_long_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<6>(&argContainer[ index ]);
}

unsigned constexpr long long& details::ArgContainer::u_long_long_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<7>(&argContainer[ index ]);
}

constexpr bool& details::ArgContainer::bool_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<8>(&argContainer[ index ]);
}

constexpr char& details::ArgContainer::char_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<9>(&argContainer[ index ]);
}

constexpr float& details::ArgContainer::float_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<10>(&argContainer[ index ]);
}

constexpr double& details::ArgContainer::double_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<11>(&argContainer[ index ]);
}

constexpr long double& details::ArgContainer::long_double_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<12>(&argContainer[ index ]);
}
constexpr const void* details::ArgContainer::const_void_ptr_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<13>(&argContainer[ index ]);
}

constexpr void* details::ArgContainer::void_ptr_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<14>(&argContainer[ index ]);
}

constexpr serenity::CustomValue& serenity::msg_details::ArgContainer::custom_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	return *std::get_if<15>(&argContainer[ index ]);
}