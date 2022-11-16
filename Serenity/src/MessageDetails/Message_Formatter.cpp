#include <serenity/MessageDetails/Message_Formatter.h>
#include <iostream>

namespace serenity::msg_details {

	void Message_Formatter::SetLocaleReference(const std::locale& loc) {
		localeRef = loc;
	}

	std::locale Message_Formatter::Locale() {
		return localeRef;
	}

	Message_Formatter::Message_Formatter(std::string_view pattern, Message_Info* details)
		: msgInfo(*&details), localeRef(globals::default_locale), sourceFlag(source_flag::empty) {
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
		SetPattern(pattern);
	}

	constexpr static bool IsCustomFlag(const char& ch) {
		switch( ch ) {
				case 'l': [[fallthrough]];
				case 's': [[fallthrough]];
				case 't': [[fallthrough]];
				case 'L': [[fallthrough]];
				case 'N': [[fallthrough]];
				case '+': return true;
				default: return false;
			}
	}

	constexpr static bool IsTimeFlag(const char& ch) {
		switch( ch ) {
				case 'a': [[fallthrough]];
				case 'h': [[fallthrough]];
				case 'b': [[fallthrough]];
				case 'c': [[fallthrough]];
				case 'd': [[fallthrough]];
				case 'e': [[fallthrough]];
				case 'g': [[fallthrough]];
				case 'j': [[fallthrough]];
				case 'm': [[fallthrough]];
				case 'p': [[fallthrough]];
				case 'r': [[fallthrough]];
				case 'u': [[fallthrough]];
				case 'w': [[fallthrough]];
				case 'D': [[fallthrough]];
				case 'x': [[fallthrough]];
				case 'y': [[fallthrough]];
				case 'z': [[fallthrough]];
				case 'A': [[fallthrough]];
				case 'B': [[fallthrough]];
				case 'C': [[fallthrough]];
				case 'F': [[fallthrough]];
				case 'G': [[fallthrough]];
				case 'H': [[fallthrough]];
				case 'I': [[fallthrough]];
				case 'M': [[fallthrough]];
				case 'R': [[fallthrough]];
				case 'S': [[fallthrough]];
				case 'T': [[fallthrough]];
				case 'U': [[fallthrough]];
				case 'V': [[fallthrough]];
				case 'W': [[fallthrough]];
				case 'X': [[fallthrough]];
				case 'Y': [[fallthrough]];
				case 'Z': [[fallthrough]];
				case 'n': [[fallthrough]];
				case 't': [[fallthrough]];
				case '%': return true; break;
				default: return false; break;
			}
	}

	void Message_Formatter::SetPattern(std::string_view pattern) {
		fmtPattern.clear();
		for( ;; ) {
				auto pos { -1 };
				auto size { pattern.size() };
				for( ;; ) {
						if( ++pos >= size ) {
								return;
								fmtPattern.append(LineEnding());
						}
						if( pattern[ pos ] != '%' ) continue;
						break;
					}
				fmtPattern.append(pattern.substr(0, pos));
				++pos;    // advance past the '%' and on to the next character
				if( IsCustomFlag(pattern[ pos ]) ) {
						if( pattern[ pos ] == 's' ) {
								fmtPattern.append("{0:%s");
								if( ++pos >= size ) {
										fmtPattern.append(":a}");
										return;
								}
								if( pattern[ pos ] != ':' ) {
										// decrement due to the increment that occurs after `continue` is hit
										--pos;
										fmtPattern.append(":a}");
										pattern.remove_prefix(++pos);
										continue;
								}
								fmtPattern += ':';
								// we've now hit ':' so deal with any modifiers here
								for( ;; ) {
										if( ++pos >= size ) {
												fmtPattern += '}';
												return;
										}
										switch( pattern[ pos ] ) {
												case 'a': fmtPattern += 'a'; continue;
												case 'c': fmtPattern += 'c'; continue;
												case 'f': fmtPattern += 'f'; continue;
												case 'l': fmtPattern += 'l'; continue;
												case 'F': fmtPattern += 'F'; continue;
												default: break;
											}
										pattern.remove_prefix(++pos);
										continue;
									}
						} else if( pattern[ pos ] == 't' ) {
								fmtPattern.append("{0:%t");
								if( ++pos >= size ) {
										fmtPattern.append(":0}");
										return;
								}
								if( pattern[ pos ] != ':' ) {
										// decrement due to the increment that occurs after `continue` is hit
										--pos;
										fmtPattern.append(":0 }");
										pattern.remove_prefix(++pos);
										continue;
								}
								fmtPattern += ':';
								// we've now hit ':' so deal with any modifiers here
								for( ;; ) {
										if( ++pos >= size ) {
												fmtPattern += '}';
												return;
										}
										if( IsDigit(pattern[ pos ]) ) {
												if( pattern[ pos ] == '1' ) {
														// check that the next pos is either a 0 in the case of modifier "10" or not a digit in the case the modifier
														// is "1"
														if( ++pos > size ) {
																fmtPattern.append("1}");
																return;
														}
														if( !IsDigit(pattern[ pos ]) ) {
																fmtPattern.append("1}");
																break;
														}
														if( pattern[ pos ] != '0' ) {
																// warn that the modifier is using a larger number than allowed and that it will be set to the
																// default value
																fmtPattern.append("10}");
																break;
														}
												}
												fmtPattern += pattern[ pos ];
												fmtPattern += "}";
												break;
										}
									}
								pattern.remove_prefix(++pos);
								continue;
						} else {
								fmtPattern.append("{0:%").append(1, pattern[ pos ]).append("}");
								pattern.remove_prefix(++pos);
								continue;
							}
				} else if( IsTimeFlag(pattern[ pos ]) ) {
						--pos;    // decrement to take into account the increment that happens in the loop below
						fmtPattern.append("{1:");
						for( ;; ) {
								if( ++pos >= size ) {
										fmtPattern += '}';
										break;
								}
								switch( pattern[ pos ] ) {
										case 'a': fmtPattern.append("%a"); continue;
										case 'h': fmtPattern.append("%h"); continue;
										case 'b': fmtPattern.append("%b"); continue;
										case 'c': fmtPattern.append("%c"); continue;
										case 'd': fmtPattern.append("%d"); continue;
										case 'e': fmtPattern.append("%e"); continue;
										case 'g': fmtPattern.append("%g"); continue;
										case 'j': fmtPattern.append("%j"); continue;
										case 'm': fmtPattern.append("%m"); continue;
										case 'p': fmtPattern.append("%p"); continue;
										case 'r': fmtPattern.append("%r"); continue;
										case 'u': fmtPattern.append("%u"); continue;
										case 'w': fmtPattern.append("%w"); continue;
										case 'D': fmtPattern.append("%D"); continue;
										case 'x': fmtPattern.append("%x"); continue;
										case 'y': fmtPattern.append("%y"); continue;
										case 'z': fmtPattern.append("%z"); continue;
										case 'A': fmtPattern.append("%A"); continue;
										case 'B': fmtPattern.append("%B"); continue;
										case 'C': fmtPattern.append("%C"); continue;
										case 'F': fmtPattern.append("%F"); continue;
										case 'G': fmtPattern.append("%G"); continue;
										case 'H': fmtPattern.append("%H"); continue;
										case 'I': fmtPattern.append("%I"); continue;
										case 'M': fmtPattern.append("%M"); continue;
										case 'R': fmtPattern.append("%R"); continue;
										case 'S': fmtPattern.append("%S"); continue;
										case 'T': fmtPattern.append("%T"); continue;
										case 'U': fmtPattern.append("%U"); continue;
										case 'V': fmtPattern.append("%V"); continue;
										case 'W': fmtPattern.append("%W"); continue;
										case 'X': fmtPattern.append("%X"); continue;
										case 'Y': fmtPattern.append("%Y"); continue;
										case 'Z': fmtPattern.append("%Z"); continue;
										case 'n': fmtPattern.append("%n"); continue;
										case 't': fmtPattern.append("%t"); continue;
										case '%':
											{
												if( ++pos >= size ) {
														// warn on missing flag or unescaped '%'
														fmtPattern += '}';
														return;
												}
												if( pattern[ pos ] != '%' ) {
														--pos;    // decrement from the earlier bounds check and this character peek check
														continue;
												}
												fmtPattern += '%';
												continue;
											}
										case ' ': fmtPattern += ' '; continue;
										default: fmtPattern += '}'; break;
									}
								break;
							}
						pattern.remove_prefix(pos);
						continue;
				}
			}
	}

	const Message_Info* Message_Formatter::MessageDetails() {
		return msgInfo;
	}

	std::string_view serenity::msg_details::Message_Formatter::LineEnding() const {
		return SERENITY_LUTS::line_ending[ platformEOL ];
	}
	const std::string& Message_Formatter::Pattern() {
		return fmtPattern;
	}
}    // namespace serenity::msg_details
