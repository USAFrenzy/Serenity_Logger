#include <serenity/MessageDetails/ArgFormatter.h>

// Only reason for this file at the moment is strictly due to the fact that the numpunct functions aren't constexpr...
namespace serenity::arg_formatter {
	void ArgFormatter::LocalizeArgument(const std::locale& loc, int precision, msg_details::SpecType type) {
		using enum serenity::msg_details::SpecType;
		// NOTE: The following types should have been caught in the verification process:
		//       monostate, string, c-string, string view, const void*, void *
		switch( type ) {
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: return LocalizeIntegral(loc, precision, type); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType: [[fallthrough]];
				case U_LongLongType: return LocalizeFloatingPoint(loc, precision, type); break;
				case BoolType: return LocalizeBool(loc); break;
			}
		unreachable();
	}

	void ArgFormatter::LocalizeIntegral(const std::locale& loc, int precision, msg_details::SpecType type) {
		FormatRawValueToStr(precision, type);
		FormatIntegralGrouping(loc, rawValueTemp);
	}

	void ArgFormatter::LocalizeFloatingPoint(const std::locale& loc, int precision, msg_details::SpecType type) {
		FormatRawValueToStr(precision, type);
		size_t pos { 0 };
		auto size { rawValueTemp.size() };
		auto data { rawValueTemp.begin() };
		std::string_view sv { rawValueTemp };
		localeTemp.clear();
		for( ;; ) {
				if( pos >= size ) break;
				if( sv[ pos ] == '.' ) {
						localeTemp.append(sv.substr(0, pos));
						FormatIntegralGrouping(loc, localeTemp);
						localeTemp += std::use_facet<std::numpunct<char>>(loc).decimal_point();
						localeTemp.append(sv.substr(pos + 1, sv.size()));
						break;
				}
				++pos;
			}
		if( localeTemp.size() != 0 ) {
				rawValueTemp.clear();
				rawValueTemp.append(std::move(localeTemp));
				return;
		}
		FormatIntegralGrouping(loc, rawValueTemp);
	}

	void ArgFormatter::LocalizeBool(const std::locale& loc) {
		rawValueTemp.append(argStorage.bool_state(specValues.argPosition) ? std::use_facet<std::numpunct<char>>(loc).truename()
		                                                                  : std::use_facet<std::numpunct<char>>(loc).falsename());
	}

	void ArgFormatter::FormatIntegralGrouping(const std::locale& loc, std::string& section) {
		size_t groups { 0 };
		auto groupings { std::use_facet<std::numpunct<char>>(loc).grouping() };
		auto separator { std::use_facet<std::numpunct<char>>(loc).thousands_sep() };
		auto groupBegin { groupings.begin() };
		int groupGap { *groupBegin };
		auto end { section.size() };
		if( section.size() < groupGap ) return;
		localeTemp.clear();
		std::string_view sv { section };
		if( specValues.hasAlt ) {
				sv.remove_prefix(2);
		}

		// TODO: Rework this to avoid using insert (Used insert to just get this working, but it's very slow)
		if( groupings.size() > 1 ) {
				/********************* grouping is unique *********************/
				if( groupings.size() == 3 ) {
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);
						localeTemp.insert(0, 1, separator);

						groupGap = *(++groupBegin);
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);
						localeTemp.insert(0, 1, separator);

						groupGap = *(++groupBegin);
						groups   = end / groupGap - end % groupGap;
						for( ; groups; --groups ) {
								if( groups > 0 ) {
										localeTemp.insert(0, 1, separator);
								}
								if( sv.size() > groupGap ) {
										localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
								} else {
										localeTemp.insert(0, sv);
										break;
									}
								if( sv.size() >= groupGap ) {
										sv.remove_suffix(groupGap);
								}
							}
				} else {
						// grouping is one group and then uniform
						localeTemp.append(sv.data() + sv.size() - groupGap, sv.data() + sv.size());
						sv.remove_suffix(groupGap);

						groupGap = *(++groupBegin);
						groups   = end / groupGap - end % groupGap;

						for( ; groups; --groups ) {
								if( groups > 0 ) {
										localeTemp.insert(0, 1, separator);
								}
								if( sv.size() > groupGap ) {
										localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
								} else {
										localeTemp.insert(0, sv);
										break;
									}
								if( sv.size() >= groupGap ) {
										sv.remove_suffix(groupGap);
								}
							}
					}
		} else {
				/********************* grouping is uniform *********************/
				groups = end / groupGap + end % groupGap;
				for( ; groups; --groups ) {
						if( sv.size() > groupGap ) {
								localeTemp.insert(0, sv.substr(sv.size() - groupGap, sv.size()));
						} else {
								localeTemp.insert(0, sv);
								break;
							}
						if( groups > 0 ) {
								localeTemp.insert(0, 1, separator);
						}
						if( sv.size() >= groupGap ) {
								sv.remove_suffix(groupGap);
						}
					}
			}
		if( specValues.hasAlt ) {
				localeTemp.insert(0, section.substr(0, 2));
		}
		section.clear();
		section.append(std::move(localeTemp));
	}
}    // namespace serenity::arg_formatter
