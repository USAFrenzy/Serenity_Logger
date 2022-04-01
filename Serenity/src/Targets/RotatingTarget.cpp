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

	void RotateSettings::EnableFirstRotation(bool enabled) {
		initalRotationEnabled = enabled;
	}

	bool RotateSettings::IsIntervalRotationEnabled() const {
		return initalRotationEnabled;
	}
}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	RotatingTarget::RotatingTarget()
		: TargetBase("Rotating_Log.txt"), RotateSettings(""), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		SyncTargetHelpers(TargetHelper());
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
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
		: TargetBase(name), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		SyncTargetHelpers(TargetHelper());
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
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
		: TargetBase(name, formatPattern), RotateSettings(std::string(filePath)), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		SyncTargetHelpers(TargetHelper());
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
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
		EnableFirstRotation(rotationEnabled);
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

	bool RotatingTarget::ReplaceOldFIleInRotation() {
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
				std::cerr << std::vformat("Warning: Unable To Locate Oldest File With Base Name \"{}\". "
				                          "Opening And Truncating "
				                          "Previous File, \"{}\"\n",
				                          std::make_format_args(OriginalName(), previousFile));
				success = false;
			}
		if( !OpenFile(true) ) {
				if( FileCacheHelper()->FilePath() != previousFile ) {
						std::cerr << std::vformat("Error: Unable To Finish Rotating From File \"{}\" To File "
						                          "\"{}\"\n",
						                          std::make_format_args(previousFile, FileCacheHelper()->FileName()));
				} else {
						std::cerr << std::vformat("Error: Unable To Open And Truncate File \"{}\"\n",
						                          std::make_format_args(previousFile));
					}
				success = false;
		}
		return success;
	}

	void RotatingTarget::RotateFile() {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		if( !rotationEnabled ) return;
		static FlushSetting originalPrimaryMode { FlushSetting::periodically };
		auto wasFlushThreadActive { BackgoundThreadInfo()->flushThreadEnabled.load() };
		if( wasFlushThreadActive ) {
				originalPrimaryMode = TargetHelper()->Policy()->PrimarySetting();
				PauseBackgroundThread();
		}
		CloseFile();
		if( !RenameFileInRotation(OriginalPath()) ) {
				if( !ReplaceOldFIleInRotation() ) {
						// If we can't rotate to a new file or replace the oldest file,
						// then we can't log anthing so this would be considered fatal
						throw std::runtime_error("Unable To Rotate File Or Replace Oldest File\n");
				}
		}
		SetCurrentFileSize(0);
		if( wasFlushThreadActive ) {
				ResumeBackgroundThread();
				TargetHelper()->Policy()->SetPrimaryMode(originalPrimaryMode);
		}
	}

	void RotatingTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( ShouldRotate() ) {
				RotateFile();
		}
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

	bool RotatingTarget::ShouldRotate() {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( !rotationEnabled ) return false;
		if( FileSize() == 0 ) return false;

		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		auto& cache = MsgInfo()->TimeDetails().Cache();

		switch( RotationMode() ) {
				using mode = RotateSettings::IntervalMode;
				case mode::file_size:
					{
						if( IsIntervalRotationEnabled() ) {
								return ((FileSize() + MsgInfo()->MessageSize()) >= fileSizeLimit);
						}
					}
					break;
				case mode::hourly:
					{
						if( currentHour != cache.tm_hour ) {
								currentHour = cache.tm_hour;
								EnableFirstRotation(true);
						} else {
								EnableFirstRotation(false);
							}
						return IsIntervalRotationEnabled();
					}
					break;
				case mode::daily:
					{
						if( currentDay != cache.tm_mday ) {
								currentDay = cache.tm_mday;
								EnableFirstRotation(true);
						}

						if( IsIntervalRotationEnabled() ) {
								if( dayModeSettingHour == cache.tm_hour ) {
										if( (cache.tm_min >= dayModeSettingMinute) ) {
												EnableFirstRotation(false);
												return true;
										}
								}
						}
					}
					break;
				case mode::weekly:
					if( currentWeekday != cache.tm_wday ) {
							currentWeekday = cache.tm_wday;
							EnableFirstRotation(true);
					}

					if( IsIntervalRotationEnabled() ) {
							if( (currentWeekday == weekModeSetting) && (cache.tm_hour == dayModeSettingHour) ) {
									if( (cache.tm_min >= dayModeSettingMinute) ) {
											EnableFirstRotation(false);
											return true;
									}
							}
					}
					break;
				case mode::monthly:
					{
						if( currentDay != cache.tm_mday ) {
								currentDay = cache.tm_mday;
								EnableFirstRotation(true);
						}

						int numberOfDays { SERENITY_LUTS::daysPerMonth.at(cache.tm_mon) };
						int rotationDay { monthModeSetting };
						// clang-format off
			/********************************************** Simple Summary *****************************************************************
			* - If the number of days per month is less than the setting ( for example, if the setting is 31 but the month has 30 days):
			*   - The rotation day will be the 30th instead (so as not to skip a month by accident)
			*	 - If the current month is February, checks to see if the current year is a leap year.
			*     - If the year is a leap year, then sets the rotation day to 29 instead of 28
			* - Otherwise, this snippet uses the monthModeSetting value
			*******************************************************************************************************************************/
			// clang-format off
			if (numberOfDays < monthModeSetting) {
				rotationDay = numberOfDays;
				if (cache.tm_mon == serenity::SERENITY_LUTS::FEBRUARY) {
					if (MsgInfo()->TimeDetails().isLeapYear()) {
						rotationDay = 29;
					}
				}
			}
			if (IsIntervalRotationEnabled()) {
				if ((currentDay == rotationDay) && (cache.tm_hour == dayModeSettingHour)) {
					if (cache.tm_min >= dayModeSettingMinute) {
						EnableFirstRotation(false);
						return true;
					}
				}
			}
		} break;
		}
		// In case interval rotation isn't enabled, then we just return false here
		return false;
	}

	void RotatingTarget::SetLocale(const std::locale& loc)
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (TargetHelper()->isMTSupportEnabled()) {
			lock.lock();
		}
		if (FileHandle().getloc() != loc) {
			FileHandle().imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void RotatingTarget::SetRotationSetting(IntervalMode mode, int  setting, int secondSetting)
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (TargetHelper()->isMTSupportEnabled()) {
			lock.lock();
		}
		using mType = RotateSettings::IntervalMode;
		switch (m_mode) {
		case mType::file_size: fileSizeLimit = setting; break;
			// Including hourly just to avoid compiler whining. Since we check current hour value with internal cached
			// hour value, there's no need to set anything or check anything here.
		case mType::hourly:  break;
		case mType::daily:
		{
			if ((setting > 23) || (setting < 0)) {
				setting = 0;
				std::cerr << "Hour setting passed in for IntervalMode::daily is out of bounds. \nValue expected is "
					"between 0-23 where 0 is 12AM and 23 is 11PM. \nValue passed in: "
					<< setting << " \n";
			}
			if ((secondSetting > 59) || (secondSetting < 0)) {
				secondSetting = 0;
				std::cerr << "Minute setting passed in for IntervalMode::daily is out of bounds. \nValue expected is "
					"between 0-59. \nValue passed in: "
					<< secondSetting << " \n";
			}
			dayModeSettingHour = setting;
			dayModeSettingMinute = secondSetting;

		} break;
		case mType::weekly:
		{
			if ((setting > 6) || (setting < 0)) {
				setting = 0;
				std::cerr << "Weekday setting passed in for IntervalMode::weekly is out of bounds. \nValue expected is "
					"between 0-6 where 0 is Sunday and 6 is Saturday. \nValue passed in: "
					<< setting << " \n";
			}
			weekModeSetting = setting;
		} break;
		case mType::monthly:
		{
			if ((setting > 31) || (setting < 1)) {
				setting = 1;
				std::cerr << "Day setting passed in for IntervalMode::monthly is out of bounds. \nValue expected is "
					"between 1-31 where 1 is the first day of the month and 31 is the max value of possible "
					"days in a month. \nValue passed in: "
					<< setting << " \n";
			}
			monthModeSetting = setting;
		} break;
		}
	}

	void RotatingTarget::SetRotationMode(IntervalMode mode)
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (TargetHelper()->isMTSupportEnabled()) {
			lock.lock();
		}
		m_mode = mode;
	}

	RotateSettings::IntervalMode RotatingTarget::RotationMode() const
	{
		return m_mode;
	}

	// This is just copy-pasted from the File Target.
	// Inheritance prevented this copy pasta but still wanted it fully de-coupled
	void RotatingTarget::PolicyFlushOn()
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (TargetHelper()->isMTSupportEnabled()) {
			lock.lock();
		}
		auto& policy{ TargetHelper()->Policy() };
		if (policy->PrimarySetting() == serenity::experimental::FlushSetting::never) {
			// If the flush thread was active, no need to hog a thread if never flushing
			StopBackgroundThread();
		};
		if (policy->PrimarySetting() == serenity::experimental::FlushSetting::always) {
			// Similar reasoning as Never setting except for the fact of ALWAYS flushing
			StopBackgroundThread();
			Flush();
		}
		switch (policy->SubSetting()) {
		case serenity::experimental::PeriodicOptions::timeBased:
		{
			StartBackgroundThread();
		}
		break;    // time based bounds
		case serenity::experimental::PeriodicOptions::logLevelBased:
		{
			if (MsgInfo()->MsgLevel() < policy->SecondarySettings().flushOn) return;
			Flush();
		}
		break;
		}    // Sub Option Check
	}                    // PolicyFlushOn( ) Function

}  // namespace serenity::experimental::targets