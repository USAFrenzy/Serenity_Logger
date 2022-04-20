#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental {

	RotateSettings::RotateSettings(const std::string& path)
		: FileHelper(path), maxNumberOfFiles(5), fileSizeLimit(512 * KB), dayModeSettingHour(0), dayModeSettingMinute(0),
		  weekModeSetting(0), monthModeSetting(1), currentFileSize(0), initalRotationEnabled(true) {
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

	std::filesystem::path RotateSettings::OriginalDirectory() const {
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

}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	RotatingTarget::RotatingTarget()
		: TargetBase("Rotating_Log.txt"), RotateSettings(""), rotationEnabled(true), m_mode(IntervalMode::hourly),
		  currentCache(MsgInfo()->TimeInfo()), shouldRotate(false), messageSize(0), dsCache(RotatingDaylightCache {}) {
		dsCache.initialDSValue = MsgInfo()->TimeDetails().IsDaylightSavings();
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path rotationReadyFile { FileCacheHelper()->FilePath() };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(FileCacheHelper()->FilePath(), rotationReadyFile);
				FileCacheHelper()->SetFilePath(rotationReadyFile);
				OpenFile(true);
		} else {
				std::filesystem::remove(FileCacheHelper()->FilePath());
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(FileCacheHelper()->FilePath()));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists)
		: TargetBase(name), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::hourly),
		  currentCache(MsgInfo()->TimeInfo()), shouldRotate(false), messageSize(0), dsCache(RotatingDaylightCache {}) {
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
		: TargetBase(name, formatPattern), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::hourly),
		  currentCache(MsgInfo()->TimeInfo()), shouldRotate(false), messageSize(0), dsCache(RotatingDaylightCache {}) {
		dsCache.initialDSValue = MsgInfo()->TimeDetails().IsDaylightSavings();
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path rotationReadyFile { FileCacheHelper()->FilePath() };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(FileCacheHelper()->FilePath(), rotationReadyFile);
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

	void RotatingTarget::SetRotateSettings(RotateSettings settings) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		fileSizeLimit        = settings.fileSizeLimit;
		maxNumberOfFiles     = settings.maxNumberOfFiles;
		dayModeSettingHour   = settings.dayModeSettingHour;
		dayModeSettingMinute = settings.dayModeSettingMinute;
		monthModeSetting     = settings.monthModeSetting;
		weekModeSetting      = settings.weekModeSetting;
	}

	bool RotatingTarget::RenameFileInRotation(std::filesystem::path originalPath) {
		bool rotationRenameSuccessful { false };
		auto newFilePath { originalPath };
		for( size_t fileNumber { 1 }; fileNumber <= maxNumberOfFiles; ++fileNumber ) {
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
		auto logDirectory { std::filesystem::directory_iterator(OriginalDirectory()) };
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
		SetMessageSize(formatted.size());
		if( ShouldRotate() ) {
				RotateFile();
		}
		auto formattedSize { formatted.size() };
		FileHandle().rdbuf()->sputn(formatted.data(), formattedSize);
		SetCurrentFileSize(FileSize() + formattedSize);
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				backgroundThread->threadWriting.store(false);
				backgroundThread->threadWriting.notify_all();
		}
	}

	// TODO: Test the added Daylight Savings logic, flesh it out, and then add to the other fields
	// EDIT #1: Hourly seems to work when manipulated to change in mock DS change setting, but the other
	//          fields are probably going to require a little bit more in depth offsets to check
	// EDIT #2: Trying to reason about the daily case with regards to DS changes. If Daylights value
	//          changes, take the offset and if we're coming from DS, subtract the offset, otherwise,
	//          add the offset to the hour and minutes field of the dsCache. If we're coming from DS
	//          and the offset hour == the rotation hour, then we shouldn't rotate. However, if we're
	//          going to DS, if the offset hour == the rotation hour, then we SHOULD rotate do to time
	//          springing forward.
	//          - Therefore:
	//            - if we should rotate based on the dsShouldRotate value, we set shouldRotate to true
	//            - if the current day of the month != what the cached value is, we set shouldRotate
	//              to true
	//            - if the current day of the month == what the cached value is AND dsShouldRotate is
	//              false, then we set shouldRotate to false
	//          - THEN depending on the value of shouldRotate, if its false, short-circuit the checks and
	//            break, returning false, otherwise, if shouldRotate is true, check if we've meet the
	//            conditions for rotating based on the normal settings OR, if dsShouldRotate was set to
	//            true, check the conditions based on the daylight savings offsets
	//          - If dsShouldRotate is set to false (whether the condition proved that we shouldn't
	//            rotate or whether the check was circumvented all-together by already being run last
	//            iteration), then the conditional check for rotation compares that the rotation settings
	//            are equal to current time AND not equal to the DS settings in order to prevent double
	//            rotations for the same condition already being met by the Daylight Savings check.
	//            - If the conditions are met, returns true, otherwise, returns false.
	//          I believe this might cover all bases but I need to think this through just in case I'm
	//          missing something.
	bool RotatingTarget::ShouldRotate() {
		using mode = RotateSettings::IntervalMode;

		if( !rotationEnabled ) return false;
		if( FileSize() == 0 ) return false;
		auto& cache = MsgInfo()->TimeDetails().Cache();

		switch( RotationMode() ) {
				case mode::file_size:
					{
						return ((FileSize() + MessageSize()) >= fileSizeLimit);
					}
					break;
				case mode::hourly:
					{
						bool previousDSValue { dsCache.initialDSValue };
						if( (MsgInfo()->TimeMode() == message_time_mode::local) &&
						    (dsCache.initialDSValue != MsgInfo()->TimeDetails().IsDaylightSavings()) ) {
								dsCache.initialDSValue = !dsCache.initialDSValue;
						}
						if( (currentCache.tm_hour != cache.tm_hour) || (previousDSValue != dsCache.initialDSValue) ) {
								currentCache.tm_hour = cache.tm_hour;
								return true;
						}
					}
					break;
				case mode::daily:
					{
						bool dsShouldRotate { false };
						if( (MsgInfo()->TimeMode() == message_time_mode::local) &&
						    (dsCache.initialDSValue != MsgInfo()->TimeDetails().IsDaylightSavings()) ) {
								dsCache.initialDSValue = !dsCache.initialDSValue;
								auto offset { std::abs(
								MsgInfo()->TimeDetails().DaylightSavingsOffsetMin().count()) };
								if( dsCache.initialDSValue ) {
										dsCache.dsHour   = cache.tm_hour - (offset / 60);
										dsCache.dsMinute = cache.tm_min - (offset % 60);
										dsShouldRotate   = !(dsCache.dsHour == dayModeSettingHour);
								} else {
										dsCache.dsHour   = cache.tm_hour + (offset / 60);
										dsCache.dsMinute = cache.tm_min + (offset % 60);
										dsShouldRotate   = (dsCache.dsHour == dayModeSettingHour);
									}
						}
						if( (currentCache.tm_mday != cache.tm_mday) || (dsShouldRotate) ) {
								currentCache.tm_mday = cache.tm_mday;
								shouldRotate         = true;
						} else {
								shouldRotate = false;
								break;
							}
						bool isSameHour { ((dayModeSettingHour == cache.tm_hour) &&
							           (dayModeSettingHour != dsCache.dsHour)) ||
							          (dsShouldRotate && (dsCache.dsHour == dayModeSettingHour)) };
						bool meetsThreshold { ((dayModeSettingMinute != dsCache.dsMinute) &&
							               (cache.tm_min >= dayModeSettingMinute)) ||
							              (dsShouldRotate && (dsCache.dsMinute >= dayModeSettingMinute)) };
						return (shouldRotate && isSameHour && meetsThreshold);
					}
					break;
				case mode::weekly:
					{
						if( currentCache.tm_wday != cache.tm_wday ) {
								currentCache.tm_wday = cache.tm_wday;
								shouldRotate         = true;
						} else {
								shouldRotate = false;
								break;
							}
						bool isSameWkDay { currentCache.tm_wday == weekModeSetting };
						bool isSameHour { cache.tm_hour == dayModeSettingHour };
						bool meetsThreshold { cache.tm_min >= dayModeSettingMinute };
						if( shouldRotate && isSameWkDay && isSameHour && meetsThreshold ) return true;
					}
					break;
				case mode::monthly:
					{
						if( currentCache.tm_mday != cache.tm_mday ) {
								currentCache.tm_mday = cache.tm_mday;
								shouldRotate         = true;
						} else {
								shouldRotate = false;
								break;
							}
						int numberOfDays { SERENITY_LUTS::daysPerMonth.at(cache.tm_mon) };
						int rotationDay { monthModeSetting };
						// This tidbit is here to make sure a month isn't accidentally skipped
						if( numberOfDays < rotationDay ) {
								rotationDay = numberOfDays;
								if( (cache.tm_mon == serenity::SERENITY_LUTS::FEBRUARY) &&
								    (MsgInfo()->TimeDetails().isLeapYear()) ) {
										rotationDay = 29;
								}
						}
						bool isRotationDay { currentCache.tm_mday == rotationDay };
						bool isRotationHour { cache.tm_hour == dayModeSettingHour };
						bool meetsThreshold { cache.tm_min >= dayModeSettingMinute };
						return (shouldRotate && isRotationDay && isRotationHour && meetsThreshold);
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
		using mType = RotateSettings::IntervalMode;
		switch( m_mode ) {
				case mType::file_size:
					fileSizeLimit = setting;
					break;
					// Including hourly just to avoid compiler whining. Since we check current hour value with internal
					// cached hour value, there's no need to set anything or check anything here.
				case mType::hourly: break;
				case mType::daily:
					{
						if( (setting > 23) || (setting < 0) ) {
								setting = 0;
								std::cerr << "Hour setting passed in for IntervalMode::daily is out of bounds. "
									     "\nValue expected is "
									     "between 0-23 where 0 is 12AM and 23 is 11PM. \nValue passed in: "
									  << setting << " \n";
						}
						if( (secondSetting > 59) || (secondSetting < 0) ) {
								secondSetting = 0;
								std::cerr << "Minute setting passed in for IntervalMode::daily is out of "
									     "bounds. \nValue expected is "
									     "between 0-59. \nValue passed in: "
									  << secondSetting << " \n";
						}
						dayModeSettingHour   = setting;
						dayModeSettingMinute = secondSetting;
					}
					break;
				case mType::weekly:
					{
						if( (setting > 6) || (setting < 0) ) {
								setting = 0;
								std::cerr << "Weekday setting passed in for IntervalMode::weekly is out of "
									     "bounds. \nValue expected is "
									     "between 0-6 where 0 is Sunday and 6 is Saturday. \nValue passed "
									     "in: "
									  << setting << " \n";
						}
						weekModeSetting = setting;
					}
					break;
				case mType::monthly:
					{
						if( (setting > 31) || (setting < 1) ) {
								setting = 1;
								std::cerr << "Day setting passed in for IntervalMode::monthly is out of bounds. "
									     "\nValue expected is "
									     "between 1-31 where 1 is the first day of the month and 31 is the "
									     "max value of possible "
									     "days in a month. \nValue passed in: "
									  << setting << " \n";
						}
						monthModeSetting = setting;
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

	RotateSettings::IntervalMode RotatingTarget::RotationMode() const {
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