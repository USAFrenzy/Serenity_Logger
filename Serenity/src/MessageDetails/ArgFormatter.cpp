#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	// not implemented
	void ArgFormatter::LocalizeCTime(const std::locale& loc, const std::tm& timeStruct, const int& precision) { }

	void ArgFormatter::LocalizeArgument(const std::locale& loc, const int& precision, const int& totalWidth, const msg_details::SpecType& type) {
		using enum serenity::msg_details::SpecType;
		// NOTE: The following types should have been caught in the verification process:  monostate, string, c-string, string view, const void*, void *
		switch( type ) {
				case IntType: [[fallthrough]];
				case U_IntType: [[fallthrough]];
				case LongLongType: LocalizeIntegral(loc, precision, totalWidth, type); break;
				case FloatType: [[fallthrough]];
				case DoubleType: [[fallthrough]];
				case LongDoubleType: [[fallthrough]];
				case U_LongLongType: LocalizeFloatingPoint(loc, precision, totalWidth, type); break;
				case BoolType: LocalizeBool(loc); break;
			}
	}

	void ArgFormatter::LocalizeIntegral(const std::locale& loc, const int& precision, const int& totalWidth, const msg_details::SpecType& type) {
		FormatArgument(precision, totalWidth, type);
		FormatIntegralGrouping(loc, valueSize);
	}

	void ArgFormatter::LocalizeFloatingPoint(const std::locale& loc, const int& precision, const int& totalWidth, const msg_details::SpecType& type) {
		FormatArgument(precision, totalWidth, type);
		size_t pos { 0 };
		bool hasMantissa { false };
		for( ;; ) {
				if( pos >= valueSize ) break;
				if( buffer[ pos ] == '.' ) {
						hasMantissa = true;
						FormatIntegralGrouping(loc, pos);
						buffer[ pos++ ] = std::use_facet<std::numpunct<char>>(loc).decimal_point();
						break;
				}
				++pos;
			}
		if( !hasMantissa ) {
				FormatIntegralGrouping(loc, valueSize);
		}
	}

	void ArgFormatter::LocalizeBool(const std::locale& loc) {
		std::string_view sv { argStorage.bool_state(specValues.argPosition) ? std::use_facet<std::numpunct<char>>(loc).truename()
			                                                                : std::use_facet<std::numpunct<char>>(loc).falsename() };
		valueSize = sv.size();
		std::copy(sv.data(), sv.data() + valueSize, buffer.begin());
	}

	void ArgFormatter::FormatIntegralGrouping(const std::locale& loc, size_t end) {
		auto groupings { std::use_facet<std::numpunct<char>>(loc).grouping() };
		if( end <= *groupings.begin() ) return;
		auto separator { std::use_facet<std::numpunct<char>>(loc).thousands_sep() };
		auto groupBegin { groupings.begin() };
		int groupGap { *groupBegin };
		std::string_view sv { buffer.data(), buffer.data() + end };
		size_t groups { 0 };

		// TODO: Pretty Sure Grouping of 3 will not work at the moment as it's currently mostly copy-pasta logic (too brain-dead at the moment to walk through
		// this so will test later)
		switch( groupings.size() ) {
				case 3:
					{
						// grouping is unique
						int secondGroup { *(++groupBegin) };
						int repeatingGroup { *(++groupBegin) };
						groups = (valueSize - groupGap - secondGroup) / static_cast<size_t>(repeatingGroup) +
						         (valueSize - groupGap - secondGroup) % static_cast<size_t>(repeatingGroup);
						auto spaceRequired { valueSize + groups + 1 };    // add one for intermediate grouping
						valueSize = spaceRequired;

						std::copy(sv.end() - groupGap, sv.end(), buffer.data() + spaceRequired - groupGap);
						sv.remove_suffix(groupGap);
						spaceRequired -= groupGap;
						buffer[ --spaceRequired ] = separator;

						std::copy(sv.end() - secondGroup, sv.end(), buffer.data() + spaceRequired - secondGroup);
						spaceRequired -= (secondGroup);
						sv.remove_suffix(secondGroup);
						buffer[ --spaceRequired ] = separator;

						for( ; groups; --groups ) {
								if( sv.size() > repeatingGroup ) {
										std::copy(sv.end() - repeatingGroup, sv.end(), buffer.data() + spaceRequired - repeatingGroup);
										spaceRequired -= repeatingGroup;
								} else {
										std::copy(sv.begin(), sv.end() - 1, buffer.begin());
										return;
									}
								if( groups > 0 ) {
										buffer[ --spaceRequired ] = separator;
								}
								if( sv.size() > repeatingGroup ) {
										sv.remove_suffix(repeatingGroup);
								}
							}
						break;
					}
				case 2:
					{
						// grouping is one group and then uniform
						int repeatingGroup { *(++groupBegin) };
						groups = (valueSize - groupGap) / static_cast<size_t>(repeatingGroup) + (valueSize - groupGap) % static_cast<size_t>(repeatingGroup);
						auto spaceRequired { valueSize + groups };
						valueSize = spaceRequired;

						std::copy(sv.end() - groupGap, sv.end(), buffer.data() + spaceRequired - groupGap);
						sv.remove_suffix(groupGap);
						spaceRequired -= groupGap;
						buffer[ --spaceRequired ] = separator;

						for( ; groups; --groups ) {
								if( sv.size() > repeatingGroup ) {
										std::copy(sv.end() - repeatingGroup, sv.end(), buffer.data() + spaceRequired - repeatingGroup);
										spaceRequired -= repeatingGroup;
								} else {
										std::copy(sv.begin(), sv.end() - 1, buffer.begin());
										return;
									}
								if( groups > 0 ) {
										buffer[ --spaceRequired ] = separator;
								}
								if( sv.size() > repeatingGroup ) {
										sv.remove_suffix(repeatingGroup);
								}
							}
						break;
					}
				case 1:
					{
						// grouping is uniform
						groups = valueSize / static_cast<size_t>(groupGap) + valueSize % static_cast<size_t>(groupGap);
						size_t spaceRequired { valueSize + groups - 1 };
						valueSize = spaceRequired;
						for( ; groups; --groups ) {
								if( sv.size() > groupGap ) {
										std::copy(sv.end() - groupGap, sv.end(), buffer.data() + spaceRequired - groupGap);
										spaceRequired -= (groupGap);    // taking into consideration both the grouping and separator per group
								} else {
										std::copy(sv.begin(), sv.end() - 1, buffer.begin());
										return;
									}
								if( groups > 0 ) {
										buffer[ --spaceRequired ] = separator;
								}
								if( sv.size() > groupGap ) {
										sv.remove_suffix(groupGap);
								}
							}
						break;
					}
				default: break;
			}
	}
}    // namespace serenity::arg_formatter