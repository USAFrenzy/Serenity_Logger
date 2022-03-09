#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental::targets {
	using namespace serenity::targets;

	RotatingTarget::RotatingTarget(): FileTarget("Rotating_Log.txt", true), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { fileOptions.filePath };
		rotationReadyFile.make_preferred();
		CacheOriginalPathComponents(fileOptions.filePath);
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileOptions.filePath, rotationReadyFile);
				fileOptions.filePath = std::move(rotationReadyFile);
				OpenFile(true);
		} else {
				std::filesystem::remove(fileOptions.filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileOptions.filePath));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists)
		: FileTarget(name, filePath, replaceIfExists), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred();
		CacheOriginalPathComponents(filePath);
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileOptions.filePath, rotationReadyFile);
				fileOptions.filePath = std::move(rotationReadyFile);
				OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(fileOptions.filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileOptions.filePath));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists)
		: FileTarget(name, formatPattern, filePath, replaceIfExists), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred();
		CacheOriginalPathComponents(filePath);
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileOptions.filePath, rotationReadyFile);
				fileOptions.filePath = std::move(rotationReadyFile);
				OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(fileOptions.filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileOptions.filePath));
	}

	RotatingTarget::~RotatingTarget() {
		CloseFile();
	}

	void RotatingTarget::WriteToBaseBuffer(bool fmtToBuf) {
		TargetBase::WriteToBaseBuffer(fmtToBuf);
	}

	const bool RotatingTarget::isWriteToBuf() {
		return TargetBase::isWriteToBuf();
	}

	std::string* const RotatingTarget::Buffer() {
		return TargetBase::Buffer();
	}

	bool RotatingTarget::RenameFile(std::string_view newFileName) {
		try {
				// make copy for old file conversion and cache new values
				std::filesystem::path newFile { fileOptions.filePath };
				newFile.replace_filename(newFileName);
				CacheOriginalPathComponents(newFile);
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
		this->rotationEnabled = rotationEnabled;
		SetCurrentFileSize(std::filesystem::file_size(fileOptions.filePath));
		EnableFirstRotation(rotationEnabled);
	}

	void RotatingTarget::SetRotateSettings(RotateSettings settings) {
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
						fileOptions.filePath = newFilePath;
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
		auto previousFile { fileOptions.filePath.filename().string() };
		if( !fileToReplace.empty() ) {
				fileOptions.filePath = std::move(fileToReplace);
		} else {
				std::cerr << std::vformat("Warning: Unable To Locate Oldest File With Base Name \"{}\". "
				                          "Opening And Truncating "
				                          "Previous File, \"{}\"\n",
				                          std::make_format_args(OriginalName(), previousFile));
				success = false;
			}
		if( !OpenFile(true) ) {
				if( fileOptions.filePath != previousFile ) {
						std::cerr
						<< std::vformat("Error: Unable To Finish Rotating From File \"{}\" To File "
						                "\"{}\"\n",
						                std::make_format_args(previousFile, fileOptions.filePath.filename().string()));
				} else {
						std::cerr << std::vformat("Error: Unable To Open And Truncate File \"{}\"\n",
						                          std::make_format_args(previousFile));
					}
				success = false;
		}
		return success;
	}

	void RotatingTarget::RotateFile() {
		if( !rotationEnabled ) return;
		currrentlyRotatingFile.store(true);
		CloseFile();
		if( !RenameFileInRotation(OriginalPath()) ) {
				if( !ReplaceOldFIleInRotation() ) {
						// If we can't rotate to a new file or replace the oldest file,
						// then we can't log anthing so this would be considered fatal
						throw std::runtime_error("Unable To Rotate File Or Repace Oldest File\n");
				}
		}
		SetCurrentFileSize(0);
		currrentlyRotatingFile.store(false);
		currrentlyRotatingFile.notify_all();
	}

	void RotatingTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		auto flushThreadEnabled { flushWorker.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !flushWorker.flushComplete.load() ) {
						flushWorker.flushComplete.wait(false);
				}
				flushWorker.threadWriting.store(true);
		}
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		if( ShouldRotate() ) {
				RotateFile();
		}
		auto formattedSize { formatted.size() };
		fileHandle.rdbuf()->sputn(formatted.data(), formattedSize);
		SetCurrentFileSize(FileSize() + formattedSize);
		if( flushThreadEnabled ) {
				flushWorker.threadWriting.store(false);
				flushWorker.threadWriting.notify_all();
		}
	}

	bool RotatingTarget::ShouldRotate() {
		if( !rotationEnabled ) return false;
		// If file was empty - no need to rotate
		if( FileSize() == 0 ) return false;

		using mode  = RotateSettings::IntervalMode;
		auto& cache = MsgInfo()->TimeDetails().Cache();

		switch( RotationMode() ) {
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
		// Explicitly stating the override to ensure file target's function is called rather that the base class
		FileTarget::SetLocale(loc);
	}

	void RotatingTarget::SetRotationSetting(IntervalMode mode, int  setting, int secondSetting)
	{
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
		m_mode = mode;
	}

	const RotateSettings::IntervalMode RotatingTarget::RotationMode()
	{
		return m_mode;
	}

	void RotatingTarget::PolicyFlushOn()
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (isMTSupportEnabled()) {
			lock.lock();
		}
		if (policy.PrimarySetting() == serenity::experimental::FlushSetting::never) return;
		if (policy.PrimarySetting() == serenity::experimental::FlushSetting::always) {
			Flush();
		}
		switch (policy.SubSetting()) {
		case serenity::experimental::PeriodicOptions::timeBased:
		{
			if (!flushWorker.flushThreadEnabled.load()) {
				flushWorker.flushThread = std::thread(&RotatingTarget::BackgroundFlush, this);
				flushWorker.flushThreadEnabled.store(true);
			}
		}
		break;    // time based bounds
		case serenity::experimental::PeriodicOptions::logLevelBased:
		{
			if (MsgInfo()->MsgLevel() < policy.SecondarySettings().flushOn) return;
			Flush();
		}
		break;
		}    // Sub Option Check
	}

	void RotatingTarget::BackgroundFlush()
	{
		while (!flushWorker.cleanUpThreads.load()) {
				if (currrentlyRotatingFile.load()) {
					currrentlyRotatingFile.wait(true);
				}
				Flush();
				std::this_thread::sleep_for(policy.SecondarySettings().flushEvery);
			}
	}

}  // namespace serenity::experimental::targets