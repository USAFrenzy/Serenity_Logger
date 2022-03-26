#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental {

	RotateSettings::RotateSettings(const std::string& path)
		: maxNumberOfFiles(5), fileSizeLimit(512 * KB), dayModeSettingHour(0), dayModeSettingMinute(0), weekModeSetting(0),
		  monthModeSetting(1), currentFileSize(0), initalRotationEnabled(true) {
		std::filesystem::path pathToCache { path };
		CacheOriginalPathComponents(pathToCache);
	}

	void RotateSettings::CacheOriginalPathComponents(const std::filesystem::path& filePath) {
		auto fPath { filePath };
		auto directory { filePath };
		this->filePath = fPath.make_preferred().string();
		auto name { fPath.filename() };
		name.replace_extension();
		fileName  = name.string();
		extension = fPath.extension().string();
		directory._Remove_filename_and_separator();
		fileDir = directory.stem().string();
	}

	const std::filesystem::path RotateSettings::OriginalPath() {
		return filePath;
	}

	const std::filesystem::path RotateSettings::OriginalDirectory() {
		return fileDir;
	}

	const std::string RotateSettings::OriginalName() {
		return fileName;
	}

	void RotateSettings::SetCurrentFileSize(size_t currentSize) {
		currentFileSize = currentSize;
	}

	const std::string RotateSettings::OriginalExtension() {
		return extension;
	}

	const size_t RotateSettings::FileSize() {
		return currentFileSize;
	}

	void RotateSettings::EnableFirstRotation(bool enabled) {
		initalRotationEnabled = enabled;
	}

	const bool RotateSettings::IsIntervalRotationEnabled() {
		return initalRotationEnabled;
	}
}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	RotatingTarget::RotatingTarget()
		: FileTarget("Rotating_Log.txt", true), RotateSettings(FilePath()), rotationEnabled(true), m_mode(IntervalMode::file_size) {
		fileHelper.CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { fileHelper.FileOptions().filePath };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileHelper.FileOptions().filePath, rotationReadyFile);
				fileHelper.FileOptions().filePath = std::move(rotationReadyFile);
				fileHelper.OpenFile(true);
		} else {
				std::filesystem::remove(fileHelper.FileOptions().filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileHelper.FileOptions().filePath));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists)
		: FileTarget(name, filePath, replaceIfExists), RotateSettings(std::string(filePath.data(), filePath.size())),
		  rotationEnabled(true), m_mode(IntervalMode::file_size) {
		fileHelper.CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileHelper.FileOptions().filePath, rotationReadyFile);
				fileHelper.FileOptions().filePath = std::move(rotationReadyFile);
				fileHelper.OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(fileHelper.FileOptions().filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileHelper.FileOptions().filePath));
	}

	RotatingTarget::RotatingTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists)
		: FileTarget(name, formatPattern, filePath, replaceIfExists), RotateSettings(std::string(filePath.data(), filePath.size())),
		  rotationEnabled(true), m_mode(IntervalMode::file_size) {
		fileHelper.CloseFile();
		auto& cache { MsgInfo()->TimeDetails().Cache() };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred();
		std::string rotateFile { OriginalName() };
		rotateFile.append("_01").append(OriginalExtension());
		rotationReadyFile.replace_filename(rotateFile);
		if( !std::filesystem::exists(rotationReadyFile) ) {
				std::filesystem::rename(fileHelper.FileOptions().filePath, rotationReadyFile);
				fileHelper.FileOptions().filePath = std::move(rotationReadyFile);
				fileHelper.OpenFile(replaceIfExists);
		} else {
				std::filesystem::remove(fileHelper.FileOptions().filePath);
				RotateFile();
			}
		SetCurrentFileSize(std::filesystem::file_size(fileHelper.FileOptions().filePath));
	}

	RotatingTarget::~RotatingTarget() {
		fileHelper.StopBackgroundThread();
		fileHelper.CloseFile();
	}

	bool RotatingTarget::RenameFile(std::string_view newFileName) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		// make copy for old file conversion and cache new values
		std::filesystem::path newFile { fileHelper.FileOptions().filePath };
		newFile.replace_filename(newFileName);
		CacheOriginalPathComponents(newFile);
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
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		this->rotationEnabled = rotationEnabled;
		SetCurrentFileSize(std::filesystem::file_size(fileHelper.FileOptions().filePath));
		EnableFirstRotation(rotationEnabled);
	}

	void RotatingTarget::SetRotateSettings(RotateSettings settings) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( fileHelper.isMTSupportEnabled() ) {
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
						fileHelper.FileOptions().filePath = newFilePath;
						if( fileHelper.OpenFile(true) ) {
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
		auto previousFile { fileHelper.FileOptions().filePath.filename().string() };
		if( !fileToReplace.empty() ) {
				fileHelper.FileOptions().filePath = std::move(fileToReplace);
		} else {
				std::cerr << std::vformat("Warning: Unable To Locate Oldest File With Base Name \"{}\". "
				                          "Opening And Truncating "
				                          "Previous File, \"{}\"\n",
				                          std::make_format_args(OriginalName(), previousFile));
				success = false;
			}
		if( !fileHelper.OpenFile(true) ) {
				if( fileHelper.FileOptions().filePath != previousFile ) {
						std::cerr << std::vformat(
						"Error: Unable To Finish Rotating From File \"{}\" To File "
						"\"{}\"\n",
						std::make_format_args(previousFile, fileHelper.FileOptions().filePath.filename().string()));
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
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( !rotationEnabled ) return;
		static FlushSetting originalPrimaryMode { FlushSetting::periodically };
		auto wasFlushThreadActive { fileHelper.BackgoundThreadInfo().flushThreadEnabled.load() };
		if( wasFlushThreadActive ) {
				originalPrimaryMode = fileHelper.Policy().PrimarySetting();
				fileHelper.PauseBackgroundThread();
		}
		fileHelper.CloseFile();
		if( !RenameFileInRotation(OriginalPath()) ) {
				if( !ReplaceOldFIleInRotation() ) {
						// If we can't rotate to a new file or replace the oldest file,
						// then we can't log anthing so this would be considered fatal
						throw std::runtime_error("Unable To Rotate File Or Replace Oldest File\n");
				}
		}
		SetCurrentFileSize(0);
		if( wasFlushThreadActive ) {
				fileHelper.ResumeBackgroundThread();
				fileHelper.Policy().SetPrimaryMode(originalPrimaryMode);
		}
	}

	void RotatingTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( ShouldRotate() ) {
				RotateFile();
		}
		auto& backgroundThread { fileHelper.BackgoundThreadInfo() };
		auto flushThreadEnabled { backgroundThread.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !backgroundThread.flushComplete.load() ) {
						backgroundThread.flushComplete.wait(false);
				}
				backgroundThread.threadWriting.store(true);
		}
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		auto formattedSize { formatted.size() };
		fileHelper.FileHandle().rdbuf()->sputn(formatted.data(), formattedSize);
		SetCurrentFileSize(FileSize() + formattedSize);
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				backgroundThread.threadWriting.store(false);
				backgroundThread.threadWriting.notify_all();
		}
	}

	bool RotatingTarget::ShouldRotate() {
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if( !rotationEnabled ) return false;
		if( FileSize() == 0 ) return false;

		if( fileHelper.isMTSupportEnabled() ) {
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
		if (fileHelper.isMTSupportEnabled()) {
			lock.lock();
		}
		// Explicitly stating the override to ensure file target's function is called rather that the base class
		FileTarget::SetLocale(loc);
	}

	void RotatingTarget::SetRotationSetting(IntervalMode mode, int  setting, int secondSetting)
	{
		std::unique_lock<std::mutex> lock(rotatingMutex, std::defer_lock);
		if (fileHelper.isMTSupportEnabled()) {
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
		if (fileHelper.isMTSupportEnabled()) {
			lock.lock();
		}
		m_mode = mode;
	}

	const RotateSettings::IntervalMode RotatingTarget::RotationMode()
	{
		return m_mode;
	}

}  // namespace serenity::experimental::targets