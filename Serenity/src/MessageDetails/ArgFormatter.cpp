#include <serenity/MessageDetails/ArgFormatter.h>

namespace serenity::arg_formatter {
	using u8_string_stream = std::basic_ostringstream<unsigned char>;
	using u8_string        = std::basic_string<unsigned char>;

	// Note: there's no distinction made here for the overlapping case of 'Ey' and 'Oy' yet
	static u8_string LocalizedFormat(const char& ch) {
		u8_string tmp;
		switch( ch ) {
				case 'c': [[fallthrough]];
				case 'x': [[fallthrough]];
				case 'C': [[fallthrough]];
				case 'X': [[fallthrough]];
				case 'Y': tmp.append(1, 'E').append(1, ch); break;
				default: tmp.append(1, 'O').append(1, ch); break;
			}
		return tmp;
	}

	// NOTE: After comparing with the use of std::format() in this case, this result actually matches up, so it may be container-based. I would still need to be able
	// to adequately store the values though, but the conversion may only need to occur on the container writing portion instead
	void ArgFormatter::LocalizeCTime(const std::locale& loc, std::tm& timeStruct, const int& precision) {
		static u8_string_stream localeStream;
		u8_string localeFmt;
		int pos { -1 };
		auto end { specValues.timeSpecCounter };
		auto& cont { specValues.timeSpecContainer };
		auto format { specValues.timeSpecFormat };
		for( ;; ) {
				if( ++pos >= end ) break;
				if( cont[ pos ] != ' ' ) {
						localeFmt += '%';
						format[ pos ] == LocaleFormat::standard ? localeFmt.append(1, cont[ pos ]) : localeFmt.append(LocalizedFormat(cont[ pos ]));
				} else {
						localeFmt.append(1, ' ');
					}
			}
		localeStream.str(u8_string {});
		localeStream.clear();
		localeStream.imbue(loc);
		// NOTE: most likely due to using unsigned char as the code unit, what was showing up as a valid character for the chinese locale now only shows up as a '?'
		// with this line; it may be a MSVC thing, but changing the code unit to use char16_t or char32_t doesn't work here either, only wchar_t does, which is
		// striking me as odd since char32_t in and of itself should be more than adequate here for the Chinese locale test case...
		localeStream << std::put_time(&timeStruct, localeFmt.data());
		auto locData { std::move(localeStream.str()) };
		for( auto& ch: locData ) {
				if( static_cast<unsigned char>(ch) >= 0x80 ) {
						buffer[ valueSize++ ] = '?';    // placeholder for now
						/*********************************************  handle code units above ascii codepoints here *********************************************/
						// Unsure how to approach this atm, I believe I might just need to somehow form the appropriate grapheme cluster to get how many
						// code units are present and increment the buffer in a manner that makes sense with this. I'm currently reading through utf8everywhere.org
						// and doing some more research into this, but as of now, this is unhandled and just replaces the character with '?' instead. In all honesty,
						// I may be misunderstanding what I have to do in this block. I don't believe I have to do any widening or narrowing here but I do think I
						// need to adequately space out the buffer based on what the codepoint is.
				} else {
						buffer[ valueSize++ ] = ch;
					}
			}
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