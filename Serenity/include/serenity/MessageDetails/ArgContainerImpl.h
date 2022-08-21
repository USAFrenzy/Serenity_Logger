#pragma once

#include <serenity/MessageDetails/ArgContainer.h>
#include <serenity/Utilities/UtfHelper.h>

namespace details = serenity::msg_details;

constexpr std::array<serenity::VType, details::MAX_ARG_COUNT>& details::ArgContainer::ArgStorage() {
	return argContainer;
}

constexpr std::array<details::SpecType, details::MAX_ARG_COUNT>& details::ArgContainer::SpecTypesCaptured() {
	return specContainer;
}

template<typename Iter, typename T> constexpr auto details::ArgContainer::StoreCustomArg(Iter&& iter, T&& value) -> decltype(iter) {
	argContainer[ counter ] =
	CustomValue(IteratorAccessHelper(std::remove_reference_t<decltype(iter)>(iter)).Container(), std::forward<FwdConstRef<T>>(FwdConstRef<T>(value)));
	return std::move(std::add_rvalue_reference_t<decltype(iter)>(iter));
}

template<typename T> constexpr void details::ArgContainer::StoreNativeArg(T&& value) {
	if constexpr( is_supported_ptr_type_v<T> || std::is_same_v<type<T>, std::string> ) {
			if constexpr( std::is_same_v<type<T>, std::tm*> ) {
					argContainer[ counter ] = std::forward<FwdRef<T>>(FwdRef<T>(*value));
			} else {
					argContainer[ counter ] = std::forward<type<T>>(type<T>(value));
				}
	} else {
			argContainer[ counter ] = std::forward<FwdRef<T>>(FwdRef<T>(value));
		}
}

template<typename Iter, typename... Args> constexpr auto details::ArgContainer::StoreArgs(Iter&& iter, Args&&... args) -> decltype(iter) {
	(
	[ this ](auto&& arg, auto&& iter) {
		using ArgType = decltype(arg);
		using namespace utf_helper;
		using enum bom_type;

		// handle string types as a special case
		if constexpr( utf_constraints::is_string_v<ArgType> || utf_constraints::is_string_view_v<ArgType> ) {
				switch( DetectBom(std::forward<ArgType>(arg)) ) {
						case utf8_bom: [[fallthrough]];
						case utf8_no_bom:
							{
								if constexpr( std::is_same_v<typename type<ArgType>::value_type, unsigned char> && std::is_signed_v<char> ) {
										std::string tmp;
										tmp.reserve(arg.size());
										for( auto& ch: arg ) {
												tmp += static_cast<char>(ch);
											}
										if constexpr( utf_constraints::is_string_v<ArgType> ) {
												argContainer[ counter ]  = std::move(tmp);
												specContainer[ counter ] = SpecType::StringType;
										} else {
												argContainer[ counter ]  = std::string_view(std::move(tmp));
												specContainer[ counter ] = SpecType::StringViewType;
											}
								} else {
										using remove_ref        = type<decltype(arg)>;
										argContainer[ counter ] = remove_ref(arg);
										if constexpr( utf_constraints::is_string_v<ArgType> ) {
												specContainer[ counter ] = SpecType::StringType;
										} else {
												specContainer[ counter ] = SpecType::StringViewType;
											}
									}
								break;
							}
						case utf16LE_bom: [[fallthrough]];
						case utf16BE_bom: [[fallthrough]];
						case utf16_no_bom:
							{
								std::string tmp;
								tmp.reserve(ReserveLengthForU8(std::forward<ArgType>(arg)));
								U16ToU8(std::forward<ArgType>(arg), tmp);
								if constexpr( utf_constraints::is_string_v<ArgType> ) {
										argContainer[ counter ]  = std::move(tmp);
										specContainer[ counter ] = SpecType::StringType;
								} else {
										argContainer[ counter ]  = std::string_view(std::move(tmp));
										specContainer[ counter ] = SpecType::StringViewType;
									}
								break;
							}
						case utf32LE_bom: [[fallthrough]];
						case utf32BE_bom: [[fallthrough]];
						case utf32_no_bom:
							{
								std::string tmp;
								tmp.reserve(ReserveLengthForU8(std::forward<ArgType>(arg)));
								U32ToU8(std::forward<ArgType>(arg), tmp);
								if constexpr( utf_constraints::is_string_v<ArgType> ) {
										argContainer[ counter ]  = std::move(tmp);
										specContainer[ counter ] = SpecType::StringType;
								} else {
										argContainer[ counter ]  = std::string_view(std::move(tmp));
										specContainer[ counter ] = SpecType::StringViewType;
									}
								break;
							}
						default: SE_ASSERT(false, "Unknown Encoding Detected"); break;
					}
				++counter;
				SE_ASSERT(counter < MAX_ARG_COUNT, "Too Many Arguments Supplied To Formatting Function");
		} else {
				specContainer[ counter ] = GetArgType(std::forward<FwdRef<ArgType>>(FwdRef<ArgType>(arg)));
				if constexpr( is_supported_v<type<ArgType>> ) {
						StoreNativeArg(std::forward<FwdRef<ArgType>>(FwdRef<ArgType>(arg)));
				} else {
						iter = std::move(StoreCustomArg(std::move(iter), std::forward<FwdRef<ArgType>>(FwdRef<ArgType>(arg))));
					}
				++counter;
				SE_ASSERT(counter < MAX_ARG_COUNT, "Too Many Arguments Supplied To Formatting Function");
			}
	}(std::forward<FwdRef<Args>>(FwdRef<Args>(args)), std::move(iter)),
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
	SE_ASSERT(std::holds_alternative<std::string>(argContainer[ index ]), "Error Retrieving std::string: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<1>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::c_string_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<const char*>(argContainer[ index ]), "Error Retrieving const char*: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<2>(&argContainer[ index ]);
}

constexpr std::string_view details::ArgContainer::string_view_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	// clang-format off
	SE_ASSERT(std::holds_alternative<std::string_view>(argContainer[ index ]),
		"Error Retrieving std::string_view: Variant At Index Provided Doesn't Contain This Type.");
	// clang-format on
	return *std::get_if<3>(&argContainer[ index ]);
}

constexpr int& details::ArgContainer::int_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<int>(argContainer[ index ]), "Error Retrieving int: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<4>(&argContainer[ index ]);
}

constexpr unsigned int& details::ArgContainer::uint_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<unsigned int>(argContainer[ index ]), "Error Retrieving unsigned int: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<5>(&argContainer[ index ]);
}

constexpr long long& details::ArgContainer::long_long_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<long long>(argContainer[ index ]), "Error Retrieving long long: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<6>(&argContainer[ index ]);
}

unsigned constexpr long long& details::ArgContainer::u_long_long_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	// clang-format off
	SE_ASSERT(std::holds_alternative<unsigned long long>(argContainer[ index ]), 
		"Error Retrieving unsigned long long: Variant At Index Provided Doesn't Contain This Type.");
	// clang-format on
	return *std::get_if<7>(&argContainer[ index ]);
}

constexpr bool& details::ArgContainer::bool_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<bool>(argContainer[ index ]), "Error Retrieving bool: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<8>(&argContainer[ index ]);
}

constexpr char& details::ArgContainer::char_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<char>(argContainer[ index ]), "Error Retrieving char: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<9>(&argContainer[ index ]);
}

constexpr float& details::ArgContainer::float_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<float>(argContainer[ index ]), "Error Retrieving float: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<10>(&argContainer[ index ]);
}

constexpr double& details::ArgContainer::double_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<double>(argContainer[ index ]), "Error Retrieving double: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<11>(&argContainer[ index ]);
}

constexpr long double& details::ArgContainer::long_double_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<long double>(argContainer[ index ]), "Error Retrieving long double: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<12>(&argContainer[ index ]);
}
constexpr const void* details::ArgContainer::const_void_ptr_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<const void*>(argContainer[ index ]), "Error Retrieving const void*: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<13>(&argContainer[ index ]);
}

constexpr void* details::ArgContainer::void_ptr_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<void*>(argContainer[ index ]), "Error Retrieving Void*: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<14>(&argContainer[ index ]);
}

constexpr std::tm& serenity::msg_details::ArgContainer::c_time_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	SE_ASSERT(std::holds_alternative<std::tm>(argContainer[ index ]), "Error Retrieving std::tm: Variant At Index Provided Doesn't Contain This Type.");
	return *std::get_if<15>(&argContainer[ index ]);
}

constexpr serenity::CustomValue& serenity::msg_details::ArgContainer::custom_state(size_t index) {
	SE_ASSERT(index <= MAX_ARG_INDEX, "Error Retrieving Stored Value - Index Is Out Of Bounds");
	// clang-format off
	SE_ASSERT(std::holds_alternative<serenity::CustomValue>(argContainer[ index ]),
		"Error Retrieving custom value type: Variant At Index Provided Doesn't Contain This Type.");
	// clang-format on
	return *std::get_if<16>(&argContainer[ index ]);
}