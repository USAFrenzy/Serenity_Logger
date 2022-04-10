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
		auto sWkday { SERENITY_LUTS::short_weekdays[ lastWkday ] };
		result.clear();
		return result.append(sWkday.data(), sWkday.size());
	}

	std::string_view Format_Arg_a::FormatUserPattern() {
		return (lastWkday != cacheRef.tm_wday) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %b functions
	/*********************************************************************************************************************/
	Format_Arg_b::Format_Arg_b(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) { }

	std::string& Format_Arg_b::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto mon { SERENITY_LUTS::short_months[ lastMonth ] };
		result.clear();
		return result.append(mon.data(), mon.size());
	}

	std::string_view Format_Arg_b::FormatUserPattern() {
		return (lastMonth != cacheRef.tm_wday) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %c
	/*********************************************************************************************************************/
	Format_Arg_c::Format_Arg_c(Message_Info& info): timeRef(info.TimeDetails()) { }

	std::string_view Format_Arg_c::FormatUserPattern() {
		result.clear();
		auto& cache { timeRef.Cache() };
		auto wkday { serenity::SERENITY_LUTS::short_weekdays[ cache.tm_wday ] };
		auto mon { serenity::SERENITY_LUTS::short_months[ cache.tm_mon ] };
		auto day { serenity::SERENITY_LUTS::numberStr[ cache.tm_mday ] };
		auto hour { serenity::SERENITY_LUTS::numberStr[ cache.tm_hour ] };
		auto min { serenity::SERENITY_LUTS::numberStr[ cache.tm_min ] };
		auto sec { serenity::SERENITY_LUTS::numberStr[ cache.tm_sec ] };
		timeRef.CalculateCurrentYear(cache.tm_year);
		auto year { timeRef.GetCurrentYearSV(false) };
		result.reserve(wkday.size() + mon.size() + day.size() + hour.size() + min.size() + sec.size() + year.size() + 2);
		return result.append(wkday.data(), wkday.size())
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

	std::string_view Format_Arg_d::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %e Functions
	/*********************************************************************************************************************/
	Format_Arg_e::Format_Arg_e(Message_Info& info, size_t precision)
		: timeRef(info.TimeDetails()), buffer(std::array<char, defaultBufferSize> {}), m_precision(precision) {
		result.reserve(20);
	}

	std::string_view Format_Arg_e::FormatUserPattern() {
		result.clear();
		std::fill(buffer.data(), buffer.data() + buffer.size(), '\0');

		auto& cache { timeRef.Cache() };
		auto hour { SERENITY_LUTS::numberStr[ cache.tm_hour ] };
		auto min { SERENITY_LUTS::numberStr[ cache.tm_min ] };
		auto sec { SERENITY_LUTS::numberStr[ cache.tm_sec ] };

		namespace ch = std::chrono;
		auto msec { ch::duration_cast<ch::nanoseconds>(ch::system_clock::now().time_since_epoch()) };
		std::to_chars(buffer.data(), buffer.data() + buffer.size(), (msec.count()));

		size_t i { 0 };
		for( ;; ) {
				if( buffer[ i ] == '\0' ) break;
				++i;
			}
		auto startPoint { buffer.begin() };
		auto midPointOffset { (i / 2) + (i % 2) };
		std::string_view sv { startPoint + midPointOffset, startPoint + i };
		sv.remove_suffix(maxPrecision - m_precision);
		return result.append(hour).append(":").append(min).append(":").append(sec).append(":").append(sv.data(), sv.size());
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

	std::string_view Format_Arg_l::FormatUserPattern() {
		return (levelRef != lastLevel) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %m Functions
	/*********************************************************************************************************************/
	Format_Arg_m::Format_Arg_m(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) {
		UpdateInternalView();
	}

	std::string& Format_Arg_m::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto paddedMonth { SERENITY_LUTS::numberStr[ lastMonth ] };
		result.clear();
		return result.append(paddedMonth.data(), paddedMonth.size());
	}

	std::string_view Format_Arg_m::FormatUserPattern() {
		return (cacheRef.tm_mon != lastMonth) ? UpdateInternalView() : result;
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
		auto day { SERENITY_LUTS::numberStr[ cacheRef.tm_mday ] };
		auto month { SERENITY_LUTS::short_months[ cacheRef.tm_mon ] };
		auto year { timeRef.GetCurrentYearSV(true) };
		return result.append(day.data(), day.size()).append(month.data(), month.size()).append(year.data(), year.size());
	}

	std::string_view Format_Arg_n::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? UpdateInternalView() : result;
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

	std::string_view Format_Arg_p::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %r Functions
	/*********************************************************************************************************************/
	Format_Arg_r::Format_Arg_r(Message_Info& info): cacheRef(info.TimeInfo()), lastMin(0) {
		UpdateInternalView();
	}

	std::string& Format_Arg_r::UpdateInternalView() {
		auto hr { cacheRef.tm_hour };
		if( lastMin != cacheRef.tm_min ) {
				lastMin = cacheRef.tm_min;
				hour    = (hr > 12) ? SERENITY_LUTS::numberStr[ static_cast<int64_t>(hr) - 12 ] : SERENITY_LUTS::numberStr[ hr ];
				min     = SERENITY_LUTS::numberStr[ lastMin ];
		}
		auto sec = SERENITY_LUTS::numberStr[ cacheRef.tm_sec ];
		result.clear();
		return result.append(hour.data(), hour.size()).append(":").append(min.data(), min.size()).append(":").append(sec.data(), sec.size());
	}

	std::string_view Format_Arg_r::FormatUserPattern() {
		UpdateInternalView();
		return result;
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

	std::string_view Format_Arg_t::FormatUserPattern() {
		return result;
	}

	// Format %w Functions
	/*********************************************************************************************************************/
	Format_Arg_w::Format_Arg_w(Message_Info& info): cacheRef(info.TimeInfo()) {
		UpdateInternalView();
	}

	std::string& Format_Arg_w::UpdateInternalView() {
		lastDay = cacheRef.tm_wday;
		result.clear();
		return result.append(std::move(std::vformat("{}", std::make_format_args(lastDay))));
	}

	std::string_view Format_Arg_w::FormatUserPattern() {
		return (cacheRef.tm_wday != lastDay) ? UpdateInternalView() : result;
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

	std::string_view Format_Arg_y::FormatUserPattern() {
		return (cacheRef.tm_year != lastYear) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %A Functions
	/*********************************************************************************************************************/
	Format_Arg_A::Format_Arg_A(Message_Info& info): cacheRef(info.TimeInfo()), lastWkday(cacheRef.tm_wday) {
		UpdateInternalView();
	}

	std::string& Format_Arg_A::UpdateInternalView() {
		lastWkday = cacheRef.tm_wday;
		auto lWkday { SERENITY_LUTS::long_weekdays[ lastWkday ] };
		result.clear();
		return result.append(lWkday.data(), lWkday.size());
	}

	std::string_view Format_Arg_A::FormatUserPattern() {
		return (cacheRef.tm_wday != lastWkday) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %B Functions
	/*********************************************************************************************************************/
	Format_Arg_B::Format_Arg_B(Message_Info& info): cacheRef(info.TimeInfo()), lastMonth(cacheRef.tm_mon) {
		UpdateInternalView();
	}

	std::string& Format_Arg_B::UpdateInternalView() {
		lastMonth = cacheRef.tm_mon;
		auto lMonth { SERENITY_LUTS::long_months[ lastMonth ] };
		result.clear();
		return result.append(lMonth.data(), lMonth.size());
	}

	std::string_view Format_Arg_B::FormatUserPattern() {
		return (cacheRef.tm_mon != lastMonth) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %C Functions
	/*********************************************************************************************************************/
	Format_Arg_C::Format_Arg_C(Message_Info& info): cacheRef(info.TimeInfo()), lastYear(cacheRef.tm_year) {
		UpdateInternalView();
	}

	std::string& Format_Arg_C::UpdateInternalView() {
		lastYear = cacheRef.tm_year;
		auto year { SERENITY_LUTS::numberStr[ lastYear % 100 ] };
		result.clear();
		return result.append(year.data(), year.size());
	}

	std::string_view Format_Arg_C::FormatUserPattern() {
		return (cacheRef.tm_year != lastYear) ? UpdateInternalView() : result;
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
		auto m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto d { SERENITY_LUTS::numberStr[ lastDay ] };
		auto y { timeRef.GetCurrentYearSV(true) };
		return result.append(m.data(), m.size()).append("/").append(d.data(), d.size()).append("/").append(y.data(), y.size());
	}

	std::string_view Format_Arg_D::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? UpdateInternalView() : result;
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
		auto m { SERENITY_LUTS::numberStr[ static_cast<int64_t>(cacheRef.tm_mon) + 1 ] };
		auto d { SERENITY_LUTS::numberStr[ lastDay ] };
		return result.append(y.data(), y.size()).append("-").append(m.data(), m.size()).append("-").append(d.data(), d.size());
	}

	std::string_view Format_Arg_F::FormatUserPattern() {
		return (cacheRef.tm_mday != lastDay) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %H Functions
	/*********************************************************************************************************************/
	Format_Arg_H::Format_Arg_H(Message_Info& info): cacheRef(info.TimeInfo()), lastHour(cacheRef.tm_hour) {
		UpdateInternalView();
	}

	std::string& Format_Arg_H::UpdateInternalView() {
		lastHour = cacheRef.tm_hour;
		auto hr { SERENITY_LUTS::numberStr[ lastHour ] };
		result.clear();
		return result.append(hr.data(), hr.size());
	}

	std::string_view Format_Arg_H::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? UpdateInternalView() : result;
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

	std::string_view Format_Arg_I::FormatUserPattern() {
		return (cacheRef.tm_hour != lastHour) ? UpdateInternalView() : result;
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

	std::string_view Format_Arg_L::FormatUserPattern() {
		return (levelRef != lastLevel) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %M Functions
	/*********************************************************************************************************************/
	Format_Arg_M::Format_Arg_M(Message_Info& info): cacheRef(info.TimeInfo()), lastMin(cacheRef.tm_min) {
		UpdateInternalView();
	}

	std::string& Format_Arg_M::UpdateInternalView() {
		lastMin = cacheRef.tm_min;
		auto minute { SERENITY_LUTS::numberStr[ lastMin ] };
		result.clear();
		return result.append(minute.data(), minute.size());
	}

	std::string_view Format_Arg_M::FormatUserPattern() {
		return (cacheRef.tm_min != lastMin) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %N Functions
	/*********************************************************************************************************************/
	Format_Arg_N::Format_Arg_N(Message_Info& info): name(info.Name()) { }

	std::string_view Format_Arg_N::FormatUserPattern() {
		return name;
	}
	/*********************************************************************************************************************/

	// Format %S Functions
	/*********************************************************************************************************************/
	Format_Arg_S::Format_Arg_S(Message_Info& info): cacheRef(info.TimeInfo()), lastSec(cacheRef.tm_sec) {
		UpdateInternalView();
	}

	std::string& Format_Arg_S::UpdateInternalView() {
		lastSec = cacheRef.tm_sec;
		auto second { SERENITY_LUTS::numberStr[ lastSec ] };
		result.clear();
		return result.append(second.data(), second.size());
	}

	std::string_view Format_Arg_S::FormatUserPattern() {
		return (cacheRef.tm_sec != lastSec) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format %T Functions
	/*********************************************************************************************************************/
	Format_Arg_T::Format_Arg_T(Message_Info& info): cacheRef(info.TimeInfo()), lastMin(0) {
		UpdateInternalView();
	}

	std::string& Format_Arg_T::UpdateInternalView() {
		if( lastMin != cacheRef.tm_min ) {
				lastMin = cacheRef.tm_min;
				hour    = SERENITY_LUTS::numberStr[ cacheRef.tm_hour ];
				min     = SERENITY_LUTS::numberStr[ lastMin ];
		}
		auto sec = SERENITY_LUTS::numberStr[ cacheRef.tm_sec ];
		result.clear();
		return result.append(hour.data(), hour.size()).append(":").append(min.data(), min.size()).append(":").append(sec.data(), sec.size());
	}

	std::string_view Format_Arg_T::FormatUserPattern() {
		UpdateInternalView();
		return result;
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

	std::string_view Format_Arg_Y::FormatUserPattern() {
		return (cacheRef.tm_year != lastYear) ? UpdateInternalView() : result;
	}
	/*********************************************************************************************************************/

	// Format_Message Functions
	/*********************************************************************************************************************/
	Format_Arg_Message::Format_Arg_Message(Message_Info& info): message(info.Message()) { }

	std::string_view Format_Arg_Message::FormatUserPattern() {
		return message;
	}
	/*********************************************************************************************************************/

	// Format_Char Functions
	/*********************************************************************************************************************/
	Format_Arg_Char::Format_Arg_Char(std::string_view ch) {
		result.append(ch.data(), ch.size());
	}

	std::string_view Format_Arg_Char::FormatUserPattern() {
		return result;
	}
	/*********************************************************************************************************************/

}    // namespace serenity::msg_details