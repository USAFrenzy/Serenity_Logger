#include <serenity/MessageDetails/Message_Formatter.h>

namespace serenity::msg_details {

	void LazyParseHelper::ClearBuffer() {
		resultBuffer.fill(0);
	}

	void LazyParseHelper::SetBracketPosition(bracket_type bracket, const size_t& pos) {
		switch( bracket ) {
				case bracket_type::open: openBracketPos = pos; break;
				case bracket_type::close: closeBracketPos = pos; break;
				default: break;
			}
	}

	size_t LazyParseHelper::BracketPosition(bracket_type bracket) const {
		switch( bracket ) {
				case bracket_type::open: return openBracketPos; break;
				case bracket_type::close: return closeBracketPos; break;
				default: break;
			}
	}

	std::array<char, SERENITY_ARG_BUFFER_SIZE>& LazyParseHelper::ConversionResultBuffer() {
		return resultBuffer;
	}

	const std::to_chars_result LazyParseHelper::ConversionResultInfo() const {
		return result;
	}

	std::string& LazyParseHelper::StringBuffer() {
		return temp;
	}

	void LazyParseHelper::SetConversionResult(const std::to_chars_result& convResult) {
		result = convResult;
	}

	void LazyParseHelper::SetPartition(partition_type pType, std::string_view sv) {
		switch( pType ) {
				case partition_type::primary:
					partitionUpToArg.clear();
					partitionUpToArg.append(sv.data(), sv.size());
					break;
				case partition_type::remainder:
					remainder.clear();
					remainder.append(sv.data(), sv.size());
					break;
			}
	}

	std::string& LazyParseHelper::PartitionString(partition_type pType) {
		switch( pType ) {
				case partition_type::primary: return partitionUpToArg; break;
				case partition_type::remainder: return remainder; break;
			}
	}

	const std::vector<ArgContainer::LazilySupportedTypes>& ArgContainer::ArgStorage() const {
		return argContainer;
	}

	LazyParseHelper& ArgContainer::ParseHelper() {
		return parseHelper;
	}

	void ArgContainer::Reset() {
		argContainer.clear();
		argIndex = maxIndex = 0;
		endReached          = false;
	}

	void ArgContainer::AdvanceToNextArg() {
		if( argIndex < maxIndex ) {
				++argIndex;
		} else {
				endReached = true;
			}
	}

	bool ArgContainer::ContainsArgSpecs(const std::string_view fmt) {
		auto size { fmt.size() };
		std::string_view argBracket;
		using B_Type = LazyParseHelper::bracket_type;
		for( size_t i { 0 }; i < size; ++i ) {
				argBracket = "";
				if( fmt.at(i) == '{' ) {
						parseHelper.SetBracketPosition(B_Type::open, fmt.find_first_of('{'));
						parseHelper.SetBracketPosition(B_Type::close, fmt.find_first_of('}'));

						if( (parseHelper.BracketPosition(B_Type::open) != std::string_view::npos) &&
						    (parseHelper.BracketPosition(B_Type::close) != std::string_view::npos) )
							{
								argBracket = std::move(fmt.substr(parseHelper.BracketPosition(B_Type::open),
								                                  parseHelper.BracketPosition(B_Type::close) + 1));
						}
						auto argBracketSize { argBracket.size() };
						switch( argBracketSize ) {
								case 0: break;
								case 1: break;
								case 2: break;
								case 3:
									if( argBracket.at(1) != ' ' ) {
											return true;
									}
									break;
								default:
									argBracket.remove_prefix(1);
									argBracket.remove_suffix(1);
									for( auto& ch: argBracket ) {
											if( ch != ' ' ) {
													return true;
											}
										}
									break;
							}
						if( argBracketSize == size ) break;
				}
			}
		return false;
	}

	/*************************************** Variant Order *******************************************
	 * [0] std::monostate, [1] std::string, [2] const char*, [3] std::string_view, [4] int,
	 * [5] unsigned int, [6] long long, [7] unsigned long long, [8] bool, [9] char, [10] float,
	 * [11] double, [12] long double, [13] const void*
	 ************************************************************************************************/
	/*************************************************************************************************/
	// TODO: As far as any more optimizations go, this function eats up ~3x more cpu cycles compared
	// TODO: to the very next cpu hungry function given a test of 3 empty specifier arguments (an int,
	// TODO: a float, and a string). Given the timings, this may not be any real issue, but it'd be
	// TODO: cool to see how I may be able to speed this up as well since any gains here are massive
	// TODO: gains everywhere else.
	/*************************************************************************************************/
	std::string&& ArgContainer::GetArgValue() {
		auto& strRef { parseHelper.StringBuffer() };
		strRef.clear();
		parseHelper.ClearBuffer();
		auto& arg { argContainer[ argIndex ] };
		auto& buffer { parseHelper.ConversionResultBuffer() };
		auto& result { parseHelper.ConversionResultInfo() };

		switch( arg.index() ) {
				case 0: return std::move(strRef); break;
				case 1: return std::move(strRef.append(std::move(std::get<1>(arg)))); break;
				case 2: return std::move(strRef.append(std::move(std::get<2>(arg)))); break;
				case 3: return std::move(strRef.append(std::move(std::get<3>(arg)))); break;
				case 4:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<4>(arg)));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 5:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<5>(arg)));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 6:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<6>(arg)));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 7:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<7>(arg)));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 8:
					strRef.append(std::get<8>(arg) == true ? "true" : "false");
					return std::move(strRef);
					break;
				case 9:
					strRef += std::move(std::get<9>(arg));
					return std::move(strRef);
					break;
				case 10:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<10>(arg)));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 11:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<11>(arg)));

					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 12:
					parseHelper.SetConversionResult(
					std::to_chars(buffer.data(), buffer.data() + buffer.size(), std::get<12>(arg)));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				case 13:
					// Couldn't get this to work with dynamic_cast, but reinterpret_cast at least isn't giving any issues.
					// Still need to test that this works as intended; changed base 10 to 16 for 0-F addressing.
					parseHelper.SetConversionResult(std::to_chars(buffer.data(), buffer.data() + buffer.size(),
					                                              reinterpret_cast<size_t>(std::get<13>(arg)), 16));
					if( result.ec != std::errc::value_too_large ) {
							strRef.append("0x").append(buffer.data(), buffer.size());
					}
					return std::move(strRef);
					break;
				default: return std::move(strRef); break;
			}
	}

	bool ArgContainer::EndReached() const {
		return endReached;
	}

	bool ArgContainer::ContainsUnsupportedType() const {
		return containsUnknownType;
	}

	Message_Formatter::Message_Formatter(std::string_view pattern, Message_Info* details)
		: msgInfo(*&details), localeRef(&details->GetLocale()) {
		SetPattern(pattern);
#ifdef WINDOWS_PLATFORM
		platformEOL = LineEnd::windows;
#elif defined MAC_PLATFORM
		platformEOL = LineEnd::mac;
#else
		platformEOL = LineEnd::unix;
#endif    // WINDOWS_PLATFORM
	}

	// Generally speaking, I would've liked to keep these as function pointers to
	// formatting functions, but doing that led to a ~0.3us pessimization.
	// TODO: See if I can still somehow make vector of function pointers just as
	// fast as current vector of structs.
	// TODO: Reason being that it would make life really easy to add custom
	// formatting arguments from user later on...
	// TODO: Not to mention, It would make upkeep a bit easier since I could rework
	// this to take in a fptr and just
	// TODO: focus on the actual function if changes need to be made (I guess it
	// comes down to what's more convenient)
	void Message_Formatter::FlagFormatter(size_t flag) {
		switch( flag ) {
				case 0: formatter.Emplace_Back(std::make_unique<Format_Arg_a>(*msgInfo)); break;
				case 1: formatter.Emplace_Back(std::make_unique<Format_Arg_b>(*msgInfo)); break;
				case 2: formatter.Emplace_Back(std::make_unique<Format_Arg_d>(*msgInfo)); break;
				case 3: formatter.Emplace_Back(std::make_unique<Format_Arg_l>(*msgInfo)); break;
				case 4: formatter.Emplace_Back(std::make_unique<Format_Arg_n>(*msgInfo)); break;
				case 5: formatter.Emplace_Back(std::make_unique<Format_Arg_t>(*msgInfo)); break;
				case 6: formatter.Emplace_Back(std::make_unique<Format_Arg_w>(*msgInfo)); break;
				case 7: formatter.Emplace_Back(std::make_unique<Format_Arg_x>(*msgInfo)); break;
				case 8: formatter.Emplace_Back(std::make_unique<Format_Arg_y>(*msgInfo)); break;
				case 9: formatter.Emplace_Back(std::make_unique<Format_Arg_A>(*msgInfo)); break;
				case 10: formatter.Emplace_Back(std::make_unique<Format_Arg_B>(*msgInfo)); break;
				case 11: formatter.Emplace_Back(std::make_unique<Format_Arg_D>(*msgInfo)); break;
				case 12: formatter.Emplace_Back(std::make_unique<Format_Arg_F>(*msgInfo)); break;
				case 13: formatter.Emplace_Back(std::make_unique<Format_Arg_H>(*msgInfo)); break;
				case 14: formatter.Emplace_Back(std::make_unique<Format_Arg_L>(*msgInfo)); break;
				case 15: formatter.Emplace_Back(std::make_unique<Format_Arg_M>(*msgInfo)); break;
				case 16: formatter.Emplace_Back(std::make_unique<Format_Arg_N>(*msgInfo)); break;
				case 17: formatter.Emplace_Back(std::make_unique<Format_Arg_S>(*msgInfo)); break;
				case 18: formatter.Emplace_Back(std::make_unique<Format_Arg_T>(*msgInfo)); break;
				case 19: formatter.Emplace_Back(std::make_unique<Format_Arg_X>(*msgInfo)); break;
				case 20: formatter.Emplace_Back(std::make_unique<Format_Arg_Y>(*msgInfo)); break;
				case 21: formatter.Emplace_Back(std::make_unique<Format_Arg_Message>(*msgInfo)); break;

				default:
					// if arg after "%" isn't a flag handled here, do nothing
					// In the future, check to see if it's a user defined format func
					break;
			}
	}

	void Message_Formatter::SetPattern(std::string_view pattern) {
		fmtPattern.clear();
		fmtPattern.append(pattern.data(), pattern.size());
		StoreFormat();
	}

	Message_Formatter::Formatters& Message_Formatter::GetFormatters() {
		return formatter;
	}

	void Message_Formatter::Formatters::Clear() {
		m_Formatter.clear();
	}

	// Initialize for no conflicts with FlagFormatter() for user defined format funcs if implemented later on
	constexpr unsigned int MAX_INDEX = 128;

	void Message_Formatter::StoreFormat() {
		std::string fmt { fmtPattern };
		std::string flag;
		size_t index { MAX_INDEX };
		formatter.Clear();

		while( !fmt.empty() ) {
				if( fmt.front() == '%' ) {
						flag.clear();
						flag.append(fmt.substr(0, 2));
						auto position = std::find(SERENITY_LUTS::allValidFlags.begin(),
						                          SERENITY_LUTS::allValidFlags.end(), flag);
						if( position != SERENITY_LUTS::allValidFlags.end() ) {
								index = std::distance(SERENITY_LUTS::allValidFlags.begin(), position);
						}
						FlagFormatter(index);
						fmt.erase(0, flag.size());
						if( fmt.empty() ) {
								break;
						}
				} else {
						formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(fmt.substr(0, 1)));
						fmt.erase(0, 1);
						if( fmt.empty() ) {
								break;
						}
					}
			}
	}

	void Message_Formatter::LazySubstitute(std::string& msg, std::string arg) {
		std::string_view temp { msg };
		auto& parseHelper { argStorage.ParseHelper() };
		bool bracketPositionsValid;
		using B_Type = LazyParseHelper::bracket_type;
		using P_Type = LazyParseHelper::partition_type;

		parseHelper.SetBracketPosition(B_Type::open, temp.find_first_of(static_cast<char>('{')));
		parseHelper.SetBracketPosition(B_Type::close, temp.find_first_of(static_cast<char>(' }')));
		bracketPositionsValid = ((parseHelper.BracketPosition(B_Type::open) != std::string::npos) &&
		                         (parseHelper.BracketPosition(B_Type::close) != std::string::npos));
		if( bracketPositionsValid ) {
				parseHelper.SetPartition(P_Type::primary, temp.substr(0, parseHelper.BracketPosition(B_Type::open)));
				temp.remove_prefix(parseHelper.BracketPosition(B_Type::close) + 1);
				parseHelper.SetPartition(P_Type::remainder, temp);
		}
		msg.clear();
		msg.append(std::move(parseHelper.PartitionString(P_Type::primary))
		           .append(arg.data(), arg.size())
		           .append(std::move(parseHelper.PartitionString(P_Type::remainder))));
	}

	const Message_Info* Message_Formatter::MessageDetails() {
		return msgInfo;
	}

	std::string Message_Formatter::Formatter::UpdateInternalView() {
		return "";
	}

	// Format_a Functions
	Message_Formatter::Format_Arg_a::Format_Arg_a(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastHour(cacheRef.tm_hour) {
		hour = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_a::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		auto hr { lastHour };
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		std::string_view paddedHour { (hr > 12) ? SERENITY_LUTS::numberStr[ static_cast<int64_t>(hr) - 12 ]
			                                : SERENITY_LUTS::numberStr[ hr ] };
		return std::string { paddedHour.data(), paddedHour.size() };
	}

	std::string_view Message_Formatter::Format_Arg_a::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? hour = std::move(UpdateInternalView()) : hour;
	}

	// Format_b Functions
	Message_Formatter::Format_Arg_b::Format_Arg_b(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastMonth(cacheRef.tm_mon) {
		month = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_b::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		std::string_view paddedMonth { SERENITY_LUTS::numberStr[ lastMonth ] };
		return std::string { paddedMonth.data(), paddedMonth.size() };
	}

	std::string_view Message_Formatter::Format_Arg_b::FormatUserPattern() {
		return (cacheRef.tm_mon != lastMonth) ? month = std::move(UpdateInternalView()) : month;
	}

	// Format_d Functions
	Message_Formatter::Format_Arg_d::Format_Arg_d(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastDay(cacheRef.tm_mday) {
		day = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_d::UpdateInternalView() {
		lastDay = cacheRef.tm_mday;
		std::string_view paddedDay { SERENITY_LUTS::numberStr[ lastDay ] };
		return std::string { paddedDay.data(), paddedDay.size() };
	}

	std::string_view Message_Formatter::Format_Arg_d::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? day = std::move(UpdateInternalView()) : day;
	}

	// Format_l Functions
	Message_Formatter::Format_Arg_l::Format_Arg_l(Message_Info& info): levelRef(info.MsgLevel()) {
		levelStr = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_l::UpdateInternalView() {
		lastLevel = levelRef;
		auto lvl { LevelToShortView(lastLevel) };
		return std::string { lvl.data(), lvl.size() };
	}

	std::string_view Message_Formatter::Format_Arg_l::FormatUserPattern() {
		return (levelRef != lastLevel) ? levelStr = std::move(UpdateInternalView()) : levelStr;
	}

	// Format_n Functions
	Message_Formatter::Format_Arg_n::Format_Arg_n(Message_Info& info)
		: timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		ddmmyy = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_n::UpdateInternalView() {
		lastDay = cacheRef.tm_mday;
		std::string result;
		auto day { SERENITY_LUTS::numberStr[ cacheRef.tm_mday ] };
		auto month { SERENITY_LUTS::short_months[ cacheRef.tm_mon ] };
		auto year { timeRef.GetCurrentYearSV(true) };
		return result.append(day).append(month).append(year);
	}

	std::string_view Message_Formatter::Format_Arg_n::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? ddmmyy = std::move(UpdateInternalView()) : ddmmyy;
	}

	// Format_t Functions
	Message_Formatter::Format_Arg_t::Format_Arg_t(Message_Info& info)
		: cacheRef(info.TimeDetails().Cache()), lastMin(info.TimeDetails().Cache().tm_min) {
		hmStr = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_t::UpdateInternalView() {
		lastMin = cacheRef.tm_min;
		auto hr { cacheRef.tm_hour };    // just to avoid another lookup, make local copy
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		auto hour { (hr > 12) ? SERENITY_LUTS::numberStr[ static_cast<int64_t>(hr) - 12 ] : SERENITY_LUTS::numberStr[ hr ] };
		auto min { SERENITY_LUTS::numberStr[ lastMin ] };
		std::string result;
		return result.append(hour).append(":").append(min);
	}

	std::string_view Message_Formatter::Format_Arg_t::FormatUserPattern() {
		auto sec = SERENITY_LUTS::numberStr[ cacheRef.tm_sec ];
		if( cacheRef.tm_min != lastMin ) {
				auto result { hmStr = std::move(UpdateInternalView()) };
				return result.append(":").append(sec);
		} else {
				auto result { hmStr };
				return result.append(":").append(sec);
			}
	}

	// Format_w Functions
	Message_Formatter::Format_Arg_w::Format_Arg_w(Message_Info& info): cacheRef(info.TimeDetails().Cache()) {
		lastDecDay = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_w::UpdateInternalView() {
		lastDay = cacheRef.tm_wday;
		return std::move(std::vformat("{}", std::make_format_args(lastDay)));
	}

	std::string_view Message_Formatter::Format_Arg_w::FormatUserPattern() {
		return (cacheRef.tm_wday != lastDay) ? lastDecDay = std::move(UpdateInternalView()) : lastDecDay;
	}

	// Format_x Functions
	Message_Formatter::Format_Arg_x::Format_Arg_x(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastWkday(cacheRef.tm_wday) {
		wkday = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_x::UpdateInternalView() {
		lastWkday = cacheRef.tm_wday;
		std::string_view sWkday { SERENITY_LUTS::short_weekdays[ lastWkday ] };
		return std::string { sWkday.data(), sWkday.size() };
	}

	std::string_view Message_Formatter::Format_Arg_x::FormatUserPattern() {
		return (lastWkday != cacheRef.tm_wday) ? wkday = std::move(UpdateInternalView()) : wkday;
	}

	// Format_y Functions
	Message_Formatter::Format_Arg_y::Format_Arg_y(Message_Info& info)
		: timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastYear(cacheRef.tm_year) {
		year = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_y::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto yr { timeRef.GetCurrentYearSV(true) };
		return std::string { yr.data(), yr.size() };
	}

	std::string_view Message_Formatter::Format_Arg_y::FormatUserPattern() {
		return (cacheRef.tm_year != lastYear) ? year = std::move(UpdateInternalView()) : year;
	}

	// Format_A Functions
	Message_Formatter::Format_Arg_A::Format_Arg_A(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastHour(cacheRef.tm_hour) {
		dayHalf = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_A::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		std::string_view dHalf { (lastHour >= 12) ? "PM" : "AM" };
		return std::string { dHalf.data(), dHalf.size() };
	}

	std::string_view Message_Formatter::Format_Arg_A::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? dayHalf = std::move(UpdateInternalView()) : dayHalf;
	}

	// Format_B Functions
	Message_Formatter::Format_Arg_B::Format_Arg_B(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastMonth(cacheRef.tm_mon) {
		month = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_B::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		std::string_view lMonth { SERENITY_LUTS::long_months[ lastMonth ] };
		return std::string { lMonth.data(), lMonth.size() };
	}

	std::string_view Message_Formatter::Format_Arg_B::FormatUserPattern() {
		return (cacheRef.tm_mon != lastMonth) ? month = std::move(UpdateInternalView()) : month;
	}

	// Format_D Functions
	Message_Formatter::Format_Arg_D::Format_Arg_D(Message_Info& info)
		: timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		mmddyy = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_D::UpdateInternalView() {
		std::string result;
		lastDay = cacheRef.tm_mday;
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		auto m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto d { SERENITY_LUTS::numberStr[ lastDay ] };
		auto y { timeRef.GetCurrentYearSV(true) };
		return result.append(m).append("/").append(d).append("/").append(y);
	}

	std::string_view Message_Formatter::Format_Arg_D::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? mmddyy = std::move(UpdateInternalView()) : mmddyy;
	}

	// Format_F Functions
	Message_Formatter::Format_Arg_F::Format_Arg_F(Message_Info& info)
		: timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		yymmdd = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_F::UpdateInternalView() {
		std::string result;
		lastDay = cacheRef.tm_mday;
		auto y { timeRef.GetCurrentYearSV(true) };
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		auto m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto d { SERENITY_LUTS::numberStr[ lastDay ] };
		return result.append(y).append("-").append(m).append("-").append(d);
	}

	std::string_view Message_Formatter::Format_Arg_F::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? yymmdd = std::move(UpdateInternalView()) : yymmdd;
	}

	// Format_H Functions
	Message_Formatter::Format_Arg_H::Format_Arg_H(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastHour(cacheRef.tm_hour) {
		hour = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_H::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		std::string_view hr { SERENITY_LUTS::numberStr[ lastHour ] };
		return std::string { hr.data(), hr.size() };
	}

	std::string_view Message_Formatter::Format_Arg_H::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? hour = std::move(UpdateInternalView()) : hour;
	}

	// Format_L Functions
	Message_Formatter::Format_Arg_L::Format_Arg_L(Message_Info& info): levelRef(info.MsgLevel()) {
		levelStr = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_L::UpdateInternalView() {
		lastLevel = levelRef;
		auto lvl { LevelToLongView(lastLevel) };
		return std::string { lvl.data(), lvl.size() };
	}

	std::string_view Message_Formatter::Format_Arg_L::FormatUserPattern() {
		return (levelRef != lastLevel) ? levelStr = std::move(UpdateInternalView()) : levelStr;
	}

	// Format_M Functions
	Message_Formatter::Format_Arg_M::Format_Arg_M(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastMin(cacheRef.tm_min) {
		min = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_M::UpdateInternalView() {
		lastMin = cacheRef.tm_min;
		std::string_view minute { SERENITY_LUTS::numberStr[ lastMin ] };
		return std::string { minute.data(), minute.size() };
	}

	std::string_view Message_Formatter::Format_Arg_M::FormatUserPattern() {
		return (cacheRef.tm_min != lastMin) ? min = std::move(UpdateInternalView()) : min;
	}

	// Format_N Functions
	Message_Formatter::Format_Arg_N::Format_Arg_N(Message_Info& info): name(info.Name()) { }

	std::string_view Message_Formatter::Format_Arg_N::FormatUserPattern() {
		return name;
	}

	// Format_S Functions
	Message_Formatter::Format_Arg_S::Format_Arg_S(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastSec(cacheRef.tm_sec) {
		sec = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_S::UpdateInternalView() {
		lastSec = cacheRef.tm_sec;
		std::string_view second { SERENITY_LUTS::numberStr[ lastSec ] };
		return std::string { second.data(), second.size() };
	}

	std::string_view Message_Formatter::Format_Arg_S::FormatUserPattern() {
		return (cacheRef.tm_sec != lastSec) ? sec = std::move(UpdateInternalView()) : sec;
	}

	// Format_T Functions
	Message_Formatter::Format_Arg_T::Format_Arg_T(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastMin(cacheRef.tm_min) {
		hmStr = UpdateInternalView();
	}

	std::string Message_Formatter::Format_Arg_T::UpdateInternalView() {
		lastMin = cacheRef.tm_min;
		auto hour { SERENITY_LUTS::numberStr[ cacheRef.tm_hour ] };
		auto min { SERENITY_LUTS::numberStr[ lastMin ] };
		std::string result { hour };
		return result.append(":").append(min);
	}

	std::string_view Message_Formatter::Format_Arg_T::FormatUserPattern() {
		auto sec = SERENITY_LUTS::numberStr[ cacheRef.tm_sec ];
		if( cacheRef.tm_min != lastMin ) {
				hmStr = UpdateInternalView();
		}
		std::string result { hmStr };
		return result.append(":").append(sec);
	}

	// Format_X Functions
	Message_Formatter::Format_Arg_X::Format_Arg_X(Message_Info& info): cacheRef(info.TimeDetails().Cache()), lastWkday(cacheRef.tm_wday) {
		wkday = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_X::UpdateInternalView() {
		lastWkday = cacheRef.tm_wday;
		std::string_view lWkday { SERENITY_LUTS::long_weekdays[ lastWkday ] };
		return std::string { lWkday.data(), lWkday.size() };
	}

	std::string_view Message_Formatter::Format_Arg_X::FormatUserPattern() {
		return (cacheRef.tm_wday != lastWkday) ? wkday = std::move(UpdateInternalView()) : wkday;
	}

	// Format_Y Functions
	Message_Formatter::Format_Arg_Y::Format_Arg_Y(Message_Info& info)
		: timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastYear(cacheRef.tm_year) {
		year = std::move(UpdateInternalView());
	}

	std::string Message_Formatter::Format_Arg_Y::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto yr { timeRef.GetCurrentYearSV(lastYear) };
		return std::string { yr.data(), yr.size() };
	}

	std::string_view Message_Formatter::Format_Arg_Y::FormatUserPattern() {
		return (cacheRef.tm_year != lastYear) ? year = std::move(UpdateInternalView()) : year;
	}

	// Format_Message Functions
	Message_Formatter::Format_Arg_Message::Format_Arg_Message(Message_Info& info): message(info.Message()) { }

	std::string_view Message_Formatter::Format_Arg_Message::FormatUserPattern() {
		return message;
	}

	// Format_Char Functions
	Message_Formatter::Format_Arg_Char::Format_Arg_Char(std::string_view ch): m_char(ch.data(), ch.size()) { }

	std::string_view Message_Formatter::Format_Arg_Char::FormatUserPattern() {
		return m_char;
	}

	// Formatters Functions
	Message_Formatter::Formatters::Formatters(std::vector<std::unique_ptr<Formatter>>&& container): m_Formatter(std::move(container)) {
		// Reserve an estimated amount based off arg sizes
		localBuffer.reserve(m_Formatter.size() * 32);
	}

	void Message_Formatter::Formatters::Emplace_Back(std::unique_ptr<Formatter>&& formatter) {
		m_Formatter.emplace_back(std::move(formatter));
		// Reserve an estimated amount based off arg sizes
		localBuffer.reserve(m_Formatter.size() * 32);
	}

	std::string_view Message_Formatter::Formatters::FormatUserPattern() {
		localBuffer.clear();
		for( auto& formatter: m_Formatter ) {
				auto formatted { formatter->FormatUserPattern() };
				localBuffer.append(formatted.data(), formatted.size());
			}
		return localBuffer;
	}

}    // namespace serenity::msg_details
