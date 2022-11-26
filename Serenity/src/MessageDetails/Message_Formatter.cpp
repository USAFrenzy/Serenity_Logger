#include <serenity/MessageDetails/Message_Formatter.h>
#include <iostream>

namespace serenity::msg_details {

	void Message_Formatter::SetLocaleReference(const std::locale& loc) {
		localeRef = loc;
	}

	const std::locale& Message_Formatter::Locale() const {
		return localeRef;
	}

	Message_Formatter::Message_Formatter(std::string_view pattern, Message_Info* details)
		: msgInfo(*&details), localeRef(globals::default_locale), sourceFlag(source_flag::empty), fmtFlags(SeFmtFuncFlags::Base),
		  threadLength(custom_flags::defaultThreadIdLength) {
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
				case 'k': [[fallthrough]];
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

	void serenity::msg_details::Message_Formatter::ModifyInternalFormatStringIfNeeded() {
		using enum SeFmtFuncFlags;

		// There will always be the 'base' case in the formatting call due to how it's currently set up, so if the flag is only set to one of these 3, set it to the
		// base version of themselves and continue on to the modifier step
		switch( fmtFlags ) {
				default: break;
				case Src: fmtFlags = SeFmtFuncFlags::Src_Base; break;
				case Thread: fmtFlags = SeFmtFuncFlags::Thread_Base; break;
				case Time: fmtFlags = SeFmtFuncFlags::Time_Base; break;
			}

		switch( fmtFlags ) {
					// no modifications to internal format string needed due to positions of arguments in the TargetBase's "FormatLogMessage()" function
				case Base: [[fallthrough]];
				case Src_Base: [[fallthrough]];
				case Src_Thread_Base: [[fallthrough]];
				case Time_Src_Thread_Base: return;
				case Time_Base:
					{
						/* alter the position of time field from '5' to '3' for any time field bracket(s) */
						auto fmtPos { -1 };
						std::string tmp;
						auto fmtSize { fmtPattern.size() };
						tmp.reserve(fmtSize);
						for( ;; ) {
								if( ++fmtPos >= fmtSize ) {
										if( fmtSize != 0 ) {
												tmp.append(fmtPattern);
										}
										fmtPattern.clear();
										fmtPattern.append(std::move(tmp));
										return;
								}
								if( fmtPattern[ fmtPos ] != '5' ) continue;
								tmp.append(fmtPattern.substr(0, fmtPos)).append("3");
								fmtPattern.erase(0, ++fmtPos);
								fmtSize = fmtPattern.size();
								fmtPos  = -1;
							}
						return;
					}
				case Time_Src_Base:
					{
						/* alter the position of any time bracket(s) from '5' to '4'; no changes needed for src field */
						auto fmtPos { -1 };
						std::string tmp;
						auto fmtSize { fmtPattern.size() };
						tmp.reserve(fmtSize);
						for( ;; ) {
								if( ++fmtPos >= fmtSize ) {
										if( fmtSize != 0 ) {
												tmp.append(fmtPattern);
										}
										fmtPattern.clear();
										fmtPattern.append(std::move(tmp));
										return;
								}
								if( fmtPattern[ fmtPos ] != '5' ) continue;
								tmp.append(fmtPattern.substr(0, fmtPos)).append("4");
								fmtPattern.erase(0, ++fmtPos);
								fmtSize = fmtPattern.size();
								fmtPos  = -1;
							}
						return;
					}
				case Time_Thread_Base:
					{
						/* alter the position of any thread bracket(s) from '4' to '3'  & any time bracket(s) from '5' to '4'*/
						auto fmtPos { -1 };
						std::string tmp;
						auto fmtSize { fmtPattern.size() };
						tmp.reserve(fmtSize);
						for( ;; ) {
								if( ++fmtPos >= fmtSize ) {
										if( fmtSize != 0 ) {
												tmp.append(fmtPattern);
										}
										fmtPattern.clear();
										fmtPattern.append(std::move(tmp));
										return;
								}
								switch( fmtPattern[ fmtPos ] ) {
										case '4':
											tmp.append(fmtPattern.substr(0, fmtPos)).append("3");
											fmtPattern.erase(0, ++fmtPos);
											fmtSize = fmtPattern.size();
											fmtPos  = -1;
											continue;
										case '5':
											tmp.append(fmtPattern.substr(0, fmtPos)).append("4");
											fmtPattern.erase(0, ++fmtPos);
											fmtSize = fmtPattern.size();
											fmtPos  = -1;
											continue;
										default: continue;
									}
							}
						return;
					}
				default:
					{
						fmtFlags = SeFmtFuncFlags::Invalid;
						throw std::runtime_error("Error: In Message_Formatter.cpp `ModifyInternalFormatStringIfNeeded()` - Unknown Combination Of Flags Detected In "
						                         "Internal Storage Of User Defined String");
						break;
					}
			}
	}

	void Message_Formatter::SetPattern(std::string_view pattern) {
		fmtFlags = SeFmtFuncFlags::Base;
		fmtPattern.clear();
		for( ;; ) {
				auto pos { -1 };
				auto size { pattern.size() };
				for( ;; ) {
						if( ++pos >= size ) {
								fmtPattern.append(LineEnding());
								ModifyInternalFormatStringIfNeeded();
								return;
						}
						if( pattern[ pos ] != '%' ) continue;
						break;
					}
				fmtPattern.append(pattern.substr(0, pos));
				++pos;    // advance past the '%' and on to the next character
				if( IsCustomFlag(pattern[ pos ]) ) {
						switch( pattern[ pos ] ) {
								case 'L':
									{
										fmtPattern.append("{0:L}");
										pattern.remove_prefix(++pos);
										continue;
									}
								case 'l':
									{
										fmtPattern.append("{0:l}");
										pattern.remove_prefix(++pos);
										continue;
									}
								case 'N':
									{
										fmtPattern.append("{1}");
										pattern.remove_prefix(++pos);
										continue;
									}
								case '+':
									{
										fmtPattern.append("{2}");
										pattern.remove_prefix(++pos);
										continue;
									}
								case 's':
									{
										fmtFlags |= SeFmtFuncFlags::Src;
										fmtPattern.append("{3");
										if( ++pos >= size ) {
												fmtPattern.append("}");
												return;
										}
										if( pattern[ pos ] != ':' ) {
												// decrement due to the increment that occurs after `continue` is hit
												--pos;
												fmtPattern.append("}");
												pattern.remove_prefix(++pos);
												continue;
										}
										// we've now hit ':' so deal with any modifiers here
										for( ;; ) {
												if( ++pos >= size ) {
														fmtPattern += '}';
														return;
												}
												switch( pattern[ pos ] ) {
														case 'a': sourceFlag |= serenity::source_flag::all; continue;
														case 'c': sourceFlag |= serenity::source_flag::column; continue;
														case 'f': sourceFlag |= serenity::source_flag::file; continue;
														case 'l': sourceFlag |= serenity::source_flag::line; continue;
														case 'F': sourceFlag |= serenity::source_flag::function; continue;
														default: break;
													}
												pattern.remove_prefix(pos);
												fmtPattern += '}';
												break;
											}
										continue;
									}
								case 't':
									{
										fmtFlags |= SeFmtFuncFlags::Thread;
										fmtPattern.append("{4");
										if( ++pos >= size ) {
												fmtPattern.append("}");
												return;
										}
										if( pattern[ pos ] != ':' ) {
												// decrement due to the increment that occurs after `continue` is hit
												--pos;
												fmtPattern.append("}");
												pattern.remove_prefix(++pos);
												continue;
										}
										// we've now hit ':' so deal with any modifiers here
										for( ;; ) {
												if( ++pos >= size ) {
														fmtPattern += '}';
														return;
												}
												if( IsDigit(pattern[ pos ]) ) {
														switch( pattern[ pos ] ) {
																case '0': threadLength = 0; break;
																case '1':
																	{
																		if( ++pos > size ) {
																				threadLength = 1;
																				break;
																		}
																		if( !IsDigit(pattern[ pos ]) ) {
																				threadLength = 1;
																				break;
																		}
																		threadLength = custom_flags::defaultThreadIdLength;
																		break;
																	}
																case '2': threadLength = 2; break;
																case '3': threadLength = 3; break;
																case '4': threadLength = 4; break;
																case '5': threadLength = 5; break;
																case '6': threadLength = 6; break;
																case '7': threadLength = 7; break;
																case '8': threadLength = 8; break;
																case '9': threadLength = 9; break;
																default: break;
															}
														break;
												}
											}
										fmtPattern += '}';
										pattern.remove_prefix(++pos);
										continue;
									}
								default: throw std::runtime_error("Unhandled Serenity Flag Detected In Message_Formatter.cpp SetPattern()");
							}
				} else if( IsTimeFlag(pattern[ pos ]) ) {
						fmtFlags |= SeFmtFuncFlags::Time;
						--pos;    // decrement to take into account the increment that happens in the loop below
						fmtPattern.append("{5:");
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
										case 'k':
											{
#ifdef USE_BUILT_IN_FMT
												fmtPattern.append("%k");
#else
												fmtPattern.append("%a %d%b%y %T");
#endif    // USE_NATIVEFMT
												continue;
											}

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
										case 'E':
											{
												// add the localized flag IFF it's not already added
												if( auto state { static_cast<int>(fmtFlags) }; state < 16 && state >= 9 ) {
														fmtFlags -= SeFmtFuncFlags::Time;
														fmtFlags |= SeFmtFuncFlags::Localize_Time;
												}
												if( ++pos >= size ) {
														throw std::runtime_error("Localized Time Modifier Detected With No Flag To Modify");
												}
												switch( pattern[ pos ] ) {
														case 'c': fmtPattern.append("%Ec"); continue;
														case 'C': fmtPattern.append("%EC"); continue;
														case 'x': fmtPattern.append("%Ex"); continue;
														case 'X': fmtPattern.append("%EX"); continue;
														case 'y': fmtPattern.append("%Ey"); continue;
														case 'Y': fmtPattern.append("%EY"); continue;
														default:
															throw std::runtime_error("Localized Time Modifier Detected But Next Character Is Not A Modifiable Flag");
													}
											}
										case 'O':
											{
												// add the localized flag IFF it's not already added
												if( auto state { static_cast<int>(fmtFlags) }; state < 16 && state >= 9 ) {
														fmtFlags -= SeFmtFuncFlags::Time;
														fmtFlags |= SeFmtFuncFlags::Localize_Time;
												}
												if( ++pos >= size ) {
														throw std::runtime_error("Localized Time Modifier Detected With No Flag To Modify");
												}
												switch( pattern[ pos ] ) {
														case 'd': fmtPattern.append("%Od"); continue;
														case 'e': fmtPattern.append("%Oe"); continue;
														case 'H': fmtPattern.append("%OH"); continue;
														case 'I': fmtPattern.append("%OI"); continue;
														case 'm': fmtPattern.append("%Om"); continue;
														case 'M': fmtPattern.append("%OM"); continue;
														case 'S': fmtPattern.append("%OS"); continue;
														case 'u': fmtPattern.append("%Ou"); continue;
														case 'U': fmtPattern.append("%OU"); continue;
														case 'V': fmtPattern.append("%OV"); continue;
														case 'w': fmtPattern.append("%Ow"); continue;
														case 'W': fmtPattern.append("%OW"); continue;
														case 'y': fmtPattern.append("%Oy"); continue;
														default:
															throw std::runtime_error("Localized Time Modifier Detected But Next Character Is Not A Modifiable Flag");
													}
											}
										case 'F': fmtPattern.append("%F"); continue;
										case 'G': fmtPattern.append("%G"); continue;
										case 'H': fmtPattern.append("%H"); continue;
										case 'I': fmtPattern.append("%I"); continue;
										case 'M': fmtPattern.append("%M"); continue;
										case 'R': fmtPattern.append("%R"); continue;
										case 'S': fmtPattern.append("%S"); continue;
										case 'T':
											{
												fmtPattern.append("%T");
												if( ++pos >= size ) {
														fmtPattern += '}';
														return;
												}
												if( pattern[ pos ] != '.' ) {
														fmtPattern += '}';
														break;
												}
												if( ++pos >= size ) {
														throw std::runtime_error("Error In User Defined Format String: Subsecond Precision Detected For Flag '%T' "
														                         "Without Any Modifiers");
														fmtPattern += '}';
														return;
												}
												fmtPattern += '.';
												for( ;; ) {
														if( !IsDigit(pattern[ pos ]) ) break;
														fmtPattern += pattern[ pos ];
														if( ++pos >= size ) {
																fmtPattern += '}';
																break;
														}
													}
												continue;
											}
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
										case ' ':
											// this is just for consistency's sake and doesn't affect the funtionality
											if( ++pos >= size ) return;
											if( fmtPattern[ pos ] != '}' ) {
													--pos;
													fmtPattern += ' ';
													continue;
											} else {
													--pos;
													continue;
												}
										default:
											if( fmtPattern[ --pos ] == ' ' ) {
													// this is just for consistency's sake and doesn't affect the funtionality
													++pos;
													fmtPattern[ fmtPattern.size() - 1 ] = '}';
													fmtPattern += ' ';
											} else {
													fmtPattern += '}';
													break;
												}
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
	const std::string& Message_Formatter::Pattern() const {
		return fmtPattern;
	}

	SeFmtFuncFlags serenity::msg_details::Message_Formatter::FmtFunctionFlag() const {
		return fmtFlags;
	}

	source_flag serenity::msg_details::Message_Formatter::SourceFmtFlag() const {
		return sourceFlag;
	}

	int serenity::msg_details::Message_Formatter::ThreadFmtLength() const {
		return threadLength;
	}
}    // namespace serenity::msg_details
