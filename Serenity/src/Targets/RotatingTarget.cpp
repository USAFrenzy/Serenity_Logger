#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental {

	RotateSettings::RotateSettings(const std::string& path)
		: FileHelper(path), currentFileSize(0), initalRotationEnabled(true), settingLimits(RotateLimits {}) {
		if( !path.empty() ) {
				std::filesystem::path pathToCache = path;
				std::filesystem::directory_entry dirCheck { pathToCache };
				if( dirCheck.is_directory() ) {
						pathToCache /= "Rotating_Log.txt";
				}
				FileCacheHelper()->CacheFile(pathToCache.string(), true);
		} else {
				std::string fileName { "Rotating_Log.txt" };
				std::string logDir { "Logs" };
				std::filesystem::path defaultPath { std::filesystem::current_path() };
				defaultPath /= logDir;
				if( !std::filesystem::exists(defaultPath) ) {
						std::filesystem::create_directories(defaultPath);
				}
				defaultPath /= fileName;
				FileCacheHelper()->CacheFile(defaultPath.string(), true);
			}
	}

	RotateSettings::RotateSettings(const RotateLimits& limits)
		: FileHelper(std::filesystem::current_path().string()), currentFileSize(0), initalRotationEnabled(true), settingLimits(limits) {
		FileCacheHelper()->CacheFile(std::filesystem::current_path().string(), true);
		std::string fileName { "Rotating_Log.txt" };
		std::string logDir { "Logs" };
		std::filesystem::path defaultPath { std::filesystem::current_path() };
		defaultPath /= logDir;
		if( !std::filesystem::exists(defaultPath) ) {
				std::filesystem::create_directories(defaultPath);
		}
		defaultPath /= fileName;
		FileCacheHelper()->CacheFile(defaultPath.string(), true);
	}

	RotateSettings::RotateSettings(const std::string& path, const RotateLimits& limits)
		: FileHelper(path), currentFileSize(0), initalRotationEnabled(true), settingLimits(limits) {
		if( !path.empty() ) {
				std::filesystem::path pathToCache = path;
				FileCacheHelper()->CacheFile(pathToCache.string(), true);
		} else {
				std::string fileName { "Rotating_Log.txt" };
				std::string logDir { "Logs" };
				std::filesystem::path defaultPath { std::filesystem::current_path() };
				defaultPath /= logDir;
				if( !std::filesystem::exists(defaultPath) ) {
						std::filesystem::create_directories(defaultPath);
				}
				defaultPath /= fileName;
				FileCacheHelper()->CacheFile(defaultPath.string(), true);
			}
	}

	std::filesystem::path RotateSettings::OriginalPath() const {
		return FileCacheHelper()->FilePath();
	}

	std::filesystem::path RotateSettings::OriginalDirectoryPath() const {
		return FileCacheHelper()->DirPath();
	}

	std::string RotateSettings::OriginalDirName() const {
		return FileCacheHelper()->DirName();
	}

	std::string RotateSettings::OriginalName() const {
		return FileCacheHelper()->FileName();
	}

	void RotateSettings::SetCurrentFileSize(size_t currentSize) {
		currentFileSize = currentSize;
	}

	std::string RotateSettings::OriginalExtension() const {
		return FileCacheHelper()->Extenstion();
	}

	size_t RotateSettings::FileSize() const {
		return currentFileSize;
	}

	void RotateSettings::SetRotationLimits(const RotateLimits& limits) {
		settingLimits = limits;
	}

	RotateLimits RotateSettings::RotationLimits() const {
		return settingLimits;
	}

}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	RotatingTarget::RotatingTarget()
		: TargetBase("Rotating_Logger"), RotateSettings(""), rotationEnabled(true), m_mode(IntervalMode::file_size),
		  currentCache(MsgInfo()->TimeInfo()), messageSize(0), dsCache(RotatingDaylightCache {}), isAboveMsgLimit(false),
		  truncateMessage(true) {
		dsCache.initialDSValue = MsgInfo()->TimeDetails().IsDaylightSavings();
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path rotationReadyFile { FileCacheHelper()->FilePath() };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				FileCacheHelper()->SetFilePath(rotationReadyFile);
				OpenFile(true);
		} else {
				std::filesystem::remove(FileCacheHelper()->FilePath());
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists)
		: TargetBase(name), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::file_size),
		  currentCache(MsgInfo()->TimeInfo()), messageSize(0), dsCache(RotatingDaylightCache {}), isAboveMsgLimit(false),
		  truncateMessage(true) {
		dsCache.initialDSValue = MsgInfo()->TimeDetails().IsDaylightSavings();

		SyncTargetHelpers(TargetHelper());
		std::filesystem::path rotationReadyFile { FileCacheHelper()->FilePath() };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				FileCacheHelper()->SetFilePath(rotationReadyFile);
				OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(FileCacheHelper()->FilePath());
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists)
		: TargetBase(name, formatPattern), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::file_size),
		  currentCache(MsgInfo()->TimeInfo()), messageSize(0), dsCache(RotatingDaylightCache {}), isAboveMsgLimit(false),
		  truncateMessage(true) {
		dsCache.initialDSValue = MsgInfo()->TimeDetails().IsDaylightSavings();
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path rotationReadyFile { FileCacheHelper()->FilePath() };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				FileCacheHelper()->SetFilePath(rotationReadyFile);
				OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(FileCacheHelper()->FilePath());
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
	}

	RotatingTarget::~RotatingTarget() {
		StopBackgroundThread();
		CloseFile();
	}

	bool RotatingTarget::RenameFile(std::string_view newFileName) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		// make copy for old file conversion and cache new values
		std::filesystem::path newFile { FileCacheHelper()->FilePath() };
		newFile.replace_filename(newFileName);
		FileCacheHelper()->CacheFile(newFile.string(), true);
		try {
				RotateFile();
				return true;
			}
		catch( const std::exception& e ) {
				std::cerr << "Error In Renaming File:\n";
				std::cerr << e.what();
				return false;
			}
	}

	void RotatingTarget::EnableRotation(bool rotationEnabled) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		this->rotationEnabled = rotationEnabled;
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
	}

	RotateLimits RotatingTarget::RotationLimits() const {
		return RotateSettings::RotationLimits();
	}

	void RotatingTarget::SetRotationLimits(const RotateLimits& limits) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		RotateSettings::SetRotationLimits(limits);
	}

	bool RotatingTarget::RenameFileInRotation(std::filesystem::path originalPath) {
		bool rotationRenameSuccessful { false };
		auto newFilePath { originalPath };
		for( size_t fileNumber { 1 }; fileNumber <= RotationLimits().maxNumberOfFiles; ++fileNumber ) {
				std::string newFile { OriginalName() };    // effectively reset each loop iteration
				newFile.append("_").append(SERENITY_LUTS::numberStr[ fileNumber ]).append(OriginalExtension());
				newFilePath.replace_filename(newFile);
				if( !std::filesystem::exists(newFilePath) ) {
						FileCacheHelper()->SetFilePath(newFilePath);
						if( OpenFile(true) ) {
								rotationRenameSuccessful = true;
								break;
						}
				}
			}
		return rotationRenameSuccessful;
	}

	bool RotatingTarget::ReplaceOldFileInRotation() {
		bool success { true };
		auto logDirectory { std::filesystem::directory_iterator(OriginalDirectoryPath()) };
		std::filesystem::file_time_type oldestWriteTime { std::chrono::file_clock::now() };
		std::string fileNameToFind { OriginalName() };
		std::filesystem::path fileToReplace;
		for( auto& file: logDirectory ) {
				if( file.is_regular_file() ) {
						if( file.path().filename().string().find(fileNameToFind) != std::string::npos ) {
								if( file.last_write_time() < oldestWriteTime ) {
										oldestWriteTime = file.last_write_time();
										fileToReplace   = file.path();
								}
						}
				}
			}
		std::filesystem::remove(fileToReplace);
		auto previousFile { FileCacheHelper()->FileName() };
		if( !fileToReplace.empty() ) {
				FileCacheHelper()->SetFilePath(fileToReplace);
		} else {
				std::cerr << "Warning: Unable To Locate Oldest File With Base Name \"" << OriginalName()
					  << "\". Opening And Truncating Previous File, \"" << previousFile << "\"\n";
				success = false;
			}
		if( !OpenFile(true) ) {
				if( FileCacheHelper()->FilePath() != previousFile ) {
						std::cerr << "Error: Unable To Finish Rotating From File \"" << previousFile << "\" To File \""
							  << FileCacheHelper()->FileName() << "\"\n";
				} else {
						std::cerr << "Error: Unable To Open And Truncate File \"" << previousFile << "\"\n";
					}
				success = false;
		}
		return success;
	}

	void RotatingTarget::RotateFile() {
		if( !rotationEnabled ) return;
		static FlushSetting originalPrimaryMode { FlushSetting::periodically };
		auto wasFlushThreadActive { BackgoundThreadInfo()->flushThreadEnabled.load() };
		if( wasFlushThreadActive ) {
				originalPrimaryMode = TargetHelper()->Policy()->PrimarySetting();
				PauseBackgroundThread();
		}
		CloseFile();
		if( !RenameFileInRotation(OriginalPath()) ) {
				if( !ReplaceOldFileInRotation() ) {
						// If we can't rotate to a new file or replace the oldest file,
						// then we can't log anthing so this would be considered fatal
						throw std::runtime_error("Unable To Rotate File Or Replace Oldest File\n");
				}
		}
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
		if( wasFlushThreadActive ) {
				ResumeBackgroundThread();
				TargetHelper()->Policy()->SetPrimaryMode(originalPrimaryMode);
		}
	}

	void RotatingTarget::TruncLogOnFileSize(bool truncate) {
		truncateMessage = truncate;
	}

	void RotatingTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		auto& backgroundThread { BackgoundThreadInfo() };
		auto flushThreadEnabled { backgroundThread->flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !backgroundThread->flushComplete.load() ) {
						backgroundThread->flushComplete.wait(false);
				}
				backgroundThread->threadWriting.store(true);
		}
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}

		size_t formattedSize { formatted.size() };
		SetMessageSize(formattedSize);

		if( !ShouldRotate() ) {
				FileHandle().rdbuf()->sputn(formatted.data(), formattedSize);
				SetCurrentFileSize(FileSize() + formattedSize);
		} else if( (m_mode == IntervalMode::file_size) && isAboveMsgLimit ) {
				formattedSize = RotationLimits().fileSizeLimit - FileSize();
				auto remainder { formatted.substr(formattedSize, formatted.size()) };
				FileHandle().rdbuf()->sputn(formatted.data(), formattedSize);
				RotateFile();
				if( !truncateMessage ) {
						FileHandle().rdbuf()->sputn(remainder.data(), remainder.size());
						SetCurrentFileSize(FileSize() + remainder.size());
				}
		} else {
				RotateFile();
				FileHandle().rdbuf()->sputn(formatted.data(), formattedSize);
				SetCurrentFileSize(FileSize() + formattedSize);
			}
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				backgroundThread->threadWriting.store(false);
				backgroundThread->threadWriting.notify_all();
		}
	}

	// TODO: Test That The Daylight Savings Logic Actually Works As Intended
	bool RotatingTarget::ShouldRotate() {
		auto limits { RotationLimits() };

		if( !rotationEnabled ) return false;
		auto& cache = MsgInfo()->TimeDetails().Cache();

		switch( RotationMode() ) {
				case IntervalMode::file_size:
					{
						return isAboveMsgLimit = ((FileSize() + MessageSize()) >= limits.fileSizeLimit);
					}
					break;
				case IntervalMode::hourly:
					{
						auto& details { MsgInfo()->TimeDetails() };
						bool dsChanged { dsCache.initialDSValue != details.IsDaylightSavings() };
						if( MsgInfo()->TimeMode() == message_time_mode::local && dsChanged ) {
								namespace ch           = std::chrono;
								// clang-format off
								auto hours {ch::duration_cast<ch::hours>(details.DaylightSavingsOffsetMin()).count()/60};
							   dsCache.initialDSValue ? dsCache.dsHour = (cache.tm_hour - hours) : dsCache.dsHour = cache.tm_hour;
								// clang-format on
								dsCache.initialDSValue = !dsCache.initialDSValue;
						}
						if( (currentCache.tm_hour != cache.tm_hour) || (cache.tm_hour == dsCache.dsHour) ) {
								currentCache.tm_hour = cache.tm_hour;
								return true;
						}
					}
					break;
				case IntervalMode::daily:
					{
						if( (MsgInfo()->TimeMode() == message_time_mode::local) &&
						    (dsCache.initialDSValue != MsgInfo()->TimeDetails().IsDaylightSavings()) ) {
								dsCache.initialDSValue = !dsCache.initialDSValue;
								auto offset { std::abs(
								MsgInfo()->TimeDetails().DaylightSavingsOffsetMin().count()) };
								int tempHr {}, tempMin {};
								if( dsCache.initialDSValue ) {
										tempHr                 = cache.tm_hour - (offset / 60);
										tempMin                = cache.tm_min - (offset % 60);
										dsCache.dsHour         = tempHr >= 0 ? tempHr : 0;
										dsCache.dsMinute       = tempMin >= 0 ? tempMin : 0;
										dsCache.dsShouldRotate = !(dsCache.dsHour ==
										                           limits.dayModeSettingHour);
								} else {
										tempHr           = cache.tm_hour + (offset / 60);
										tempMin          = cache.tm_min + (offset % 60);
										dsCache.dsHour   = tempHr <= 23 ? tempHr : 0;
										dsCache.dsMinute = tempMin <= 59 ? tempMin : 0;
										// clang-format off
										dsCache.dsShouldRotate = (dsCache.dsHour == limits.dayModeSettingHour);
									}
						}
						if( (currentCache.tm_mday != cache.tm_mday) || (dsCache.dsShouldRotate) ) {
								currentCache.tm_mday = cache.tm_mday;
								bool isSameHour { false }, meetsThreshold { false };
								if( !dsCache.dsShouldRotate ) {
									isSameHour = (limits.dayModeSettingHour == cache.tm_hour) && (limits.dayModeSettingHour != dsCache.dsHour);
									meetsThreshold = (limits.dayModeSettingMinute != dsCache.dsMinute) && (cache.tm_min >= limits.dayModeSettingMinute);
										// clang-format on
								} else {
										isSameHour     = dsCache.dsHour == limits.dayModeSettingHour;
										meetsThreshold = dsCache.dsMinute >= limits.dayModeSettingMinute;
										dsCache.dsShouldRotate = !(isSameHour && meetsThreshold);
									}
								return (isSameHour && meetsThreshold);
						}
					}
					break;
				case IntervalMode::weekly:
					{
						if( (MsgInfo()->TimeMode() == message_time_mode::local) &&
						    (dsCache.initialDSValue != MsgInfo()->TimeDetails().IsDaylightSavings()) ) {
								dsCache.initialDSValue = !dsCache.initialDSValue;
								auto offset { std::abs(
								MsgInfo()->TimeDetails().DaylightSavingsOffsetMin().count()) };
								int wkDay {}, tempHr {}, tempMin {};
								if( dsCache.initialDSValue ) {
										tempHr           = cache.tm_hour - (offset / 60);
										tempMin          = cache.tm_min - (offset % 60);
										dsCache.dsHour   = tempHr >= 0 ? tempHr : 0;
										dsCache.dsMinute = tempMin >= 0 ? tempMin : 0;
										cache.tm_wday < 6 ? wkDay = (cache.tm_wday + 1) : wkDay = 0;
										// clang-format off
										cache.tm_hour == 0 ? dsCache.dsWkDay = wkDay : dsCache.dsWkDay = cache.tm_wday;
										dsCache.dsShouldRotate = ((dsCache.dsHour != limits.dayModeSettingHour) || (dsCache.dsWkDay != cache.tm_wday));
										// clang-format on
								} else {
										tempHr           = cache.tm_hour + (offset / 60);
										tempMin          = cache.tm_min + (offset % 60);
										dsCache.dsHour   = tempHr <= 23 ? tempHr : 0;
										dsCache.dsMinute = tempMin <= 59 ? tempMin : 0;
										cache.tm_wday > 0 ? wkDay = (cache.tm_wday - 1) : wkDay = 6;
										// clang-format off
										cache.tm_hour == 0 ? dsCache.dsWkDay = wkDay : dsCache.dsWkDay = cache.tm_wday;
										dsCache.dsShouldRotate = ((dsCache.dsHour == limits.dayModeSettingHour) || (dsCache.dsWkDay == cache.tm_wday));
										// clang-format on
									}
						}
						if( currentCache.tm_wday != cache.tm_wday ) {
								currentCache.tm_wday = cache.tm_wday;
								bool isSameWkday { false }, isSameHour { false }, meetsThreshold { false };
								if( !dsCache.dsShouldRotate ) {
										// clang-format off
										isSameWkday = (currentCache.tm_wday == limits.weekModeSetting) && (dsCache.dsWkDay != limits.weekModeSetting);
										isSameHour = (limits.dayModeSettingHour == cache.tm_hour) && (limits.dayModeSettingHour != dsCache.dsHour);
										meetsThreshold = (limits.dayModeSettingMinute != dsCache.dsMinute) && (cache.tm_min >= limits.dayModeSettingMinute);
								} else {
										isSameWkday    = dsCache.dsWkDay == limits.weekModeSetting;
										isSameHour     = dsCache.dsHour == limits.dayModeSettingHour;
										meetsThreshold = dsCache.dsMinute >= limits.dayModeSettingMinute;
										dsCache.dsShouldRotate = !(isSameWkday && isSameHour && meetsThreshold);
										// clang-format on
									}
								return (isSameWkday && isSameHour && meetsThreshold);
						}
					}
					break;
				case IntervalMode::monthly:
					{
						int numberOfDays { SERENITY_LUTS::daysPerMonth.at(cache.tm_mon) };
						int rotationDay { limits.monthModeSetting };
						// This tidbit is here to make sure a month isn't accidentally skipped
						if( numberOfDays < rotationDay ) {
								rotationDay = numberOfDays;
								if( (cache.tm_mon == serenity::SERENITY_LUTS::FEBRUARY) &&
								    (MsgInfo()->TimeDetails().isLeapYear()) ) {
										rotationDay = 29;
								}
						}
						if( (MsgInfo()->TimeMode() == message_time_mode::local) &&
						    (dsCache.initialDSValue != MsgInfo()->TimeDetails().IsDaylightSavings()) ) {
								dsCache.initialDSValue = !dsCache.initialDSValue;
								auto offset { std::abs(
								MsgInfo()->TimeDetails().DaylightSavingsOffsetMin().count()) };
								dsCache.dsDayOfMonth = cache.tm_mday;
								int tempHr {}, tempMin {};
								if( dsCache.initialDSValue ) {
										tempHr           = cache.tm_hour - (offset / 60);
										tempMin          = cache.tm_min - (offset % 60);
										dsCache.dsHour   = tempHr >= 0 ? tempHr : 0;
										dsCache.dsMinute = tempMin >= 0 ? tempMin : 0;
										if( dsCache.dsHour == 0 ) {
												--dsCache.dsDayOfMonth;
										}
										if( dsCache.dsDayOfMonth < 1 ) {
												// clang-format off
											dsCache.dsDayOfMonth = SERENITY_LUTS::daysPerMonth.at((cache.tm_mon > 0 ? cache.tm_mon - 1 : 0));
										}
										dsCache.dsShouldRotate = (dsCache.dsHour != limits.dayModeSettingHour) && (dsCache.dsDayOfMonth != rotationDay);
										// clang-format on
								} else {
										tempHr           = cache.tm_hour + (offset / 60);
										tempMin          = cache.tm_min + (offset % 60);
										dsCache.dsHour   = tempHr <= 23 ? tempHr : 0;
										dsCache.dsMinute = tempMin <= 59 ? tempMin : 0;
										if( dsCache.dsHour == 0 ) {
												++dsCache.dsDayOfMonth;
										}
										if( dsCache.dsDayOfMonth > numberOfDays )
											dsCache.dsDayOfMonth = 1;
										// clang-format off
									dsCache.dsShouldRotate = (dsCache.dsHour == limits.dayModeSettingHour) && (dsCache.dsDayOfMonth == rotationDay);
										// clang-format on
									}
						}
						if( currentCache.tm_mday != cache.tm_mday ) {
								currentCache.tm_mday = cache.tm_mday;
								bool isRotationDay { false }, isRotationHour { false }, meetsThreshold { false };
								if( !dsCache.dsShouldRotate ) {
										isRotationDay = currentCache.tm_mday == rotationDay;
										// clang-format off
								isRotationHour = (limits.dayModeSettingHour == cache.tm_hour) && (limits.dayModeSettingHour != dsCache.dsHour);
								meetsThreshold = (limits.dayModeSettingMinute != dsCache.dsMinute) && (cache.tm_min >= limits.dayModeSettingMinute);
								} else {
										isRotationDay  = dsCache.dsDayOfMonth == rotationDay;
										isRotationHour = dsCache.dsHour == limits.dayModeSettingHour;
										meetsThreshold = dsCache.dsMinute >= limits.dayModeSettingMinute;
										dsCache.dsShouldRotate = !(isRotationDay && isRotationHour && meetsThreshold);
										// clang-format on
									}
								return (isRotationDay && isRotationHour && meetsThreshold);
						}
					}
					break;
				default: break;
			}
		return false;
	}

	void RotatingTarget::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		if( FileHandle().getloc() != loc ) {
				FileHandle().imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void RotatingTarget::SetRotationSetting(IntervalMode mode, int setting, int secondSetting) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		auto limits { RotationLimits() };
		switch( m_mode ) {
				case IntervalMode::file_size:
					limits.fileSizeLimit = setting;
					break;
					// Including hourly just to avoid compiler whining. Since we check current hour value with
					// internal cached hour value, there's no need to set anything or check anything here.
				case IntervalMode::hourly: break;
				case IntervalMode::daily:
					{
						if( (setting > 23) || (setting < 0) ) {
								// clang-format off
						std::cerr << "Hour setting passed in for IntervalMode::daily is out of bounds.\n";
						std::cerr << "Value expected is between 0-23 where 0 is 12AM and 23 is 11PM.\n";
						std::cerr << "Value passed in: " << setting << " \n";
						setting = 0;
								// clang-format on
						}
						if( (secondSetting > 59) || (secondSetting < 0) ) {
								// clang-format off
						std::cerr << "Minute setting passed in for IntervalMode::daily is out of bounds.\n";
						std::cerr << "Value expected is between 0-59.\n";
						std::cerr << "Value passed in: " << secondSetting << " \n";
						secondSetting = 0;
								// clang-format on
						}
						limits.dayModeSettingHour   = setting;
						limits.dayModeSettingMinute = secondSetting;
					}
					break;
				case IntervalMode::weekly:
					{
						if( (setting > 6) || (setting < 0) ) {
								// clang-format off
						std::cerr << "Weekday setting passed in for IntervalMode::weekly is out of bounds.\n";
						std::cerr << "Value expected is between 0-6 where 0 is Sunday and 6 is Saturday.\n";
						std::cerr << "Value passed in: " << setting << " \n";
						setting = 0;
								// clang-format on
						}
						limits.weekModeSetting = setting;
					}
					break;
				case IntervalMode::monthly:
					{
						if( (setting > 31) || (setting < 1) ) {
								// clang-format off
						std::cerr << "Day setting passed in for IntervalMode::monthly is out of bounds.\n";
						std::cerr << "Value expected is between 1-31 where 1 is the first day of the "
							"month and 31 is the max value of possible days in a month.\n";
						std::cerr << "Value passed in: " << setting << " \n";
						setting = 1;
								// clang-format on
						}
						limits.monthModeSetting = setting;
					}
					break;
			}
	}

	void RotatingTarget::SetRotationMode(IntervalMode mode) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		m_mode = mode;
	}

	IntervalMode RotatingTarget::RotationMode() const {
		return m_mode;
	}

	// This is just copy-pasted from the File Target.
	// Inheritance prevented this copy pasta but still wanted it fully de-coupled
	void RotatingTarget::PolicyFlushOn() {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		auto& policy { TargetHelper()->Policy() };
		if( policy->PrimarySetting() == serenity::experimental::FlushSetting::never ) {
				// If the flush thread was active, no need to hog a thread if never flushing
				StopBackgroundThread();
		};
		if( policy->PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				// Similar reasoning as Never setting except for the fact of ALWAYS flushing
				StopBackgroundThread();
				Flush();
		}
		switch( policy->SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						StartBackgroundThread();
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < policy->SecondarySettings().flushOn ) return;
						Flush();
					}
					break;
			}    // Sub Option Check
	}                    // PolicyFlushOn( ) Function

	void RotatingTarget::SetMessageSize(size_t size) {
		messageSize = size;
	}

	size_t RotatingTarget::MessageSize() const {
		return messageSize;
	}

}    // namespace serenity::experimental::targets