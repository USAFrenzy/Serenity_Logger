#pragma once

#include <concepts>
#include <string>
#include <vector>

namespace utf_helper {

#ifdef _DEBUG
	#ifndef UTF_ASSERT
		#define UTF_ASSERT(condition, message)                                                                                                                      \
			if( !(condition) ) {                                                                                                                                    \
					fprintf(stderr, "Assertion Failed: (%s) |File: %s | Line: %i\nMessage:%s\n", #condition, __FILE__, __LINE__, message);                          \
					abort();                                                                                                                                        \
			}
	#endif
#else
	#ifndef UTF_ASSERT
		#define UTF_ASSERT(condition, message) void(0)
	#endif
#endif

	// Stick to using wchar_t here and deal with UTF-16/UTF-32 decoding to UTF-8 encoding. If the standard changes to
	// require specializations for char16_t & char32_t for stringstreams with time formatting, then this SHOULD also change
	using se_wchar                  = wchar_t;
	using se_wstring                = std::basic_string<se_wchar>;
	template<typename T> using Type = std::remove_cvref_t<T>;
	template<typename T> using Ref  = std::add_lvalue_reference_t<Type<T>>;

	namespace utf_boms {
		constexpr int UTF8_BOM_1 { 0xEF };
		constexpr int UTF8_BOM_2 { 0xBB };
		constexpr int UTF8_BOM_3 { 0xBF };
		constexpr int UTF16_FULL_BOM_LE { 0xFFFE };
		constexpr int UTF16_FULL_BOM_BE { 0xFEFF };
		constexpr unsigned int UTF32_FULL_BOM_LE { 0xFFFE0000 };
		constexpr unsigned int UTF32_FULL_BOM_BE { 0x0000FEFF };
	}    // namespace utf_boms

	namespace utf_bounds {
		constexpr int UTF_MAX_CODEPOINT { 0x10FFFF };

		constexpr int UTF16_SURROGATE_CLAMP { 0x10000 };
		constexpr int UTF16_HIGH_SURROGATE_MIN { 0xD800 };
		constexpr int UTF16_HIGH_SURROGATE_MAX { 0xDBFF };
		constexpr int UTF16_LOW_SURROGATE_MIN { 0xDC00 };
		constexpr int UTF16_LOW_SURROGATE_MAX { 0xDFFF };
		constexpr int WIDE_ENCODING_TO_UTF8_2 { 0x7FF };
		constexpr int WIDE_ENCODING_TO_UTF8_3 { 0xFFFF };
		constexpr int WIDE_ENCODING_TO_UTF8_4 { 0x10FFFF };
		constexpr int REPLACEMENT_CHARACTER { 0xFFDD };

		constexpr int UTF8_MAX_1 { 0x7F };
		constexpr int UTF8_MIN_2 { 0xC0 };
		constexpr int UTF8_MAX_2 { 0xDF };
		constexpr int UTF8_MIN_3 { 0xE0 };
		constexpr int UTF8_MAX_3 { 0xEF };
		constexpr int UTF8_MIN_4 { 0xF0 };
		constexpr int UTF8_MAX_4 { 0xF7 };
		constexpr int UTF8_MIN_TRAILING { 0x80 };
		constexpr int UTF8_MAX_TRAILING { 0xBF };
		constexpr int UTF8_INVALID_BYTE_1 { 0xFE };
		constexpr int UTF8_INVALID_BYTE_2 { 0xFF };
		constexpr int UTF8_REPLACEMENT_CHARACTER_BYTE_1 { 0xFF };
		constexpr int UTF8_REPLACEMENT_CHARACTER_BYTE_2 { 0xDD };
	}    // namespace utf_bounds

	namespace utf_masks {
		constexpr int UTF8_LEAD_MASK_SEQUENCE_1 { 0xC0 };
		constexpr int UTF8_LEAD_MASK_SEQUENCE_2 { 0xE0 };
		constexpr int UTF8_LEAD_MASK_SEQUENCE_3 { 0xF0 };
		constexpr int UTF8_LEAD_MASK_SEQUENCE_4 { 0xF8 };
		constexpr int UTF8_TRAIL_BASE { 0x80 };
		constexpr int UTF_NIBBLE_3 { 0x07 };
		constexpr int UTF_NIBBLE_4 { 0x0F };
		constexpr int UTF_NIBBLE_5 { 0x1F };
		constexpr int UTF_NIBBLE_6 { 0x3F };
		constexpr int UTF_NIBBLE_7 { 0x7F };
		constexpr int UTF_NIBBLE_10 { 0x03FF };
	}    // namespace utf_masks

	namespace utf_constraints {
		/***************************************** convenience usage for template constraints for utf_helper *****************************************/

		// support is given for all but char8_t until that's ironed out by the standard more
		template<typename T> struct is_char_type;
		template<typename T>
		struct is_char_type: std::bool_constant<std::is_same_v<Type<T>, char> || std::is_same_v<Type<T>, unsigned char> || std::is_same_v<Type<T>, wchar_t> ||
		                                        std::is_same_v<Type<T>, char16_t> || std::is_same_v<Type<T>, char32_t>>
		{
		};
		template<typename T> inline constexpr bool is_char_type_v = is_char_type<T>::value;

		// string view Type constraints
		template<typename T> using StringView                     = std::basic_string_view<Type<T>>;

		template<typename T> struct is_basic_string_view;
		template<typename T>
		struct is_basic_string_view: std::bool_constant<std::is_same_v<Type<T>, StringView<char>> || std::is_same_v<Type<T>, StringView<unsigned char>>>
		{
		};
		template<typename T> inline constexpr bool is_basic_string_view_v = is_basic_string_view<T>::value;

		template<typename T> struct is_u16_type_string_view;
		template<typename T>
		struct is_u16_type_string_view
			: std::bool_constant<(std::is_same_v<Type<T>, StringView<wchar_t>> && sizeof(wchar_t) == 2) || std::is_same_v<Type<T>, StringView<char16_t>>>
		{
		};
		template<typename T> inline constexpr bool is_u16_type_string_view_v = is_u16_type_string_view<T>::value;

		template<typename T> struct is_u32_type_string_view;
		template<typename T>
		struct is_u32_type_string_view
			: std::bool_constant<(std::is_same_v<Type<T>, StringView<wchar_t>> && sizeof(wchar_t) == 4) || std::is_same_v<Type<T>, StringView<char32_t>>>
		{
		};
		template<typename T> inline constexpr bool is_u32_type_string_view_v = is_u32_type_string_view<T>::value;

		template<typename T> struct is_string_view;
		template<typename T>
		struct is_string_view: std::bool_constant<is_basic_string_view_v<Type<T>> || is_u16_type_string_view_v<Type<T>> || is_u32_type_string_view_v<Type<T>>>
		{
		};
		template<typename T> inline constexpr bool is_string_view_v = is_string_view<T>::value;

		// string Type constraints
		template<typename T> using String                           = std::basic_string<Type<T>>;

		template<typename T> struct is_basic_string;
		template<typename T> struct is_basic_string: std::bool_constant<std::is_same_v<Type<T>, String<char>> || std::is_same_v<Type<T>, String<unsigned char>>>
		{
		};
		template<typename T> inline constexpr bool is_basic_string_v = is_basic_string<T>::value;

		template<typename T> struct is_u16_type_string;
		template<typename T>
		struct is_u16_type_string: std::bool_constant<(std::is_same_v<Type<T>, String<wchar_t>> && sizeof(wchar_t) == 2) || std::is_same_v<Type<T>, String<char16_t>>>
		{
		};
		template<typename T> inline constexpr bool is_u16_type_string_v = is_u16_type_string<T>::value;

		template<typename T> struct is_u32_type_string;
		template<typename T>
		struct is_u32_type_string: std::bool_constant<(std::is_same_v<Type<T>, String<wchar_t>> && sizeof(wchar_t) == 4) || std::is_same_v<Type<T>, String<char32_t>>>
		{
		};
		template<typename T> inline constexpr bool is_u32_type_string_v = is_u32_type_string<T>::value;

		template<typename T> struct is_string;
		template<typename T> struct is_string: std::bool_constant<is_basic_string_v<T> || is_u16_type_string_v<T> || is_u32_type_string_v<T>>
		{
		};
		template<typename T> inline constexpr bool is_string_v = is_string<T>::value;

		// vector Type constraints
		template<typename T> struct is_basic_char_vector;
		template<typename T>
		struct is_basic_char_vector: std::bool_constant<std::is_same_v<Type<T>, std::vector<unsigned char>> || std::is_same_v<Type<T>, std::vector<char>>>
		{
		};
		template<typename T> static inline constexpr bool is_basic_char_vector_v = is_basic_char_vector<T>::value;

		template<typename T> struct is_u32_type_vector;
		template<typename T>
		struct is_u32_type_vector
			: std::bool_constant<std::is_same_v<Type<T>, std::vector<char32_t>> || (std::is_same_v<Type<T>, std::vector<wchar_t>> && sizeof(wchar_t) == 4)>
		{
		};
		template<typename T> static inline constexpr bool is_u32_type_vector_v = is_u32_type_vector<T>::value;

		template<typename T> struct is_u16_type_vector;
		template<typename T>
		struct is_u16_type_vector
			: std::bool_constant<std::is_same_v<Type<T>, std::vector<char16_t>> || (std::is_same_v<Type<T>, std::vector<wchar_t>> && sizeof(wchar_t) == 2)>
		{
		};
		template<typename T> static inline constexpr bool is_u16_type_vector_v = is_u16_type_vector<T>::value;

		template<typename T> struct is_vector;
		template<typename T> struct is_vector: std::bool_constant<is_basic_char_vector_v<T> || is_u16_type_vector_v<T> || is_u32_type_vector_v<T>>
		{
		};
		template<typename T> static inline constexpr bool is_vector_v = is_vector<T>::value;

		template<typename StringishType>
		concept IsStringType = requires {
			is_string_v<StringishType> || is_string_view_v<StringishType>;
		};

		// concept constraints that use the above individual constraints
		template<typename StringishType>
		concept ConvertibleToSuppSV = requires {
			// I don't support char8_t here so I need to check that char Type is supported instead and then make sure that UTF8_LEAD_MASK_SEQUENCE_3 string_view is
			// constructible in the case that UTF8_LEAD_MASK_SEQUENCE_3 user passes UTF8_LEAD_MASK_SEQUENCE_3 char-like Type that aliases UTF8_LEAD_MASK_SEQUENCE_3
			// supported char Type but may or may not be constructible as UTF8_LEAD_MASK_SEQUENCE_3 string view
			is_char_type_v<typename Type<StringishType>::value_type> &&
			(is_string_view_v<StringishType> ||
			 std::is_convertible_v<std::basic_string<typename Type<StringishType>::value_type>, std::basic_string_view<typename Type<StringishType>::value_type>>);
		};

		template<typename Source>
		concept IsSupportedU16Source = requires {
			is_u16_type_string_v<Source> || is_u16_type_string_view_v<Source> || is_u16_type_vector_v<Source>;
		};

		template<typename Buffer>
		concept IsSupportedU16Container = requires {
			is_u16_type_vector_v<Buffer> || is_u16_type_string_v<Buffer>;
		};

		template<typename Source>
		concept IsSupportedU32Source = requires {
			is_u32_type_string_v<Source> || is_u32_type_string_v<Source> || is_u32_type_vector_v<Source>;
		};

		template<typename Buffer>
		concept IsSupportedU32Container = requires {
			is_u32_type_vector_v<Buffer> || is_u32_type_string_v<Buffer>;
		};

		template<typename Source>
		concept IsSupportedU8Source = requires {
			is_basic_string_v<Source> || is_basic_string_view_v<Source> || is_basic_char_vector_v<Source>;
		};

		template<typename Buffer>
		concept IsSupportedU8Container = requires {
			is_basic_char_vector_v<Buffer> || is_basic_string_v<Buffer>;
		};

		template<typename Source>
		concept IsSupportedUSource = requires {
			IsSupportedU8Source<Source> || IsSupportedU16Source<Source> || IsSupportedU32Source<Source>;
		};

		template<typename Buffer>
		concept IsSupportedUContainer = requires {
			IsSupportedU8Container<Buffer> || IsSupportedU16Container<Buffer> || IsSupportedU32Container<Buffer>;
		};

	}    // namespace utf_constraints

	static bool IsLittleEndian() {
		constexpr short int val = 0x0001;
		return reinterpret_cast<const char*>(&val)[ 0 ] ? true : false;
	}

	template<typename T> constexpr auto ConvertToSv(T&& v) requires utf_constraints::ConvertibleToSuppSV<T> {
		using Type = std::basic_string_view<typename Type<T>::value_type>;
		return std::forward<Type>(Type(v));
	}

	template<typename StringishType>
	requires utf_constraints::IsStringType<StringishType>
	static constexpr size_t ReserveLengthForU8(StringishType&& s) {
		UTF_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                             "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");
		using namespace utf_masks;
		using namespace utf_bounds;
		using CharType = typename Type<StringishType>::value_type;

		size_t reserveSize {};
		int pos { -1 };
		auto size { s.size() };

		if constexpr( std::is_same_v<CharType, char16_t> || (std::is_same_v<CharType, wchar_t> && sizeof(se_wchar) == 2) ) {
				for( ;; ) {
						if( ++pos >= size ) return reserveSize;
						if( auto& ch { s[ pos ] }; ch <= UTF8_MAX_1 ) {
								++reserveSize;
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_2 ) {
								reserveSize += 2;
								continue;
						} else if( ch >= UTF16_HIGH_SURROGATE_MIN && ch <= UTF16_HIGH_SURROGATE_MAX ) {
								if( ++pos >= size ) {
										UTF_ASSERT(false, "Error In Measuring UTF-16 Length: High Surrogate Byte Present Without A Low Surrogate Byte");
										return reserveSize += 2;    // reserve for replacement character that will inevitably be written
								}
								if( auto& next { s[ pos ] }; next >= UTF16_LOW_SURROGATE_MIN && next <= UTF16_HIGH_SURROGATE_MAX ) {
										char32_t cp { UTF16_SURROGATE_CLAMP };
										cp += (ch & UTF_NIBBLE_10) << 10;
										cp += (next & UTF_NIBBLE_10) << 10;
										// If  cp > WIDE_ENCODING_TO_UTF8_4 , reserve 2 for replacement character, otherwise reserve the correct amount
										reserveSize += (cp <= WIDE_ENCODING_TO_UTF8_3 ? 3 : cp <= WIDE_ENCODING_TO_UTF8_4 ? 4 : 2);
										continue;
								}
						} else if( ch <= WIDE_ENCODING_TO_UTF8_3 ) {
								reserveSize += 3;
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_4 ) {
								reserveSize += 4;
								continue;
						} else {
								reserveSize += 2;    // this will be used to add the replacement character bytes
								continue;
							}
					}
		} else if constexpr( std::is_same_v<CharType, char32_t> || (std::is_same_v<CharType, wchar_t> && sizeof(se_wchar) == 4) ) {
				for( ;; ) {
						if( ++pos >= size ) return reserveSize;
						if( auto& ch { s[ pos ] }; ch <= UTF8_MAX_1 ) {
								++reserveSize;
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_2 ) {
								reserveSize += 2;
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_3 ) {
								reserveSize += 3;
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_4 ) {
								reserveSize += 4;
								continue;
						} else {
								reserveSize += 2;    // this will be used to add the replacement character bytes
							}
					}
		} else {
				UTF_ASSERT(false, "Reserving Size From Any Other Encoding Besides UTF-32LE Or UTF-16LE Is Currently Unsupported At The Moment");
				return 0;
			}
	}

	template<typename Buffer, typename Char>
	requires utf_constraints::IsSupportedUContainer<Buffer> && utf_constraints::is_char_type_v<Char>
	static constexpr void WriteChToUContainerImpl(Buffer& buff, Char&& ch) {
		if constexpr( utf_constraints::is_string_v<Buffer> && std::is_rvalue_reference_v<decltype(ch)> ) {
				buff += std::move(ch);
		} else if constexpr( utf_constraints::is_string_v<Buffer> ) {
				buff += ch;
		} else if constexpr( utf_constraints::is_vector_v<Buffer> && std::is_rvalue_reference_v<decltype(ch)> ) {
				buff.emplace_back(std::move(ch));
		} else if constexpr( utf_constraints::is_vector_v<Buffer> ) {
				buff.emplace_back(ch);
		}
	}

	template<typename Buffer, typename Char>
	requires utf_constraints::IsSupportedUContainer<Buffer> && utf_constraints::is_char_type_v<Char>
	static constexpr void WriteChToUContainer(Buffer& buff, Char&& ch) {
		using BuffCharType = typename Type<Buffer>::value_type;
		if constexpr( std::is_same_v<BuffCharType, char> && std::is_same_v<Type<Char>, unsigned char> && std::is_signed_v<char> ) {
				WriteChToUContainerImpl(buff, static_cast<char>(ch));
		} else {
				WriteChToUContainerImpl(buff, ch);
			}
	}

	template<typename CharType>
	requires utf_constraints::is_char_type_v<CharType>
	constexpr bool CheckTrailingByte(CharType& byte) {
		using namespace utf_bounds;
		// valid trailing byte lays between 1000 0000 and 1011 1111
		return (byte >= static_cast<CharType>(UTF8_MIN_TRAILING) && byte <= static_cast<CharType>(UTF8_MAX_TRAILING)) ? true : false;
	};

	// If BOM, replaces byte(s) and returns false, otherwise, returns true
	template<typename CharType>
	requires utf_constraints::is_char_type_v<CharType>
	constexpr bool Utf8BomOrRepCh(CharType byte1, CharType byte2) {
		using namespace utf_bounds;
		using namespace utf_boms;
		if( byte1 == static_cast<CharType>(UTF8_INVALID_BYTE_2) ) {
				if( byte2 == static_cast<CharType>(UTF8_INVALID_BYTE_1) ) {
						UTF_ASSERT(false, "Error Validating UTF-8 Sequence: A Non UTF-8 Byte Order Mark Sequence Was Detected");
						byte1 = static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_1);
						byte2 = static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_2);
						return false;
				} else if( byte2 == static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_2) ) {
						// the byte sequence of the replacement character for utf-8 is 0xFF and 0xDD, so false flag here
						return true;
				} else {
						UTF_ASSERT(false, "Error Validating UTF-8 Sequence: Invalid Byte In Sequence Detected. A Byte Of Value 255 Is Illegal");
						byte1 = '?';    // Don't want to shift bytes around for the replacement box character, so just replace the single byte with a '?'
						return false;
					}
		} else if( byte1 == static_cast<CharType>(UTF8_INVALID_BYTE_1) ) {
				UTF_ASSERT(false, "Error Validating UTF-8 Sequence: Invalid Byte In Sequence Detected. A Byte Of Value 254 Is Illegal");
				byte1 = '?';    // Don't want to shift bytes around for the replacement box character, so just replace the single byte with a'?'
				return false;
		} else {
				// Skip checking the second byte as any byte failing the CheckTrailingByte() validation will call this lambda with the next byte to check
				// anyways
				return true;
			}
	};

	template<typename Buffer>
	requires utf_constraints::IsSupportedU8Container<Buffer>
	constexpr bool IsValidU8(Buffer& buff) {
		using CharType = typename Type<Buffer>::value_type;
		using namespace utf_bounds;

		auto size { buff.size() };
		int pos { -1 };
		for( ;; ) {
				if( ++pos >= size ) return true;    // validated all bytes in string sequence
				const auto& byte { buff[ pos ] };
				if( byte <= static_cast<CharType>(UTF8_MAX_1) ) {
						continue;
				}
				int bytesToCheck { 0 };
				if( byte >= static_cast<CharType>(UTF8_MIN_2) && byte <= static_cast<CharType>(UTF8_MAX_2) ) {
						bytesToCheck = 1;
				} else if( byte >= static_cast<CharType>(UTF8_MIN_3) && byte <= static_cast<CharType>(UTF8_MAX_3) ) {
						bytesToCheck = 2;
				} else if( byte >= static_cast<CharType>(UTF8_MIN_4) && byte <= static_cast<CharType>(UTF8_MAX_4) ) {
						bytesToCheck = 3;
				} else {
						// As of Unicode Version 14.0 (2021Sep14), there are only 144,697 characters in use, including emojis, so while 5-byte and 6-byte
						// sequences are technically supported, they are not defined so return false here. Only time this really would need to be updated is once
						// the Unicode Version dictates more than 1,114,111 characters in use as that number is the max value a 4-byte sequence can map to.
						return false;
					}
				for( ; bytesToCheck > 0; --bytesToCheck ) {
						if( ++pos >= size ) {
								UTF_ASSERT(false, "Error In Validating UTF-8 Sequence: Incomplete UTF-8 Sequence Detected");
								return false;
						}
						if( auto& byte1 { buff[ pos ] }; !CheckTrailingByte(byte1) ) {
								if( !Utf8BomOrRepCh(byte1, buff[ pos ]) ) return false;
						}
					}
				return true;
			}
	}

	// although big endian currently isn't supported, add the BE values for future changes
	enum class bom_type
	{
		utf8_bom,
		utf8_no_bom,
		utf16LE_bom,
		utf16BE_bom,
		utf16_no_bom,
		utf32LE_bom,
		utf32BE_bom,
		utf32_no_bom,
	};

	template<typename Source>
	requires utf_constraints::IsSupportedUSource<Source>
	constexpr bom_type DetectBom(Source&& src) {
		UTF_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                             "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");
		using namespace utf_constraints;
		using namespace utf_boms;
		using enum bom_type;

		auto size { src.size() };
		if constexpr( IsSupportedU8Source<Source> ) {
				if( size > 3 && src[ 0 ] == UTF8_BOM_1 && src[ 1 ] == UTF8_BOM_2 && src[ 2 ] == UTF8_BOM_3 ) return utf8_bom;
				return utf8_no_bom;
		} else if constexpr( IsSupportedU16Source<Source> ) {
				if( size > 2 && src[ 0 ] == UTF16_FULL_BOM_LE ) return utf16LE_bom;
				return utf16_no_bom;
		} else {
				// handle as a possible utf-32 encoding
				if( src[ 0 ] == UTF32_FULL_BOM_LE ) return utf32LE_bom;
				return utf32_no_bom;
			}
	}

	// NOTE:  Decided to handle wchar_t in these functions so that it's more convenient to call these functions without dealing with the non-standard wchar_t
	// sizes NOTE: When everything is done and works as intended, may move the *Impl versions directly into the functions to avoid the unneccessary string_view
	// conversions

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU16Source<Source> && utf_constraints::IsSupportedU8Container<Buffer>
	constexpr void U16ToU8(Source&& wstr, Buffer& buff, Pos&& startingPos = 0) {
		UTF_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                             "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");
		using CharType = typename Type<Buffer>::value_type;
		using RvRef    = std::add_rvalue_reference_t<CharType>;
		using BRef     = Ref<Buffer>;
		using namespace utf_bounds;
		using namespace utf_masks;

		if constexpr( std::is_same_v<CharType, wchar_t> && sizeof(wchar_t) != 2 ) {
				U32ToU8(std::forward<Source>(wstr), std::forward<Buffer>(buff), std::forward<Pos>(startingPos));
		} else {
				int pos { startingPos == 0 ? -1 : static_cast<int>(startingPos) - 1 };
				auto size { wstr.size() };
				auto rSize { ReserveLengthForU8(wstr) };
				if( buff.capacity() < rSize ) buff.reserve(rSize);
				// This is really close to U32ToU8()'s implementation -> only difference being how the 3-byte and 4-byte sequence is encoded due to surrogate
				// pairs
				for( ;; ) {
						if( ++pos >= size ) {
								IsValidU8(std::forward<BRef>(BRef(buff)));
								if constexpr( std::is_lvalue_reference_v<Pos> ) {
										startingPos += buff.size();
								}
								return;
						}
						if( auto& ch { wstr[ pos ] }; ch <= UTF8_MAX_1 ) {
								WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(wstr[ pos ]));
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_2 ) {
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_1 | (ch >> 6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (ch & UTF_NIBBLE_6)));
								continue;
						} else if( ch < UTF16_HIGH_SURROGATE_MIN ) {
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_2 | (ch >> 12)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((ch >> 6) & UTF_NIBBLE_6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (ch & UTF_NIBBLE_6)));
								continue;
						} else {
								if( ch > UTF16_HIGH_SURROGATE_MAX ) {
										UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-8: Illegal Surrogate Byte In High Byte");
										WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_1));
										WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_2));
										continue;
								}
								if( ++pos >= size ) {
										UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-8: High Surrogate Byte Present Without A Low Surrogate Byte");
										WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_1));
										WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_2));
										continue;
								}
								if( auto& next { wstr[ pos ] }; next >= UTF16_LOW_SURROGATE_MIN ) {
										if( next > UTF16_HIGH_SURROGATE_MAX ) {
												UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-8: Illegal Surrogate Byte In Low Byte");
												WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_1));
												WriteChToUContainer(std::forward<BRef>(buff), static_cast<CharType>(UTF8_REPLACEMENT_CHARACTER_BYTE_2));
												continue;
										}
										char32_t cp { UTF16_SURROGATE_CLAMP };
										cp += (ch & UTF_NIBBLE_10) << 10;
										cp += (next & UTF_NIBBLE_10);
										if( cp <= UTF_MAX_CODEPOINT ) {
												WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_3 | (cp >> 18)));
												WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((cp >> 12) & UTF_NIBBLE_6)));
												WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((cp >> 6) & UTF_NIBBLE_6)));
												WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (cp & UTF_NIBBLE_6)));
										} else {
												UTF_ASSERT(false, "Error In Encoding UTF-16 To UTF-8: Illegal Codepoint Detected");
												WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
											}
										continue;
								}
							}
					}
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU16Source<Source> && utf_constraints::IsSupportedU32Container<Buffer>
	constexpr void U16ToU32(Source&& wstr, Buffer& buff, Pos&& startingPos = 0) {
		UTF_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                             "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");
		using CharType = typename Type<Buffer>::value_type;
		using BRef     = Ref<Buffer>;
		using namespace utf_bounds;
		using namespace utf_masks;

		if constexpr( std::is_same_v<Type<Source>, wchar_t> && sizeof(wchar_t) != 2 ) {
				for( auto& ch: wstr ) WriteChToUContainer(buff, static_cast<char32_t>(ch));
		} else {
				int pos { startingPos == 0 ? -1 : static_cast<int>(startingPos) - 1 };
				auto size { wstr.size() };
				if( buff.capacity() < size ) buff.reserve(size);
				if constexpr( sizeof(se_wchar) == 2 ) {
						for( ;; ) {
								if( ++pos >= size ) {
										if constexpr( std::is_lvalue_reference_v<Pos> ) {
												startingPos += buff.size();
										}
										return;
								}
								if( wstr[ pos ] < UTF16_HIGH_SURROGATE_MIN ) {
										WriteChToUContainer(std::forward<BRef>(buff), static_cast<char32_t>(wstr[ pos ]));
										continue;
								} else {
										// check for valid high byte in pair, can skip checking for 0xD800, as that was already checked above
										if( wstr[ pos ] > UTF16_HIGH_SURROGATE_MAX ) {
												UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-32: Illegal Surrogate Byte In High Byte");
												WriteChToUContainer(std::forward<BRef>(buff), static_cast<char32_t>(REPLACEMENT_CHARACTER));
												continue;
										}
										if( ++pos >= size ) {
												UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-32: Incomplete Surrogate Pairing When Decoding UTF-16 "
												                  "Codepoint");
												WriteChToUContainer(std::forward<BRef>(buff), static_cast<char32_t>(REPLACEMENT_CHARACTER));
												continue;    // continue instead of return here in case Pos Type is a lvalue ref
										}
										// check for valid low byte in pair
										if( wstr[ pos ] < UTF16_LOW_SURROGATE_MIN || wstr[ pos ] > UTF16_LOW_SURROGATE_MAX ) {
												UTF_ASSERT(false, "Error In Decoding UTF-16 To UTF-32: Illegal Surrogate Byte In Low Byte");
												WriteChToUContainer(std::forward<BRef>(buff), static_cast<char32_t>(REPLACEMENT_CHARACTER));
												continue;
										}
										char32_t cp { UTF16_SURROGATE_CLAMP };
										cp += (wstr[ --pos ] & UTF_NIBBLE_10) << 10;
										cp += (wstr[ ++pos ] & UTF_NIBBLE_10);
										if( cp <= UTF_MAX_CODEPOINT ) {
												WriteChToUContainer(buff, static_cast<CharType>(cp));
										} else {
												UTF_ASSERT(false, "Error In Encoding UTF-16 To UTF-32: Illegal Codepoint Detected");
												WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
											}
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
								WriteChToUContainer(std::forward<BRef>(buff), static_cast<char32_t>(wstr[ pos ]));
							}
					}
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU32Source<Source> && utf_constraints::IsSupportedU8Container<Buffer>
	constexpr void U32ToU8(Source&& sv, Buffer& buff, Pos&& startingPos = 0) {
		UTF_ASSERT(IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                             "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");
		using CharType = typename Type<Buffer>::value_type;
		using RvRef    = std::add_rvalue_reference_t<CharType>;
		using BRef     = Ref<Buffer>;
		using namespace utf_bounds;
		using namespace utf_masks;

		if constexpr( std::is_same_v<Type<Source>, wchar_t> && sizeof(wchar_t) != 4 ) {
				U16ToU8(std::forward<Source>(sv), buff, std::forward<Pos>(startingPos));
		} else {
				int pos { startingPos == 0 ? -1 : static_cast<int>(startingPos) - 1 };
				auto size { sv.size() };
				auto rSize { ReserveLengthForU8(sv) };
				if( buff.capacity() < rSize ) buff.reserve(rSize);
				for( ;; ) {
						if( ++pos >= size ) {
								IsValidU8(std::forward<BRef>(BRef(buff)));
								if constexpr( std::is_lvalue_reference_v<Pos> ) {
										startingPos += buff.size();
								}
								return;
						}
						if( auto ch { static_cast<char32_t>(sv[ pos ]) }; ch <= UTF8_MAX_1 ) {
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<const CharType&>(ch));
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_2 ) {
								// handle encoding to 2 byte utf-8 sequence here
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_1 | (ch >> 6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (ch & UTF_NIBBLE_6)));
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_3 ) {
								// handle encoding to 3 byte utf-8 sequence here
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_2 | (ch >> 12)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((ch >> 6) & UTF_NIBBLE_6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (ch & UTF_NIBBLE_6)));
								continue;
						} else if( ch <= WIDE_ENCODING_TO_UTF8_4 ) {
								// handle encoding to 4 byte utf-8 sequence here
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_LEAD_MASK_SEQUENCE_3 | (ch >> 18)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((ch >> 12) & UTF_NIBBLE_6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | ((ch >> 6) & UTF_NIBBLE_6)));
								WriteChToUContainer(std::forward<BRef>(BRef(buff)), static_cast<RvRef>(UTF8_TRAIL_BASE | (ch & UTF_NIBBLE_6)));
								continue;
						} else {
								// 5-byte and 6-byte sequences are apparently supported for the long-term but are not defined, therefore, instead of encoding
								// these cases to utf-8, use the repacement character instead and in debug builds, raise an error when the 0xFF byte is detected
								// in validation.
								UTF_ASSERT(false, "Error In Decoding UTF-32 To UTF-8: Illegal Codepoint Detected");
								WriteChToUContainer(std::forward<BRef>(buff), static_cast<RvRef>(UTF8_REPLACEMENT_CHARACTER_BYTE_1));
								WriteChToUContainer(std::forward<BRef>(buff), static_cast<RvRef>(UTF8_REPLACEMENT_CHARACTER_BYTE_2));
								continue;
							}
					}
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU8Source<Source> && utf_constraints::IsSupportedU16Container<Buffer>
	constexpr void U8ToU16(Source&& sv, Buffer& buff, Pos&& startingPos = 0) {
		using SourceChar = typename Type<Source>::value_type;
		using CharType   = typename Type<Buffer>::value_type;
		using RvRef      = std::add_rvalue_reference_t<CharType>;
		using namespace utf_bounds;
		using namespace utf_masks;

		if constexpr( std::is_same_v<CharType, wchar_t> && sizeof(wchar_t) == 4 ) {
				U8ToU32(std::forward<Source>(sv), buff, std::forward<Pos>(startingPos));
		} else {
				int pos { startingPos == 0 ? -1 : static_cast<int>(--startingPos) };
				char32_t cp {};
				short int cpLength {};
				auto size { sv.size() };
				if( buff.capacity() < size ) buff.reserve(size);
				for( ;; ) {
						if( ++pos >= size ) return;
						// retrieve the codepoint value by first looking at the leading byte to see how many bytes need to be observed
						// for the codepoint value and then accumulate the appropriate bits to the appropriate codepoint value
						if( auto& ch { sv[ pos ] }; (ch & UTF8_TRAIL_BASE) == 0 ) {
								cp = static_cast<char16_t>(ch & UTF_NIBBLE_7);
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_2) == UTF8_LEAD_MASK_SEQUENCE_1 ) {
								cp = ch & UTF_NIBBLE_5;
								++cpLength;
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_3) == UTF8_LEAD_MASK_SEQUENCE_2 ) {
								cp = ch & UTF_NIBBLE_4;
								cpLength += 2;
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_4) == UTF8_LEAD_MASK_SEQUENCE_3 ) {
								cp = ch & UTF_NIBBLE_3;
								cpLength += 3;
						} else {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-16: Illegal Leading Byte Value Detected In Sequence");
								WriteChToUContainer(buff, static_cast<const CharType&>(REPLACEMENT_CHARACTER));
								continue;
							}

						// validate that the size indicated from the leading byte doesn't exceed past the source size
						if( pos + cpLength >= size ) {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-16: Incomplete UTF-8 Sequence Detected");
								WriteChToUContainer(buff, static_cast<const CharType&>(REPLACEMENT_CHARACTER));
								continue;
						}
						// get the actual code point value free from leading/trailing nibbles
						for( ; cpLength > 0; --cpLength ) {
								if( CheckTrailingByte(sv[ ++pos ]) ) {
										cp = (cp << 6) | (sv[ pos ] & UTF_NIBBLE_6);
								} else {
										WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
										pos += cpLength;
										continue;
									}
							}

						if( cp <= UTF_MAX_CODEPOINT ) {
								// encode the code point value
								if( cp > UTF16_SURROGATE_CLAMP ) {
										// make a surrogate pair and append them to the buffer
										cp -= static_cast<char16_t>(UTF16_SURROGATE_CLAMP);
										WriteChToUContainer(buff, static_cast<RvRef>((cp >> 10) + UTF16_HIGH_SURROGATE_MIN));
										WriteChToUContainer(buff, static_cast<RvRef>((cp & UTF_NIBBLE_10) + UTF16_LOW_SURROGATE_MIN));
								} else {
										WriteChToUContainer(buff, static_cast<const CharType&>(cp));
									}
						} else {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-16: Illegal Codepoint Detected");
								WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
							}
					}    // end for loop
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU8Source<Source> && utf_constraints::IsSupportedU32Container<Buffer>
	constexpr void U8ToU32(Source&& sv, Buffer& buff, Pos&& startingPos = 0) {
		using SourceChar = typename Type<Source>::value_type;
		using CharType   = typename Type<Buffer>::value_type;
		using namespace utf_bounds;
		using namespace utf_masks;

		if constexpr( std::is_same_v<CharType, wchar_t> && sizeof(wchar_t) == 2 ) {
				U8ToU16(std::forward<Source>(sv), buff, std::forward<Pos>(startingPos));
		} else {
				int pos { startingPos == 0 ? -1 : static_cast<int>(--startingPos) };
				char32_t cp {};
				short int cpLength {};
				auto size { sv.size() };
				if( buff.capacity() < size ) buff.reserve(size);
				for( ;; ) {
						if( ++pos >= size ) return;
						// retrieve the codepoint value by first looking at the leading byte to see how many bytes need to be observed
						// for the codepoint value and then accumulate the appropriate bits to the appropriate codepoint value
						if( auto& ch { sv[ pos ] }; (ch & UTF8_TRAIL_BASE) == 0 ) {
								cp = static_cast<char16_t>(ch & UTF_NIBBLE_7);
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_2) == UTF8_LEAD_MASK_SEQUENCE_1 ) {
								cp = ch & UTF_NIBBLE_5;
								++cpLength;
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_3) == UTF8_LEAD_MASK_SEQUENCE_2 ) {
								cp = ch & UTF_NIBBLE_4;
								cpLength += 2;
						} else if( (ch & UTF8_LEAD_MASK_SEQUENCE_4) == UTF8_LEAD_MASK_SEQUENCE_3 ) {
								cp = ch & UTF_NIBBLE_3;
								cpLength += 3;
						} else {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-32: Illegal Leading Byte Value Detected In Sequence");
								WriteChToUContainer(buff, static_cast<const CharType&>(REPLACEMENT_CHARACTER));
								continue;
							}

						// validate that the size indicated from the leading byte doesn't exceed past the source size
						if( pos + cpLength >= size ) {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-32: Incomplete UTF-8 Sequence Detected");
								WriteChToUContainer(buff, static_cast<const CharType&>(REPLACEMENT_CHARACTER));
								continue;
						}

						// get the actual code point value free from leading/trailing nibbles
						for( ; cpLength > 0; --cpLength ) {
								if( CheckTrailingByte(sv[ ++pos ]) ) {
										cp = (cp << 6) | (sv[ pos ] & UTF_NIBBLE_6);
								} else {
										WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
										pos += cpLength;
										continue;
									}
							}

						if( cp <= UTF_MAX_CODEPOINT ) {
								WriteChToUContainer(buff, static_cast<CharType>(cp));
						} else {
								UTF_ASSERT(false, "Error In Encoding UTF-8 To UTF-32: Illegal Codepoint Detected");
								WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
							}
					}
			}
	}

	template<typename Source, typename Buffer, typename Pos = size_t>
	requires utf_constraints::IsSupportedU32Source<Source> && utf_constraints::IsSupportedU16Container<Buffer>
	constexpr void U32ToU16(Source&& sv, Buffer& buff, Pos&& startingPos = 0) {
		using SourceChar = typename Type<Source>::value_type;
		using CharType   = typename Type<Buffer>::value_type;
		using namespace utf_bounds;
		using namespace utf_masks;

		if( buff.capacity() < sv.size() ) buff.reserve(sv.size());
		for( auto& ch: sv ) {
				if( ch <= UTF_MAX_CODEPOINT ) {
						// encode the code point value
						if( ch > UTF16_SURROGATE_CLAMP ) {
								// make a surrogate pair and append them to the buffer
								ch -= static_cast<char16_t>(UTF16_SURROGATE_CLAMP);
								WriteChToUContainer(buff, static_cast<CharType>((ch >> 10) + UTF16_HIGH_SURROGATE_MIN));
								WriteChToUContainer(buff, static_cast<CharType>((ch & UTF_NIBBLE_10) + UTF16_LOW_SURROGATE_MIN));
						} else {
								WriteChToUContainer(buff, static_cast<const CharType&>(ch));
							}
				} else {
						UTF_ASSERT(false, "Error In Encoding UTF-32 To UTF-16: Illegal Codepoint Detected");
						WriteChToUContainer(buff, static_cast<CharType>(REPLACEMENT_CHARACTER));
					}
			}
	}

}    // namespace utf_helper
