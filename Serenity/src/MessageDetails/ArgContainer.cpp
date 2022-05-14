#include <serenity/MessageDetails/ArgContainer.h>
#include <iostream>

namespace serenity::experimental::msg_details {

	const std::array<ArgContainer::LazilySupportedTypes, 24>& ArgContainer::ArgStorage() const {
		return testContainer;
	}

	void ArgContainer::EnableFallbackToStd(bool enable) {
		isStdFallbackEnabled = enable;
	}

	std::string ArgContainer::string_state(size_t index) {
		return *std::get_if<1>(&testContainer[ index ]);
	}

	const char* ArgContainer::c_string_state(size_t index) {
		return *std::get_if<2>(&testContainer[ index ]);
	}

	std::string_view ArgContainer::string_view_state(size_t index) {
		return *std::get_if<3>(&testContainer[ index ]);
	}

	int ArgContainer::int_state(size_t index) {
		return *std::get_if<4>(&testContainer[ index ]);
	}

	unsigned int ArgContainer::uint_state(size_t index) {
		return *std::get_if<5>(&testContainer[ index ]);
	}

	long long ArgContainer::long_long_state(size_t index) {
		return *std::get_if<6>(&testContainer[ index ]);
	}

	unsigned long long ArgContainer::u_long_long_state(size_t index) {
		return *std::get_if<7>(&testContainer[ index ]);
	}

	bool ArgContainer::bool_state(size_t index) {
		return *std::get_if<8>(&testContainer[ index ]);
	}

	char ArgContainer::char_state(size_t index) {
		return *std::get_if<9>(&testContainer[ index ]);
	}

	float ArgContainer::float_state(size_t index) {
		return *std::get_if<10>(&testContainer[ index ]);
	}

	double ArgContainer::double_state(size_t index) {
		return *std::get_if<11>(&testContainer[ index ]);
	}

	long double ArgContainer::long_double_state(size_t index) {
		return *std::get_if<12>(&testContainer[ index ]);
	}
	const void* ArgContainer::const_void_ptr_state(size_t index) {
		return *std::get_if<13>(&testContainer[ index ]);
	}

	void* ArgContainer::void_ptr_state(size_t index) {
		return *std::get_if<14>(&testContainer[ index ]);
	}

	std::array<SpecType, 24>& ArgContainer::SpecTypesCaptured() {
		return testSpecContainer;
	}

}    // namespace serenity::experimental::msg_details