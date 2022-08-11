#pragma once

#include <concepts>
#include <string>
#include <vector>

namespace utf_helper {
	/***************************************** convenience usage for template constraints for utf_helper *****************************************/

	template<typename T> using type = std::remove_cvref_t<T>;
	template<typename T> using Ref  = std::add_lvalue_reference_t<type<T>>;

	// support is given for all but char8_t until that's ironed out by the standard more
	template<typename T> struct is_char_type;
	template<typename T>
	struct is_char_type: std::bool_constant<std::is_same_v<T, char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, wchar_t> ||
	                                        std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>>
	{
	};
	template<typename T> inline constexpr bool is_char_type_v = is_char_type<T>::value;

	// string view type constraints
	template<typename T> using StringView                     = std::basic_string_view<type<T>>;

	template<typename T> struct is_basic_string_view;
	template<typename T>
	struct is_basic_string_view: std::bool_constant<std::is_same_v<type<T>, StringView<char>> || std::is_same_v<type<T>, StringView<unsigned char>>>
	{
	};
	template<typename T> inline constexpr bool is_basic_string_view_v = is_basic_string_view<T>::value;

	template<typename T> struct is_u16_type_string_view;
	template<typename T>
	struct is_u16_type_string_view: std::bool_constant<std::is_same_v<type<T>, StringView<wchar_t>> || std::is_same_v<type<T>, StringView<char16_t>>>
	{
	};
	template<typename T> inline constexpr bool is_u16_type_string_view_v = is_u16_type_string_view<T>::value;

	template<typename T> struct is_string_view;
	template<typename T>
	struct is_string_view: std::bool_constant<is_basic_string_view_v<type<T>> || is_u16_type_string_view_v<type<T>> || std::is_same_v<type<T>, StringView<char32_t>>>
	{
	};
	template<typename T> inline constexpr bool is_string_view_v = is_string_view<T>::value;

	// string type constraints
	template<typename T> using String                           = std::basic_string<type<T>>;

	template<typename T> struct is_basic_string;
	template<typename T> struct is_basic_string: std::bool_constant<std::is_same_v<type<T>, String<char>> || std::is_same_v<type<T>, String<unsigned char>>>
	{
	};
	template<typename T> inline constexpr bool is_basic_string_v = is_basic_string<T>::value;

	template<typename T> struct is_u16_type_string;
	template<typename T> struct is_u16_type_string: std::bool_constant<std::is_same_v<type<T>, String<wchar_t>> || std::is_same_v<type<T>, String<char16_t>>>
	{
	};
	template<typename T> inline constexpr bool is_u16_type_string_v = is_u16_type_string<T>::value;

	template<typename T> struct is_string;
	template<typename T> struct is_string: std::bool_constant<is_basic_string_v<T> || is_u16_type_string_v<T> || std::is_same_v<type<T>, String<char32_t>>>
	{
	};
	template<typename T> inline constexpr bool is_string_v = is_string<T>::value;

	// vector type constraints
	template<typename T> struct is_basic_char_vector;
	template<typename T>
	struct is_basic_char_vector: std::bool_constant<std::is_same_v<type<T>, std::vector<unsigned char>> || std::is_same_v<type<T>, std::vector<char>>>
	{
	};
	template<typename T> static inline constexpr bool is_basic_char_vector_v = is_basic_char_vector<T>::value;

	template<typename T> struct is_char32_vector;
	template<typename T> struct is_char32_vector: std::bool_constant<std::is_same_v<type<T>, std::vector<char32_t>>>
	{
	};
	template<typename T> static inline constexpr bool is_char32_vector_v = is_char32_vector<T>::value;

	template<typename T> struct is_wide_vector;
	template<typename T> struct is_wide_vector: std::bool_constant<std::is_same_v<type<T>, std::vector<char16_t>> || std::is_same_v<type<T>, std::vector<wchar_t>>>
	{
	};
	template<typename T> static inline constexpr bool is_wide_vector_v = is_wide_vector<T>::value;

	// concept constraints that use the above individual constraints
	template<typename StringishType>
	concept ConvertibleToSuppSV = requires {
		// I don't support char8_t here so I need to check that char type is supported instead and then make sure that a string_view is constructible
		// in the case that a user passes a char-like type that aliases a supported char type but may or may not be constructible as a string view
		is_char_type_v<typename type<StringishType>::value_type> &&
		(is_string_view_v<StringishType> ||
		 std::is_convertible_v<std::basic_string<typename type<StringishType>::value_type>, std::basic_string_view<typename type<StringishType>::value_type>>);
	};

	template<typename Source>
	concept IsSupportedU16Source = requires {
		is_u16_type_string_v<Source> || is_u16_type_string_view_v<Source> || is_wide_vector_v<Source>;
	};

	template<typename Buffer>
	concept IsSupportedU16Container = requires {
		is_wide_vector_v<Buffer> || std::is_same_v<type<Buffer>, std::wstring> || std::is_same_v<type<Buffer>, std::u16string>;
	};

	template<typename Source>
	concept IsSupportedU32Source = requires {
		std::is_same_v<type<Source>, std::u32string> || std::is_same_v<type<Source>, std::u32string_view> || std::is_same_v<type<Source>, std::vector<char32_t>>;
	};

	template<typename Buffer>
	concept IsSupportedU32Container = requires {
		is_char32_vector_v<Buffer> || std::is_same_v<type<Buffer>, std::u32string>;
	};

	template<typename Buffer>
	concept IsSupportedU8Container = requires {
		is_basic_char_vector_v<Buffer> || is_basic_string_v<Buffer>;
	};

	template<typename Buffer>
	concept IsSupportedUContainer = requires {
		IsSupportedU8Container<Buffer> || IsSupportedU16Container<Buffer> || IsSupportedU32Container<Buffer>;
	};

	// Stick to using wchar_t here and deal with UTF-16/UTF-32 decoding to UTF-8 encoding. If the standard changes to
	// require specializations for char16_t & char32_t for stringstreams with time formatting, then this SHOULD also change
	using se_wchar   = wchar_t;
	using se_wstring = std::basic_string<se_wchar>;

	static constexpr bool IsLittleEndian() {
		constexpr short int val = 0x0001;
		return reinterpret_cast<const char*>(&val)[ 0 ] ? true : false;
	}

	template<typename T> static constexpr auto ConvertToSv(T&& v) requires ConvertibleToSuppSV<T> {
		using type = std::basic_string_view<typename type<T>::value_type>;
		return std::forward<type>(type(v));
	}

	// As more utility functions are created, need to update this to reflect varying codepoint sizes. Right now it works as-is due to the
	// fact that the conversions currently happening are from a 32 bit value with a fixed width, but when utf-8 -> any other encoding,
	// utf-16 -> utf-8 , utf-32 -> utf-16,  or any other conversions occur. this won't be the case.
	template<typename SVType> static constexpr size_t ReserveLengthForU8Impl(SVType sv) requires is_string_view_v<SVType> {
		using Type = typename type<SVType>::value_type;
		size_t reserveSize {};
		int pos { -1 };
		auto size { sv.size() };

		if constexpr( std::is_same_v<Type, char32_t> ) {
				for( ;; ) {
						if( ++pos >= size ) return reserveSize;
						if( auto& ch { sv[ pos ] }; ch < 0x80 ) {
								++reserveSize;
								continue;
						} else if( ch <= 0x7FF ) {
								reserveSize += 2;
								continue;
						} else if( ch <= 0xFFFF ) {
								reserveSize += 3;
								continue;
						} else if( ch <= 0x10FFFF ) {
								reserveSize += 4;
								continue;
						} else {
								reserveSize += 2;    // this will be used to add the replacement character bytes
							}
					}
		} else {
				SE_ASSERT(false, "Reserving Size For Any Other Encoding Besides UTF-32LE Is Currently Unsupported At The Moment");
			}
	}

	template<typename StringishType>
	requires ConvertibleToSuppSV<StringishType>
	static constexpr size_t ReserveLengthForU8(StringishType&& s) {
		using valType = type<decltype(s)>;
		if constexpr( is_string_view_v<StringishType> ) {
				return ReserveLengthForU8Impl(std::forward<valType>(s));
		} else {
				return ReserveLengthForU8Impl(ConvertToSv(std::forward<Ref<valType>>(s)));
			}
	}

	template<typename Buffer, typename Char>
	requires IsSupportedUContainer<Buffer> && is_char_type_v<Char>
	static constexpr void WriteChToUContainer(Buffer&& buff, Char&& ch) {
		if constexpr( is_string_v<Buffer> ) {
				if constexpr( std::is_rvalue_reference_v<Char> ) {
						buff += std::move(ch);
				} else {
						buff += ch;
					}
		} else {
				if constexpr( std::is_rvalue_reference_v<Char> ) {
						buff.emplace_back(std::move(ch));
				} else {
						buff.emplace_back(ch);
					}
			}
	}

	template<typename StringView, typename Buffer, typename Pos = size_t>
	requires is_u16_type_string_view_v<StringView> && IsSupportedU16Container<Buffer>
	static constexpr void U16ToU32Impl(StringView wstr, Buffer&& buff, Pos&& startingPos) {
		SE_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                            "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");

		using BuffRef = Ref<Buffer>;
		int pos { startingPos == 0 ? -1 : static_cast<int>(startingPos) - 1 };
		auto size { wstr.size() };
		if( buff.capacity() < size ) buff.reserve(buff.size() + size);

		if constexpr( sizeof(se_wchar) == 2 ) {
				for( ;; ) {
						if( ++pos >= size ) {
								if constexpr( std::is_lvalue_reference_v<Pos> ) {
										startingPos += buff.size();
								}
								return;
						}
						if( wstr[ pos ] < 0xD800 ) {
								WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<char32_t>(wstr[ pos ]));
								continue;
						} else {
								// check for valid high byte in pair, can skip checking for 0xD800, as that was already checked above
								if( wstr[ pos ] > 0xDBFF ) {
										SE_ASSERT(false, "Illegal Surrogate Byte In High Byte");
										WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<char32_t>(0xFFDD));
										continue;
								}
								if( ++pos >= size ) {
										SE_ASSERT(false, "Incomplete Surrogate Pairing When Decoding UTF-16 Codepoint");
										WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<char32_t>(0xFFDD));
										continue;    // continue instead of return here in case Pos type is a lvalue ref
								}
								// check for valid low byte in pair
								if( wstr[ pos ] < 0xDC00 || wstr[ pos ] > 0xDFFF ) {
										SE_ASSERT(false, "Illegal Surrogate Byte In Low Byte");
										WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<char32_t>(0xFFDD));
										continue;
								}
								char32_t cp { 0x10000 };
								cp += (wstr[ --pos ] & 0x03FF) << 10;
								cp += (wstr[ ++pos ] & 0x03FF);
								WriteChToUContainer(std::forward<BuffRef>(buff), std::move(cp));
								continue;
							}
					}
		} else {    // se_wchar/wchar_t is 4 bytes long so just cast to char32_t instead
				for( ;; ) {
						if( ++pos >= size ) {
								if constexpr( std::is_lvalue_reference_v<Pos> ) {
										startingPos += buff.size();
								}
								return;
						}
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<char32_t>(wstr[ pos ]));
					}
			}
	}

	// Extremely basic and rudimentary validation
	template<typename StringView>
	requires is_basic_string_view_v<StringView>
	static constexpr bool IsValidU8(StringView sv) {
		auto size { sv.size() };
		int pos { -1 };
		auto CheckTrailingByte = [](unsigned char byte) {
			// valid trailing byte lays between 1000 0000 and 1011 1111
			return (byte >= 0x80 && byte <= 0xBF) ? true : false;
		};
		for( ;; ) {
				if( ++pos >= size ) return true;    // validated all bytes in string sequence
				auto& byte { sv[ pos ] };
				if( byte < 0x80 ) continue;    // no additional checks need to happen on a one byte sequence
				/* For the rest, check the appropriate number of trailing bytes for proper trailing signatures */
				int bytesToCheck { 0 };
				if( byte >= 0xC0 && byte <= 0xDF ) {
						bytesToCheck = 1;
				} else if( byte >= 0xE0 && byte <= 0xEF ) {
						bytesToCheck = 2;
				} else if( byte >= 0xF0 && byte <= 0xF7 ) {
						bytesToCheck = 3;
				} else {
						// As of Unicode Version 14.0 (2021Sep14), there are only 144,697 characters in use, including emojis, so while
						// 5-byte and 6-byte sequences are technically supported, they are not defined so return false here. Only time
						// this really would need to be updated is once the Unicode Version dictates more than 1,114,111 characters in
						// use as that number is the max value a 4-byte sequence can map to.
						return false;
					}
				for( ; bytesToCheck > 0; --bytesToCheck ) {
						if( !CheckTrailingByte(sv[ ++pos ]) ) return false;
					}
			}
	}

	template<typename Buffer, typename Pos = size_t>
	requires IsSupportedU32Container<Buffer>
	static constexpr void U32ToU8Impl(std::u32string_view sv, Buffer&& buff, Pos&& startingPos) {
		int pos { startingPos == 0 ? -1 : static_cast<int>(startingPos) - 1 };
		auto size { sv.size() };
		buff.reserve(buff.size() + ReserveLengthForU8(sv));
		using ValType = typename type<Buffer>::value_type;
		using BuffRef = Ref<Buffer>;
		for( ;; ) {
				if( ++pos >= size ) {
						SE_ASSERT(IsValidU8(std::basic_string_view<ValType>(buff)), "Invalid Code Point Detected In UTF-8 Byte Sequence");
						if constexpr( std::is_lvalue_reference_v<Pos> ) {
								startingPos += buff.size();
						}
						return;
				}
				if( auto& ch { sv[ pos ] }; ch < 0x80 ) {
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(ch));
						continue;
				} else if( ch <= 0x7FF ) {
						// handle encoding to 2 byte utf-8 sequence here
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0xC0 | (ch >> 6)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | (ch & 0x3F)));
						continue;
				} else if( ch <= 0xFFFF ) {
						// handle encoding to 3 byte utf-8 sequence here
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0xE0 | (ch >> 12)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | ((ch >> 6) & 0x3F)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | (ch & 0x3F)));
						continue;
				} else if( ch <= 0x10FFFF ) {
						// handle encoding to 4 byte utf-8 sequence here
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0xF0 | (ch >> 18)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | ((ch >> 12) & 0x3F)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | ((ch >> 6) & 0x3F)));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0x80 | (ch & 0x3F)));
						continue;
				} else {
						// 5-byte and 6-byte sequences are apparently supported for the long-term but are not defined, therefore, instead of encoding these
						// cases to utf-8, use the repacement character instead and in debug builds, raise an error when the 0xFF byte is detected in validation.
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0xFF));
						WriteChToUContainer(std::forward<BuffRef>(buff), static_cast<ValType>(0xDD));
						continue;
					}
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires IsSupportedU16Source<Source> && IsSupportedU32Container<Buffer>
	static constexpr void U16ToU32(Source wstr, Buffer&& buff, Pos&& startingPos = 0) {
		using ValType    = typename type<Source>::value_type;
		using StringView = std::basic_string_view<ValType>;
		if constexpr( is_u16_type_string_view_v<Source> ) {
				U16ToU32mpl(std::forward<Source>(wstr), std::forward<Buffer>(buff), startingPos);
		} else {
				U16ToU32Impl(StringView(std::begin(wstr), std::end(wstr)), std::forward<Buffer>(buff), startingPos);
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires IsSupportedU32Source<Source> && IsSupportedU8Container<Buffer>
	static constexpr void U32ToU8(Source&& sv, Buffer&& buff, Pos&& startingPos = 0) {
		if constexpr( is_string_view_v<Source> ) {
				U32ToU8Impl(std::forward<Source>(sv), std::forward<Buffer>(buff), startingPos);
		} else {
				U32ToU8Impl(std::u32string_view(std::begin(sv), std::end(sv)), std::forward<Buffer>(buff), startingPos);
			}
	}
}    // namespace utf_helper
