#include <serenity/MessageDetails/Message_Formatter.h>

#include <iostream>

#define EXPERIMENTAL_STREAMLINE 0

namespace serenity::msg_details {

	void Message_Formatter::SetLocaleReference(const std::locale& loc) {
		localeRef = loc;
	}

	std::locale Message_Formatter::Locale() {
		return localeRef;
	}

	Message_Formatter::Message_Formatter(std::string_view pattern, Message_Info* details)
		: msgInfo(*&details), localeRef(globals::default_locale), sourceFlag(source_flag::empty) {
		SetPattern(pattern);
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
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

	void Message_Formatter::FlagFormatter(size_t index, size_t precision) {
		switch( index ) {
				case 0:
					formatter.Emplace_Back(std::make_unique<Format_Arg_a>(*msgInfo));
					formatter.reserveCapacity += 3;
					break;
				case 1:
					formatter.Emplace_Back(std::make_unique<Format_Arg_b>(*msgInfo));
					formatter.reserveCapacity += 3;
					break;
				case 2:
					formatter.Emplace_Back(std::make_unique<Format_Arg_c>(*msgInfo));
					formatter.reserveCapacity += 24;
					break;
				case 3:
					formatter.Emplace_Back(std::make_unique<Format_Arg_d>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 4:
					formatter.Emplace_Back(std::make_unique<Format_Arg_e>(precision));
					formatter.reserveCapacity += 10;
					break;
				case 5:
					formatter.Emplace_Back(std::make_unique<Format_Arg_b>(*msgInfo));
					formatter.reserveCapacity += 3;
					break;
				case 6:
					formatter.Emplace_Back(std::make_unique<Format_Arg_l>(*msgInfo));
					++formatter.reserveCapacity;
					break;
				case 7:
					formatter.Emplace_Back(std::make_unique<Format_Arg_m>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 8:
					formatter.Emplace_Back(std::make_unique<Format_Arg_n>(*msgInfo));
					formatter.reserveCapacity += 7;
					break;
				case 9:
					formatter.Emplace_Back(std::make_unique<Format_Arg_p>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 10:
					formatter.Emplace_Back(std::make_unique<Format_Arg_r>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 11:
					formatter.Emplace_Back(std::make_unique<Format_Arg_s>(*msgInfo, sourceFlag));
					formatter.reserveCapacity += 2;
					break;
				case 12:
					formatter.Emplace_Back(std::make_unique<Format_Arg_t>(precision));
					formatter.reserveCapacity += 8;
					break;
				case 13:
					formatter.Emplace_Back(std::make_unique<Format_Arg_w>(*msgInfo));
					++formatter.reserveCapacity;
					break;
				case 14:
					formatter.Emplace_Back(std::make_unique<Format_Arg_D>(*msgInfo));
					formatter.reserveCapacity += 8;
					break;
				case 15:
					formatter.Emplace_Back(std::make_unique<Format_Arg_y>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 16:
					formatter.Emplace_Back(std::make_unique<Format_Arg_z>(*msgInfo));
					formatter.reserveCapacity += 4;
					break;
				case 17:
					formatter.Emplace_Back(std::make_unique<Format_Arg_A>(*msgInfo));
					formatter.reserveCapacity += 10;
					break;
				case 18:
					formatter.Emplace_Back(std::make_unique<Format_Arg_B>(*msgInfo));
					formatter.reserveCapacity += 10;
					break;
				case 19:
					formatter.Emplace_Back(std::make_unique<Format_Arg_C>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 20:
					formatter.Emplace_Back(std::make_unique<Format_Arg_D>(*msgInfo));
					formatter.reserveCapacity += 8;
					break;
				case 21:
					formatter.Emplace_Back(std::make_unique<Format_Arg_F>(*msgInfo));
					formatter.reserveCapacity += 10;
					break;
				case 22:
					formatter.Emplace_Back(std::make_unique<Format_Arg_H>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 23:
					formatter.Emplace_Back(std::make_unique<Format_Arg_I>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 24:
					formatter.Emplace_Back(std::make_unique<Format_Arg_L>(*msgInfo));
					formatter.reserveCapacity += 6;
					break;
				case 25:
					formatter.Emplace_Back(std::make_unique<Format_Arg_M>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 26:
					formatter.Emplace_Back(std::make_unique<Format_Arg_N>(*msgInfo));
					formatter.reserveCapacity += msgInfo->Name().size();
					break;
				case 27:
					formatter.Emplace_Back(std::make_unique<Format_Arg_R>(*msgInfo));
					formatter.reserveCapacity += 5;
					break;
				case 28:
					formatter.Emplace_Back(std::make_unique<Format_Arg_S>(*msgInfo));
					formatter.reserveCapacity += 2;
					break;
				case 29:
					formatter.Emplace_Back(std::make_unique<Format_Arg_T>(*msgInfo));
					formatter.reserveCapacity += 8;
					break;
				case 30:
					formatter.Emplace_Back(std::make_unique<Format_Arg_Y>(*msgInfo));
					formatter.reserveCapacity += 4;
					break;
				case 31:
					formatter.Emplace_Back(std::make_unique<Format_Arg_T>(*msgInfo));
					formatter.reserveCapacity += 8;
					break;
				case 32:
					formatter.Emplace_Back(std::make_unique<Format_Arg_Z>(*msgInfo));
					formatter.reserveCapacity += 4;
					break;
				case 33:
					formatter.Emplace_Back(std::make_unique<Format_Arg_Message>(*msgInfo));
					// this is most likely where additional allocations would be made so reserve is called on the formatting side
					break;
				default: break;
			}

				// TODO: Come back to this if there's enough time to flesh this out. Need to just continue wrapping this project up instead of adding more work right
				// now.
				/**************************************************************  TESTING **************************************************************/
				// NOTE: I do get the appropriate string when all is settled -> now to figure out how to structure the formatting aspect and simplify the
				//              FormatterArgs functions. I could probably just add some logic to the current ArgFormatter class for when it encounters a '%'
				//              sign and call a separate function alltogether since verification of args in this case may not be needed? Either that or call a
				//              different validator function that would have to be implemented for these cases and the arguments supplied to the format func.
				//              One idea is just to update the std::tm cache and supply the cache to the formatter since all but logger level, logger name, and
				//              message use this. I could then just hard-code the order that the logger levels, name, and message appear in the formatter call.
				//              I could even supply overloads for combinations of what is needed.
				//              ************************************************************ EX ************************************************************
				//               - Where the call may be something like FormatUserPattern(...) -> with the overloads being a buffer, a string_view,  and then
				//                  any combo of std::tm, LoggerLevel, string_view (AND be able to reserve an appropriate size as well with these calls) such as:
				//              ****************************************************************************************************************************
				//              - se_format_to(buffer, internalFmt, TimeDetails()->Cache(), MsgInfo()->Level(), MsgInfo()->Name());
				//              - se_format_to(buffer, internalFmt, TimeDetails()->Cache(), MsgInfo()->Level());
				//              - se_format_to(buffer, internalFmt, TimeDetails()->Cache(), MsgInfo()->Name());
				//              - se_format_to(buffer, internalFmt, TimeDetails()->Cache());
				//              - se_format_to(buffer, internalFmt, MsgInfo()->Level());
				//              - se_format_to(buffer, internalFmt, MsgInfo()->Name());
				//              ****************************************************************************************************************************
				// NOTE 2: With the above idea, I could then focus on getting std::vector<> cases of formatting optimized and just format to the file
				//                  buffer directly with this; with the '%+' specifier in place, I could  then just straight-up format the message as-is  as well so
				//                  this would essentially go from formatting the time-stamp into a string by repeated calls of append and no real way of
				//                  reserving capacity before-hand, copying said string to the buffer, formatting the message into another string, copying that
				//                  string to the buffer, and then writing to the file when either the file size limit or page size has been matched to just simply
				//                  formatting everything in one go into the buffer and essentially only incurring the cost of writing to the file buffer itself.
				// NOTE 3: Now that I've fixed some of the speed problems associated with using a std::vector<char> with the se_format_to(), this idea
				//                   is much more realistic in the sense of writing directly to the file buffer if the target is a file type target given there's
				//                   basically no trade-off in formatting performance now.
				// NOTE 4: First thing to do here is implement Time Spec Formatting, but that still leaves the custom flags of  'L', 'l', 'n', 'N', 's', 'e', 't' and
				// '+'...
				//                  So for now, this will be left as-is since I still have no idea how to go about adding a callback for custom types. Adding time
				//                  spec formatting won't be hard at all - I just need to add a time spec char field in FormatterSpecs and the relevant formatter
				//                  function calls for the spec provided, it's just the damn custom formatting I need to figure out here... (which would be nice
				//                  since the whole ArgFormatter class would finally be considered a light-weight drop-in replacement for libfmt and <format> at that
				//                  point)

#if EXPERIMENTAL_STREAMLINE
		/*  AS OF  18SEP22, THIS IS A W.I.P FOR STREAMLINING USING THE NEW ADDITIONS TO ARGFORMATTER FOR CUSTOM FORMATTING AND TIME FORMATTING  */

		//! NOTE: The logic below is being implemented here, however, this logic, if found to work properly and more
		//! efficiently, will replace the logic in the call of StoreFormat() instead and eliminate this function entirely.

		// Placement  of the internal representation is temporarily function local until the contents below are fleshed out and working
		std::string internalFmt;
		/*
		    Initial thoughts here is to append all time-related flags into one substitution bracket ONLY if there are no spaces, other text, or custom flags
		    separating the flags in order to make the formatting call more efficient than several formatting calls of individual time flags.
		    -1-   If there is ANYTHING separating the time flags, then for ANY flag, a new substitution bracket will need to be created.
		    -2a- For each flag that is added to the internal representation, the bracket the flag corresponds to will use a positional index of where the formatting
		            content is stored in the args vector (the vector hasn't been implemented yet and may not even be implemented -> See note at 5a & 5b ).
		    -2b- If the content for the flag to reference isn't already in the args vector, then a pointer will be stored in the vector so the flag can reference the
		             appropriate material from the target that was created.
		    -3-   I.E.  the internal representation might look like: " [{0:%l}] {1:%T} {1:%d%M%y} {2:%+}"
		             - where '0' here is a pointer to the logger level itself, '2'  is a pointer to the message contents, and '1' references the cache as a whole.
		    -4-    This should allow the unfolding of  args content with the formatting pattern in one call vs several individual calls.
		    -5a-  An alternative to this is to simply create the substituion string and have differing similar functions for the combinations that could be
		             present, similar to the old note above with the "se_format_to()" variations.
		    -5b-	 In this manner, the functions would have hard-coded placements of where the arguments are placed and the only thing that needs to be
		             tracked is if we are upgrading the function call to something more complex or not and what positional index is being referenced in each
		             argument bracket in the format string itself.
		*/

		/*************************************************************************************************************
		 * This is working as-is for creating the substitution brackets for a call to a yet-to-be-implemented formatting
		 * function call using both the cache and logger elements -> Still needs to be thoroughly tested though.
		 *************************************************************************************************************/
		auto size { fmtPattern.size() };
		auto pos { -1 };
		bool isTimeFlagProcessing { false };
		for( ;; ) {
				if( ++pos >= size ) break;
				if( fmtPattern[ pos ] == '%' ) {
						if( internalFmt.size() > 0 && internalFmt.back() == '%' ) {
								internalFmt += '%';
								continue;
						} else {
								const auto& ch { fmtPattern[ ++pos ] };
								if( IsCustomFlag(ch) ) {
										// Handle the custom flags here -> most likely by just instatiating the
										// structs or hard-coding the values in place that won't change
										if( internalFmt.size() > 0 && internalFmt.back() != '}' ) {
												isTimeFlagProcessing ? internalFmt.append("}{1:%").append(1, ch).append(1, '}')
																	 : internalFmt.append("{1:%").append(1, ch).append(1, '}');
												isTimeFlagProcessing = false;
										}

								} else if( IsTimeFlag(ch) ) {
										// check if current working set bracket is a time-related bracket and if the
										// conditions have been met, otherwise, create a new bracket for this flag
										if( isTimeFlagProcessing ) {
												internalFmt += ch;
										} else {
												internalFmt.append("{0:%").append(1, ch);
												isTimeFlagProcessing = true;
											}
								}
							}
				} else {
						//  if it's not a possible flag, just copy over any literals that didn't fall into the above two cases
						if( isTimeFlagProcessing ) {
								if( fmtPattern[ pos ] == ' ' ) {
										internalFmt += fmtPattern[ pos ];
										continue;
								}
								internalFmt += '}';
								isTimeFlagProcessing = false;
						}
						internalFmt += fmtPattern[ pos ];
					}
			}

		// This is just here to test the process out a little -> the %n isn't formatted specifically because that was originally a custom flag, but it DOES seem like
		// this is working as intended (loosely) for everything else? Don't have access to logger info and have yet to make a custom formatter template
		// specialization for the logger details/info to test that portion out, but I assume it should work as intended as well. Obviously will need to test that
		// once I can implement some more small changes further down the pipeline here.
		//! NOTE: AS OF RIGHT NOW - THE RESULT DUPLICATES THE TIME FLAG RESULTS DUE TO NOT HANDLING AND CATCHING AN INDEX THAT DOESN'T EXIST.
		//!               THE INTENDED BEHAVIOR IS TO WARN THE USER OF THIS AT THE TIME OF FORMATTING, HOWEVER, THE FORMATTER RE-USES THE LAST
		//!              ARGUMENT PROVIDED INSTEAD. WHILE THIS DOES INDEED PREVENT CRASHES, THIS IS A BUG IN THE DESIGN AND A WARNING NEEDS TO
		//!             BE ISSUED AND ANY FURTHER FORMATTING NEEDS TO BE HALTED AT THAT TIME -> ORIGINAL INTENT WAS TO THROW AN ERROR, BUT IT
		//!            MAY BE BETTER TO AT LEAST LET THE USER HANDLE THE ERROR (STILL PREVENT FURTHER FORMATTING EITHER WAY THOUGH).
		std::string tmp;
		std::tm tmpTime {};
		auto now { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
		LOCAL_TIME(tmpTime, now);
		VFORMAT_TO(tmp, localeRef, internalFmt, tmpTime /*  , loggerDetails */);
#endif
		/**************************************************************************************************************************************/
	}

	void Message_Formatter::SetPattern(std::string_view pattern) {
		fmtPattern.clear();
		fmtPattern.append(pattern.data(), pattern.size());
		StoreFormat();
		formatter.Reserve();
	}

	Message_Formatter::Formatters& Message_Formatter::GetFormatters() {
		return formatter;
	}

	void Message_Formatter::Formatters::Clear() {
		m_Formatter.clear();
	}

	void Message_Formatter::StoreFormat() {
		namespace lut = serenity::SERENITY_LUTS;
		temp.clear();
		formatter.Clear();
		temp.append(fmtPattern);
		while( temp.size() != 0 ) {
				if( temp.front() == '%' ) {
						auto flag { temp.substr(0, 2) };
						temp.erase(0, flag.size());
						auto position { std::find(lut::allValidFlags.begin(), lut::allValidFlags.end(), flag) };
						if( position == lut::allValidFlags.end() ) {
								formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(flag));
								continue;
						}
						size_t index { static_cast<size_t>(std::distance(lut::allValidFlags.begin(), position)) };
						FlagFormatter(index, ParseUserPatternForSpec(temp, index));
				} else {
						auto pos { temp.find_first_of('%') };
						if( pos == std::string::npos ) {
								formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(temp));
								temp.clear();
								continue;
						}
						std::string subStr { temp.substr(0, pos) };
						formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(subStr));
						temp.erase(0, subStr.size());
					}
			}
	}

	// Currently overkill since it's just validating one token
	void Message_Formatter::ValidateUserCharSpec(size_t index, std::vector<char> specs) {
		switch( index ) {
				case 11:
					if( specs.size() == 0 ) {
							sourceFlag |= source_flag::all;
							return;
					}
					for( auto& spec: specs ) {
							switch( spec ) {
									case 'l': sourceFlag |= source_flag::line; break;
									case 'c': sourceFlag |= source_flag::column; break;
									case 'f': sourceFlag |= source_flag::file; break;
									case 'F': sourceFlag |= source_flag::function; break;
									default: break;
								}
						}
					break;
				default: break;
			}
	}

	void Message_Formatter::ValidateUserPrecisionSpec(size_t index, size_t& value) {
		switch( index ) {
				case 4:
					if( (value > 9) || (value < 0) ) {
							printf("%s : Specifier \"%zu\"", precisionWarningMessage[ 0 ], value);
							value = defaultSubSecondPrecision;
					}
					break;
				case 10:
					if( (value > 10) || (value < 0) ) {
							printf("%s : Specifier \"%zu\"", precisionWarningMessage[ 1 ], value);
							value = defaultThreadIdLength;
					}
					break;
				default: break;
			}
	}

	static constexpr std::array<char, 4> validCharSpecs = { 'l', 'c', 'f', 'F' };

	size_t Message_Formatter::ParseUserPatternForSpec(std::string& parseStr, size_t index) {
		size_t tmpValue { 0 };
		if( parseStr.size() == 0 ) return tmpValue;
		std::vector<char> specsOutput {};

		if( parseStr.front() == ':' ) {
				auto ch { parseStr.at(1) };
				if( IsDigit(ch) ) {
						parseStr.erase(0, 1);
						std::string precision;
						for( ;; ) {
								if( !IsDigit(parseStr.front()) ) break;
								precision += parseStr.front();
								parseStr.erase(0, 1);
							}
						std::from_chars(precision.data(), precision.data() + precision.size(), tmpValue);
						ValidateUserPrecisionSpec(index, tmpValue);
				}
				if( IsAlpha(ch) ) {
						parseStr.erase(0, 1);
						for( ;; ) {
								auto ch { parseStr.front() };
								auto end { validCharSpecs.end() };
								if( !IsAlpha(ch) ) break;
								if( std::find(validCharSpecs.begin(), end, ch) == end ) break;
								specsOutput.emplace_back(ch);
								parseStr.erase(0, 1);
							}
						ValidateUserCharSpec(index, specsOutput);
				}
		}
		if( (specsOutput.size() == 0) && (tmpValue == 0) ) {
				switch( index ) {
						case 4: tmpValue = defaultSubSecondPrecision; break;
						case 10: tmpValue = defaultThreadIdLength; break;
						case 11: sourceFlag = source_flag::all; break;
						default: break;
					}
		}
		return tmpValue;
	}

	const Message_Info* Message_Formatter::MessageDetails() {
		return msgInfo;
	}
	// Formatters Functions
	Message_Formatter::Formatters::Formatters(std::vector<std::unique_ptr<Formatter>>&& container): m_Formatter(std::move(container)) { }

	void Message_Formatter::Formatters::Emplace_Back(std::unique_ptr<Formatter>&& formatter) {
		m_Formatter.emplace_back(std::move(formatter));
	}

	void Message_Formatter::Formatters::Reserve() {
		localBuffer.reserve(reserveCapacity);
	}

	std::string_view Message_Formatter::Formatters::FormatUserPattern() {
		localBuffer.clear();
		for( const auto& formatter: m_Formatter ) {
				formatter->FormatUserPattern(localBuffer);
			}
		return std::string_view(localBuffer.data(), localBuffer.size());
	}

	std::string_view serenity::msg_details::Message_Formatter::LineEnding() const {
		return SERENITY_LUTS::line_ending[ platformEOL ];
	}
}    // namespace serenity::msg_details
