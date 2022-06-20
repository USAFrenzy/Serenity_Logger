#include <serenity/MessageDetails/FormatterArgs.h>

namespace serenity::msg_details {
	std::string& Formatter::UpdateInternalView() {
		result.clear();
		return result;
	}

	// Format %a Functions
	/*********************************************************************************************************************/
	Format_Arg_a::Format_Arg_a(Message_Info& info): cacheRef(info.TimeInfo()), lastWkday(cacheRef.tm_wday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_a::UpdateInternalView() {
		lastWkday = cacheRef.tm_wday;
		auto& sWkday { SERENITY_LUTS::short_weekdays[ lastWkday ] };
		result.clear();
		return result.append(sWkday.data(), sWkday.size());
	}

	void Format_Arg_a::FormatUserPattern(std::string& container) {
		(lastWkday != cacheRef.tm_wday) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %b functions
	/*********************************************************************************************************************/
	Format_Arg_b::Format_Arg_b(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) { }

	std::string& Format_Arg_b::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto& mon { SERENITY_LUTS::short_months[ lastMonth ] };
		result.clear();
		return result.append(mon.data(), mon.size());
	}

	void Format_Arg_b::FormatUserPattern(std::string& container) {
		(lastMonth != cacheRef.tm_wday) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %c
	/*********************************************************************************************************************/
	Format_Arg_c::Format_Arg_c(Message_Info& info): timeRef(info.TimeDetails()) { }

	void Format_Arg_c::FormatUserPattern(std::string& container) {
		auto& cache { timeRef.Cache() };
		auto& wkday { serenity::SERENITY_LUTS::short_weekdays[ cache.tm_wday ] };
		auto& mon { serenity::SERENITY_LUTS::short_months[ cache.tm_mon ] };
		auto& day { serenity::SERENITY_LUTS::numberStr[ cache.tm_mday ] };
		auto& hour { serenity::SERENITY_LUTS::numberStr[ cache.tm_hour ] };
		auto& min { serenity::SERENITY_LUTS::numberStr[ cache.tm_min ] };
		auto& sec { serenity::SERENITY_LUTS::numberStr[ cache.tm_sec ] };
		timeRef.CalculateCurrentYear(cache.tm_year);
		auto year { timeRef.GetCurrentYearSV(false) };
		container.append(wkday.data(), wkday.size())
		.append(" ")
		.append(mon.data(), mon.size())
		.append(" ")
		.append(day.data(), day.size())
		.append(" ")
		.append(hour.data(), hour.size())
		.append(":")
		.append(min.data(), min.size())
		.append(":")
		.append(sec.data(), sec.size())
		.append(" ")
		.append(year.data(), year.size());
	}

	/*********************************************************************************************************************/

	// Format %d Functions
	/*********************************************************************************************************************/
	Format_Arg_d::Format_Arg_d(Message_Info& info): cacheRef(info.TimeInfo()), lastDay(cacheRef.tm_mday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_d::UpdateInternalView() {
		lastDay = cacheRef.tm_mday;
		std::string_view paddedDay { SERENITY_LUTS::numberStr[ lastDay ] };
		result.clear();
		return result.append(paddedDay.data(), paddedDay.size());
	}

	void Format_Arg_d::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mday != lastDay) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %e Functions
	/*********************************************************************************************************************/
	Format_Arg_e::Format_Arg_e(size_t precision): buffer(std::array<char, defaultBufferSize> {}), m_precision(precision) { }

	void Format_Arg_e::FormatUserPattern(std::string& container) {
		std::fill(buffer.data(), buffer.data() + buffer.size(), '\0');
		// Note: It seems the trailing zeros are an OS limitation in how fine the clocks' granularity is.
		//       I haven't tested this on other platforms other than Windows at the moment though.
		auto subSeconds { std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count() };
		std::to_chars(buffer.data(), buffer.data() + buffer.size(), subSeconds);
		// Note: Ignoring the first 10 digits as those don't equate to the points of interest. The 10th digit
		//       is the second offset so start at the second offset and find the end of the precision digits.
		size_t bufferStartOffset { 10 };
		size_t i { bufferStartOffset };
		for( ;; ) {
				if( buffer[ i ] != '\0' )
					++i;
				else
					break;
			}
		auto startPoint { buffer.begin() };
		std::string_view sv { startPoint + bufferStartOffset, startPoint + i - (maxPrecision - m_precision) };
		container.append(sv.data(), sv.size());
	}
	/*********************************************************************************************************************/

	// Format %l Functions
	/*********************************************************************************************************************/
	Format_Arg_l::Format_Arg_l(Message_Info& info): levelRef(info.MsgLevel()) {
		UpdateInternalView();
	}

	std::string& Format_Arg_l::UpdateInternalView() {
		lastLevel = levelRef;
		auto lvl { LevelToShortView(lastLevel) };
		result.clear();
		return result.append(lvl.data(), lvl.size());
	}

	void Format_Arg_l::FormatUserPattern(std::string& container) {
		(levelRef != lastLevel) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %m Functions
	/*********************************************************************************************************************/
	Format_Arg_m::Format_Arg_m(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) {
		UpdateInternalView();
	}

	std::string& Format_Arg_m::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto& paddedMonth { SERENITY_LUTS::numberStr[ lastMonth ] };
		result.clear();
		return result.append(paddedMonth.data(), paddedMonth.size());
	}

	void Format_Arg_m::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mon != lastMonth) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %n Functions
	/*********************************************************************************************************************/
	Format_Arg_n::Format_Arg_n(Message_Info& info): timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_n::UpdateInternalView() {
		lastDay = cacheRef.tm_mday;
		result.clear();
		auto& day { SERENITY_LUTS::numberStr[ cacheRef.tm_mday ] };
		auto& month { SERENITY_LUTS::short_months[ cacheRef.tm_mon ] };
		auto year { timeRef.GetCurrentYearSV(true) };
		return result.append(day.data(), day.size()).append(month.data(), month.size()).append(year.data(), year.size());
	}

	void Format_Arg_n::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mday != lastDay) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %p Functions
	/*********************************************************************************************************************/
	Format_Arg_p::Format_Arg_p(Message_Info& info): cacheRef(info.TimeInfo()), lastHour(cacheRef.tm_hour) {
		UpdateInternalView();
	}

	std::string& Format_Arg_p::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		std::string_view dHalf { (lastHour >= 12) ? "PM" : "AM" };
		result.clear();
		return result.append(dHalf.data(), dHalf.size());
	}

	void Format_Arg_p::FormatUserPattern(std::string& container) {
		(cacheRef.tm_hour != lastHour) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %r Functions
	/*********************************************************************************************************************/
	Format_Arg_r::Format_Arg_r(Message_Info& info): cacheRef(info.TimeInfo()), lastMin(0) {
		UpdateInternalView();
	}

	std::string& Format_Arg_r::UpdateInternalView() {
		auto hr { cacheRef.tm_hour };
		auto ampm { hr >= 12 ? "pm" : "am" };
		if( lastMin != cacheRef.tm_min ) {
				lastMin = cacheRef.tm_min;
				hour    = (hr > 12) ? SERENITY_LUTS::numberStr[ static_cast<int64_t>(hr) - 12 ] : SERENITY_LUTS::numberStr[ hr ];
				min     = SERENITY_LUTS::numberStr[ lastMin ];
		}
		auto& sec = SERENITY_LUTS::numberStr[ cacheRef.tm_sec ];
		result.clear();
		return result.append(hour.data(), hour.size()).append(":").append(min.data(), min.size()).append(":").append(sec.data(), sec.size()).append(" ").append(ampm);
	}

	void Format_Arg_r::FormatUserPattern(std::string& container) {
		container.append(UpdateInternalView());
	}
	/*********************************************************************************************************************/

	// Format %s Functions
	/*********************************************************************************************************************/
	Format_Arg_s::Format_Arg_s(Message_Info& info, source_flag flag): srcLocation(info.SourceLocation()), buff(std::array<char, 6> {}), spec(flag) { }

	void Format_Arg_s::FormatAll(std::string& container) {
		file = srcLocation.file_name();
		container.append("[ ").append(file.filename().string()).append("(");
		std::memset(buff.data(), 0, buff.size());
		auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.line());
		container.append(buff.data(), end - buff.data()).append(":");
		std::memset(buff.data(), 0, buff.size());
		end = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.column()).ptr;
		container.append(buff.data(), end - buff.data()).append(") ");
		container.append(srcLocation.function_name()).append(" ]");
	}

	void Format_Arg_s::FormatFile(std::string& container) {
		file = srcLocation.file_name();
		container.append(file.make_preferred().filename().string());
	}
	void Format_Arg_s::FormatLine(std::string& container) {
		std::memset(buff.data(), 0, buff.size());
		auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.line());
		container.append(buff.data(), end - buff.data());
	}
	void Format_Arg_s::FormatColumn(std::string& container) {
		std::memset(buff.data(), 0, buff.size());
		auto [ end, ec ] = std::to_chars(buff.data(), buff.data() + buff.size(), srcLocation.column());
		container.append(buff.data(), end - buff.data());
	}
	void Format_Arg_s::FormatFunction(std::string& container) {
		container.append(srcLocation.function_name());
	}

	// NOTE: cases 6 (File and Column) and 10 (Function and Column) seemed useless so they were left out of here
	void Format_Arg_s::FormatUserPattern(std::string& container) {
		using enum source_flag;
		result.clear();

		switch( static_cast<int>(spec) ) {
				case 0: break;
				case 1: FormatLine(container); break;
				case 2: FormatColumn(container); break;
				case 3:
					FormatLine(container);
					container.append(":");
					FormatColumn(container);
					break;
				case 4: FormatFile(container); break;
				case 5:
					FormatFile(container);
					container.append(" ");
					FormatLine(container);
					break;
				case 7:
					FormatFile(container);
					container.append("(");
					FormatLine(container);
					container.append(":");
					FormatColumn(container);
					container.append(")");
					break;
				case 8: FormatFunction(container); break;
				case 9:
					FormatFunction(container);
					container.append(" ");
					FormatLine(container);
					break;
				case 11:
					FormatFunction(container);
					FormatLine(container);
					container.append(":");
					FormatColumn(container);
					container.append(")");
					break;
				case 12:
					FormatFile(container);
					container.append(" ");
					FormatFunction(container);
					break;
				default: FormatAll(container); break;
			}
	}
	/*********************************************************************************************************************/

	// Format %t Functions
	Format_Arg_t::Format_Arg_t(size_t precision): thread(std::hash<std::thread::id>()(std::this_thread::get_id())) {
		std::array<char, 64> buf {};
		std::to_chars(buf.data(), buf.data() + buf.size(), thread);
		std::string_view sv { buf.data(), buf.size() };
		(precision != 0) ? sv.remove_suffix(sv.size() - precision) : sv.remove_suffix(sv.size() - defaultThreadIdLength);
		result.append(sv.data(), sv.size());
	}

	void Format_Arg_t::FormatUserPattern(std::string& container) {
		container.append(result);
	}

	// Format %w Functions
	/*********************************************************************************************************************/
	Format_Arg_w::Format_Arg_w(Message_Info& info): cacheRef(info.TimeInfo()), buff(std::array<char, 3> {}) {
		buff[ 0 ] = '0';
		buff[ 2 ] = '\0';
		UpdateInternalView();
	}

	// Since tm_wkday is one digit, it's possible to manually pad the first index, leave the third index as null
	// terminator, and only ever have to change the second index by offsetting to_chars to affect only that index
	// TODO: Take this as an example and possibly eliminate the usage of lookup tables from Common.h
	std::string& Format_Arg_w::UpdateInternalView() {
		result.clear();
		lastDay   = cacheRef.tm_wday;
		buff[ 1 ] = '\0';
		std::to_chars(buff.data() + 1, buff.data() + 2, lastDay);
		return result.append(buff.data(), buff.size());
	}

	void Format_Arg_w::FormatUserPattern(std::string& container) {
		(cacheRef.tm_wday != lastDay) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %y Functions
	/*********************************************************************************************************************/
	Format_Arg_y::Format_Arg_y(Message_Info& info): timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastYear(cacheRef.tm_year) {
		UpdateInternalView();
	}

	std::string& Format_Arg_y::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto yr { timeRef.GetCurrentYearSV(true) };
		result.clear();
		return result.append(yr.data(), yr.size());
	}

	void Format_Arg_y::FormatUserPattern(std::string& container) {
		(cacheRef.tm_year != lastYear) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %z Functions
	/*********************************************************************************************************************/
	Format_Arg_z::Format_Arg_z(Message_Info& info): infoRef(info), lastMin(0) {
		info.TimeDetails().EnableZoneThread();
	}

	void Format_Arg_z::FormatUserPattern(std::string& container) {
		auto& timeRef { infoRef.TimeDetails() };
		auto currentMinute { infoRef.TimeInfo().tm_min };
		if( lastMin == currentMinute ) return;
		lastMin = currentMinute;
		auto offset { timeRef.UtcOffset() };
		auto hours { std::abs(std::chrono::duration_cast<std::chrono::hours>(offset).count()) };
		std::string_view sign { (offset.count() >= 0) ? "+" : "-" };
		auto& hour { serenity::SERENITY_LUTS::numberStr[ hours ] };
		auto& min { serenity::SERENITY_LUTS::numberStr[ hours / 60 ] };
		container.append(sign.data(), sign.size()).append(hour.data(), hour.size()).append(":").append(min.data(), min.size());
	}
	/*********************************************************************************************************************/

	// Format %A Functions
	/*********************************************************************************************************************/
	Format_Arg_A::Format_Arg_A(Message_Info& info): cacheRef(info.TimeInfo()), lastWkday(cacheRef.tm_wday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_A::UpdateInternalView() {
		lastWkday = cacheRef.tm_wday;
		auto& lWkday { SERENITY_LUTS::long_weekdays[ lastWkday ] };
		result.clear();
		return result.append(lWkday.data(), lWkday.size());
	}

	void Format_Arg_A::FormatUserPattern(std::string& container) {
		(cacheRef.tm_wday != lastWkday) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %B Functions
	/*********************************************************************************************************************/
	Format_Arg_B::Format_Arg_B(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) {
		UpdateInternalView();
	}

	std::string& Format_Arg_B::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto& lMonth { SERENITY_LUTS::long_months[ lastMonth ] };
		result.clear();
		return result.append(lMonth.data(), lMonth.size());
	}

	void Format_Arg_B::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mon != lastMonth) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %C Functions
	/*********************************************************************************************************************/
	Format_Arg_C::Format_Arg_C(Message_Info& info): cacheRef(info.TimeInfo()), lastYear(cacheRef.tm_year) {
		UpdateInternalView();
	}

	std::string& Format_Arg_C::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto& year { SERENITY_LUTS::numberStr[ lastYear % 100 ] };
		result.clear();
		return result.append(year.data(), year.size());
	}

	void Format_Arg_C::FormatUserPattern(std::string& container) {
		(cacheRef.tm_year != lastYear) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %D Functions
	/*********************************************************************************************************************/
	Format_Arg_D::Format_Arg_D(Message_Info& info): timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_D::UpdateInternalView() {
		result.clear();
		lastDay = cacheRef.tm_mday;
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		auto& m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto& d { SERENITY_LUTS::numberStr[ lastDay ] };
		auto y { timeRef.GetCurrentYearSV(true) };
		return result.append(m.data(), m.size()).append("/").append(d.data(), d.size()).append("/").append(y.data(), y.size());
	}

	void Format_Arg_D::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mday != lastDay) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %F Functions
	/*********************************************************************************************************************/
	Format_Arg_F::Format_Arg_F(Message_Info& info): timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastDay(cacheRef.tm_mday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_F::UpdateInternalView() {
		result.clear();
		lastDay = cacheRef.tm_mday;
		auto y { timeRef.GetCurrentYearSV(false) };
		// static_cast to 8 byte value to remove C2451's warning (which would never
		// occur here anyways...)
		auto& m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto& d { SERENITY_LUTS::numberStr[ lastDay ] };
		return result.append(y.data(), y.size()).append("-").append(m.data(), m.size()).append("-").append(d.data(), d.size());
	}

	void Format_Arg_F::FormatUserPattern(std::string& container) {
		(cacheRef.tm_mday != lastDay) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %H Functions
	/*********************************************************************************************************************/
	Format_Arg_H::Format_Arg_H(Message_Info& info): cacheRef(info.TimeInfo()), lastHour(cacheRef.tm_hour) {
		UpdateInternalView();
	}

	std::string& Format_Arg_H::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		auto& hr { SERENITY_LUTS::numberStr[ lastHour ] };
		result.clear();
		return result.append(hr.data(), hr.size());
	}

	void Format_Arg_H::FormatUserPattern(std::string& container) {
		(cacheRef.tm_hour != lastHour) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %I functions
	/*********************************************************************************************************************/
	Format_Arg_I::Format_Arg_I(Message_Info& info): cacheRef(info.TimeInfo()), lastHour(cacheRef.tm_hour) {
		UpdateInternalView();
	}

	std::string& Format_Arg_I::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		auto hr { lastHour };
		auto paddedHour { (hr > 12) ? SERENITY_LUTS::numberStr[ static_cast<int64_t>(hr) - 12 ] : SERENITY_LUTS::numberStr[ hr ] };
		result.clear();
		return result.append(paddedHour.data(), paddedHour.size());
	}

	void Format_Arg_I::FormatUserPattern(std::string& container) {
		(cacheRef.tm_hour != lastHour) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %L Functions
	/*********************************************************************************************************************/
	Format_Arg_L::Format_Arg_L(Message_Info& info): levelRef(info.MsgLevel()) {
		UpdateInternalView();
	}

	std::string& Format_Arg_L::UpdateInternalView() {
		lastLevel = levelRef;
		auto lvl { LevelToLongView(lastLevel) };
		result.clear();
		return result.append(lvl.data(), lvl.size());
	}

	void Format_Arg_L::FormatUserPattern(std::string& container) {
		(levelRef != lastLevel) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %M Functions
	/*********************************************************************************************************************/
	Format_Arg_M::Format_Arg_M(Message_Info& info): cacheRef(info.TimeInfo()), lastMin(cacheRef.tm_min) {
		UpdateInternalView();
	}

	std::string& Format_Arg_M::UpdateInternalView() {
		lastMin = cacheRef.tm_min;
		auto& minute { SERENITY_LUTS::numberStr[ lastMin ] };
		result.clear();
		return result.append(minute.data(), minute.size());
	}

	void Format_Arg_M::FormatUserPattern(std::string& container) {
		(cacheRef.tm_min != lastMin) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %N Functions
	/*********************************************************************************************************************/
	Format_Arg_N::Format_Arg_N(Message_Info& info): name(info.Name()) { }

	void Format_Arg_N::FormatUserPattern(std::string& container) {
		container.append(name);
	}
	/*********************************************************************************************************************/

	// Format %R Functions
	/*********************************************************************************************************************/
	Format_Arg_R::Format_Arg_R(Message_Info& info): cacheRef(info.TimeInfo()) { }

	void Format_Arg_R::FormatUserPattern(std::string& container) {
		auto& hour = SERENITY_LUTS::numberStr[ cacheRef.tm_hour ];
		auto& min  = SERENITY_LUTS::numberStr[ cacheRef.tm_min ];
		container.append(hour.data(), hour.size()).append(":").append(min.data(), min.size());
	}
	/*********************************************************************************************************************/

	// Format %S Functions
	/*********************************************************************************************************************/
	Format_Arg_S::Format_Arg_S(Message_Info& info): cacheRef(info.TimeInfo()), lastSec(cacheRef.tm_sec) {
		UpdateInternalView();
	}

	std::string& Format_Arg_S::UpdateInternalView() {
		lastSec = cacheRef.tm_sec;
		auto& second { SERENITY_LUTS::numberStr[ lastSec ] };
		result.clear();
		return result.append(second.data(), second.size());
	}

	void Format_Arg_S::FormatUserPattern(std::string& container) {
		(cacheRef.tm_sec != lastSec) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %T Functions
	/*********************************************************************************************************************/
	Format_Arg_T::Format_Arg_T(Message_Info& info): cacheRef(info.TimeInfo()), cachedHour(cacheRef.tm_hour), cachedMin(cacheRef.tm_min) {
		using SERENITY_LUTS::numberStr;
		result.append(numberStr[ cacheRef.tm_hour ]).append(":").append(numberStr[ cacheRef.tm_min ]).append(":");
	}

	void Format_Arg_T::FormatUserPattern(std::string& container) {
		using SERENITY_LUTS::numberStr;
		if( cacheRef.tm_hour == cachedHour && cacheRef.tm_min == cachedMin ) {
				container.append(result.data(), result.size()).append(numberStr[ cacheRef.tm_sec ]);
		} else {
				cachedHour = cacheRef.tm_hour;
				cachedMin  = cacheRef.tm_min;
				result.clear();
				container.append(result.append(numberStr[ cachedHour ]).append(":").append(numberStr[ cachedMin ])).append(numberStr[ cacheRef.tm_sec ]);
			}
	}
	/*********************************************************************************************************************/

	// Format %Y Functions
	/*********************************************************************************************************************/
	Format_Arg_Y::Format_Arg_Y(Message_Info& info): timeRef(info.TimeDetails()), cacheRef(timeRef.Cache()), lastYear(cacheRef.tm_year) {
		UpdateInternalView();
	}

	std::string& Format_Arg_Y::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto yr { timeRef.GetCurrentYearSV(lastYear) };
		result.clear();
		return result.append(yr.data(), yr.size());
	}

	void Format_Arg_Y::FormatUserPattern(std::string& container) {
		(cacheRef.tm_year != lastYear) ? container.append(UpdateInternalView()) : container.append(result);
	}
	/*********************************************************************************************************************/

	// Format %Z Functions
	/*********************************************************************************************************************/
	serenity::msg_details::Format_Arg_Z::Format_Arg_Z(Message_Info& info): timeRef(info.TimeDetails()), timeModeRef(timeRef.Mode()), cachedMode(timeModeRef) {
		timeModeRef == message_time_mode::local ? result = timeRef.TimeZoneAbbrev() : result = "UTC";
		timeRef.EnableZoneThread();
	}

	std::string& serenity::msg_details::Format_Arg_Z::UpdateInternalView() {
		return timeModeRef == message_time_mode::local ? result = timeRef.TimeZoneAbbrev() : result = "UTC";
	}

	void serenity::msg_details::Format_Arg_Z::FormatUserPattern(std::string& container) {
		cachedMode == timeModeRef ? container.append(result) : container.append(UpdateInternalView());
	}
	/*********************************************************************************************************************/

	// Format_Message Functions
	/*********************************************************************************************************************/
	Format_Arg_Message::Format_Arg_Message(Message_Info& info): message(info.Message()) { }

	void Format_Arg_Message::FormatUserPattern(std::string& container) {
		container.append(message);
	}
	/*********************************************************************************************************************/

	// Format_Char Functions
	/*********************************************************************************************************************/
	Format_Arg_Char::Format_Arg_Char(std::string_view ch) {
		result.append(ch.data(), ch.size());
	}

	void Format_Arg_Char::FormatUserPattern(std::string& container) {
		container.append(result);
	}
	/*********************************************************************************************************************/

}    // namespace serenity::msg_details