#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {

	using u_char_string = std::basic_string<unsigned char>;

	// Note: there's no distinction made here for the overlapping case of 'Ey' and 'Oy' yet
	static constexpr serenity::utf_helper::se_wstring LocalizedFormat(const unsigned char& ch) {
		serenity::utf_helper::se_wstring tmp;
		switch( ch ) {
				case 'c': [[fallthrough]];
				case 'x': [[fallthrough]];
				case 'C': [[fallthrough]];
				case 'X': [[fallthrough]];
				case 'Y': tmp.append(1, 'E').append(1, ch); break;
				default: tmp.append(1, 'O').append(1, ch); break;
			}
		return std::move(tmp);
	}

	// NOTE: The use of std::put_time() incurs some massive overhead -> there may be a faster way to do this with the
	//               locale's facets  instead but haven't looked into this quite yet, so currently unsure if that's a valid approach.
	//              Other than that though, the recreation of the format string to use for std::put_time() also has some overhead;
	//              only a small fraction of the overhead of std::put_time() itself, but it's the next line of code that hogs cycles in
	//              this funtion. For reference, in 100,000,000 iterations, std::put_time() took ~60% of cpu cycles while the
	//              reconstruction of the format string took ~13% of cpu cycles. So this can definitely be optimized more I think.
	//              Still results (in a char based container) in timings that are ~2.5x faster than MSVC though -> though I'm not
	//              encoding/decoding which may be why they have some more overhead.
	void ArgFormatter::LocalizeCTime(const std::locale& loc, std::tm& timeStruct, const int& precision) {
		auto end { specValues.timeSpecCounter };
		// If the locale matches any of the below, they're taken care of by standard formatting via FormatCTime()
		if( auto name { loc.name() }; name == "" || name == "C" || name == "en_US" || name == "en_US.UTF8" ) {
				specValues.localize = false;    // set to false so that when writing to the container, it doesn't call the localization buffer
				return FormatCTime(timeStruct, precision, 0, end);
		}
		// Due to major shifts over to little endian back in the early 2000's, this is making the assumption that the system is LE and NOT BE.
		SE_ASSERT(utfHelper.IsLittleEndian(), "Big Endian Format Is Currently Unsupported. If Support Is Neccessary, Please Open A New Issue At "
		                                      "'https://github.com/USAFrenzy/Serenity_Logger/issues' And/Or Define either USE_STD_FORMAT Or USE_FMTLIB instead.");

		std::u32string locData;
		static std::basic_ostringstream<serenity::utf_helper::se_wchar> localeStream;
		serenity::utf_helper::se_wstring localeFmt;

		auto pos { -1 };
		auto format { specValues.timeSpecFormat };
		auto& cont { specValues.timeSpecContainer };
		for( ;; ) {
				if( ++pos >= end ) break;
				if( cont[ pos ] != ' ' ) {
						localeFmt += '%';
						format[ pos ] == LocaleFormat::standard ? localeFmt.append(1, cont[ pos ]) : localeFmt.append(LocalizedFormat(cont[ pos ]));
				} else {
						localeFmt.append(1, ' ');
					}
			}
		localeStream.str(serenity::utf_helper::se_wstring {});
		localeStream.clear();
		localeStream.imbue(loc);
		localeStream << std::put_time<serenity::utf_helper::se_wchar>(&timeStruct, localeFmt.data());
		auto initialData { std::move(localeStream.str()) };    // moving the string value so as not to continuously have to call localeStream.str() later on
		auto& localeBuff { specValues.localizationBuff };
		// widen the wchar_t bytes to char32_t and then encode them as utf-8 bytes
		utfHelper.U16ToU32(initialData, locData);    // se_whar size == 4, this only widens each byte instead
		localeBuff.clear();
		localeBuff.resize(utfHelper.CodeUnitLengthInU8<char32_t>(locData));
		utfHelper.U32ToU8(locData, localeBuff, valueSize);
		//*******************************************************************************************
	}

	void ArgFormatter::FormatSubseconds(int precision) {
		auto begin { buffer.data() };
		auto subSeconds { std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count() };
		if( !specValues.localize ) {
				buffer[ valueSize++ ] = '.';
				std::to_chars(begin + valueSize, begin + SERENITY_ARG_BUFFER_SIZE, subSeconds);
		}
		valueSize += precision;
	}

	void ArgFormatter::FormatUtcOffset() {
		auto& utcOffset { serenity::globals::UtcOffset() };
		auto hours { std::chrono::duration_cast<std::chrono::hours>(utcOffset).count() };
		if( hours < 0 ) hours *= -1;
		auto min { static_cast<int>(hours * 0.166f) };
		if( !specValues.localize ) {
				buffer[ valueSize++ ] = (utcOffset.count() >= 0) ? '+' : '-';
		}
		Format24HM(hours, min);
	}

	void ArgFormatter::FormatTZName() {
		std::string_view name { serenity::globals::TZInfo().abbrev };
		auto size { name.size() };
		int pos {};
		for( ;; ) {
				buffer[ valueSize++ ] = name[ pos ];
				if( ++pos >= size ) return;
			}
	}

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