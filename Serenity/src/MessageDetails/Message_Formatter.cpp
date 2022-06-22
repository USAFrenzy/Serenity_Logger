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
		SetPattern(pattern);
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
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

		// TODO: Come back to this if there's enough time to flesh this out. Need to just continue wrapping this project up instead of adding more work right now.
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
		//                   is much more realistic in the sense of writing directly to the file buffer if the target is a file type target given there's basically
		//                   no trade-off in formatting performance now.
		// NOTE 4: First thing to do here is implement Time Spec Formatting, but that still leaves the custom flags of  'L', 'l', 'n', 'N', 's', 'e', 't' and '+'...
		//                  So for now, this will be left as-is since I still have no idea how to go about adding a callback for custom types. Adding time spec
		//                  formatting won't be hard at all - I just need to add a time spec char field in FormatterSpecs and the relevant formatter function
		//                  calls for the spec provided, it's just the damn custom formatting I need to figure out here... (which would be nice since the whole
		//                  ArgFormatter class would finally be considered a light-weight drop-in replacement for libfmt and <format> at that point)

		// switch( index ) {
		//		case 0: internalFmt.append("{:%a}"); break;
		//		case 1: internalFmt.append("{:%b}"); break;
		//		case 2: internalFmt.append("{:%c}"); break;
		//		case 3: internalFmt.append("{:%d}"); break;
		//		case 4: internalFmt.append("{:%e}"); break;		/*  Custom Flag */
		//		case 5: internalFmt.append("{:%b}"); break;
		//		case 6: internalFmt.append("{:%l}"); break;		/*  Custom Flag */
		//		case 7: internalFmt.append("{:%m}"); break;
		//		case 8: internalFmt.append("{:%n}"); break;		/*  Custom Flag */
		//		case 9: internalFmt.append("{:%p}"); break;
		//		case 10: internalFmt.append("{:%r}"); break;
		//		case 11: internalFmt.append("{:%s}"); break;	/*  Custom Flag */
		//		case 12: internalFmt.append("{:%t}"); break;		/*  Custom Flag */
		//		case 13: internalFmt.append("{:%w}"); break;
		//		case 14: internalFmt.append("{:%D}"); break;
		//		case 15: internalFmt.append("{:%y}"); break;
		//		case 16: internalFmt.append("{:%z}"); break;
		//		case 17: internalFmt.append("{:%A}"); break;
		//		case 18: internalFmt.append("{:%B}"); break;
		//		case 19: internalFmt.append("{:%C}"); break;
		//		case 20: internalFmt.append("{:%D}"); break;
		//		case 21: internalFmt.append("{:%F}"); break;
		//		case 22: internalFmt.append("{:%H}"); break;
		//		case 23: internalFmt.append("{:%I}"); break;
		//		case 24: internalFmt.append("{:%L}"); break;	/*  Custom Flag */
		//		case 25: internalFmt.append("{:%M}"); break;
		//		case 26: internalFmt.append("{:%N}"); break;	/*  Custom Flag */
		//		case 27: internalFmt.append("{:%R}"); break;
		//		case 28: internalFmt.append("{:%S}"); break;
		//		case 29: internalFmt.append("{:%T}"); break;
		//		case 30: internalFmt.append("{:%Y}"); break;
		//		case 31: internalFmt.append("{:%T}"); break;
		//		case 32: internalFmt.append("{:%Z}"); break;
		//		case 33: internalFmt.append("{:%+}"); break;	/*  Custom Flag */
		//		default: break;
		//	}

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
