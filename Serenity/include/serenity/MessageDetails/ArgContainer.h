#pragma once

#include <string_view>
#include <array>
#include <functional>
#include <iterator>
#include <memory>
#include <variant>

namespace serenity {
	template<typename T> struct IteratorContainer: std::back_insert_iterator<T>
	{
		using std::back_insert_iterator<T>::container_type;
		constexpr IteratorContainer(std::back_insert_iterator<T>&(Iter)): std::back_insert_iterator<T>(Iter) { }
		constexpr const auto& Container() {
			return this->container;
		}
	};

	template<typename T, typename U> struct is_supported;
	template<typename T, typename... Ts> struct is_supported<T, std::variant<Ts...>>: std::bool_constant<(std::is_same<T, Ts>::value || ...)>
	{
	};
	// NOTE: Would love to have the Format() overloads either return string_views or  directly write into the buffer.
	//               The second option hasn't been tried and seems like a decent option; the first one left garbage values
	//               being returned in Debug mode, although, in Release mode it worked fine.
	struct CustomFormatterBase
	{
		explicit CustomFormatterBase() = default;
		CustomFormatterBase(const CustomFormatterBase&) { }
		CustomFormatterBase& operator=(const CustomFormatterBase&) = default;
		CustomFormatterBase(CustomFormatterBase&&)                 = default;
		CustomFormatterBase& operator=(CustomFormatterBase&&)      = default;
		virtual ~CustomFormatterBase()                             = default;

		template<typename T> constexpr auto Format(T&&) { }
		virtual constexpr void Parse(std::string_view) = 0;
	};

	template<typename T> struct CustomFormatter: public CustomFormatterBase
	{
		explicit CustomFormatter(): CustomFormatterBase() { }
		CustomFormatter(const CustomFormatter&) { }
		CustomFormatter& operator=(const CustomFormatter&) = default;
		CustomFormatter(CustomFormatter&& o)               = default;
		CustomFormatter& operator=(CustomFormatter&& o)    = default;
		~CustomFormatter()                                 = default;

		constexpr void Parse(std::string_view) override { }
		template<typename T> constexpr auto Format(T&&) { }
	};

	template<typename T> struct has_formatter: std::bool_constant<std::is_default_constructible_v<CustomFormatter<T>>>
	{
	};

	struct CustomValue
	{
		template<typename T>
		explicit CustomValue(T&& value)
			: data(std::addressof(value)), CustomFormatCallBack([](std::string_view parseView, const void* ptr) -> std::string {
				  using UnqualifiedType = std::remove_cvref_t<T>;
				  using QualifiedType   = std::add_const_t<UnqualifiedType>;
				  auto formatter { CustomFormatter<UnqualifiedType> {} };
				  formatter.Parse(parseView);
				  return formatter.Format(*static_cast<QualifiedType*>(ptr));
			  }) { }

		template<typename T> constexpr void FormatCallBack(std::string_view parseView, std::back_insert_iterator<T>&& iter) {
			std::string sv { CustomFormatCallBack(parseView, data) };
			auto& cont { *IteratorContainer(iter).Container() };
			if constexpr( std::is_same_v<T, std::string> ) {
					cont.append(sv.data(), sv.size());
			} else if constexpr( std::is_same_v<T, std::vector<typename T::value_type>> ) {
					cont.insert(cont.end(), sv.begin(), sv.end());
					if( sv.back() != '\0' ) {
							cont.push_back('\0');
					}
			} else {
					std::copy(sv.begin(), sv.end(), iter);
				}
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
		std::string (*CustomFormatCallBack)(std::string_view parseView, const void* data);
		// std::function<std::string(std::string_view parseView, const void* data)> CustomFormatCallBack;
	};

	// clang-format off
	using VType = std::variant<std::monostate, std::string, const char*, std::string_view, int, unsigned int,
		long long, unsigned long long, bool, char, float, double, long double, const void*, void*, CustomValue>;
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
				static_assert(is_formattable<base_type>::value, "A Template Specialization Must Exist For A Custom Type Argument.\n\t"
				                                                "For Serenity, This Can Be Done By Specializing The CustomFormatter Template For Your Type And "
				                                                "Implementing The Parse() And Format() Functions.");
				return std::forward<SpecType>(CustomType);
			}
	}
#include <serenity/MessageDetails/ArgContainerImpl.h>
}    // namespace serenity::msg_details
