#include <serenity/MessageDetails/ArgContainer.h>

namespace details = serenity::msg_details;

static constexpr std::string_view details::SpecTypeString(SpecType type) {
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
constexpr size_t details::ArgContainer::CurrentSize() {
	return counter;
}

template<typename T> static constexpr details::SpecType GetArgType(T&& val) {
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
			// TODO: Write the logic for and include the build options for using either <format> or libfmt instead of the built-in formatter
			static_assert(false, "Type Not Natively Supported. Please Enable USE_STD_FORMAT Or USE_FMTLIB  Instead.");
		}
}

constexpr std::array<details::ArgContainer::VType, details::MAX_ARG_COUNT>& details::ArgContainer::ArgStorage() {
	return argContainer;
}

constexpr std::array<details::SpecType, details::MAX_ARG_COUNT>& details::ArgContainer::SpecTypesCaptured() {
	return specContainer;
}

template<typename... Args> constexpr void details::ArgContainer::StoreArgs(Args&&... args) {
	(
	[ = ](auto&& arg) {
		using base_type          = std::remove_cvref_t<decltype(arg)>;
		using ref                = std::add_lvalue_reference_t<base_type>;
		argContainer[ counter ]  = std::forward<ref>(ref(arg));
		specContainer[ counter ] = GetArgType(arg);
		if( ++counter > MAX_ARG_INDEX ) {
				std::printf("Warning: Max Argument Count Of  25 Reached - Ignoring Any Further Arguments\n");
				return;
		}
	}(args),
	...);
}
template<typename... Args> constexpr void details::ArgContainer::CaptureArgs(Args&&... args) {
	counter = 0;
	StoreArgs(std::forward<Args>(args)...);
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